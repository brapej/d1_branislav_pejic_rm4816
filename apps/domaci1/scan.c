#include <fcntl.h>
#include <string.h>

#include "../utils.h"


typedef struct sc_mne{
char scc;
char mne[64]; 
	}sc_mne;
/*
	Deklaracije za nizove i tabele ovde
	tabelu za prevodjenje scancodeova i tabelu
	za cuvanje mnemonika cuvati kao staticke
	varijable unutar ovog fajla i ucitavati
	ih uz pomoc funkcije load_config

*/
#define MAXLEN 128
#define CLEN 2
static sc_mne scancodes[CLEN][MAXLEN];


void dispstruct(sc_mne strct){
	const char *c = &strct.scc;
	write(1,c,1);
	write(1,"\n ",sizeof("\n"));
	write(1,strct.mne,strlen(strct.mne));
	return;
	
	
}

void load_config(const char *scancodes_filename, const char *mnemonic_filename)
{
	int len;
	char buffer[MAXLEN];

	/*******UCITAVANJE SCAN I SHIFT CODE-OVA********/	
	int fd = open(scancodes_filename,O_RDONLY);
		
	if(fd == -1){
		write(1,"ERROR opening a file",strlen("ERROR opening a file"));
		_exit(0);
	}
	
	buffer[0]='\0';	

	len = fgets(buffer,MAXLEN,fd);
	buffer[len-1] = '\0';	
	len-=1;

	
	do{
	scancodes[0][len].scc = buffer[len]; 
	}while(len--);	
	

	len = fgets(buffer,MAXLEN,fd);
	buffer[len-1] = '\0';
	len-=1;	
	int len1 = len;
	

	do{
	scancodes[1][len].scc = buffer[len]; 
	}while(len--);
	
	
	close(fd);

	/*******UCITAVANJE MNEMONIKA********/
	fd = open(mnemonic_filename,O_RDONLY);

	if(fd == -1){
		write(1,"ERROR opening a file",strlen("ERROR opening a file"));
		return;
	}
	fgets(buffer,MAXLEN,fd);
	len = atoi(buffer)+1;

	int i=0;
	for(len=atoi(buffer);len>=0;len--){
	
	int mnlen = fgets(buffer,67,fd);
	buffer[mnlen-1] = '\0';
	
	for(i=0;i<len1;i++){
	if(scancodes[0][i].scc == buffer[0]){
		strcpy(scancodes[0][i].mne,&buffer[2]);
		/*dispstruct(scancodes[0][i]);
		pause();*/
		break;
		}
		else{
		scancodes[0][i].mne[0]=='\0';
		}
	}
	i=0;
	
	for(i=0;i<len1;i++){	
	if(scancodes[1][i].scc == buffer[0]){
		strcpy(scancodes[1][i].mne,&buffer[2]);
		/*dispstruct(scancodes[1][i]);
		pause();*/
		break;
		}
		else{
		scancodes[1][i].mne[0]=='\0';
		}
			
	}	
	
	}
	close(fd);
	
	/*for(i=0;i<=len1;i++){
	dispstruct(scancodes[0][i]);
	pause();	
	}
	
	pause();	
	
	for(i=0;i<=len1;i++){
	dispstruct(scancodes[1][i]);
	pause();	
	}*/
	
	
	return;
	
}



char status=0;
char ascii=0;


static const int SUP_SC = 300;
static const int SDN_SC = 200;

static const int CUP_SC = 301;
static const int CDN_SC = 201;

static const int AUP_SC = 302;
static const int ADN_SC = 202;

static const int EXT_SC = 400;
/////////////////////////////////


int process_scancode(int scancode, char *buffer)
{	
	int result;
	/*
		Your code goes here!
		Remember, only inline assembly.
		Good luck!
	*/

	
	__asm__ __volatile__ (
	
		
	
	"cmpl $200, %%eax;"
	"jge SCS;"
	
	"cmpl $128,%%eax;"
	"jle OCS;"
	

	"jmp END;"
//////////////////////////////////////////////ORDINARY CODES
	"OCS:"
	"cld;"			   //DF = 0;
	"xorl %%ecx,%%ecx;"	  //CX = 0;	
	"cmpb $1,(status);"	 //STATUS = 1(shift) ?
	"je S;"	 	        //JESTE - > SKOCI NA S
	"cmpb $3,(status);"    // STATUS(shift,ctrl) = 3 ?
	"je S;"	 	      //JESTE - > SKOCI NA S
	"SR:imull $65,%%eax;"//POVRATAK IZ SHIFTA + MNOZIMO EAX(index niza sc-a)
			    // SA 65 (velicina strukture) DA BISMO GA ADRESIRALI 
	"addl %%eax,%%esi;"// DODAJEMO NA %%ESI DA BI GA ADRESIRALI

	"cmpb $4,(status);"
	 "je A;"		//OTKOMENTARISATI PO ZELJI
	"cmpb $5,(status);"
	"je A;"
	"cmpb $6,(status);"
	"je A;"
	"cmpb $7,(status);"
	"je A;"
	
	"cmpb $0,8(%%esi,%%ecx,8);"// GLEDAMO DA LI POSTOJI MNEMONIK 
	"je NOMNEM;" //AKO NE POSTOJI , SAMO ISPISUJEMO KARAKTER
	"cmpb $2,(status);"//(ctrl)?
	"je C;"//DA -> SKACEMO NA C
	"cmpb $3,(status);"//(shift,ctrl)?
	"je C;"	// AKO POSTOJI ISPISUJEMO MNEMONIK
	"NOMNEM:movsb;"//SMESTAMO IZ NIZA SC-OVA U BUFFER
	"incl %%ecx;" //POVECAVAMO CX , ON BROJI KOLIKO JE KARAKTERA SMESTENO
	"CR:addl $1, %%edi;" // POMERAMO ADRESU BUFFERA ZA 1	
	"movb (ascii),%%al;" 
	"movb %%al, (%%edi);"// TERMINISEMO STRING 
	"jmp END;"
//////////////////////////////////////////shiftmd

	"S:addl $128, %%eax;"//DODAJEMO 128 DA BI STIGLI DO SLEDECEG REDA MATRICE	
	   "jmp SR;"

/////////////////////////////////////////ctrlmd
	"C:addl $1, %%esi;" //IDEMO NA SLEDECU ADRESU NIZA SC-OVA SA KOJE POCINJE MNEMONIK
	"CL:movsb;"//SMESTAMO SLOVA MNEMONIKA DOK NE NALETIMO NA TERMINATOR
	"incl %%ecx;"//BROJIMO SMESTENA SLOVA
	"cmpb $0, (%%esi);"//...DOK NE NALETIMO NA TERMINATOR
	"jne CL;" //... DOK NE NALETIMO NA TERMINATOR 
	"je CR;"// SKACEMO NA POMERANJE ADRESE  I TERMINISANJE BUFFERA 
	
	
	
	"jmp END;"
/////////////////////////////////////////altmd
	"A:"
	"xorl %%eax, %%eax;"	
	"movb (ascii), %%bl;"
	"movb (%%esi), %%al;"
	"subb $48, %%al;"
	"xorb %%bh,%%bh;"
	"imulw $10, %%bx;"
	"addb %%al, %%bl;"
	"movb %%bl,(ascii);"
	"jmp END;"

//////////////////////////////////////////////STATUS CODES
	"SCS:"	
	"cmpl (SUP_SC), %%eax;"
	"je  SUP;"

	"cmpl (CUP_SC), %%eax;"
	"je  CUP;"

	"cmpl (AUP_SC), %%eax;"
	"je  AUP;"

	"cmpl (SDN_SC), %%eax;"
	"je  SDN;"	
	
	"cmpl (CDN_SC), %%eax;"
	"je  CDN;"

	"cmpl (ADN_SC), %%eax;"
	"je  ADN;"

	"cmpl (EXT_SC), %%eax;"
	"je EXT;"	
/////////////////////////////////////////////	
	"SUP:"
	"andb $6,(status);"
	"movl $0,%%ecx;"
	"jmp END;"
		
	"CUP:"
	"andb $5,(status);"
	"movl $0,%%ecx;"
	"jmp END;"

	"AUP:"
	"andb $3,(status);"
	"movl $0,%%ecx;"
	"jmp END;"


	"SDN:"
	"orb $1,(status);"
	"movl $0,%%ecx;"	
	"jmp END;"
		
	"CDN:"
	"orb $2,(status);"
	"movl $0,%%ecx;"	
	"jmp END;"

	"ADN:"

	"orb $4,status;"
	"movl $0,%%ecx;"
	"movb (ascii),%%al;"
	"xorb %%al,%%al;"
	"movb %%al, (ascii);"
	//"cmpb $0, %%bl;"
	//"je CR;"
	 
	"movb %%al, (%%edi);" //OTKOMENTARISATI PO ZELJI
	"incl %%ecx;"
	"jmp CR;"
	"jmp END;" 

	"EXT:"
	"xorL %%ecx,%%ecx;"
	"je END;"
	
	"END:"
	
	

		: "=c" (result)
		:"a" (scancode),"S"(scancodes),"D"(buffer)
		:"memory","%ebx"

	);
	
	//write(1,"\n",1);
	
	
	//vardump(ascii);
	//vardump(status);
	//pause();
	/*vardump(buffer[0]);
	vardump(buffer[1]);
	vardump(buffer[2]);
	vardump(buffer[3]);
	vardump(result);
	vardump(buff_index);
	write(1,buffer,buff_index);
	*/
	

	return result;
}
