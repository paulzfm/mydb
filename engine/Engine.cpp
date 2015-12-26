#include "Engine.h"

#include <iostream>
#include <string>

std::string Engine::execute(std::string& input)
{
    return "done";
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
        } else if (buf.back() != ';') { // non terminate
            while (true) {
                std::cout << "   -> ";
                std::getline(std::cin, buf1);
                buf1 = trim(buf1);
                buf += " ";
                buf += buf1;
                if (buf1.back() == ';') { // terminate
                    break;
                }
            }
        }

        std::cout << execute(buf) << "\n";
    }
}

int main()
{
    InteractiveEngine ie;
    ie.run();

    return 0;
}
