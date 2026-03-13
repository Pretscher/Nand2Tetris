#include <iostream>
#include <map>
#include "Parser.hpp"
#include "InstructionHandler.hpp"
using namespace std;


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Incorret amount of arguments. Exiting...";
        return -1;
    }
    Parser parser(argv[1]);

    InstructionHandler handler(parser);
    vector<string> instructions = handler.translate();

    for (string instruction : instructions) {
        std::cout << instruction << "\n";
    }

    parser.writeFile(argv[1], instructions);
    return 0;
}