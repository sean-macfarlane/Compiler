/* Filename: stable.h
 * Declarations necessary for the Symbol Table implementation 
 * CST8152, Assignment #4
 * Version: 1.15.01
 * Date: 17 April 2015
 * Provided by: Svillen Ranev
 * The file is complete and MUST NOT be modified. 
 */

#ifndef STABLE
#define STABLE

#define ST_FAIL -1
#define DEFAULT 0xFFF8  /* 1111 1111 1111 1000 */
#define FLOAT	0x0002	/* 0000 0000 0000 0010 */
#define CONVERT	0x0006	/* 0000 0000 0000 0110 */
#define INT		0x0004	/* 0000 0000 0000 0100 */
#define STRING	0x0006	/* 0000 0000 0000 0110 */
#define UPDATE	0x0001	/* 0000 0000 0000 0001 */
#define ST_CAPACITY 200	/* Initial Capacity of Lexeme Storage Buffer */
#define ST_INC_FACTOR 15 /* Increment Factor of Lexeme Storage Buffer */
#define LINE_TEN 10	/* For Adding a space to line numbers less than 10 */ 
#define SUCCESS 1 /* Return Success */

typedef union InitialValue {
	int int_val; /* integer variable initial value */
	float fpl_val; /* floating-point variable initial value */
	int str_offset; /* string variable initial value (offset) */
} InitialValue;
typedef struct SymbolTableVidRecord {
	unsigned short status_field; /* variable record status field*/
	char * plex; /* pointer to lexeme (VID name) in CA */
	int o_line; /* line of first occurrence */
	InitialValue i_value; /* variable initial value */
	size_t reserved; /*reserved for future use*/
}STVR;
typedef struct SymbolTableDescriptor {
	STVR *pstvr; /* pointer to array of STVR */
	int st_size; /* size in number of STVR elements */
	int st_offset; /*offset in number of STVR elements */
	Buffer *plsBD; /* pointer to the lexeme storage buffer descriptor */
} STD;


STD st_create(int st_size);
int st_install(STD sym_table, char *lexeme, char type, int line);
int st_lookup(STD sym_table, char *lexeme);
int st_update_type(STD sym_table,int vid_offset,char v_type);
int st_update_value(STD sym_table, int vid_offset, InitialValue i_value);
char st_get_type (STD sym_table, int vid_offset);
void st_destroy(STD sym_table);
int st_print(STD sym_table);
int st_store(STD sym_table);
int st_sort(STD sym_table, char s_order);

#endif
