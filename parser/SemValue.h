#include "Tree.h"

#define YYSTYPE_IS_DECLARED

typedef union YYSTYPE {
    int val;
    char sym;
    char ident[1024];
    Tree *tree;
    std::vector<Tree*> *tlist;
};
