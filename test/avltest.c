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
    int i;
    char filename[100];

    for(i='z';i>='a';i--) {
       tree_insert(ptree, (void *)i);
       printf("After inserting data=%d, tree ht = %d\n",
               i,tree_height(ptree));
    sprintf(filename,"avltree.%d.dot",i);   
    tree_dump(ptree,filename);
    }


    tree_destroy(ptree);

}
