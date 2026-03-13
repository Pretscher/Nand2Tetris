#include <iostream>
#include <map>
#include "Parser.hpp"
#include "CommandHandler.hpp"
using namespace std;


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Incorret amount of arguments. Exiting...";
        return -1;
    }
    Parser parser(argv[1]);
    
   // string testPath = "pong/Pong.asm";
   // Parser parser(testPath.c_str());
    CommandHandler handler(parser);
    vector<string> instructions = handler.translate();
    
    
    for (string instruction : instructions) {
        std::cout << instruction << "\n";
    }

    parser.writeFile(instructions);
  //  parser.writeFile(testPath, instructions);
    return 0;
}