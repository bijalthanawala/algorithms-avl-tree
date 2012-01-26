
/********************************************************************
*  Licence: The MIT license
*
*  File: gnrcstack.c
*  Description: Generic stack implmentation
*  
********************************************************************/

/* Includes
**************/
#include <stdio.h>
#include "../gnrclist/gnrclist.h"
#include "gnrcstack.h"

/* Routines
*************/


gnrcstack* gnrcstack_create(PFN_STACKELEMENT_DEL pfndel)
{
	return gnrclist_create(pfndel);
}



void gnrcstack_destroy(gnrcstack *pstack)
{
    return gnrclist_destroy(pstack);
}


BOOL gnrcstack_push(gnrcstack* pstack, void *pvoid)
{
   return gnrclist_insert_front(pstack,pvoid);
}



void *gnrcstack_pop(gnrcstack* pstack)
{
    void *payload = gnrclist_getfirst(pstack);

    gnrclist_delfirst(pstack);

    return payload;
}


