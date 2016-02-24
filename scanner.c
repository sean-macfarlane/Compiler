/* File Name: scanner.c
* Compiler: MS Visual Studio 2012
* Author: Sean Macfarlane, 040-779-100
* Course: CST8152 - Compliers, Lab Section: 011
* Assignment: 4
* Date: 17 April 2015
* Professor: Sv. Ranev
* Purpose: Functions implementing a Lexical Analyzer (Scanner)
* Function List: 
*					init_scanner();
*					mlwpar_next_token();
*					get_next_state();
*					char_class();
*					aa_func02();
*					aa_func03();
*					aa_func08();
*					aa_func05();
*					aa_func11();
*					aa_func13();
*					iskeyword();
*					atool();
*					runTimeError();
*/

/* The #define _CRT_SECURE_NO_WARNINGS should be used in MS Visual Studio projects
* to suppress the warnings about using "unsafe" functions like fopen()
* and standard sting library functions defined in string.h.
* The define does not have any effect in Borland compiler projects.
*/
#define _CRT_SECURE_NO_WARNINGS

/* standard header files */
#include <stdio.h>   /* standard input / output */
#include <ctype.h>   /* conversion functions */
#include <stdlib.h>  /* standard library functions and constants */
#include <string.h>  /* string functions */
#include <limits.h>  /* integer types constants */
#include <float.h>   /* floating-point types constants */
#include <math.h>	/* math functions */

/*#define NDEBUG        to suppress assert() call */
#include <assert.h>  /* assert() prototype */

/* project header files */
#include "buffer.h"
#include "token.h"
#include "table.h"
#include "stable.h"

#define DEBUG  /* for conditional processing */
#undef  DEBUG

/* Global objects - variables */
/* This buffer is used as a repository for string literals.
It is defined in platy_st.c */
extern Buffer * str_LTBL; /*String literal table */
int line; /* current line number of the source code */
extern int scerrnum;     /* defined in platy_st.c - run-time error number */
extern STD sym_table;

/* Local(file) global objects - variables */
static Buffer *lex_buf;/*pointer to temporary lexeme buffer*/

/* No other global variable declarations/definitiond are allowed */

/* scanner.c static(local) function  prototypes */ 
static int char_class(char c); /* character class function */
static int get_next_state(int, char, int *); /* state machine function */
static int iskeyword(char * kw_lexeme); /*keywords lookup functuion */
static long atool(char * lexeme); /* converts octal string to decimal value */

Token runTimeError();	/* Run Time Error Token function prototype */

/********************************************************************
Purpose:	Initializes Scanner
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions: b_isempty(), b_setmark(), b_retract_to_mark(), b_reset()
Parameters:	Buffer*
Return Value: int
Algorithm:
**********************************************************************/	
int scanner_init(Buffer * sc_buf) {
	if(b_isempty(sc_buf)) return EXIT_FAILURE;/*1*/
	/* in case the buffer has been read previously  */
	b_setmark(sc_buf, 0);
	b_retract_to_mark(sc_buf);
	b_reset(str_LTBL);
	line = 1;
	return EXIT_SUCCESS;/*0*/
	/*   scerrnum = 0;  *//*no need - global ANSI C */
}

/********************************************************************
Purpose:	Reads lexeme from input buffer and returns matching token
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions: b_addc(), b_setmark(), b_retract_to_mark(), b_size(), 
b_getc(), b_retract(), b_getc_offset(), b_mark(), b_create(),
b_destroy(), get_next_state(), , aa_func02(), aa_func03(), 
aa_func05(), aa_func08(), aa_func11(), aa_func13(),
runTimeError(), isalpha(), isdigit()
Parameters:	Buffer*
Return Value: Token
Algorithm:	token driven processing
tranistion table driven processing
**********************************************************************/	
Token mlwpar_next_token(Buffer * sc_buf)
{
	Token t; /* token to return after recognition */
	unsigned char c; /* input symbol */
	int state = 0; /* initial state of the FSM */
	short lexstart;  /*start offset of a lexeme in the input buffer */
	short lexend;    /*end   offset of a lexeme in the input buffer */
	int accept = NOAS; /* type of state - initially not accepting */                                     

	short lex_capacity;
	char tempchar;  /* To temporarily store a charactor */
	int ctr = 0;	/* Counter */

	while (1){ /* endless loop broken by token returns it will generate a warning */

		c = b_getc(sc_buf);

		/* special cases or token driven processing */
		if(c == ' ' || c == '\t'){continue;}	/* if a space, new line, or tab */
		if(c == '\n'){
			line++;
			continue;
		}		
		if(c == '{'){ /* check if Left Brace Character */
			t.code = LBR_T; /*no attribute */
			return t; 
		}
		if(c == '='){ /* check if Assignment operator*/
			tempchar = b_getc(sc_buf);
			if(tempchar == '='){ /* check if Equal To operator*/
				t.code = REL_OP_T;
				t.attribute.rel_op = EQ;
				return t;
			}
			b_retract(sc_buf);
			t.code = ASS_OP_T;
			return t;
		}
		if(c == '}'){ /* check if Right Brace Character*/
			t.code = RBR_T; /*no attribute */
			return t; 
		}
		if(c == '+'){ /* check if Addition Operator */
			t.code = ART_OP_T;
			t.attribute.arr_op = PLUS; 
			return t; 
		}
		if(c == '-'){ /* check if Subtraction Operator */
			t.code = ART_OP_T;
			t.attribute.arr_op = MINUS; 
			return t; 
		}
		if(c == '*'){ /* check if Multiplication Operator */
			t.code = ART_OP_T;
			t.attribute.arr_op = MULT; 
			return t; 
		}
		if(c == '/'){ /* check if Division Operator */
			t.code = ART_OP_T;
			t.attribute.arr_op = DIV; 
			return t; 
		}
		if(c == '('){ /* check if Left Parenthesis */
			t.code = LPR_T; /*no attribute */
			return t;
		}
		if(c == ')'){ /* check if Right Parenthesis */
			t.code = RPR_T; /*no attribute */
			return t;
		}
		if(c == '>'){ /* check if Greater Than Operator */
			t.code = REL_OP_T;
			t.attribute.rel_op = GT;
			return t;
		}
		if(c == '<'){ /* check if Less Than Operator */
			tempchar = b_getc(sc_buf);
			if(tempchar == '>'){ /* check if Not Equal to Operator */
				t.code = REL_OP_T;
				t.attribute.rel_op = NE;
				return t;
			}
			else if(tempchar == '<'){ /* check if Concatention Operator */
				t.code = SCC_OP_T; /*no attribute */
				return t;
			}
			b_retract(sc_buf);
			t.code = REL_OP_T;
			t.attribute.rel_op = LT;
			return t;

		}
		if(c == ','){ /* check if Comma Character */
			t.code = COM_T; /*no attribute */
			return t;

		}
		if(c == ';'){ /* check if Semi-Colon Character */
			t.code = EOS_T; /*no attribute */	
			return t;
		}
		if(c == '\0' || c == SEOF){ /* check if End Of File Character */
			t.code = SEOF_T; /*no attribute */
			return t;
		}
		if(c == '.'){ /* check if Period Character */
			b_setmark(sc_buf, b_getc_offset(sc_buf));
			tempchar = b_getc(sc_buf);

			if(tempchar != 'A' && tempchar!='O'){
				b_retract(sc_buf);
				t.code = ERR_T;
				t.attribute.err_lex[0] = c;
				t.attribute.err_lex[ARIX_1] = '\0';
				return t;
			}
			else if(tempchar == 'A' &&  b_getc(sc_buf) == 'N' && b_getc(sc_buf) == 'D' &&  b_getc(sc_buf) == '.'){ /* check if AND operator */
				t.code = LOG_OP_T;
				t.attribute.log_op = AND;
				return t;
			}
			b_retract_to_mark(sc_buf);
			tempchar = b_getc(sc_buf);

			if(tempchar == 'O' &&  b_getc(sc_buf) == 'R' && b_getc(sc_buf) == '.'){ /* check if OR operator */
				t.code = LOG_OP_T;
				t.attribute.log_op = OR;
				return t;
			}
			b_retract_to_mark(sc_buf);
			t.code = ERR_T;
			t.attribute.err_lex[0] = c;
			t.attribute.err_lex[ARIX_1] = '\0';
			return t;
		}
		if(c == '!'){ /* check if legal Comment */
			tempchar = b_getc(sc_buf);

			if(tempchar != '<'){ /* check if illegal Comment */
				t.code = ERR_T;
				t.attribute.err_lex[0] = '!';
				t.attribute.err_lex[ARIX_1] = tempchar;
				t.attribute.err_lex[ARIX_2] = '\0';
				while(tempchar != '\n'){
					tempchar = b_getc(sc_buf);
				}
				line++;
				return t;
			}
			else{	/* Legal Comment */
				while(tempchar != '\n'){
					tempchar = b_getc(sc_buf);
				}
				line++;
				continue;
			}
		}
		if(c == '"'){ /* check if legal String Literal */
			b_setmark(sc_buf, b_getc_offset(sc_buf));
			tempchar = b_getc(sc_buf);

			while(tempchar != '"'){ /* checks if string is legal */
				if(tempchar == '\0' || tempchar == SEOF || tempchar == EOF){ /* if end of file is reached its an illegal String */
					t.code = ERR_T;
					b_retract_to_mark(sc_buf);
					t.attribute.err_lex[ctr] = c;
					ctr++;
					while(ctr<ERR_LEN){ /* store String in error attribute */
						tempchar = b_getc(sc_buf);

						if(tempchar == '\0' || tempchar == SEOF || tempchar == EOF)
							break;
						if(ctr < ERR_LEN_17){
							t.attribute.err_lex[ctr] = tempchar;
						}
						else{
							t.attribute.err_lex[ctr] = '.';
						}
						ctr++;
					}
					t.attribute.err_lex[ctr] = '\0';
					while(tempchar != '\0'){
						if(tempchar == EOF)
							break;
						tempchar = b_getc(sc_buf);
					}
					b_retract(sc_buf);
					return t;
				}
				if(tempchar == '\n')
					++line;
				tempchar = b_getc(sc_buf);
			}
			/* If closing " is found it is legal */
			b_retract_to_mark(sc_buf);
			t.code = STR_T; 
			t.attribute.str_offset = b_size(str_LTBL);
			t.attribute.get_int = t.attribute.str_offset;
			tempchar = b_getc(sc_buf);
			/* add String to String Literal Buffer */
			while( tempchar != '"'){
				if(b_addc(str_LTBL, tempchar) == NULL){
					return runTimeError();
				}
				tempchar = b_getc(sc_buf);
			}
			if(b_addc(str_LTBL, '\0')==NULL){
				return runTimeError();
			}
			return t;
		}

		/* Process state transition table */   
		if(isdigit(c) || isalpha(c)){ /* If a digit or letter */
			b_setmark(sc_buf, b_getc_offset(sc_buf)-1);	

			while(1){	/* Loop until final state found */
				state = get_next_state(state, c, &accept);
				if(accept!=NOAS)
					break;
				c= b_getc(sc_buf);	
			}
			/* Temporary lexeme buffer create for use when finding matching Token */
			lexstart = b_mark(sc_buf);
			if(accept == ASWR)	/* If state is a Retracting state or if the next lexeme is end of file */
				b_retract(sc_buf);
			lexend = b_getc_offset(sc_buf);
			b_retract_to_mark(sc_buf);
			lex_capacity = lexend - lexstart+1;
			lex_buf = b_create(lex_capacity,0,'f');
			if(lex_buf==NULL)	/* checks for run time error */
				return runTimeError();

			while(lexstart!=lexend){	/* add the characters from input buffer to lexeme buffer */
				if(b_addc(lex_buf, b_getc(sc_buf))==NULL) /* checks for run time error */
					return runTimeError();
				lexstart++;
			}
			t = aa_table[state](b_setmark(lex_buf, 0));	/* gets matching Token */
			b_destroy(lex_buf);		/* destroy lexeme buffer */
			return t;
		}
		else {	/* if lexeme can not be matched to a Token return as Error Token */
			t.code = ERR_T;
			t.attribute.err_lex[0] = c;
			t.attribute.err_lex[ARIX_1] = '\0';
			return t;
		} 
	}//end while(1)
}// end mlwpar_next_token()

/********************************************************************
Purpose:	Returns the next state of character
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions: char_class(), assert()
Parameters:	int, char, int
Return Value: int
Algorithm:	
**********************************************************************/	
int get_next_state(int state, char c, int *accept)
{
	int col;	/* Column Number */
	int next;	/* Next State Number */
	col = char_class(c); /* gets Class number */
	next = st_table[state][col]; /* gets next State number */
#ifdef DEBUG
	printf("Input symbol: %c Row: %d Column: %d Next: %d \n",c,state,col,next);
#endif
	assert(next != IS); /*Asserts next state is not Illegal State */
#ifdef DEBUG
	if(next == IS){
		printf("Scanner Error: Illegal state:\n");
		printf("Input symbol: %c Row: %d Column: %d\n",c,state,col);
		exit(1);
	}
#endif
	*accept = as_table[next]; /* Stores Accepting State Number */
	return next;
}

/********************************************************************
Purpose:	Returns the matching transition table class of character
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions: isalpha(), isdigit()
Parameters:	char
Return Value: int
Algorithm:	
**********************************************************************/	
int char_class (char c)
{
	int val;	/* stores Class number */

	if(isalpha(c))	/*checks if alphabetic character class */
		val = CLASS_0;
	else if(c == '0')	 /* checks if Zero class */
		val = CLASS_1; 
	else if(isdigit(c) && c <= '7')		/* checks if non-zero Digit class */
		val = CLASS_2;
	else if(isdigit(c) && c > '7')
		val = CLASS_3;
	else if(c == '.')		/* checks if period character class */
		val = CLASS_4;
	else if(c == '#')		/* checks if # character class */
		val = CLASS_5;
	else					/* otherwise other class */
		val = CLASS_6;     
	return val;
}


/********************************************************************
Purpose:	Returns the matching Token from State 2
Author:		Sean Macfarlane 
History/Version:	2.0
Called Functions:	iskeyword(), malloc(), st_install(), b_destroy(),
st_store(), exit(), free(), printf(),
Parameters:	char[]
Return Value: Token
Algorithm:	
**********************************************************************/	
Token aa_func02(char lexeme[]){
	Token t;	/* Stores the Token to be returned */
	int ctr = 0;	/* Counter */
	char *temp;		/* Temporary Lexeme */

	if(iskeyword(lexeme)>=0){	/* Checks if it's a keyword */
		t.code = KW_T;
		t.attribute.kwt_idx = iskeyword(lexeme);
		return t;
	}
	t.code = AVID_T;
	if(b_size(lex_buf)>VID_LEN){ /* if lexeme is longer than attribute length */
		temp = (char*)malloc(VID_LEN+ADD_ONE);
		for(ctr = 0; ctr<VID_LEN; ctr++)
			temp[ctr] = lexeme[ctr];
		temp[VID_LEN] = '\0'; /* adds \0 to make legal C-String */
		if(temp[0] == 'i' || temp[0] == 'n' || temp[0] == 'o' || temp[0] == 'd')
			t.attribute.vid_offset = st_install(sym_table, temp, 'I', line);
		else
			t.attribute.vid_offset = st_install(sym_table, temp, 'F', line);
		free(temp);

	}
	else{
		lexeme[b_size(lex_buf)] = '\0';
		if(lexeme[0] == 'i' || lexeme[0] == 'n' || lexeme[0] == 'o' || lexeme[0] == 'd')
			t.attribute.vid_offset = st_install(sym_table, lexeme, 'I', line);
		else
			t.attribute.vid_offset = st_install(sym_table, lexeme, 'F', line);
	}
	if(t.attribute.vid_offset == R_FAIL_1){		/* If Symbol Table is full */
		b_destroy(lex_buf);
		printf("\nError: The Symbol Table is full - install failed.\n");
		st_store(sym_table);
		exit(EXIT_FAILURE);
	}
	return t;
}

/********************************************************************
Purpose:	Returns the matching Token from State 3
Author:		Sean Macfarlane 
History/Version:	2.0
Called Functions:	b_size(), malloc(), st_install(), b_destroy(), printf(),
st_store(), exit(), free()
Parameters:	char[]
Return Value: Token
Algorithm:	
**********************************************************************/	
Token aa_func03(char lexeme[]){
	Token t;		/* Stores Token to be returned */
	int ctr = 0;	/* Counter */
	char* temp;		/* Temporary Lexeme */

	t.code = SVID_T;
	if(b_size(lex_buf)>VID_LEN-ARITH_ONE){ /* if lexeme is longer than attribute length */
		temp = (char*)malloc(VID_LEN+ADD_ONE);
		for(ctr = 0; ctr<VID_LEN-ARITH_ONE; ctr++)
			temp[ctr] = lexeme[ctr];
		temp[VID_LEN-ARITH_ONE] = '#'; /* adds # to make legal SVID */
		temp[VID_LEN] = '\0'; /* adds \0 to make legal C-String */
		t.attribute.vid_offset = st_install(sym_table, temp, 'S', line);
		free(temp);
	}
	else{
		lexeme[b_size(lex_buf)] = '\0';
		t.attribute.vid_offset = st_install(sym_table, lexeme, 'S', line);
	}
	if(t.attribute.vid_offset == R_FAIL_1){ /* If Symbol Table is full */
		b_destroy(lex_buf);
		printf("\nError: The Symbol Table is full - install failed.\n");
		st_store(sym_table);
		exit(EXIT_FAILURE);
	}
	return t;
}


/********************************************************************
Purpose:	Returns the matching Token from State 8
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions: b_size(), atof()
Parameters:	char[]
Return Value: Token
Algorithm:	
**********************************************************************/	
Token aa_func08(char lexeme[]){
	Token t;
	double f;

	f = atof(lexeme);	/* Converts lexeme String to double */
	if(f > FLT_MAX || (f > 0 && f < FLT_MIN)){ /* Checks if legal Floating Point Literal */
		t = aa_table[ES](lexeme);
		return t;
	}
	/* Floating Point Literal Token */
	t.code = FPL_T;
	t.attribute.flt_value = (float)f;

	return t;
}

/********************************************************************
Purpose:	Returns the matching Token from State 5
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions: b_size(), atoi()
Parameters:	char[]
Return Value: Token
Algorithm:	
**********************************************************************/	
Token aa_func05(char lexeme[]){
	Token t;		/* Stores Token to be returned */
	long d;			/* Stores decimal integer literal */

	if(b_size(lex_buf)>INL_LEN){
		t = aa_table[ES](lexeme);
		return t;
	}
	d = atol(lexeme);		/* Converts lexeme String to Integer */
	if(d > S_MAX || d < 0){	/* Checks if legal Decimal Integer Literal */
		t = aa_table[ES](lexeme);
		return t;
	}
	/* Decimal Integer Literal Token */
	t.code = INL_T;
	t.attribute.int_value = d;

	return t;
}

/********************************************************************
Purpose:	Returns the matching Token from State 11
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions: b_size(), atool()
Parameters:	char[]
Return Value: Token
Algorithm:	
**********************************************************************/	
Token aa_func11(char lexeme[]){
	Token t;		/* Stores Token to be returned */
	long o;			/* Stores Octal Integer Literal */

	if(b_size(lex_buf)>INL_LEN+1){
		t = aa_table[ES](lexeme);
		return t;
	}
	o = atool(lexeme); /* Converts lexeme String to Octal Integer Literal */	

	if(o > S_MAX || o < 0){	/* Checks if legal Octal Integer Literal */
		t = aa_table[ES](lexeme);
		return t;
	}
	/* Octal Integer Literal Token */
	t.code = INL_T;
	t.attribute.int_value = o;

	return t;
}

/********************************************************************
Purpose:	Returns the matching Token from State 13
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions: b_size()
Parameters:	char[]
Return Value: Token
Algorithm:	
**********************************************************************/	
Token aa_func13(char lexeme[]){
	Token t;		/* Stores Token to be returned */
	int ctr = 0;	/* Counter */

	t.code = ERR_T;
	if(b_size(lex_buf)>ERR_LEN){	/* If lexeme is longer than error attribute max */
		for(ctr = 0; ctr<ERR_LEN; ctr++)
			t.attribute.err_lex[ctr] = lexeme[ctr];
	}
	else{
		for(ctr = 0; ctr< b_size(lex_buf); ctr++)
			t.attribute.err_lex[ctr] = lexeme[ctr];
		t.attribute.err_lex[b_size(lex_buf)] = '\0';
	}
	return t;
}

/********************************************************************
Purpose:	Converts an ASCII String representing an Octal Integer
Constant to Integer Value
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions: atol(), pow()
Parameters:	char*
Return Value: long
Algorithm:	
**********************************************************************/	
static long atool(char * lexeme){
	long octal;		/* Stores Octal Integer */
	long decimal = 0;	/* Stores Decimal Integer */
	int i = 0;		/* Counter */

	octal = atol(lexeme);	/* Converts String lexeme to Octal Integer*/
	while(octal!=0){	/* Converts Octal to Decimal Integer */
		decimal = decimal + (octal % BASE_10) * (long)pow(BASE_8, i++);
		octal = octal/BASE_10;
	}
	return decimal;
}

/********************************************************************
Purpose:	Checks if the lexeme is a Keyword
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions: strlen()
Parameters:	char*
Return Value: int
Algorithm:	
**********************************************************************/	
int iskeyword(char * kw_lexeme){
	int ctr = 0;	/* Counter */
	int ctr2 = 0;	/* Counter 2 */
	int kwFlag = 0;	/* Keyword Flag */

	for(ctr = 0; ctr<KWT_SIZE; ctr++){	/* Loops through each Keyword */
		for(ctr2 = 0; (unsigned)ctr2<strlen(kw_table[ctr]); ctr2++){
			/* Loops through each character to see if they match */
			if(kw_table[ctr][ctr2]!=kw_lexeme[ctr2]){ 
				kwFlag = 0;
				break;
			}
			else /* The two characters match */
				kwFlag = 1;
		}
		if(kwFlag == 1) /* Is a Keyword */
			return ctr;
	}
	return R_FAIL_1;	/* if not a Keyword */
}

/********************************************************************
Purpose:	Returns a Run Time Error Token
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions: strcpy()
Parameters:	
Return Value: Token
Algorithm:	
**********************************************************************/	
Token runTimeError(){
	Token t;	/* Stores Token to be returned */
	scerrnum++; /* increment run time error number */
	t.code = ERR_T;
	strcpy(t.attribute.err_lex, "RUN TIME ERROR");
	return t;
}

