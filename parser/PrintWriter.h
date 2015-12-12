#ifndef PARSER_PRINT_WRITER_H
#define PARSER_PRINT_WRITER_H

#include <string>

class PrintWriter
{
public:
    PrintWriter() { depth = 0; }
    ~PrintWriter() {}

    void incIndent() { depth++; }
    void decIndent() { depth--; }

    void printIndent()
    {
        for (int i = 0; i < depth; i++) {
            printf("    ");
        }
    }

    void println(const char *line)
    {
        printIndent();
        printf("%s\n", line);
    }

    void println(const std::string& line)
    {
        printIndent();
        printf("%s\n", line.c_str());
    }

    void print(const char *c)
    {
        printf("%s", c);
    }

    void print(const std::string& c)
    {
        printf("%s", c.c_str());
    }

private:
    int depth;
};

#endif // PARSER_PRINT_WRITER_H
