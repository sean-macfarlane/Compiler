/* File Name: table.h
* Compiler: MS Visual Studio 2012
* Author: Sean Macfarlane, 040-779-100
* Course: CST8152 - Compliers, Lab Section: 011
* Assignment: 4
* Date: 17 April 2015
* Professor: Sv. Ranev
* Purpose: Preprocessor directives, type declarations and prototypes necessary for transition table implementation 
* Function List: none					
*/

#ifndef  TABLE_H_
#define  TABLE_H_ 

#ifndef BUFFER_H_
#include "buffer.h"
#endif

#ifndef NULL
#include <_null.h> /* NULL pointer constant is defined there */
#endif

/* constant definitions */
#define ES  13	 /*	Error state */
#define ENR 12	/*	Error state No Retract */
#define OIL 11	/*	Octal Integer Literal State */
#define S10 10	/*	State 10 */
#define S9	9	/*	State 9 */
#define	FPL	8	/*	Floating Point Literal State */
#define S7	7	/*	State 7 */
#define S6	6	/*	State 6 */
#define DIL	5	/*	Decimal Integer Literal */
#define S4	4	/*	State 4 */
#define	SVID 3	/*	String Variable Identifier */ 
#define AVID 2	/*	Arithmetic Variable Identifier */
#define S1	1	/*	State 1	*/
#define IS -1    /* Inavalid state */
#define NUM_S 14	/* Number of States */
#define SEOF 255 /* End of file character value */
#define CHAR_FAIL	254	/* Run Time Error character value -2 */
#define LEX_CAPACITY	200	/* Initial capacity of lex_buf */
#define ARIX_1	1	/* Array Index 1 */
#define ARIX_2	2	/* Array Index 2 */
#define LOOP_BREAK	1	/* Breaks the transition table loop */
#define CLASS_0	0	/* Transition Table Alpha Class */
#define CLASS_1	1	/* Transition Table Zero Class */
#define CLASS_2 2	/* Transition Table Non-Zero [1-7] Decimal Class */
#define CLASS_3 3	/* Transition Table Non-Zero [8-9] Decimal Class */
#define CLASS_4 4	/* Transition Table . Class */
#define CLASS_5 5	/* Transition Table # Class */
#define CLASS_6 6	/* Transition Table Other Class */
#define ARITH_ONE	1 /* Arithmetic 1 value */
#define BASE_10	10	/* Base 10 value for Octal Conversion */
#define BASE_8	8.0	/* Base 8 value for Octal Conversion */
#define ERR_LEN_17	17	/* Error message full length */
#define TABLE_COLUMNS 7	/* Number of Transition Table Columns */
#define S_MAX	32767
#define S_MIN -32768

/* State transition table definition */
int  st_table[][TABLE_COLUMNS] = {
	/* State 0 */   {S1, S6, S4, S4, ES, ES, ES,},
	/* State 1 */   {S1, S1, S1, S1, AVID, SVID, AVID},
	/* State 2 */	{IS,IS,IS,IS,IS,IS,IS},
	/* State 3 */	{IS,IS,IS,IS,IS,IS,IS},
	/* State 4 */	{ES, S4, S4, S4, S7, DIL, DIL},
	/* State 5 */	{IS,IS,IS,IS,IS,IS,IS},
	/* State 6 */	{ES, S9, S10, ES, S7, ES, DIL},
	/* State 7 */	{FPL, S7, S7, S7, FPL, FPL, FPL},
	/* State 8 */	{IS,IS,IS,IS,IS,IS,IS},
	/* State 9 */	{ES, ES, ES, ES, ES, ES, OIL}, 
	/* State 10 */	{ES, S10, S10, ES, ES, ES, OIL},
	/* State 11 */	{IS,IS,IS,IS,IS,IS,IS},
	/* State 12 */	{IS,IS,IS,IS,IS,IS,IS},
	/* State 13 */  {IS,IS,IS,IS,IS,IS,IS}
};


/* Accepting state table definition */
#define ASWR     0  /* accepting state with retract */
#define ASNR     1  /* accepting state with no retract */
#define NOAS     2  /* not accepting state */

int as_table[] = {NOAS, NOAS, ASWR, ASNR, NOAS, ASWR, NOAS, NOAS, ASWR, NOAS, NOAS, ASWR , ASNR, ASWR};

/* Accepting action function declarations */
Token aa_func02(char *lexeme);
Token aa_func03(char *lexeme); 
Token aa_func05(char *lexeme); 
Token aa_func08(char *lexeme);
Token aa_func11(char *lexeme); 
Token aa_func13(char *lexeme); 

/* Accepting function (action) callback table (array) definition */
/* If you do not want to use the typedef, the equvalent declaration is:
* Token (*aa_table[])(char lexeme[]) = {
*/
typedef Token (*PTR_AAF)(char *lexeme);

PTR_AAF aa_table[NUM_S] ={ NULL, NULL, aa_func02, aa_func03, NULL, aa_func05, NULL, NULL, aa_func08, NULL, NULL, aa_func11, aa_func13, aa_func13};

/* Keyword lookup table */
#define KWT_SIZE  8

char * kw_table []= {
	"ELSE",
	"IF",
	"INPUT",
	"OUTPUT",
	"PLATYPUS",
	"REPEAT",
	"THEN",
	"USING"   
};

#endif
