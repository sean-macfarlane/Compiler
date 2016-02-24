/* File Name: buffer.c
* Compiler: MS Visual Studio 2012
* Author: Sean Macfarlane, 040-779-100
* Course:  CST8221 – JAP, Lab Section: 301
* Assignment: 4
* Date: 17 April 2015
* Professor: Sv. Ranev
* Purpose: Function definitions for buffer implementation 
* Function List: 
*					b_create();
*					b_addc();
*					b_reset();
*					b_destroy();
*					b_isfull();
*					b_size();
*					b_capacity();
*					b_setmark();
*					b_mark(); 
*					b_mode();
*					b_inc_factor();
*					b_load();
*					b_isempty();
*					b_eob();
*					b_getc();
*	 				b_print();
*					b_pack();
*					b_rflag();
*					b_retract();
*					b_retract_to_mark();
*					b_getc_offset();
*/

/* standard header files */
#include "buffer.h"

/********************************************************************
Purpose:	Creates and initializes Buffer Descriptor and Character Buffer
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions: calloc(), malloc(), free()
Parameters:	short, char
Return Value: Buffer *, NULL
Algorithm:
**********************************************************************/	
Buffer * b_create (short init_capacity,char inc_factor,char o_mode){
	Buffer* buffer; /* pointer to Buffer Descriptor */

	/* valid parameter checking */
	if(o_mode!='f' && o_mode!='a' && o_mode!='m')
		return NULL;
	if(inc_factor>MAX_INC_F_A)
		return NULL;
	if(o_mode=='m' && inc_factor>MAX_INC_F_M)
		return NULL;

	buffer = (Buffer*)calloc(BUFFER_COUNT, sizeof(Buffer)); /* allocates memory for Buffer Descriptor */
	if(buffer==NULL)
		return NULL;

	buffer->ca_head = (char*)malloc(init_capacity); /* allocates memory for Character Buffer */
	if(buffer->ca_head==NULL)
		return NULL;

	if(o_mode=='f'){ /* if fixed mode initialization */
		buffer->mode = 0;
		buffer->inc_factor = 0;
	}
	else if(o_mode=='a'){  /* if additive mode initialization */
		buffer->mode = MODE_A;
		buffer->inc_factor= inc_factor;
	}
	else if(o_mode=='m' && inc_factor<MAX_INC_F_M){  /* if multiplicative mode initialization */
		buffer->mode = MODE_M;
		buffer->inc_factor= inc_factor;
	}
	else { return NULL;}	/* Error if none of modes are initializied */

	buffer->capacity = init_capacity;
	return buffer;
}

/********************************************************************
Purpose:	Tries to add char to Buffer, and tries to resize if full.
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions: b_isfull(), realloc()
Parameters:	pBuffer, char
Return Value: pBuffer
Algorithm: Fixed mode: none
Additive mode: new capacity = capacity+inc_factor
Multiplicative mode: 
**********************************************************************/	
pBuffer b_addc(pBuffer const pBD, char symbol){
	short new_cap = 0; /* to temporary store new capacity for multiplicative mode */
	char* temp;

	if(!pBD)
		return NULL;
	pBD->r_flag = 0;
	if(b_isfull(pBD)==R_FAIL_1) /* check for run time error */
		return NULL;
	if(b_isfull(pBD)==TRUE){ /* if the buffer is full tries to resize */
		if(pBD->mode==0) /* if fixed mode */
			return NULL;
		else if(pBD->mode==MODE_A){ /* if additive mode */
			if(pBD->capacity+(pBD->inc_factor/sizeof(char)) > SHRT_MAX) /*checks if the new capacity is greater than MAX */
				return NULL;
			else
				pBD->capacity = pBD->capacity+(pBD->inc_factor/sizeof(char));

			if(pBD->capacity<0)
				return NULL;
		}
		else if(pBD->mode==MODE_M){ /* if multiplicative mode */
			if(pBD->capacity==SHRT_MAX)
				return NULL;
			new_cap = (short)((double)pBD->capacity+(SHRT_MAX-pBD->capacity)*((double)pBD->inc_factor/PERCENTAGE));	
			if(pBD->capacity < new_cap)
				pBD->capacity = (short)new_cap;
			else if(new_cap == pBD->capacity) /* checks if cannot increment any more */
				pBD->capacity = SHRT_MAX;
			else return NULL;
		}
		temp = (char*)realloc(pBD->ca_head, pBD->capacity); /* reallocates more space equal to new capacity */
		if(!temp)
			return NULL;
		if(temp != pBD->ca_head){
			pBD->ca_head = temp;
			pBD->r_flag = SET_R_FLAG;
		}		
	}
	/* buffer is not full, just adds char to character buffer */
	pBD->ca_head[pBD->addc_offset] = symbol;
	pBD->addc_offset++;
	return pBD;
}

/********************************************************************
Purpose:	Resets the Buffer Descriptor to NULL 
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions: memset()
Parameters:	Buffer*
Return Value: int 0 or -1
Algorithm:  
**********************************************************************/	
int b_reset (Buffer * const pBD) {
	if(!pBD) /*checks for run-time error */
		return R_FAIL_1;
	pBD->addc_offset = 0;
	pBD->getc_offset = 0;
	pBD->mark_offset = 0;
	pBD->eob = 0;
	return 0;
}

/********************************************************************
Purpose:	Frees all memory allocated 
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions: free()
Parameters:	Buffer*
Return Value: 
Algorithm:  
**********************************************************************/	
void b_destroy(Buffer * const pBD) {
	if(!pBD)
		return;
	free(pBD->ca_head); /*frees character buffer */
	free(pBD); /* frees buffer descriptor */
}

/********************************************************************
Purpose:	Checks if the Buffer is full 
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions: 
Parameters:	Buffer*
Return Value: int 0, 1 or -1
Algorithm:  if size of Buffer is equal to capacity of Buffer
**********************************************************************/	
int b_isfull (Buffer * const pBD){
	if(!pBD) /*checks for run-time error */
		return R_FAIL_1;
	if((pBD->addc_offset) == pBD->capacity)
		return TRUE;
	else return 0;
}

/********************************************************************
Purpose:	Returns the size of Buffer Descriptor
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions: 
Parameters:	Buffer*
Return Value: short
Algorithm:  
**********************************************************************/	
short b_size (Buffer * const pBD){
	if(!pBD) /*checks for run-time error */
		return R_FAIL_1;
	return (pBD->addc_offset);
}

/********************************************************************
Purpose:	Returns the capacity of Buffer Descriptor
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions: 
Parameters:	Buffer*
Return Value: short
Algorithm:  
**********************************************************************/	
short b_capacity(Buffer * const pBD){
	if(pBD) /*checks for run-time error */
		return pBD->capacity;
	else return R_FAIL_1;
}

/********************************************************************
Purpose:	Sets the mark_offset to the desired mark
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions: 
Parameters:	Buffer*, short
Return Value: char
Algorithm:  
**********************************************************************/	
char * b_setmark (Buffer * const pBD, short mark) {
	if(mark>=0 && mark<=pBD->addc_offset){ /*checks for run-time error */
		pBD->mark_offset = mark;
		return &pBD->ca_head[pBD->mark_offset];
	}
	else return NULL;
}

/********************************************************************
Purpose:	Returns the mark_offset of Buffer Descriptor
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions: 
Parameters:	Buffer*
Return Value: short
Algorithm:  
**********************************************************************/	
short b_mark (Buffer * const pBD){
	if(!pBD) /*checks for run-time error */
		return R_FAIL_1;
	else return pBD->mark_offset;
}

/********************************************************************
Purpose:	Returns the mode of Buffer Descriptor
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions: 
Parameters:	Buffer*
Return Value: int 0, 1, -1, or -2
Algorithm:  
**********************************************************************/	
int b_mode (Buffer * const pBD){
	if(!pBD)
		return R_FAIL_2;
	if(pBD->mode==0 || pBD->mode==MODE_A || pBD->mode==MODE_M) /*checks for run-time error */
		return pBD->mode;
	else return R_FAIL_2;
}

/********************************************************************
Purpose:	Returns the increment factor of Buffer Descriptor
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions: 
Parameters:	Buffer*
Return Value: size_t
Algorithm:  
**********************************************************************/	
size_t b_inc_factor (Buffer * const pBD){
	if(pBD->inc_factor>=0)  /*checks for run-time error */
		return (size_t)pBD->inc_factor;
	else return INC_F_FAIL;
}

/********************************************************************
Purpose:	Loads a specified file into a specified buffer
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions: fgetc(), feof(), b_addc()
Parameters: FILE*, Buffer*
Return Value: int
Algorithm:  Loops until it reaches end of file, adding each character
to the buffer
**********************************************************************/	
int b_load (FILE * const fi, Buffer * const pBD){
	int ctr = 0; /* number of characters added to file */
	int c; /* temporary storage of character to be added */

	if(!pBD) /*checks for run-time error */
		return R_FAIL_1;
	while(TRUE){  /* endless loop broken by end of file or failure it will generate a warning */
		c = fgetc(fi);
		if(feof(fi))
			return ctr;
		if (b_addc(pBD, (char)c)==NULL) /*checks if load fail*/
			return LOAD_FAIL;
		ctr++;
	}
}

/********************************************************************
Purpose:	Checks if the Buffer Array is empty
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions: 
Parameters:	Buffer*
Return Value: int
Algorithm:  
**********************************************************************/	
int b_isempty (Buffer * const pBD){
	if(pBD){ /*checks for run-time error */
		if(!pBD->addc_offset) /* checks if size of buffer is NULL */
			return TRUE;
		else return 0;
	}
	else return R_FAIL_1;
}

/********************************************************************
Purpose:	Checks if its the end of buffer
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions: 
Parameters:	Buffer*
Return Value: int 0 or 1
Algorithm:  
**********************************************************************/	
int b_eob (Buffer * const pBD){
	if(!pBD)
		return R_FAIL_1;
	if(pBD->eob==TRUE || pBD->eob==0) /*checks for run-time error */
		return pBD->eob;
	else return R_FAIL_1;
}

/********************************************************************
Purpose:	Checks if at end of buffer and returns char at getc_offset
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions: 
Parameters:	Buffer*
Return Value: char
Algorithm:  
**********************************************************************/	
char b_getc (Buffer * const pBD){

	if(!pBD) /*checks for run-time error */
		return LOAD_FAIL;
	else {
		if(pBD->getc_offset==pBD->addc_offset){
			pBD->eob = SET_EOB;
			return R_FAIL_1;
		}
		else{
			pBD->eob=0;
			return pBD->ca_head[pBD->getc_offset++];
		}
	}
}

/********************************************************************
Purpose:	Prints out the values and contents of Buffer
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions: printf(), b_eob(), b_getc(),
Parameters:	Buffer*
Return Value: int
Algorithm:  
**********************************************************************/	
int b_print (Buffer * const pBD){
	int ctr=0; /* number of characters printed */
	int c; /* temporary storage of character to be printed */

	if(!pBD->addc_offset){ /*checks if the character buffer is empty*/
		printf("The buffer is empty.\n");
		return 0;
	}
	pBD->getc_offset= 0; 
	if(b_eob(pBD)==R_FAIL_1) /*checks for run-time error */
		return R_FAIL_1;
	while(b_eob(pBD)!=SET_EOB){ /* Loops until end of buffer */
		c = b_getc(pBD);
		if(c==LOAD_FAIL) /*checks for run-time error */
			return R_FAIL_1;
		if(b_eob(pBD)==SET_EOB)
			break;
		printf("%c", c);
		ctr++;	
	}
	pBD->getc_offset= 0; 
	printf("\n");
	return ctr;
}

/********************************************************************
Purpose:	Packs the Buffer to a new capacity
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions: realloc()
Parameters:	Buffer*
Return Value: Buffer*
Algorithm:  
**********************************************************************/	
Buffer *b_pack(Buffer * const pBD){
	char* temp;

	if(pBD){ /*checks for run-time error */
		if(pBD->addc_offset==SHRT_MAX){
			return NULL;
		}
		pBD->r_flag = 0;
		pBD->capacity= (short)(pBD->addc_offset+ADD_ONE); /* new capacity */
		temp = (char*)realloc(pBD->ca_head, pBD->capacity); /* reallocates memory for character buffer */
		if(!temp)
			return NULL;
		pBD->eob = 0;
		if(!pBD->ca_head) /*checks for run-time error */
			return NULL;
		if(temp != pBD->ca_head){
			pBD->ca_head = temp;
			pBD->r_flag = SET_R_FLAG;
		}
		return pBD;
	}
	else return NULL;
}

/********************************************************************
Purpose:	Returns the r_flag of the buffer
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions: 
Parameters:	Buffer*
Return Value: char 0, 1, or -1
Algorithm:  
**********************************************************************/	
char b_rflag (Buffer * const pBD){
	if(!pBD)/*checks for run-time error */
		return R_FAIL_1;
	if(pBD->r_flag==SET_R_FLAG || pBD->r_flag==0)
		return pBD->r_flag;
	return R_FAIL_1;
}

/********************************************************************
Purpose:	Decrements getc_offset by 1
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions: 
Parameters:	Buffer*
Return Value: short
Algorithm:  
**********************************************************************/	
short b_retract (Buffer * const pBD){
	if(pBD){ /*checks for run-time error */
		if(pBD->getc_offset>0)
			pBD->getc_offset--;
		return pBD->getc_offset;
	}
	else return R_FAIL_1;
}

/********************************************************************
Purpose:	Sets the getc_offset to mark_offset
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions: 
Parameters:	Buffer*
Return Value: short
Algorithm:  
**********************************************************************/	
short b_retract_to_mark (Buffer * const pBD){
	if(pBD){ /*checks for run-time error */
		pBD->getc_offset = pBD->mark_offset;
		return pBD->getc_offset;
	}
	else return R_FAIL_1;
}

/********************************************************************
Purpose:	Returns the getc_offset value
Author:		Sean Macfarlane 
History/Version:	1.0
Called Functions: 
Parameters:	Buffer*
Return Value: short
Algorithm:  
**********************************************************************/	
short b_getc_offset (Buffer * const pBD){
	if(pBD) /*checks for run-time error */
		return pBD->getc_offset;
	else return R_FAIL_1;
}
