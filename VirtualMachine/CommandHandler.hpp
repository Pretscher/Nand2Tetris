#include <map>
#include "Parser.hpp"

class CommandHandler {
public:
    CommandHandler(Parser& parser) : parser(parser) {

    }
    vector<string> translate();
private:
    Parser& parser;

    vector<string> lines;
    void print(string line) {
        lines.push_back(line);
    }

    void print(vector<string> lines) {
        this->lines.insert(this->lines.end(), lines.begin(), lines.end());
    }

    void translateCommand(vector<string> tokens);

    void push(vector<string> tokens);
    void pop(vector<string> tokens);
    //pushes for local, argument, this, that, static, temp
    void pushSymbol(vector<string> tokens);
    //converts symbols to their corresponding Hack Assembly pointers (e.g. local to LCL)
    string symbolToSymbolPointer(vector<string> tokens);

    /*pops the TOS and puts it in the D-Register.You can perform an operation on it, like "D=-D", before
    it gets pushed to the stack again*/
    void changeTopOfStack(string operation);
    void pushConstantToStack(string constant);

    //project 7---------------------------------------------------------------------

    //increments the sp and adds the performed operations to the input-vector
    void incrementSP();
    //decrements the sp and adds the performed operations to the input-vector
    void decrementSP();
    //pops the TOS and saves it in the register. 
    void popToD();
    void popToA();
    //pushes the Register to the stack
    void pushD();
    void pushA();
    //Writes TOS into RAM[D]
    void writeStackToRamAtD();
    /*Pops the Top 2 Elements from the Stack, the First one is in D, the second one is in A.You can now perform
    operations like "D=A-D" on these Elements. Afterwards, D is pushed to the Stack*/
    void twoToOneElementsWithOperation(vector<string> operation);
    int nameCounter = 0;//used for different names for all the labels used
    /*Takes the top 2 Elements of the Stack, and compares them via the string provided.Possible are
    "JGT", "JLT" and "JEQ"*/
    void compare(string comparison);


    //project 8-----------------------------------------------------------------------

    void gotoLabel(string label);
    void conditionalGotoLabel(string label);

    void call(string functionName, string argumentCount);
    void function(string functionName, string localVariableCount);
    void returnFromFunction(string functionName);
    string currentFunctionName;
    int functionsCalled = 0;//used to differentiate return labels

    //maps the class and the index of the static variable to the name of a variable declared with @...
    //Like this we can give the responsibility of distributing ram adresses for static variables to the assembler!
    vector<string> staticInitialisations();

    string getStaticName(string index) {
        return currentClass + ".static" + index;
    }

    void actualizeCurrentClass() {
        if (parser.isLineFunctionStart() == true) {
            int dotPos = parser.getLine().find(".");
            if (dotPos != string::npos) {//if there's a dot in the function name, its from a class
                currentClass = parser.getLine().substr(9, dotPos - 9);//from start of name (index 9) to dot
            }
        }
    }
    string currentClass;
};
