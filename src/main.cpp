#include "UCI.h"

int main()
{
    String line;
    UniversalChessInterface UCI;
    while (std::getline(std::cin, line))
    {
        UCI.ExecuteInstructionLine(line);
        if (UCI.StopUCI())
            break;
    }
}
