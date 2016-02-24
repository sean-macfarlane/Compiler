/* File Name: stable.c
* Compiler: MS Visual Studio 2012
* Author: Sean Macfarlane, 040-779-100
* Course: CST8152 - Compliers, Lab Section: 011
* Assignment: 4
* Date: 17 April 2015
* Professor: Sv. Ranev
* Purpose: Functions implementing a Symbol Table
* Function List: 
*					 st_create()
*					 st_install()
*					 st_lookup()
*					 st_update_type()
*					 st_update_value()
*					 st_get_type ()
*					 st_destroy()
*					 st_print()
*					 st_setsize()
*					 st_incoffset()
*					 st_store()
*					 st_sort()
*					 compareA()
*					 compareD()
*/

/* header files */
#include <stdlib.h>
#include "buffer.h"
#include "stable.h"

extern STD sym_table;	/* Symbol Table Descriptor */

/* Static Function Declarations */
static void st_setsize(void);
static void st_incoffset(void);
static int compareA(const void*, const void*);
static int compareD(const void*, const void*);

/********************************************************************
Purpose:	Creates Symbol Table Descriptor
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	calloc(), b_create()
Parameters:	int
Return Value: STD
Algorithm:
**********************************************************************/	
STD st_create(int st_size){
	STD std;	/* Temporary STD */

	if(st_size <= 0)
		std.st_size = 0;
	std.pstvr = (STVR*)calloc(st_size, sizeof(STVR));
	if(std.pstvr == NULL)
		std.st_size = 0;
	std.plsBD = b_create(ST_CAPACITY, ST_INC_FACTOR, 'a');
	if(std.plsBD == NULL){
		std.st_size = 0;
		free(std.pstvr);
	}
	else
		std.st_size = st_size;
	std.st_offset=0;

	return std;
}

/********************************************************************
Purpose:	Creates Symbol Table Descriptor
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	st_lookup(), b_addc(), b_rflag(), b_setmark(), strlen(),
b_size(), st_incoffset()
Parameters:	STD, char*, char, int
Return Value: int
Algorithm:
**********************************************************************/	
int st_install(STD sym_table, char *lexeme, char type, int line){
	int l_offset;	/* VID Offset returned by lookup */
	STVR temp;	/* Temporary STVR */
	int ctr =0;		/* Counter */
	int r_flag = 0;	/* Local Realocation Flag */
	int r_ctr = 0;	/* Counter when Realocating */
	int r_offset = 0;	/* Offset when realocating */

	if(sym_table.st_size == 0|| (type != 'F' && type!='I' && type!='S') || line < 0) /* Checks parameters */
		return R_FAIL_2;

	l_offset = st_lookup(sym_table, lexeme);
	if(l_offset>=0){	/* If found in Symbol Table */
		return l_offset;
	}
	else{	
		if(sym_table.st_size == sym_table.st_offset){	/* Checks if full */
			return ST_FAIL;
		}
		/* adds lexeme to Lexeme Storage */ 
		while(lexeme[ctr] != '\0'){
			b_addc(sym_table.plsBD, lexeme[ctr]);
			if(b_rflag(sym_table.plsBD))
				r_flag = SET_R_FLAG;
			++ctr;
		}
		b_addc(sym_table.plsBD, '\0');
		++ctr;
		if(r_flag){		/* if buffer reallocated */
			sym_table.pstvr[r_ctr].plex = b_setmark(sym_table.plsBD, 0);
			while(r_ctr<sym_table.st_offset){
				++r_ctr;
				r_offset += strlen(sym_table.pstvr[r_ctr-ADD_ONE].plex)+ADD_ONE;
				sym_table.pstvr[r_ctr].plex = b_setmark(sym_table.plsBD, (short)r_offset);
			}
			r_flag = 0;
		}
		temp.plex = b_setmark(sym_table.plsBD, b_size(sym_table.plsBD)-(short)ctr);
		temp.o_line = line;
		temp.status_field = DEFAULT;
		if(type == 'F'){ 
			temp.status_field = temp.status_field | FLOAT;
			temp.i_value.fpl_val = 0;
		}
		else if(type == 'I'){
			temp.status_field = temp.status_field | INT;
			temp.i_value.int_val = 0;
		}
		else{
			temp.status_field = temp.status_field | STRING;
			temp.status_field = temp.status_field | UPDATE;
			temp.i_value.str_offset = -ADD_ONE;
		}

		sym_table.pstvr[sym_table.st_offset] = temp;
		st_incoffset();
		return sym_table.st_offset;
	}
}

/********************************************************************
Purpose:	Symbol Table Look Up
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	strcmp()
Parameters:	STD, char*
Return Value: int
Algorithm:
**********************************************************************/	
int st_lookup(STD sym_table, char *lexeme){
	int offset;	/* Counter offset to loop from end of Array backward */

	if(sym_table.st_size == 0 || !lexeme) /* Checks parameters */
		return ST_FAIL;
	offset = sym_table.st_offset-ADD_ONE;
	/* Starts from last entry and looks backwards to beginning of Array of STVR */
	for(offset; offset>=0; offset--){
		if(strcmp(sym_table.pstvr[offset].plex, lexeme)== 0)
			return offset;
	}
	return ST_FAIL;
}

/********************************************************************
Purpose:	Updates the VID Type of a Symbol Table VID Record
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	
Parameters:	STD, int, char
Return Value: int
Algorithm:
**********************************************************************/	
int st_update_type(STD sym_table,int vid_offset,char v_type){
	if(sym_table.st_size == 0 || vid_offset < 0 ||vid_offset >= sym_table.st_offset)	/* Checks parameters */
		return ST_FAIL;
	if(v_type != 'F' && v_type != 'I') /* Checks type */
		return ST_FAIL;
	if(!(sym_table.pstvr[vid_offset].status_field&UPDATE)){		/* Checks if already Updated */
		sym_table.pstvr[vid_offset].status_field = sym_table.pstvr[vid_offset].status_field ^ CONVERT;
		sym_table.pstvr[vid_offset].status_field = sym_table.pstvr[vid_offset].status_field | UPDATE;
		return vid_offset;
	}
	return ST_FAIL;
}

/********************************************************************
Purpose:	Updates the VID InitialValue of a Symbol Table VID Record
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	
Parameters:	STD, int, InitialValue
Return Value: int
Algorithm:
**********************************************************************/	
int st_update_value(STD sym_table, int vid_offset, InitialValue i_value){
	if(sym_table.st_size == 0 || vid_offset < 0 ||vid_offset >= sym_table.st_offset) /* Checks parameters */
		return ST_FAIL;
	sym_table.pstvr[vid_offset].i_value = i_value;
	return vid_offset;
}

/********************************************************************
Purpose:	Gets the VID Type of Symbol Table VID Record
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	
Parameters:	STD, int
Return Value: char
Algorithm:
**********************************************************************/	
char st_get_type (STD sym_table, int vid_offset){
	if(sym_table.st_size == 0 || vid_offset < 0) /* Checks parameters */
		return ST_FAIL;
	if(!((sym_table.pstvr[vid_offset].status_field&DEFAULT)^FLOAT))
		return 'F';
	if(!((sym_table.pstvr[vid_offset].status_field&DEFAULT)^INT))
		return 'I';
	if(!((sym_table.pstvr[vid_offset].status_field&DEFAULT)^(STRING^UPDATE)))
		return 'S';
	return ST_FAIL;
}

/********************************************************************
Purpose:	Frees all memory allocated in Symbol Table
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions: free(), b_destroy()
Parameters: STD
Return Value: 
Algorithm:  
**********************************************************************/	
void st_destroy(STD sym_table){
	if(sym_table.st_size == 0) /* Checks parameters */
		return;
	b_destroy(sym_table.plsBD);
	free(sym_table.pstvr);
	st_setsize();
}

/********************************************************************
Purpose:	Prints all Symbol Table VID Records
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	printf()
Parameters:	STD
Return Value: int
Algorithm:
**********************************************************************/	
int st_print(STD sym_table){
	int ctr = 0;	/* Counter */

	if(sym_table.st_size == 0) /* Checks parameters */
		return ST_FAIL;
	printf("\nSymbol Table\n");
	printf("____________\n\n");
	printf("Line Number Variable Identifier\n");
	for(ctr = 0; ctr<sym_table.st_offset; ctr++)
		if(sym_table.pstvr[ctr].o_line<LINE_TEN) /* Prints a space before digit if less than 10 */
			printf(" %d          %s\n", sym_table.pstvr[ctr].o_line, sym_table.pstvr[ctr].plex);
		else 
			printf("%d          %s\n", sym_table.pstvr[ctr].o_line, sym_table.pstvr[ctr].plex);
	return ctr;
}

/********************************************************************
Purpose:	Sets the global Symbol Table size to 0
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	
Parameters: 
Return Value: 
Algorithm:
**********************************************************************/	
static void st_setsize(void){
	sym_table.st_size = 0;
}

/********************************************************************
Purpose:	Increments the Global Symbol Table offset
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	
Parameters: 
Return Value: 
Algorithm:
**********************************************************************/	
static void st_incoffset(void){
	++sym_table.st_offset;
}

/********************************************************************
Purpose:	Stores the Symbol Table in a File
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	fopen(), fprintf(), strlen(), st_get_type(), fclose(), printf()
Parameters:		STD
Return Value:	int
Algorithm:
**********************************************************************/	
int st_store(STD sym_table){
	FILE *st;	/* File */
	char *fname = "$stable.ste"; /* define a file name */
	int ctr = 0;	/* Counter */

	if(!sym_table.plsBD) /* Checks parameters */
		return ST_FAIL;

	/* Open a text file for writing ("wt")
	* If the file already exists, it will be overwritten
	*/ 
	if ((st = fopen(fname,"wt")) == NULL){	/* Will cause warning to use fopen_s instead of f_open */
		return ST_FAIL;
	}

	/* Store some data into the file */
	fprintf(st, "%d", sym_table.st_size);
	for(ctr = 0; ctr<sym_table.st_size; ctr++){
		fprintf(st," %hX %d %s %d ", sym_table.pstvr[ctr].status_field, strlen(sym_table.pstvr[ctr].plex), sym_table.pstvr[ctr].plex, sym_table.pstvr[ctr].o_line);
		if(st_get_type(sym_table, ctr) == 'F')	/* if type Float */
			fprintf(st, "%.2f",sym_table.pstvr[ctr].i_value.fpl_val);
		else if(st_get_type(sym_table, ctr) == 'I')		/* if type Int */
			fprintf(st, "%d",sym_table.pstvr[ctr].i_value.int_val);
		else if(st_get_type(sym_table, ctr) == 'S')		/* if type String */
			fprintf(st, "%d",sym_table.pstvr[ctr].i_value.str_offset);
	}

	/* Close the file */ 							  									 
	fclose(st);
	printf("\nSymbol Table stored.\n");
	return ctr;
}

/********************************************************************
Purpose:	Sorts the Symbol Table VID Record array
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	qsort(), sizeof(), comparA(), compareD()
Parameters:		STD, char
Return Value:	int
Algorithm:		Uses qsort to sort Symbol Table VID Records by plex. 
And then the stores sorted Character Array in a temporary
buffer and overwrites the Lexeme Storage with the sorted
Character Array and relocates the plex pointers to the 
correct lexemes in the Lexeme storage.
**********************************************************************/	
int st_sort(STD sym_table, char s_order){
	Buffer* buffer;		/* Temporary Storage Buffer */
	int ctr = 0;		/* Counter */
	int ctr2 = 0;		/* Counter */
	int r_ctr = 0;		/* Counter when relocating */
	int r_offset = 0;	/* Offset when relocating */

	if(!sym_table.plsBD)
		return ST_FAIL;

	if(s_order == 'A')	/* If Ascending Order */
		qsort(sym_table.pstvr, sym_table.st_offset, sizeof(STVR), compareA);
	else if(s_order == 'D')		/* If Descending Order */
		qsort(sym_table.pstvr, sym_table.st_offset, sizeof(STVR), compareD);
	else
		return ST_FAIL;

	/* Create Temporary buffer to store sorted Character Array */
	buffer = b_create(b_capacity(sym_table.plsBD), (char)b_inc_factor(sym_table.plsBD), (char)b_mode(sym_table.plsBD));
	if(!buffer)
		return ST_FAIL;
	for(ctr = 0; ctr< sym_table.st_offset; ctr++){
		ctr2 = 0;
		while(sym_table.pstvr[ctr].plex[ctr2] != '\0'){
			b_addc(buffer, sym_table.pstvr[ctr].plex[ctr2]);
			++ctr2;
		}
		b_addc(buffer, '\0');	
	}

	b_reset(sym_table.plsBD);	/* Resets Lexeme Storage to overwrite with sorted Character Array */
	/* Copies Sorted Character Array to Lexeme Storage */
	ctr = 0;
	while(ctr<b_size(buffer)){
		b_addc(sym_table.plsBD, b_getc(buffer));
		++ctr;
	}
	/* Relocates the plex pointers to the correct lexemes */
	sym_table.pstvr[r_ctr].plex = b_setmark(sym_table.plsBD, 0);
	++r_ctr;
	while(r_ctr<sym_table.st_offset){	
		r_offset += strlen(sym_table.pstvr[r_ctr-ADD_ONE].plex)+ADD_ONE;
		sym_table.pstvr[r_ctr].plex = b_setmark(sym_table.plsBD, (short)r_offset);
		++r_ctr;
	}
	b_destroy(buffer);
	return SUCCESS;
}

/********************************************************************
Purpose:	Compares the Lexemes in Ascending Order
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	strcmp()
Parameters:		const void*, const void*
Return Value:	int
Algorithm:		Comparator function to use with qsort to sort plex in Acsending order
**********************************************************************/	
static int compareA(const void * a, const void * b){
	const STVR* p1 = (STVR*)a;
	const STVR* p2 = (STVR*)b;

	return strcmp(p1->plex, p2->plex);
}

/********************************************************************
Purpose:	Compares the Lexemes in Descending Order
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	strcmp()
Parameters:		const void*, const void*
Return Value:	int
Algorithm:		Comparator function to use with qsort to sort plex in Acsending order
**********************************************************************/	
static int compareD(const void * a, const void * b){
	const STVR* p1 = (STVR*)a;
	const STVR* p2 = (STVR*)b;

	return -strcmp(p1->plex, p2->plex);
}
