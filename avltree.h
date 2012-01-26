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

#define NUMBER_CHILD_BINARY_TREE 2
#define LCHILD 0
#define RCHILD 1


typedef enum {
  TREE_NODE_UNKNOWN,  
  TREE_NODE_ROOT,
  TREE_NODE_LCHILD,
  TREE_NODE_RCHILD
} TREE_NODE_TYPE;

#define SET_ROOT(ptreenode) { \
                             ptreenode->nodetype = TREE_NODE_ROOT; \
                           }

#define SET_LCHILD(ptreenode) { \
                               ptreenode->nodetype = TREE_NODE_LCHILD; \
                             }

#define SET_RCHILD(ptreenode) { \
                               ptreenode->nodetype = TREE_NODE_RCHILD; \
                             }

#define IS_ROOT(ptreenode)        (ptreenode->nodetype == TREE_NODE_ROOT)
#define IS_THIS_LCHILD(ptreenode) (ptreenode->nodetype == TREE_NODE_LCHILD)
#define IS_THIS_RCHILD(ptreenode) (ptreenode->nodetype == TREE_NODE_RCHILD)
#define HAS_LCHILD(ptreenode)     (!(ptreenode->pchild[LCHILD] == NULL)
#define HAS_RCHILD(ptreenode)     (!(ptreenode->pchild[RCHILD]) == NULL)
#define GET_LCHILD(ptreenode)     (ptreenode->pchild[LCHILD])
#define GET_RCHILD(ptreenode)     (ptreenode->pchild[RCHILD])



typedef struct treenode {
    void *data_ptr;
    uint32_t height;
    struct treenode *pparent;
    TREE_NODE_TYPE nodetype;
    struct treenode *pchild[NUMBER_CHILD_BINARY_TREE];    
} TREENODE, *PTREENODE;


typedef enum {
    CMP_RESULT_UNKNOWN,
    CMP_RESULT_A_B_SAME,
    CMP_RESULT_A_LARGER,
    CMP_RESULT_A_SMALLER
} CMP_RESULT;


typedef CMP_RESULT (*PFNTREEDATACMP)(void *pdata_a, void *pdata_b);
typedef BOOL (*PFNTREEDATADEL)(void *pdata);
typedef void (*PFNGETDATAVALSTR)(void *pdata,char *dataval_strbuff,int strbufflen);


#define TREE_ATTRIB_IGNORE_DUP  0x0001

typedef struct tree {
    PTREENODE proot;
    uint32_t node_count;
    uint32_t attrib;
    PFNTREEDATACMP pfncmp;
    PFNTREEDATADEL pfndel;
    PFNGETDATAVALSTR pfngetdatastr;
} TREE, *PTREE;



PTREE tree_init(uint16_t attributes, 
                PFNTREEDATACMP pfncmp, 
                PFNTREEDATADEL pfndel,
                PFNGETDATAVALSTR pfngetdatastr);
void tree_destroy(PTREE ptree);
BOOL tree_insert(PTREE ptree, void *pdata);
uint32_t tree_height(PTREE ptree);
uint32_t tree_node_count(PTREE ptree);
int32_t tree_dump(PTREE ptree,char *filename);


#endif
