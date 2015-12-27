#include "Engine.h"

#include <iostream>
#include <fstream>
#include <string>

std::string Engine::execute(std::string& input)
{
    if (driver.parseString(input)) {
        driver.ast->accept(&visitor);
        return visitor.msg;
    }

    return "";
}


void InteractiveEngine::run()
{
    std::cout << "Welcome to mydb!\n";
    std::string buf, buf1;
    while (true) {
        std::cout << "mydb> ";
        std::getline(std::cin, buf);
        buf = trim(buf);
        if (buf == "quit" || buf == "exit") {
            std::cout << "Bye!\n";
            return;
        } else if (buf.back() != ';' || buf.length() == 0) { // non terminate
            while (true) {
                std::cout << "   -> ";
                std::getline(std::cin, buf1);
                buf1 = trim(buf1);
                buf += " ";
                buf += buf1;
                if (buf1.length() > 0 && buf1.back() == ';') { // terminate
                    break;
                }
            }
        }

        std::cout << execute(buf);
    }
}

void StringEngine::run()
{
    std::cout << execute(str);
}
