/*******************************************************
 * 
 * Project: avl-tree
 *
 * Description:  Generic implementation of AVL tree
 *
 *******************************************************/


#ifndef AVL_TREE_INCLUDED
#define AVL_TREE_INCLUDED

#include <stdint.h>

#define BOOL int16_t

#define NUMBER_CHILD_BINARY_TREE 2
#define LCHILD 0
#define RCHILD 1


#define TREE_FLAG_ROOT    0x0001
#define TREE_FLAG_LCHILD  0x0002
#define TREE_FLAG_RCHILD  0x0004

#define SET_ROOT(ptreenode) { \
                             ptreenode->flags |= TREE_FLAG_ROOT; \
                           }

#define SET_LCHILD(ptreenode) { \
                               ptreenode->flags |= TREE_FLAG_LCHILD; \
                             }

#define SET_RCHILD(ptreenode) { \
                               ptreenode->flags |= TREE_FLAG_RCHILD; \
                             }

#define IS_ROOT(ptreenode)        (ptreenode->flags & TREE_FLAG_ROOT)
#define IS_THIS_LCHILD(ptreenode) (ptreenode->flags & TREE_FLAG_LCHILD)
#define IS_THIS_RCHILD(ptreenode) (ptreenode->flags & TREE_FLAG_RCHILD)
#define HAS_LCHILD(ptreenode)     (!(ptreenode->pchild[LCHILD] == NULL)
#define HAS_RCHILD(ptreenode)     (!(ptreenode->pchild[RCHILD]) == NULL)
#define GET_LCHILD(ptreenode)     (ptreenode->pchild[LCHILD])
#define GET_RCHILD(ptreenode)     (ptreenode->pchild[RCHILD])



typedef struct treenode {
    void *pdata;
    uint32_t height;
    struct treenode *pparent;
    uint16_t flags;
    struct treenode *pchild[NUMBER_CHILD_BINARY_TREE];    
    uint32_t child_height[NUMBER_CHILD_BINARY_TREE];
} TREENODE, *PTREENODE;


typedef uint32_t (*PFNTREEDATACMP)(void *pdata_1, void *pdata_2);
typedef BOOL (*PFNTREEDATADEL)(void *pdata);

typedef struct tree {
    PTREENODE proot;
    uint32_t count;
    PFNTREEDATACMP pfncmp;
    PFNTREEDATADEL pfndel;
} TREE, *PTREE;



PTREE tree_init(PFNTREEDATACMP pfncmp, PFNTREEDATADEL pfndel);
void tree_destroy(PTREE ptree);
BOOL tree_insert(PTREE ptree, void *pdata);


#endif