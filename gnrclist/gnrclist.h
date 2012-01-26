
/********************************************************************
*  Licence: The MIT license
*
*  File: gnrclist.h
*  Description: Generic list header file
*  
*
********************************************************************/

#ifndef GENERIC_LIST
#define GENERIC_LIST


/* Includes
**************/
#include "../common/common_types.h"

/* Type definitions
**********************/
typedef void (*PFN_LISTELEMENT_DEL)(void *payload);
typedef BOOL (*PFN_LISTELEMENT_PROCESS)(void *payload,
                                        void *pcontext);

/* Structures/Unions
**********************/

struct LISTNODE {
    void *payload;
    struct LISTNODE *next;
};

struct GNRCLIST {
    int32_t count;
    PFN_LISTELEMENT_DEL pfndel;
    struct LISTNODE *head;
    struct LISTNODE *tail;
    struct LISTNODE *iterptr;
};

/* Prototypes
***************/
struct GNRCLIST *gnrclist_create(PFN_LISTELEMENT_DEL pfndel);
void   gnrclist_destroy(struct GNRCLIST *plist);
BOOL   gnrclist_insert_front(struct GNRCLIST *plist,void *payload);
BOOL   gnrclist_insert_back(struct GNRCLIST *plist,void *payload);
void   gnrclist_walk(struct GNRCLIST *plist,
                     PFN_LISTELEMENT_PROCESS pfnprocess,
                     void *pcontext);
void *gnrclist_iter_getfirst(struct GNRCLIST *pgnrclist);
void *gnrclist_iter_getnext(struct GNRCLIST *pgnrclist);

void *gnrclist_getfirst(struct GNRCLIST *plist);
void gnrclist_delfirst(struct GNRCLIST *plist);

#endif
