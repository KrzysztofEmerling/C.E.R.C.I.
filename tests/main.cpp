#include <string>

#include "PerftTests.h"
#include "zHashCalculationTests.h"

int main(int argc, char *argv[])
{
    bool quickMode = false;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "-q")
        {
            quickMode = true;
            break;
        }
    }

    if (quickMode)
    {
        PerftTests::RunQuickTests();
        zHashCalculationTests::RunTests();
    }
    else
    {
        PerftTests::RunLongTests();
        zHashCalculationTests::RunTests();
    }

    return 0;
}
