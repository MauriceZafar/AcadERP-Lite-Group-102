
// group-102

#include "grp_102_erp.h"

int main()
{
    ERP erp;
    std::string line;
    int lineNumber = 0;

    while (std::getline(std::cin, line))
    {
        lineNumber++;

        ErrorCode error = erp.processLine(line, lineNumber);

        if (error != OK)
        {
            std::cout << "ERROR " << lineNumber << " "
                      << erp.errorName(error) << "\n";
        }
    }

    return 0;
}
