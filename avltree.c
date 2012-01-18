/*******************************************************
 * 
 * Project: avl-tree
 *
 * Description:  Generic implementation of AVL tree
 *
 *******************************************************/

#include <stdlib.h>
#include "avltree.h"


PTREE tree_init(PFNTREEDATACMP pfncmp, PFNTREEDATADEL pfndel) 
{
    PTREE ptree = calloc(sizeof(TREE),1);

    if(ptree) {
        ptree->proot = NULL;
        ptree->count = 0;
        ptree->pfncmp = pfncmp;
        ptree->pfndel = pfndel;
    }

    return ptree;

}

void tree_destroy(PTREE ptree)
{
}

BOOL tree_insert(PTREE ptree, void *pdata)
{
}


