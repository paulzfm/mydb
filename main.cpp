#include <stdio.h>
#include "parser/Driver.h"

int main(int argc, char *argv[])
{
    if (argc <= 1) {
        return 0;
    }

    Driver driver;
	driver.parse(argv[1]);
    PrintWriter pw;
    driver.ast->printTo(pw);

    return 0;
}
