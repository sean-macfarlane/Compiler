/* File Name: parser.h
* Compiler: MS Visual Studio 2012
* Author: Sean Macfarlane, 040-779-100
* Course: CST8152 - Compliers, Lab Section: 011
* Assignment: 4
* Date: 17 April 2015
* Professor: Sv. Ranev
* Purpose: Preprocessor directives, type declarations and prototypes necessary for transition table implementation 					
* Function List: none
*/

#ifndef PARSE
#define PARSE

#define NO_ATTR 0	// No Attribute for match()
/* Key Word Indexes */
#define ELSE 0
#define IF 1
#define INPUT 2
#define OUTPUT 3
#define PLATYPUS 4
#define REPEAT 5
#define THEN 6
#define USING 7

static Token lookahead;		// Next input Token
static Buffer* sc_buf;		// Pointer to Scanner Buffer
int synerrno;				// Number of Syntax Errors	

/* Function Declarations */
void parser(Buffer * in_buf);
void match(int pr_token_code,int pr_token_attribute); 
void syn_eh(int sync_token_code);
void syn_printe(void);
void gen_incode(char* string);

void program(void);
void opt_statements(void);
void statements(void);
void p_statements(void);
void statement(void);
void assignment_statement(void);
void assignment_expression(void);
void selection_statement(void);
void iteration_statement(void);
void input_statement(void);
void variable_list(void);
void p_variable_list(void);
void variable_identifier(void);
void output_statement(void);
void output_list(void);
void arithmetic_expression(void);
void unary_arithmetic_expression(void);
void additive_arithmetic_expression(void);
void p_additive_arithmetic_expression(void);
void multiplicative_arithmetic_expression(void);
void p_multiplicative_arithmetic_expression(void);
void primary_arithmetic_expression(void);
void string_expression(void);
void p_string_expression(void);
void primary_string_expression(void);
void conditional_expression(void);
void logical_or_expression(void);
void p_logical_or_expression(void);
void logical_and_expression(void);
void p_logical_and_expression(void);
void relational_expression(void);
void p_primary_a_relational_expression(void);
void p_primary_s_relational_expression(void);
void primary_a_relational_expression(void);
void primary_s_relational_expression(void);

#endif
