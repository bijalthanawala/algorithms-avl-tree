#include <stdio.h>
#include "../avltree.h"

void getdatastr(void *pdata,char *dataval_strbuff,int strbufflen)
{
    snprintf(dataval_strbuff,
             strbufflen,
             "%c",
             (unsigned char)(int)pdata);
}
CMP_RESULT cmp_int(void *pdata_a, void *pdata_b)
{
    uint32_t a = (uint32_t) pdata_a;
    uint32_t b = (uint32_t) pdata_b;

    if(a > b) return CMP_RESULT_A_LARGER;
    if(a < b) return CMP_RESULT_A_SMALLER;

    return CMP_RESULT_A_B_SAME;
}

void main()
{

    PTREE ptree = tree_init(0,cmp_int,NULL,getdatastr);
    PTREE_ITER_OBJ ptreeiter = NULL;
    int i;
    char filename[100];
    int node_count = 0;

    for(i='z';i>='a';i--) {
       if(tree_insert(ptree, (void *)i))
         node_count++;

       printf("After inserting data=%d, tree ht = %d\n",
               i,tree_height(ptree));
    sprintf(filename,"avltree.%d.dot",i);   
    tree_dump(ptree,filename);
    }


    printf("AVL-TREE TEST CASE #1 "); 
    printf("(Node Count Test)");
    printf(":");
    if(tree_node_count(ptree) == node_count){
        printf(" PASSED");
    }
    else {
        printf(" FAILED");
    }
    printf("\n");

    ptreeiter = tree_iter_getobj(ptree, ITER_TYPE_PREORDER);
    if(ptreeiter) {
        printf("Preorder Result: ");
        while(i = (int)tree_iter_getnext(ptreeiter)) {
            printf("%c",i);
        }
        tree_iter_freeobj(ptreeiter);
    }
    printf("\n");

    tree_destroy(ptree);

}
