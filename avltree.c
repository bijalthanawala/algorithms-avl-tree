/*******************************************************
 * 
 * Project: avl-tree
 *
 * Description:  Generic implementation of AVL tree
 *
 *******************************************************/

/* Include
************/
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "avltree.h"


/* Macros
***********/   
#define MAX_INT(a,b)  ((a) > (b) ? (a) : (b))

/* Definitions/Constants
*************************/
#define DATA_STR_BUFF_LEN 100


/* Prototypes
****************/   
BOOL is_balance_violated(PTREENODE pnode,BOOL isnewnodetoleft);
BOOL tree_rotate(PTREE ptree, PTREENODE gp, PTREENODE p, PTREENODE c); 
BOOL left_rotate(PTREE ptree, PTREENODE p, PTREENODE c); 
BOOL right_rotate(PTREE ptree, PTREENODE p, PTREENODE c); 
void default_getdatastr(void *pdata,char *dataval_strbuff,int strbufflen);

PTREENODE tree_iter_getnext_preorder(PTREE_ITER_OBJ piterobj);
PTREENODE tree_iter_getnext_inorder(PTREE_ITER_OBJ piterobj);
PTREENODE tree_iter_getnext_postorder(PTREE_ITER_OBJ piterobj);

/* Routines
**************/   
PTREE tree_init(uint16_t attributes, 
                PFNTREEDATACMP pfncmp, 
                PFNTREEDATADEL pfndel,
                PFNGETDATAVALSTR pfngetdatastr)
{
    PTREE ptree = NULL;

    if(! pfncmp) {
         return NULL;
    }
    
    ptree = calloc(sizeof(TREE),1);

    if(ptree) {
        ptree->proot = NULL;
        ptree->attrib = attributes;
        ptree->node_count = 0;
        ptree->pfncmp = pfncmp;
        ptree->pfndel = pfndel;
        ptree->pfngetdatastr = pfngetdatastr ? pfngetdatastr : default_getdatastr;
    }

    return ptree;

}


void tree_destroy_eachnode(PTREENODE pnode,
                           PFNTREEDATADEL pfndel)
{
    if(! pnode) {
        return;
    }

    tree_destroy_eachnode(GET_LCHILD(pnode),pfndel);
    tree_destroy_eachnode(GET_RCHILD(pnode),pfndel);

    if(pfndel) {
        (*pfndel)(pnode->data_ptr);
    }

    free(pnode);
}

void tree_destroy(PTREE ptree)
{
    if(! ptree) {
        return;
    }

    tree_destroy_eachnode(ptree->proot,ptree->pfndel);

    free(ptree);
}

BOOL tree_insert(PTREE ptree, void *pnewdata)
{
    PTREENODE pwalknode = NULL;
    PTREENODE *insertpt = NULL;
    CMP_RESULT cmpresult = CMP_RESULT_UNKNOWN;
    PTREENODE gp = NULL; /* Grandparent: Incase bal. is violated */
    PTREENODE p = NULL;  /* Parent: Incase bal. is violated */
    PTREENODE c = NULL;  /* child: Incase bal. is violated */
    PTREENODE pnewnode = calloc(sizeof(TREENODE),1);

    if(! pnewnode) {
        return FALSE;
    }

    fprintf(stderr,"Inserting %d\n",(int)pnewdata);

    /* Initialize the new node's metadata */
    pnewnode->data_ptr = pnewdata;
    pnewnode->height = 1;         
    pnewnode->pparent = NULL;
    pnewnode->nodetype = TREE_NODE_UNKNOWN;
    pnewnode->pchild[LCHILD] = NULL;
    pnewnode->pchild[RCHILD] = NULL;

    /* Check if the new node is the root */
    if(! ptree->proot) {
        SET_ROOT(pnewnode);
        ptree->proot = pnewnode;
        ptree->node_count = 1;
        pnewnode->height = 1;
        return TRUE;
    }
 

    /* Traverse and find the position of the new node */
    insertpt = &ptree->proot;
    while(*insertpt) {
        pwalknode = *insertpt;
        cmpresult = (*ptree->pfncmp)(pwalknode->data_ptr,pnewdata); 

        /* Handle duplicate data */
        if(cmpresult == CMP_RESULT_A_B_SAME && 
           ptree->attrib & TREE_ATTRIB_IGNORE_DUP)
        {
            return FALSE;
        }

        /* Turn left/right as comparisons lead */
        if(cmpresult == CMP_RESULT_A_LARGER ||
           cmpresult == CMP_RESULT_A_B_SAME) {
            insertpt = &pwalknode->pchild[LCHILD];
            SET_LCHILD(pnewnode);
        } 
        else {
            insertpt = &pwalknode->pchild[RCHILD];
            SET_RCHILD(pnewnode);
        }

        /* Update height of each node while traversing */
        pwalknode->height++;

        if(is_balance_violated(pwalknode,IS_THIS_LCHILD(pnewnode))) {
            gp = pwalknode;
            p = NULL;
        }
        else {
            if(gp != NULL) {

                if(p == NULL) {
                    p = pwalknode;
                    c = NULL;
                } 
                else {
                    c = pwalknode;
                }

            }

       }

    } /* End: while() loop - Traversal ends */


    /* Found the position of the newnode in the tree.
       Insert the new node and take note of it's parent */
    *insertpt = pnewnode; 
    pnewnode->pparent = pwalknode;

    /* Record the count of nodes in the tree*/
    ptree->node_count++;

    /* Ensure all three - gp, p and c are known, in case 
       the balance factor was violated */
    if((gp!=NULL) && (p!=NULL) && (c==NULL)){
            c = pnewnode;
    }

    if((gp!=NULL) && (p==NULL)) {
        fprintf(stderr,"Unexpected Error: Balance was violated, but 'parent' "\
                       "node not determined during traversal!\n");
        assert(FALSE);
    }

    if(gp != NULL)
        tree_rotate(ptree,gp,p,c);

    return TRUE;
}

uint32_t tree_height(PTREE ptree)
{

    if(ptree && ptree->proot) {
        return ptree->proot->height;
    }

    return 0;

}


uint32_t tree_node_count(PTREE ptree)
{
    return (ptree ? ptree->node_count : 0);
}

void tree_dump_preorder(PTREE ptree, FILE *fp, PTREENODE pnode)
{
  char nodedatastr[DATA_STR_BUFF_LEN];
  char childdatastr[DATA_STR_BUFF_LEN];
  char parentdatastr[DATA_STR_BUFF_LEN];

  if(!pnode) return;

  ptree->pfngetdatastr(pnode->data_ptr,nodedatastr,sizeof(nodedatastr));

  if(pnode->pchild[LCHILD]) {
       ptree->pfngetdatastr(pnode->pchild[LCHILD]->data_ptr,
                            childdatastr,
                            sizeof(childdatastr));
       fprintf(fp,"\"%s\" -> \"%s\"\n",nodedatastr,childdatastr);
   }

   if(pnode->pchild[RCHILD]) {
        ptree->pfngetdatastr(pnode->pchild[RCHILD]->data_ptr,
                             childdatastr,
                             sizeof(childdatastr));
        fprintf(fp,"\"%s\" -> \"%s\"\n",nodedatastr,childdatastr);
   }

   if(pnode->pparent) {
     ptree->pfngetdatastr(pnode->pparent->data_ptr,
                          parentdatastr,
                          sizeof(parentdatastr));
   }
   else {
       snprintf(parentdatastr,sizeof(parentdatastr),"NULL");
   }

   fprintf(fp,
           "\"%s\" [label=\"%s\\nparent=%s\\nht=%d, lchild_ht=%d, rchild_ht=%d\"]\n",
           nodedatastr,
           nodedatastr,
           parentdatastr,
           pnode->height,
           pnode->pchild[LCHILD] ? pnode->pchild[LCHILD]->height : 0, 
           pnode->pchild[RCHILD] ? pnode->pchild[RCHILD]->height : 0); 

   if(pnode->pparent) {
     fprintf(fp,"\"%s\" -> \"%s\"\n",nodedatastr,parentdatastr);
   }

   tree_dump_preorder(ptree, fp, pnode->pchild[LCHILD]);
   tree_dump_preorder(ptree, fp, pnode->pchild[RCHILD]);
 
}

int32_t tree_dump(PTREE ptree,char *filename)
{
   FILE *fp = fopen(filename,"w");
 
   fprintf(fp,"digraph avltree {\n"); 
   tree_dump_preorder(ptree,fp,ptree->proot);
   fprintf(fp,"}\n");

   fclose(fp);

}


BOOL is_balance_violated(PTREENODE pnode,BOOL isnewnodetoleft)
{
   int16_t bal = 0;
   int lheight = pnode->pchild[LCHILD] ? pnode->pchild[LCHILD]->height : 0;
   int rheight = pnode->pchild[RCHILD] ? pnode->pchild[RCHILD]->height : 0;

   if(isnewnodetoleft) 
       lheight++;
   else
       rheight++;
   
   bal = lheight - rheight;

   fprintf(stderr, "is_balance_violated: bal = %d\n",bal);

   if(bal > 1 || bal < -1)
       return TRUE;

   return FALSE;
}


BOOL tree_rotate(PTREE ptree, PTREENODE gp, PTREENODE p, PTREENODE c)
{
   char gp_datastr[DATA_STR_BUFF_LEN];
   char p_datastr[DATA_STR_BUFF_LEN];
   char c_datastr[DATA_STR_BUFF_LEN];

   ptree->pfngetdatastr(gp->data_ptr,gp_datastr,sizeof(gp_datastr));
   ptree->pfngetdatastr(p->data_ptr ,p_datastr ,sizeof(p_datastr));
   ptree->pfngetdatastr(c->data_ptr ,c_datastr ,sizeof(c_datastr));

   fprintf(stderr,"Request to rotate: gp=%s p=%s c=%s\n",
           gp_datastr, p_datastr, c_datastr);

   if((IS_THIS_LCHILD(p) && IS_THIS_LCHILD(c)) ||
      (IS_THIS_RCHILD(p) && IS_THIS_RCHILD(c))) {

       fprintf(stderr,"Only one rotation needed\n");
       /*Only one rotation is needed*/
       if(IS_THIS_LCHILD(p)){
           right_rotate(ptree,gp,p);
       }
       else {
           left_rotate(ptree,gp,p);
       }
   } 
   else 
   {

       fprintf(stderr,"Two rotations needed\n");
       /* Two rotations are needed */
       if(IS_THIS_LCHILD(p)){
           left_rotate(ptree,p,c);
           right_rotate(ptree,gp,c);
       }
       else {
           right_rotate(ptree,p,c);
           left_rotate(ptree,gp,p);
       }

   }

   return TRUE;
} 


BOOL left_rotate(PTREE ptree, PTREENODE p, PTREENODE c)
{
    PTREENODE pwalknode = NULL;
    char p_datastr[DATA_STR_BUFF_LEN];
    char c_datastr[DATA_STR_BUFF_LEN];

   ptree->pfngetdatastr(p->data_ptr ,p_datastr ,sizeof(p_datastr));
   ptree->pfngetdatastr(c->data_ptr ,c_datastr ,sizeof(c_datastr));


    fprintf(stderr,"Request to left rotate: p=%s c=%s\n",
            p_datastr,c_datastr);


    /* First promote the child */
    if(IS_THIS_LCHILD(p)) {
        p->pparent->pchild[LCHILD] = c;
        c->pparent = p->pparent;
        SET_LCHILD(c);
    }
    else if(IS_THIS_RCHILD(p)) {
        p->pparent->pchild[RCHILD] = c;
        c->pparent = p->pparent;
        SET_RCHILD(c);
    } else {
        ptree->proot = c;
        c->pparent = NULL;
        SET_ROOT(c);
    }

    /* Now let parent(p) adopt child(c)'s left child */ 
    p->pchild[RCHILD] = c->pchild[LCHILD];
    if(c->pchild[LCHILD]) {
        c->pchild[LCHILD]->pparent = p;
        SET_RCHILD(c->pchild[LCHILD]);
    }

    /* Now make child(c) the parent, or in other words,
       make parent(p) the child */
    c->pchild[LCHILD] = p;
    p->pparent = c;
    SET_LCHILD(p);


    /* Now update the heights of all affected nodes, in the
       correct order */
    p->height = MAX_INT(p->pchild[LCHILD] ? p->pchild[LCHILD]->height : 0,
                        p->pchild[RCHILD] ? p->pchild[RCHILD]->height : 0)+1; 
    c->height = MAX_INT(c->pchild[LCHILD] ? c->pchild[LCHILD]->height : 0,
                        c->pchild[RCHILD] ? c->pchild[RCHILD]->height : 0)+1;
    pwalknode = c->pparent;
    while(pwalknode) {
      pwalknode->height = 
      MAX_INT(pwalknode->pchild[LCHILD] ? pwalknode->pchild[LCHILD]->height : 0,
              pwalknode->pchild[RCHILD] ? pwalknode->pchild[RCHILD]->height : 0)+1;  
      pwalknode = pwalknode->pparent;
    } 


    return TRUE;
}


BOOL right_rotate(PTREE ptree, PTREENODE p, PTREENODE c)
{
    PTREENODE pwalknode = NULL;
    char p_datastr[DATA_STR_BUFF_LEN];
    char c_datastr[DATA_STR_BUFF_LEN];

    ptree->pfngetdatastr(p->data_ptr ,p_datastr ,sizeof(p_datastr));
    ptree->pfngetdatastr(c->data_ptr ,c_datastr ,sizeof(c_datastr));


    fprintf(stderr,"Request to right rotate: p=%s c=%s\n",
            p_datastr,c_datastr);


    /* First promote the child */
    if(IS_THIS_LCHILD(p)) {
        p->pparent->pchild[LCHILD] = c;
        c->pparent = p->pparent;
        SET_LCHILD(c);
    }
    else if(IS_THIS_RCHILD(p)) {
        p->pparent->pchild[RCHILD] = c;
        c->pparent = p->pparent;
        SET_RCHILD(c);
    } else {
        ptree->proot = c;
        c->pparent = NULL;
        SET_ROOT(c);
    }

    /* Now let parent(p) adopt child(c)'s left child */ 
    p->pchild[LCHILD] = c->pchild[RCHILD];
    if(c->pchild[RCHILD]) {
        c->pchild[RCHILD]->pparent = p;
        SET_LCHILD(c->pchild[RCHILD]);
    }

    /* Now make child(c) the parent, or in other words,
       make parent(p) the child */
    c->pchild[RCHILD] = p;
    p->pparent = c;
    SET_RCHILD(p);


   /* Now update the heights of all affected nodes, in the
       correct order */
    p->height = MAX_INT(p->pchild[LCHILD] ? p->pchild[LCHILD]->height : 0,
                        p->pchild[RCHILD] ? p->pchild[RCHILD]->height : 0)+1; 
    c->height = MAX_INT(c->pchild[LCHILD] ? c->pchild[LCHILD]->height : 0,
                        c->pchild[RCHILD] ? c->pchild[RCHILD]->height : 0)+1;
    pwalknode = c->pparent;
    while(pwalknode) {
      pwalknode->height = 
      MAX_INT(pwalknode->pchild[LCHILD] ? pwalknode->pchild[LCHILD]->height : 0,
              pwalknode->pchild[RCHILD] ? pwalknode->pchild[RCHILD]->height : 0)+1;  
      pwalknode = pwalknode->pparent;
    } 


    return TRUE;
}



void default_getdatastr(void *pdata,char *dataval_strbuff,int strbufflen)
{
    snprintf(dataval_strbuff,
             strbufflen,
             "%p",
             pdata);
}

void free_node_iter(PVOID pvoid)
{
    PNODE_ITER pnodeiter = (PNODE_ITER)pvoid;
    free(pnodeiter);
}


PTREE_ITER_OBJ tree_iter_getobj(PTREE ptree, ITER_TYPE itertype)
{
    PTREE_ITER_OBJ piter = NULL;

    piter = calloc(sizeof(TREE_ITER_OBJ),1);
    if(! piter) {
        return NULL;
    }

    piter->begun = FALSE;
    piter->ptree = ptree;
    piter->type = itertype;
    piter->pstack = gnrcstack_create(free_node_iter);
    if(! piter->pstack) {
        free(piter);
        return NULL;
    }

    return  piter;
}

void tree_iter_freeobj(PTREE_ITER_OBJ piterobj)
{

    if(piterobj) {

        if(piterobj->pstack)
           gnrcstack_destroy(piterobj->pstack);

        free(piterobj);
    }
}

PTREENODE tree_iter_getnext(PTREE_ITER_OBJ piterobj)
{
    switch(piterobj->type)
    {
        case ITER_TYPE_PREORDER:  return tree_iter_getnext_preorder(piterobj);
                                  break;

        case ITER_TYPE_INORDER:   return tree_iter_getnext_inorder(piterobj);
                                  break;

        case ITER_TYPE_POSTORDER: return tree_iter_getnext_postorder(piterobj);
                                  break;
    }

    return NULL;
}

PTREENODE tree_iter_getnext_preorder(PTREE_ITER_OBJ piterobj)
{
    PNODE_ITER pnodeiter = NULL;
    PNODE_ITER pchildnodeiter = NULL;

    if(!piterobj->begun) {

        pnodeiter = calloc(sizeof(NODE_ITER),0);
        if(!pnodeiter)
            return NULL;

        piterobj->begun = TRUE;
        pnodeiter->pnode = piterobj->ptree->proot;
        pnodeiter->state = ITER_STATE_PREORDER_DISCOVERED_SELF;
        gnrcstack_push(piterobj->pstack,pnodeiter);
        return pnodeiter->pnode->data_ptr;
    }

    while(pnodeiter = gnrcstack_top(piterobj->pstack))
    {
        if(pnodeiter->state == ITER_STATE_PREORDER_DISCOVERED_SELF) {

            pnodeiter->state = ITER_STATE_PREORDER_LEFT_TRAVERSED;

            if(pnodeiter->pnode->pchild[LCHILD]) {

                pchildnodeiter = calloc(sizeof(NODE_ITER),0);
                if(!pchildnodeiter)
                    return NULL;

                pchildnodeiter->pnode = pnodeiter->pnode->pchild[LCHILD];
                pchildnodeiter->state = ITER_STATE_PREORDER_DISCOVERED_SELF;
                gnrcstack_push(piterobj->pstack, pchildnodeiter);      
                return pchildnodeiter->pnode->data_ptr;
            }
        }

        if(pnodeiter->state == ITER_STATE_PREORDER_LEFT_TRAVERSED) {

            pnodeiter->state = ITER_STATE_PREORDER_RIGHT_TRAVERSED;

            if(pnodeiter->pnode->pchild[RCHILD]) {

                pchildnodeiter = calloc(sizeof(NODE_ITER),0);
                if(!pchildnodeiter)
                    return NULL;

                pchildnodeiter->pnode = pnodeiter->pnode->pchild[RCHILD];
                pchildnodeiter->state = ITER_STATE_PREORDER_DISCOVERED_SELF;
                gnrcstack_push(piterobj->pstack, pchildnodeiter);      
                return pchildnodeiter->pnode->data_ptr;
            }
        }

        assert(pnodeiter->state == ITER_STATE_PREORDER_RIGHT_TRAVERSED); 

        gnrcstack_pop(piterobj->pstack);
    }

    return NULL;


}


PTREENODE tree_iter_getnext_inorder(PTREE_ITER_OBJ piterobj)
{
    PNODE_ITER pnodeiter = NULL;
    PNODE_ITER pchildnodeiter = NULL;

    if(!piterobj->begun) {

        pnodeiter = calloc(sizeof(NODE_ITER),0);
        if(!pnodeiter)
            return NULL;

        piterobj->begun = TRUE;
        pnodeiter->pnode = piterobj->ptree->proot;
        pnodeiter->state = ITER_STATE_INORDER_DISCOVERED_SELF;
        gnrcstack_push(piterobj->pstack,pnodeiter);
    }


    while(pnodeiter = gnrcstack_top(piterobj->pstack)) 
    {

        if(pnodeiter->state == ITER_STATE_INORDER_LEFT_TRAVERSED) {
            pnodeiter->state = ITER_STATE_INORDER_RIGHT_TO_BE_TRAVERSED;
            return pnodeiter->pnode->data_ptr;
        }


        if(pnodeiter->state == ITER_STATE_INORDER_RIGHT_TO_BE_TRAVERSED) {

            pnodeiter->state = ITER_STATE_INORDER_RIGHT_TRAVERSED;

            if(pnodeiter->pnode->pchild[RCHILD]) {

                pchildnodeiter = calloc(sizeof(NODE_ITER),0);
                if(!pchildnodeiter)
                    return NULL;

                pchildnodeiter->pnode = pnodeiter->pnode->pchild[RCHILD];
                pchildnodeiter->state = ITER_STATE_INORDER_DISCOVERED_SELF;
                gnrcstack_push(piterobj->pstack, pchildnodeiter);     

                pnodeiter = pchildnodeiter; 
            }
        }

        if(pnodeiter->state == ITER_STATE_INORDER_DISCOVERED_SELF) {

            pnodeiter->state = ITER_STATE_INORDER_LEFT_TRAVERSED;

            while(pnodeiter->pnode->pchild[LCHILD]) {

                pchildnodeiter = calloc(sizeof(NODE_ITER),0);
                if(!pchildnodeiter)
                    return NULL;

                pchildnodeiter->pnode = pnodeiter->pnode->pchild[LCHILD];
                pchildnodeiter->state = ITER_STATE_INORDER_LEFT_TRAVERSED;
                gnrcstack_push(piterobj->pstack, pchildnodeiter);     

                pnodeiter = pchildnodeiter; 
            }
        }


        if(pnodeiter->state == ITER_STATE_INORDER_RIGHT_TRAVERSED) 
            gnrcstack_pop(piterobj->pstack);
    }

    return NULL;

}

PTREENODE tree_iter_getnext_postorder(PTREE_ITER_OBJ piterobj)
{
    PNODE_ITER pnodeiter = NULL;
    PNODE_ITER pchildnodeiter = NULL;

    if(!piterobj->begun) {

        pnodeiter = calloc(sizeof(NODE_ITER),0);
        if(!pnodeiter)
            return NULL;

        piterobj->begun = TRUE;
        pnodeiter->pnode = piterobj->ptree->proot;
        pnodeiter->state = ITER_STATE_POSTORDER_DISCOVERED_SELF;
        gnrcstack_push(piterobj->pstack,pnodeiter);
    }


    while(pnodeiter = gnrcstack_top(piterobj->pstack)) 
    {

        if(pnodeiter->state == ITER_STATE_POSTORDER_LEFT_TRAVERSED) {
            pnodeiter->state = ITER_STATE_POSTORDER_RIGHT_TO_BE_TRAVERSED;
        }


        if(pnodeiter->state == ITER_STATE_POSTORDER_RIGHT_TO_BE_TRAVERSED) {

            pnodeiter->state = ITER_STATE_POSTORDER_RIGHT_TRAVERSED;

            if(pnodeiter->pnode->pchild[RCHILD]) {

                pchildnodeiter = calloc(sizeof(NODE_ITER),0);
                if(!pchildnodeiter)
                    return NULL;

                pchildnodeiter->pnode = pnodeiter->pnode->pchild[RCHILD];
                pchildnodeiter->state = ITER_STATE_POSTORDER_DISCOVERED_SELF;
                gnrcstack_push(piterobj->pstack, pchildnodeiter);     

                pnodeiter = pchildnodeiter; 
            }
        }

        if(pnodeiter->state == ITER_STATE_POSTORDER_DISCOVERED_SELF) {

            pnodeiter->state = ITER_STATE_POSTORDER_LEFT_TRAVERSED;

            while(pnodeiter->pnode->pchild[LCHILD]) {

                pchildnodeiter = calloc(sizeof(NODE_ITER),0);
                if(!pchildnodeiter)
                    return NULL;

                pchildnodeiter->pnode = pnodeiter->pnode->pchild[LCHILD];
                pchildnodeiter->state = ITER_STATE_POSTORDER_LEFT_TRAVERSED;
                gnrcstack_push(piterobj->pstack, pchildnodeiter);     

                pnodeiter = pchildnodeiter; 
            }
        }


        if(pnodeiter->state == ITER_STATE_POSTORDER_RIGHT_TRAVERSED) { 
            pnodeiter->state = ITER_STATE_POSTORDER_NODE_POST_ORDER_DONE;
            return pnodeiter->pnode->data_ptr;
        }

        if(pnodeiter->state == ITER_STATE_POSTORDER_NODE_POST_ORDER_DONE) {
            gnrcstack_pop(piterobj->pstack);
        }
    }

    return NULL;

}

