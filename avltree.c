/*******************************************************
 * 
 * Project: avl-tree
 *
 * Description:  Generic implementation of AVL tree
 *
 *******************************************************/

#include <stdlib.h>
#include "avltree.h"

#define MAX_INT(a,b)  ((a) > (b) ? (a) : (b))

PTREE tree_init(uint16_t attributes, 
                PFNTREEDATACMP pfncmp, 
                PFNTREEDATADEL pfndel)
{
    PTREE ptree = NULL;

    if(! pfncmp) {
         return NULL;
    }
    
    ptree = calloc(sizeof(TREE),1);

    if(ptree) {
        ptree->proot = NULL;
        ptree->attrib = attributes;
        ptree->count = 0;
        ptree->pfncmp = pfncmp;
        ptree->pfndel = pfndel;
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
        (*pfndel)(pnode->pdata);
    }

    free(pnode);
}

void tree_destroy(PTREE ptree)
{
    if(! ptree) {
        return;
    }

    tree_destroy_eachnode(ptree->proot,ptree->pfndel);
}

BOOL tree_insert(PTREE ptree, void *pnewdata)
{
    PTREENODE pwalknode = NULL;
    PTREENODE *insertpt = NULL;
    PTREENODE pnewnode = calloc(sizeof(TREENODE),1);
    CMP_RESULT cmpresult = CMP_RESULT_UNKNOWN;

    if(! pnewnode) {
        return FALSE;
    }

    pnewnode->pdata = pnewdata;
    pnewnode->height = 0;         
    pnewnode->pparent = NULL;
    pnewnode->flags = 0;
    pnewnode->pchild[LCHILD] = NULL;
    pnewnode->pchild[RCHILD] = NULL;
    pnewnode->child_height[LCHILD] = 0;
    pnewnode->child_height[RCHILD] = 0;

    //Other height related information of the node will be filled-in later 
    //here (after node's position in the tree is determined)

    if(! ptree->proot) {
        ptree->proot = pnewnode;
        ptree->count = 1;
        pnewnode->height = 1;
        return TRUE;
    }
 
    insertpt = &ptree->proot;


    while(*insertpt) {
        pwalknode = *insertpt;
        cmpresult = (*ptree->pfncmp)(pwalknode->pdata,pnewdata); 
        if(cmpresult == CMP_RESULT_A_B_SAME && 
           ptree->attrib & TREE_ATTRIB_IGNORE_DUP)
        {
            return FALSE;
        }

        if(cmpresult == CMP_RESULT_A_LARGER ||
                cmpresult == CMP_RESULT_A_B_SAME) {
            pwalknode->child_height[LCHILD]++;
            insertpt = &pwalknode->pchild[LCHILD];
        } 
        else {
            pwalknode->child_height[RCHILD]++;
            insertpt = &pwalknode->pchild[RCHILD];
        }
        pwalknode->height = MAX_INT(pwalknode->child_height[LCHILD], 
                                pwalknode->child_height[RCHILD]) + 1; 
    }

    *insertpt = pnewnode;
    pnewnode->pparent = pwalknode;

}

int32_t tree_height(PTREE ptree)
{

    if(ptree && ptree->proot) {
        return ptree->proot->height;
    }

    return 0;

}

