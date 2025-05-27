#include <string>

#include "PerftTests.h"

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
    }
    else
    {
        PerftTests::RunLongTests();
    }

    return 0;
}
