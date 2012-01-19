#include <stdio.h>
#include "../avltree.h"


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

    PTREE ptree = tree_init(0,cmp_int,NULL);
    int i;

    for(i=100;i<110;i++) {
       tree_insert(ptree, (void *)i);
       printf("After inserting data=%d, tree ht = %d\n",
               i,tree_height(ptree));
    }

    tree_dump(ptree);

    tree_destroy(ptree);

}
