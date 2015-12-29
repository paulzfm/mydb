#include "Tree.h"
#include "../lib/rapidjson/stringbuffer.h"
#include "../lib/rapidjson/prettywriter.h"
#include <iostream>

int main()
{
    rapidjson::Document doc;
    doc.SetObject();
    auto& allocator = doc.GetAllocator();

    Value val(Value::VALUE_INT, "100");
    Value val1(Value::VALUE_INT, "200");
    Col col("x", "y");
    UnonExpr neg(&val, Expr::OP_NEG);
    BinExpr bin(&val, &val1, Expr::OP_MUL);

    NullExpr nexpr(&col, BoolExpr::OP_IS_NULL);
    CompareExpr cexpr1(&col, &neg, BoolExpr::OP_NE);
    CompareExpr cexpr2(&col, &bin, BoolExpr::OP_GE);
    std::vector<Tree*> *set = new std::vector<Tree*>;
    for (int i = 0; i < 2; i++) {
        set->push_back(new CValue(new Value(Value::VALUE_INT, "233")));
    }
    InExpr iexpr(&col, set, BoolExpr::OP_NOT_IN);
    BetweenExpr bexpr(&col, &val, &val1, BoolExpr::OP_NOT_BETWEEN);
    ComplexExpr gp1(&cexpr1, &cexpr2, BoolExpr::OP_AND);
    ComplexExpr gp2(&iexpr, &bexpr, BoolExpr::OP_AND);
    ComplexExpr gp(&gp1,&gp2,BoolExpr::OP_OR);
    ComplexExpr root(&gp, &nexpr, BoolExpr::OP_OR);

    rapidjson::Value json = root.toJSONValue(allocator);

    rapidjson::StringBuffer buf;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> wr(buf);
    json.Accept(wr);
    std::cout << buf.GetString() << std::endl;

    Tree *node = toTreeNode(json);
    assert(node != nullptr);
    PrintWriter pw;
    node->printTo(pw);

    return 0;
}
