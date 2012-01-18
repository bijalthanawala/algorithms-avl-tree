#include <stdio.h>
#include "../avltree.h"

void main()
{

    PTREE ptree = tree_init(NULL,NULL);

    tree_destroy(ptree);

}
