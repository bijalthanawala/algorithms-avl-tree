
/********************************************************************
*  Licence: The MIT license
*
*  File: gnrcstack.h
*  Description: Generic stack header file
*  
********************************************************************/

#ifndef GENERIC_STACK
#define GENERIC_STACK


/* Includes
**************/
#include "../common/common_types.h"
#include "../gnrclist/gnrclist.h"

/* Definitions
****************/
typedef struct GNRCLIST gnrcstack;
typedef PFN_LISTELEMENT_DEL PFN_STACKELEMENT_DEL;

/* structures/unions
*********************/
// --None--

/* Prototypes
***************/
gnrcstack* gnrcstack_create(PFN_STACKELEMENT_DEL pfndel);
void gnrcstack_destroy(gnrcstack *pstack);
BOOL gnrcstack_push(gnrcstack* pstack, void *pvoid);
void *gnrcstack_pop(gnrcstack* pstack);


#endif
