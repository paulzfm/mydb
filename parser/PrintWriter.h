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

    void println(const char *line)
    {
        for (int i = 0; i < depth; i++) {
            printf("    ");
        }
        printf("%s\n", line);
    }

    void println(const std::string& line)
    {
        for (int i = 0; i < depth; i++) {
            printf("    ");
        }
        printf("%s\n", line.c_str());
    }

private:
    int depth;
};

#endif // PARSER_PRINT_WRITER_H
