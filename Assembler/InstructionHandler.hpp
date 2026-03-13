#include <map>
#include "Parser.hpp"

class InstructionHandler {
public:
    InstructionHandler(Parser& parser) : parser(parser) {
        initJumpMap();
        initDestMap();
        initCompMap();
    }
    
    vector<string> translate();

private:
    //the Instructionhandler is highly coupled with the parser for performance reasons. Else we would have to parser
    //the entire document and then send it to the instructionhandler, which would have to go through it again. Also,
    //the Parser isn't devoid of logic that makes more sense in it but is useful here: e.g. getSymbol(), getComp() etc.
    Parser& parser;

    void generateLabels();

    string handleAInstruction();//not const because it can call handleSymbol
    string handleNumber() const;
    map<string, string> symbols;
    unsigned int customSymbols = 16;//amount of symbols that are either prefedined or added through @Symbol
    string handleSymbol();//not const because it can add to the symbol table

    string handleCInstruction() const;
    string getComp() const;

    map<string, string> jumpMap;
    void initJumpMap();

    map<string, string> destMap;
    void initDestMap();

    map<string, string> compMap;
    void initCompMap();


};
