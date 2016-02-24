/* File Name: parser.c
* Compiler: MS Visual Studio 2012
* Author: Sean Macfarlane, 040-779-100
* Course: CST8152 - Compliers, Lab Section: 011
* Assignment: 4
* Date: 17 April 2015
* Professor: Sv. Ranev
* Purpose: Preprocessor directives, type declarations and prototypes necessary for transition table implementation 
* Function List: none		
*						 parser();
*						 match(); 
*						 syn_eh();
*						 syn_printe();
*						 gen_incode();
*						 program();
*						 opt_statements();
*						 statements();
*						 p_statements();
*						 statement();
*						 assignment_statement();
*						 assignment_expression();
*						 selection_statement();
*						 iteration_statement();
*						 input_statement();
*						 variable_list();
*						 p_variable_list();
*						 variable_identifier();
*						 output_statement();
*						 output_list();
*						 arithmetic_expression();
*						 unary_arithmetic_expression();
*						 additive_arithmetic_expression();
*						 p_additive_arithmetic_expression();
*						 multiplicative_arithmetic_expression();
*						 p_multiplicative_arithmetic_expression();
*						 primary_arithmetic_expression();
*						 string_expression();
*						 p_string_expression();
*						 primary_string_expression();
*						 conditional_expression();
*						 logical_or_expression();
*						 p_logical_or_expression();
*						 logical_and_expression();
*						 p_logical_and_expression();
*						 relational_expression();
*						 p_primary_a_relational_expression();
*						 p_primary_s_relational_expression();
*						 primary_a_relational_expression();
*						 primary_s_relational_expression();
*/

/* header files */
#include <stdlib.h>
#include "buffer.h"
#include "token.h"
#include "stable.h"
#include "parser.h"

/* External Function Declarations */
extern Token mlwpar_next_token(Buffer * sc_buf);
extern int line; /* source code line number - defined in scanner.c */
extern Buffer * str_LTBL; /* this buffer implements String Literal Table */
extern STD sym_table;    /* Symbol Table Descriptor */
extern char * kw_table [];	/* Token Key word Table */

/********************************************************************
Purpose:	Starts the Parser and parses the source file
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	mlwpar_next_token(), program(), match(), gen_incode()
Parameters:	Buffer*
Return Value: none
Algorithm:
**********************************************************************/	
void parser(Buffer * in_buf){
	sc_buf = in_buf;
	lookahead = mlwpar_next_token(sc_buf);
	program();
	match(SEOF_T, NO_ATTR);
	gen_incode("PLATY: Source file parsed\n");
}

/********************************************************************
Purpose:	Matches the current parser token with the next input token
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	mlwpar_next_token(), syn_eh(), syn_printe()
Parameters:	int (0-18), int (any value, 0 for no attribute)
Return Value: none
Algorithm:
**********************************************************************/	
void match(int pr_token_code,int pr_token_attribute){

	if(lookahead.code == pr_token_code){
		if(lookahead.code == SEOF_T)
			return;
		if((lookahead.code == KW_T && lookahead.attribute.kwt_idx != pr_token_attribute) || (lookahead.code == LOG_OP_T && lookahead.attribute.log_op != pr_token_attribute)
			|| (lookahead.code == ART_OP_T && lookahead.attribute.arr_op != pr_token_attribute) || (lookahead.code == REL_OP_T && lookahead.attribute.rel_op != pr_token_attribute)){
				syn_eh(pr_token_code);
				return;
		}
		lookahead = mlwpar_next_token(sc_buf); 
		if(lookahead.code == ERR_T){
			syn_printe();
			lookahead = mlwpar_next_token (sc_buf); 
			++synerrno;
		}
		return;
	}
	syn_eh(pr_token_code);
	return;
}

/********************************************************************
Purpose:	Syntax Error Handler
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	mlwpar_next_token(), syn_printe(), b_eob(), exit()
Parameters:	int (0-18)
Return Value: none
Algorithm:
**********************************************************************/	
void syn_eh(int sync_token_code){
	syn_printe();
	++synerrno;
	while(!b_eob(sc_buf)){
		lookahead = mlwpar_next_token (sc_buf); 
		if(lookahead.code == sync_token_code){
			if(lookahead.code != SEOF_T)
				lookahead = mlwpar_next_token (sc_buf); 
			return;
		}
	}
	if(sync_token_code != SEOF_T)
		exit(synerrno);
	return;
}

/********************************************************************
Purpose:	Prints Syntax Error
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	printf()
Parameters:	none
Return Value: none
Algorithm:
**********************************************************************/	
void syn_printe(void){
	Token t = lookahead;

	printf("PLATY: Syntax error:  Line:%3d\n",line);
	printf("*****  Token code:%3d Attribute: ", t.code);
	switch(t.code){
	case  ERR_T: /* ERR_T     0   Error token */
		printf("%s\n",t.attribute.err_lex);
		break;
	case  SEOF_T: /*SEOF_T    1   Source end-of-file token */
		printf("NA\n" );
		break;
	case  AVID_T: /* AVID_T    2   Arithmetic Variable identifier token */
	case  SVID_T :/* SVID_T    3  String Variable identifier token */
		printf("%s\n",sym_table.pstvr[t.attribute.get_int].plex);
		break;
	case  FPL_T: /* FPL_T     4  Floating point literal token */
		printf("%5.1f\n",t.attribute.flt_value);
		break;
	case INL_T: /* INL_T      5   Integer literal token */
		printf("%d\n",t.attribute.get_int);
		break;
	case STR_T:/* STR_T     6   String literal token */
		printf("%s\n",b_setmark(str_LTBL,t.attribute.str_offset));
		break;

	case SCC_OP_T: /* 7   String concatenation operator token */
		printf("NA\n" );
		break;

	case  ASS_OP_T:/* ASS_OP_T  8   Assignment operator token */
		printf("NA\n" );
		break;
	case  ART_OP_T:/* ART_OP_T  9   Arithmetic operator token */
		printf("%d\n",t.attribute.get_int);
		break;
	case  REL_OP_T: /*REL_OP_T  10   Relational operator token */ 
		printf("%d\n",t.attribute.get_int);
		break;
	case  LOG_OP_T:/*LOG_OP_T 11  Logical operator token */
		printf("%d\n",t.attribute.get_int);
		break;

	case  LPR_T: /*LPR_T    12  Left parenthesis token */
		printf("NA\n" );
		break;
	case  RPR_T: /*RPR_T    13  Right parenthesis token */
		printf("NA\n" );
		break;
	case LBR_T: /*    14   Left brace token */
		printf("NA\n" );
		break;
	case RBR_T: /*    15  Right brace token */
		printf("NA\n" );
		break;

	case KW_T: /*     16   Keyword token */
		printf("%s\n",kw_table[t.attribute.get_int]);
		break;

	case COM_T: /* 17   Comma token */
		printf("NA\n");
		break;
	case EOS_T: /*    18  End of statement *(semi - colon) */
		printf("NA\n" );
		break; 		
	default:
		printf("PLATY: Scanner error: invalid token code: %d\n", t.code);
	}/*end switch*/
}/* end syn_printe()*/

/********************************************************************
Purpose:	Prints which Production Parsed
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	printf()
Parameters:	char*
Return Value: none
Algorithm:
**********************************************************************/	
void gen_incode(char* string){
	printf("%s", string);
}

/********************************************************************
Purpose:	Parses the Program production
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	match(), gen_incode(), opt_statements()
Parameters:	none
Return Value: none
Algorithm:
FIRST(program) = { KW_T (PLATYPUS) }
**********************************************************************/	
void program(void){
	match(KW_T, PLATYPUS);
	match(LBR_T,NO_ATTR);
	opt_statements();
	match(RBR_T,NO_ATTR);
	gen_incode("PLATY: Program parsed\n");
}

/********************************************************************
Purpose:	Parses the opt_statements production
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	match(), gen_incode(), statements()
Parameters:	none
Return Value: none
Algorithm:
FIRST(opt_statements) = {AVID_T,SVID_T,KW_T(IF), KW_T(USING), KW_T(INPUT), KW_T(OUTPUT), e} 
**********************************************************************/
void opt_statements(void){
	switch(lookahead.code){
	case AVID_T:
	case SVID_T: statements();break;
	case KW_T:
		/* check for PLATYPUS, ELSE, THEN, REPEAT here and in
		statements_p()*/
		if (lookahead. attribute. get_int != PLATYPUS
			&& lookahead. attribute. get_int != ELSE
			&& lookahead. attribute. get_int != THEN
			&& lookahead. attribute. get_int != REPEAT){
				statements();
				break;
		}
	default: /*empty string – optional statements*/ ;
		gen_incode("PLATY: Opt_statements parsed\n");
	}
} 

/********************************************************************
Purpose:	Parses the statements production
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	statement(), p_statements()
Parameters:	none
Return Value: none
Algorithm:
FIRST(statements) = { AVID, SVID, IF, KW_T(IF), KW_T(USING), KW_T(INPUT), KW_T(OUTPUT) }
**********************************************************************/
void statements(void){
	statement(); p_statements(); 
}

/********************************************************************
Purpose:	Parses the p_statements production
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	statement(), p_statements()
Parameters:	none
Return Value: none
Algorithm:
FIRST(statements’) = { AVID, SVID, KW_T(IF), KW_T(USING), KW_T(INPUT), KW_T(OUTPUT), e } 
**********************************************************************/
void p_statements(void){
	switch(lookahead.code){
	case KW_T:
		if(lookahead.attribute.get_int == ELSE ||
			lookahead.attribute.get_int == PLATYPUS ||
			lookahead.attribute.get_int == REPEAT ||
			lookahead.attribute.get_int == THEN){
				return;
		}
	case AVID_T:
	case SVID_T: statement(); p_statements(); break;
	}
}

/********************************************************************
Purpose:	Parses the statement production
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	assignment_statement(), selection_statement(), iteration_statement(),
input_statement(), output_statement(), syn_printe()
Parameters:	none
Return Value: none
Algorithm:
FIRST(statements) = { AVID, SVID, KW_T(IF), KW_T(USING), KW_T(INPUT), KW_T(OUTPUT), e } 
**********************************************************************/
void statement(void){
	switch(lookahead.code){
	case AVID_T:
	case SVID_T: assignment_statement();break;
	case KW_T:
		switch(lookahead.attribute.get_int){
		case IF: selection_statement();break;
		case USING: iteration_statement();break;
		case INPUT: input_statement();break;
		case OUTPUT: output_statement();break;
		}
		break;
	default: syn_printe();
	}
}

/********************************************************************
Purpose:	Parses the assignment_statement production
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	assignment_expression(), match(), gen_incode()
Parameters:	none
Return Value: none
Algorithm:
FIRST(assignment statement) = { AVID, SVID } 
**********************************************************************/
void assignment_statement(void){
	assignment_expression();  match(EOS_T, NO_ATTR);
	gen_incode("PLATY: Assignment statement parsed\n");
}

/********************************************************************
Purpose:	Parses the assignment_expression production
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	arithmetic_expression(), string_expression(), match(), gen_incode()
Parameters:	none
Return Value: none
Algorithm:
FIRST(assignment expression) = { AVID, SVID } 
**********************************************************************/
void assignment_expression(void){
	switch(lookahead.code){
	case AVID_T: 
		match(AVID_T, NO_ATTR);
		match(ASS_OP_T, NO_ATTR);
		arithmetic_expression();
		gen_incode("PLATY: Assignment expression (arithmetic) parsed\n");
		break;
	case SVID_T:
		match(SVID_T, NO_ATTR);
		match(ASS_OP_T, NO_ATTR);
		string_expression();
		gen_incode("PLATY: Assignment expression (string) parsed\n");
		break;
	default:  syn_printe();
	}
}

/********************************************************************
Purpose:	Parses the selection_statement production
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	match(), conditional_expression(), opt_statements(), 
gen_incode()
Parameters:	none
Return Value: none
Algorithm:
FIRST(selection statement)= {KW_T(IF)}
**********************************************************************/
void selection_statement(void){
	match(KW_T, IF); match(LPR_T, NO_ATTR);
	conditional_expression(); match(RPR_T, NO_ATTR);
	match(KW_T, THEN);
	opt_statements();
	match(KW_T, ELSE); match(LBR_T, NO_ATTR);
	opt_statements();
	match(RBR_T, NO_ATTR); match(EOS_T, NO_ATTR);
	gen_incode("PLATY: IF statement parsed\n");
}

/********************************************************************
Purpose:	Parses the iteration_statement production
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	match(), assignment_expression(), conditional_expression(), 
opt_statements(),	en_incode()
Parameters:	none
Return Value: none
Algorithm:
FIRST(iteration statement)= {KW_T(USING)}
**********************************************************************/
void iteration_statement(void){
	match(KW_T, USING); match(LPR_T, NO_ATTR);
	assignment_expression(); match(COM_T, NO_ATTR);
	conditional_expression(); match(COM_T, NO_ATTR);
	assignment_expression(); match(RPR_T, NO_ATTR);
	match(KW_T, REPEAT); match(LBR_T, NO_ATTR); 
	opt_statements(); match(RBR_T, NO_ATTR); match(EOS_T, NO_ATTR);
	gen_incode("PLATY: USING statement parsed\n");
}

/********************************************************************
Purpose:	Parses the input_statement production
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	match(), variable_list(), gen_incode()
Parameters:	none
Return Value: none
Algorithm:
FIRST(input statement)= {KW_T(INPUT)}
**********************************************************************/
void input_statement(void){
	match(KW_T,INPUT);match(LPR_T,NO_ATTR);variable_list();
	match(RPR_T,NO_ATTR); match(EOS_T,NO_ATTR);
	gen_incode("PLATY: INPUT statement parsed\n");
} 

/********************************************************************
Purpose:	Parses the variable list production
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	variable_identifier(), p_variable_list(), gen_incode()
Parameters:	none
Return Value: none
Algorithm:
FIRST(variable list) = { AVID_T | SVID_T} 
**********************************************************************/
void variable_list(void){ 
	variable_identifier(); p_variable_list();
	gen_incode("PLATY: Variable list parsed\n");
}

/********************************************************************
Purpose:	Parses the prime variable list production
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	match(), variable_identifier(), p_variable_list(), 
Parameters:	none
Return Value: none
Algorithm:
FIRST(variable list’) = { , | e}
**********************************************************************/
void p_variable_list(void){
	if(lookahead.code == COM_T){
		match(COM_T, NO_ATTR);
		variable_identifier(); 
		p_variable_list();
	}
}

/********************************************************************
Purpose:	Parses the variable identifier production
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	match(), syn_printe()
Parameters:	none
Return Value: none
Algorithm:
FIRST(assignment statement) = { AVID, SVID } 
**********************************************************************/
void variable_identifier(void){
	switch(lookahead.code){
	case AVID_T: match(AVID_T, NO_ATTR); break;
	case SVID_T: match(SVID_T, NO_ATTR); break;
	default:  syn_printe();
	}
}

/********************************************************************
Purpose:	Parses the output statement production
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	match(), output_list(), gen_incode()
Parameters:	none
Return Value: none
Algorithm:
FIRST(output statement) = { KW_T(OUTPUT) }
**********************************************************************/
void output_statement(void){
	match(KW_T, OUTPUT); match(LPR_T, NO_ATTR);
	output_list(); match(RPR_T, NO_ATTR); match(EOS_T, NO_ATTR);
	gen_incode("PLATY: OUTPUT statement parsed\n");
}

/********************************************************************
Purpose:	Parses the output list production
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	match(), variable_list(), gen_incode()
Parameters:	none
Return Value: none
Algorithm:
FIRST(output list) = { AVID_T | SVID_T | STR_T | e } 
**********************************************************************/
void output_list(void){
	switch(lookahead.code){
	case AVID_T: 
	case SVID_T: variable_list(); break;
	case STR_T: match(STR_T, NO_ATTR); gen_incode("PLATY: Output list (string literal) parsed\n"); break;
	default: gen_incode("PLATY: Output list (empty) parsed\n");
	}
}

/********************************************************************
Purpose:	Parses the arithmetic expression production
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	unary_arithmetic_expression(), additive_arithmetic_expression()
syn_printe(), gen_incode()
Parameters:	none
Return Value: none
Algorithm:
FIRST(arithmetic exp) = { - , + ,  AVID_T, FPL_T, INL_T, ( }
**********************************************************************/
void arithmetic_expression(void){
	switch(lookahead.code){
	case ART_OP_T: 	unary_arithmetic_expression(); break;
	case AVID_T:
	case FPL_T:
	case INL_T:
	case LPR_T:
		additive_arithmetic_expression();
		break;
	default: syn_printe();
	}
	gen_incode("PLATY: Arithmetic expression parsed\n");
}

/********************************************************************
Purpose:	Parses the unary arithmetic expression production
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	match(), primary_arithmetic_expression(), syn_printe(),
gen_incode()
Parameters:	none
Return Value: none
Algorithm:
FIRST(unary arithm exp) = { - , + }
**********************************************************************/
void unary_arithmetic_expression(void){
	if(lookahead.attribute.arr_op == 0 || lookahead.attribute.arr_op == 1){
		match(ART_OP_T, lookahead.attribute.arr_op);
		primary_arithmetic_expression();
	}
	else
		syn_printe();
	gen_incode("PLATY: Unary arithmetic expression parsed\n");
}

/********************************************************************
Purpose:	Parses the additive arithmetic expression production
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	multiplicative_arithmetic_expression(), 
p_additive_arithmetic_expression()
Parameters:	none
Return Value: none
Algorithm:
FIRST(additive arith exp) = { AVID_T, FPL_T, INL_T, ( } 
**********************************************************************/
void additive_arithmetic_expression(void){
	multiplicative_arithmetic_expression();
	p_additive_arithmetic_expression();
}

/********************************************************************
Purpose:	Parses the prime additive arithmetic expression production
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	match(), multiplicative_arithmetic_expression(), 
p_additive_arithmetic_expression(), gen_incode()
Parameters:	none
Return Value: none
Algorithm:
FIRST(additive arith exp’) = { +, -, e }
**********************************************************************/
void p_additive_arithmetic_expression(void){
	if(lookahead.code == ART_OP_T){
		if(lookahead.attribute.arr_op == PLUS || lookahead.attribute.arr_op == MINUS){
			match(ART_OP_T, lookahead.attribute.arr_op);
			multiplicative_arithmetic_expression();
			p_additive_arithmetic_expression();
			gen_incode("PLATY: Additive arithmetic expression parsed\n");
		}
	}
}

/********************************************************************
Purpose:	Parses the multiplicative arithmetic expression production
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	primary_arithmetic_expression(), 
p_multiplicative_arithmetic_expression(), 
Parameters:	none
Return Value: none
Algorithm:
FIRST(multiplicative arith exp) = { AVID_T, FPL_T, INL_T, ( } 
**********************************************************************/
void multiplicative_arithmetic_expression(void){
	primary_arithmetic_expression();
	p_multiplicative_arithmetic_expression();
}

/********************************************************************
Purpose:	Parses the prime multiplicative arithmetic expression production
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	match(), primary_arithmetic_expression(), 
p_multiplicative_arithmetic_expression(), gen_incode()
Parameters:	none
Return Value: none
Algorithm:
FIRST(multiplicative arith exp’) = { *, /, e }
**********************************************************************/
void p_multiplicative_arithmetic_expression(void){
	if(lookahead.code == ART_OP_T){
		if(lookahead.attribute.arr_op == MULT || lookahead.attribute.arr_op == DIV){
			match(ART_OP_T, lookahead.attribute.arr_op);
			primary_arithmetic_expression();
			p_multiplicative_arithmetic_expression();
			gen_incode("PLATY: Multiplicative arithmetic expression parsed\n");
		}
	}
}

/********************************************************************
Purpose:	Parses the primary arithmetic expression production
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	match(), arithmetic_expression(), 
syn_printe(), gen_incode()
Parameters:	none
Return Value: none
Algorithm:
FIRST(primary arith exp) = { AVID_T, FPL_T, INL_T, ( }
**********************************************************************/
void primary_arithmetic_expression(void){
	switch(lookahead.code){
	case AVID_T: 
	case FPL_T: 
	case INL_T:
		match(lookahead.code, NO_ATTR); 
		break;
	case LPR_T:
		match(LPR_T, NO_ATTR);
		arithmetic_expression();
		match(RPR_T, NO_ATTR);
		break;
	default: syn_printe();
	}
	gen_incode("PLATY: Primary arithmetic expression parsed\n");
}

/********************************************************************
Purpose:	Parses the string expression production
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	primary_string_expression(), p_string_expression(),
gen_incode()
Parameters:	none
Return Value: none
Algorithm:
FIRST(string expression)  = { SVID_T | STR_T }
**********************************************************************/
void string_expression(void){
	primary_string_expression();
	p_string_expression();
	gen_incode("PLATY: String expression parsed\n");
}

/********************************************************************
Purpose:	Parses the prime string expression production
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	match(), primary_string_expression(), p_string_expression(),
Parameters:	none
Return Value: none
Algorithm:
FIRST(string expression’)  = {<< , e } 
**********************************************************************/
void p_string_expression(void){
	if(lookahead.code == SCC_OP_T){
		match(SCC_OP_T, NO_ATTR);
		primary_string_expression();
		p_string_expression();
	}
}

/********************************************************************
Purpose:	Parses the primary string expression production
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	match(), gen_incode()
Parameters:	none
Return Value: none
Algorithm:
**********************************************************************/
void primary_string_expression(void){
	match(lookahead.code, NO_ATTR); 
	gen_incode("PLATY: Primary string expression parsed\n");
}

/********************************************************************
Purpose:	Parses the conditional expression production
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	logical_or_expression(), gen_incode()
Parameters:	none
Return Value: none
Algorithm:
FIRST(conditional expression) = { AVID_T, FPL_T, INL_T , SVID_T, STR_T }
**********************************************************************/
void conditional_expression(void){
	logical_or_expression();
	gen_incode("PLATY: Conditional expression parsed\n");
}

/********************************************************************
Purpose:	Parses the logical or expression production
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	logical_and_expression(), p_logical_or_expression()
Parameters:	none
Return Value: none
Algorithm:
FIRST(logical OR expression) = { AVID_T, FPL_T, INL_T , SVID_T, STR_T }
**********************************************************************/
void logical_or_expression(void){
	logical_and_expression();
	p_logical_or_expression();
}

/********************************************************************
Purpose:	Parses the prime logical or expression production
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	match(), logical_and_expression(), p_logical_or_expression(),
gen_incode()
Parameters:	none
Return Value: none
Algorithm:
FIRST(logical OR expression’) = { LOG_T(.OR.), e }
**********************************************************************/
void p_logical_or_expression(void){
	if(lookahead.code == LOG_OP_T && lookahead.attribute.log_op == OR){
		match(LOG_OP_T, OR);
		logical_and_expression();
		p_logical_or_expression();
		gen_incode("PLATY: Logical OR expression parsed\n");
	}
}

/********************************************************************
Purpose:	Parses the logical and expression production
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	relational_expression(), p_logical_and_expression()
Parameters:	none
Return Value: none
Algorithm:
FIRST(logical AND expression) = { AVID_T, FPL_T, INL_T , SVID_T, STR_T }
**********************************************************************/
void logical_and_expression(void){
	relational_expression();
	p_logical_and_expression();
}

/********************************************************************
Purpose:	Parses the prime logical and expression production
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	match(), relational_expression(), 
p_logical_and_expression(), gen_incode()
Parameters:	none
Return Value: none
Algorithm:
FIRST(logical AND expression’) = {LOG_T(.AND.), e }
**********************************************************************/
void p_logical_and_expression(void){
	if(lookahead.code == LOG_OP_T && lookahead.attribute.log_op == AND){
		match(LOG_OP_T, AND);
		relational_expression();
		p_logical_and_expression();
		gen_incode("PLATY: Logical AND expression parsed\n");
	}
}

/********************************************************************
Purpose:	Parses the relational expression production
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	primary_a_relational_expression(), gen_incode(),
p_primary_a_relational_expression(), syn_printe()
Parameters:	none
Return Value: none
Algorithm:
FIRST(relational expression) = { AVID_T, FPL_T, INL_T , SVID_T, STR_T} 
**********************************************************************/
void relational_expression(void){
	switch(lookahead.code){
	case AVID_T:
	case FPL_T:
	case INL_T: 
		primary_a_relational_expression(); 
		p_primary_a_relational_expression(); 
		break;
	case SVID_T:
	case STR_T: 
		primary_s_relational_expression(); 
		p_primary_s_relational_expression();
		break;
	default: syn_printe();
	}
	gen_incode("PLATY: Relational expression parsed\n");
}

/********************************************************************
Purpose:	Parses the p_primary_a_relational_expression production
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	match(), primary_a_relational_expression(),
syn_printe()
Parameters:	none
Return Value: none
Algorithm:
FIRST(primary a_relational exp’) = { ==, <>, >, <}
**********************************************************************/
void p_primary_a_relational_expression(void){
	if(lookahead.code == REL_OP_T){
		if(lookahead.attribute.rel_op == EQ || lookahead.attribute.rel_op == NE || lookahead.attribute.rel_op == LT || lookahead.attribute.rel_op == GT){
			match(REL_OP_T, lookahead.attribute.rel_op); 
			primary_a_relational_expression();
			return;
		}
	}
	syn_printe();
}

/********************************************************************
Purpose:	Parses the p_primary_s_relational_expression production
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	match(), primary_s_relational_expression(),
syn_printe()
Parameters:	none
Return Value: none
Algorithm:
FIRST(primary s_relational exp’) = { ==, <>, >, <}
**********************************************************************/
void p_primary_s_relational_expression(void){
	if(lookahead.code == REL_OP_T){
		if(lookahead.attribute.rel_op == EQ || lookahead.attribute.rel_op == NE || lookahead.attribute.rel_op == LT || lookahead.attribute.rel_op == GT){
			match(REL_OP_T, lookahead.attribute.rel_op); 
			primary_s_relational_expression();
			return;
		}
	}
	syn_printe();
}

/********************************************************************
Purpose:	Parses the primary_a_relational_expression production
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	match(), gen_incode()
Parameters:	none
Return Value: none
Algorithm:
**********************************************************************/
void primary_a_relational_expression(void){
	if(lookahead.code == AVID_T || lookahead.code == FPL_T || lookahead.code == INL_T){
		match(lookahead.code, NO_ATTR);
	}
	else 
		syn_printe();
	gen_incode("PLATY: Primary a_relational expression parsed\n"); 
}

/********************************************************************
Purpose:	Parses the primary_s_relational_expression production
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions:	primary_string_expression(), gen_incode()
Parameters:	none
Return Value: none
Algorithm:
FIRST(primary string expression)  = { SVID_T | STR_T }
**********************************************************************/
void primary_s_relational_expression(void){
	primary_string_expression();
	gen_incode("PLATY: Primary s_relational expression parsed\n");
}

