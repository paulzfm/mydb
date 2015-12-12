#include "Tree.h"

#define YYSTYPE_IS_DECLARED

typedef union YYSTYPE {
    char sym; // keywords
    char ident[1024]; // identifier
    Value *value; // Value
    Tree *tree; // Tree
    std::vector<Tree*> *tlist; // [Tree]
    int integer;
    std::vector<int> *ilist;
    std::vector<const char*> *slist;
};
