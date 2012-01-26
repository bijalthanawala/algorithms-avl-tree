
/********************************************************************
*  Licence: The MIT license
*
*  File: gnrclist.c
*  Description: Generic list implementation
*  
*
********************************************************************/

/* Includes
**************/
#include <malloc.h>
#include <memory.h>
#include "gnrclist.h"

/* Prototypes 
***************/
struct LISTNODE *gnrclist_genlistnode(void *payload);

/* Routines
*************/

struct GNRCLIST *gnrclist_create(PFN_LISTELEMENT_DEL pfndel)
{
    struct GNRCLIST *pnewlist = malloc(sizeof(struct GNRCLIST));

    if(!pnewlist) {
        return NULL;
    }
    
    memset(pnewlist,0,sizeof(*pnewlist));
    pnewlist->count = 0;
    pnewlist->pfndel = pfndel;
    pnewlist->head = NULL;
    pnewlist->tail= NULL;
    pnewlist->iterptr = NULL;

    return pnewlist;

}

void gnrclist_destroy(struct GNRCLIST *plist)
{
    struct LISTNODE *pnode = NULL;
    struct LISTNODE *pnext = NULL;

    if(plist) {
        pnode = plist->head;
        while(pnode) {
            if(plist->pfndel && pnode->payload) {
                plist->pfndel(pnode->payload);
            }
            pnext = pnode->next;
            free(pnode);
            pnode = pnext;
        }
        free(plist);
    }

}


BOOL gnrclist_insert_front(struct GNRCLIST *plist,void *payload)
{
    struct LISTNODE *pnewnode = NULL;

    if(!plist) {
        return FALSE;
    }
    
    pnewnode = gnrclist_genlistnode(payload);
    if(!pnewnode) {
        return FALSE;
    }

    plist->count++;
    if(!plist->head) {
        plist->head = pnewnode;
        plist->iterptr = pnewnode;
        plist->tail = pnewnode;
        return TRUE;
    }

    pnewnode->next = plist->head;
    plist->head = pnewnode;

    return TRUE;
}


BOOL gnrclist_insert_back(struct GNRCLIST *plist,void *payload)
{
    struct LISTNODE *pnewnode = NULL;
    
    pnewnode = gnrclist_genlistnode(payload);
    if(!pnewnode) {
        return FALSE;
    }

    plist->count++;
    if(!plist->head) {
        plist->head = pnewnode;
        plist->iterptr = pnewnode;
        plist->tail = pnewnode;
        return TRUE;
    }

    plist->tail->next = pnewnode;
    plist->tail = pnewnode;

    return TRUE;
}


void gnrclist_walk(struct GNRCLIST *plist,
                   PFN_LISTELEMENT_PROCESS pfnprocessnode,
                   void *pcontext)
{
  
   struct LISTNODE *pnode = NULL;
   BOOL bcontinue = TRUE;

   if(!plist) {
    return;
   }

    pnode = plist->head;
    while(pnode && bcontinue) {
       if(pfnprocessnode) {
             bcontinue = (*pfnprocessnode)(pnode->payload,pcontext);       
       }
       pnode = pnode->next;
    }

}



struct LISTNODE *gnrclist_genlistnode(void *payload)
{
    struct LISTNODE *pnewlistnode = NULL;

    pnewlistnode = malloc(sizeof(struct LISTNODE));
    if(!pnewlistnode) {
        return NULL;
    }

    memset(pnewlistnode,0,sizeof(*pnewlistnode));
    pnewlistnode->payload = payload;
    pnewlistnode->next = NULL;

    return pnewlistnode;
}

void *gnrclist_iter_getfirst(struct GNRCLIST *plist)
{
    if(!plist) {
        return NULL;
    }

    plist->iterptr = plist->head;
    return gnrclist_iter_getnext(plist);
}


void *gnrclist_iter_getnext(struct GNRCLIST *plist)
{
   void *curr_payload = NULL;

   if(!plist) {
    return NULL;
   }

   if(plist->iterptr) {
     curr_payload = plist->iterptr->payload; 
     plist->iterptr = plist->iterptr->next;
   }

   return curr_payload;
}


void *gnrclist_getfirst(struct GNRCLIST *plist)
{
    if(!plist || !plist->head) {
        return NULL;
    }

    return plist->head->payload;
}

void gnrclist_delfirst(struct GNRCLIST *plist)
{
    struct LISTNODE *oldhead = NULL;

    if(!plist || !plist->head) {
        return;
    }

   if(plist->pfndel && plist->head->payload) {
      (*plist->pfndel)(plist->head->payload);
   }

   oldhead = plist->head;
   plist->head = plist->head->next;

   free(oldhead);
}


