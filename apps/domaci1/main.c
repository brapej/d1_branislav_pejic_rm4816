#include<fcntl.h>
#include<string.h>



#define UTIL_IMPLEMENTATION
#include "../utils.h" 


#include "scan.h"

int mystrcmp(char* str1,char* str2){
	int check=1;	
	char *p1 = str1;
	char *p2 = str2;	
	do{
	check = *p1 == *p2;
	p1++;p2++;	
	}while(check && *p1!='\0' && *p2);
	
	return check;
}


int main(int argc, char *argv[])
{	
	char buffer[64];
	int buff_index=0;
	
	char* buffprim=buffer;
	
	
	/* ucitavanje podesavanja */ 
	load_config("scancodes.tbl","ctrl.map");
	
	char fname[64],exstr[64],scc_str[5];
	char* ext = "exit\n";
	int fn_len, sc_len,fd=0,sc=0;	
	///////////////1///////////////////////
	do{
	printstr("Molim unesite ime fajla za obradu:\n");
	fn_len = read(0,fname,64);
	if(mystrcmp(fname,ext)==1){
	break;	
	}
	else{	
		
		fname[fn_len-1]='\0';
		fd = open(fname,O_RDONLY);
		if(fd == -1){
		printerr("incorrect test file");
		break;
		}
			
		do{
		int scclen =fgets(scc_str,5,fd);
		int sc = atoi(scc_str);	
			
		buff_index=process_scancode(sc,buffer);
		
		
		write(1,buffer,buff_index);
		
		
		}while(!mystrcmp(scc_str,"400"));
		close(fd);
		status = 0;
		printstr("\n");
	   }
	}while(1);
	
	
	/* ponavljamo: */
		/* ucitavanje i otvaranje test fajla */
		/*buff_index+=process_scancode(1,buffprim+buff_index);
buff_index+=process_scancode(3,buffprim+buff_index);
	buff_index+=process_scancode(23,buffprim+buff_index);
	buff_index+=process_scancode(12,buffprim+buff_index);
	buff_index+=process_scancode(200,buffprim+buff_index);
	buff_index+=process_scancode(12,buffprim+buff_index);
	buff_index+=process_scancode(12,buffprim+buff_index);
	buff_index+=process_scancode(12,buffprim+buff_index);
	
		write(1,buffer,buff_index);
	*/
	
	
		/* parsiranje fajla, obrada 	scanecodova, ispis */
	_exit(0);
		
}
