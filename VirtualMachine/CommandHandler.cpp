#include "CommandHandler.hpp"


vector<string> CommandHandler::translate() {
    //always start with those instructrions
    print({
        "@256",
        "D=A",
        "@SP",
        "M=D",
    });
    print(staticInitialisations());

    bool init = false;
    while (parser.hasMoreLines()) {
        if (parser.getTokens().size() > 0) {//just skip empty lines (or comment lines that were converted to empty lines earlier)
            vector<string> currentInstructions;
            if (init == false && parser.hasMultipleFiles() == true) {//if multiple files, add "call Sys.init 0" command
                init = true;
                 translateCommand({ "call", "Sys.init", "0" });//call sys.init at the start
            }
            else {
                actualizeCurrentClass();//we always need to know in which class we are to get the correct static segment, if a static var is used
                translateCommand(parser.getTokens());
                parser.advance();
            }
        }

    }
    return lines;
}

void CommandHandler::translateCommand(vector<string> tokens) {
    if (tokens.size() > 0) {
        string command = tokens.at(0);
        if (command == "push") {
            return push(tokens);
        }
        else if (command == "pop") {
            return pop(tokens);
        }
        else if (command == "add") {
            return twoToOneElementsWithOperation({ "D=D+A" });
        }
        else if (command == "sub") {
            return twoToOneElementsWithOperation({ "D=A-D" });
        }
        else if (command == "and") {
            return twoToOneElementsWithOperation({ "D=D&A" });
        }
        else if (command == "or") {
            return twoToOneElementsWithOperation({ "D=D|A" });
        }
        else if (command == "eq") {
            return compare("JEQ");
        }
        else if (command == "gt") {
            return compare("JGT");
        }
        else if (command == "lt") {
            return compare("JLT");
        }
        else if (command == "neg") {
            return changeTopOfStack("D=-D");
        }
        else if (command == "not") {
            return changeTopOfStack("D=!D");
        }
        else if (command == "label") {
            return print("(" + tokens[1] + ")");
        }
        else if (command == "goto") {
            return gotoLabel(tokens[1]);
        }
        else if (command == "if-goto") {
            return conditionalGotoLabel(tokens[1]);
        }
        else if (command == "call") {
            return call(tokens[1], tokens[2]);
        }
        else if (command == "function") {
            return function(tokens[1], tokens[2]);
        }
        else if (command == "return") {
            return returnFromFunction(currentFunctionName);//currentFunctionName is always set when we call
        }
        else {
            std::cout << "Invalid command. Exiting..." << std::endl;
            std::exit(0);
        }
    }
}

void CommandHandler::changeTopOfStack(string operation) {
    popToD();
    print(operation);
    pushD();
}

/* VM command: push {segment} { number }
*  Renember to remove the push-element from the tokens before calling!
*/
void CommandHandler::push(vector<string> tokens) {
    if (tokens[1] == "constant") {
        pushConstantToStack(tokens[2]);
    }
    else {
        pushSymbol(tokens);
    }
}

void CommandHandler::pushConstantToStack(string constant) {
    print({
        "@" + constant,
        "D=A"
    });
    pushD();
}

void CommandHandler::pushSymbol(vector<string> tokens) {
    if (tokens[1] == "static") {
        print({
            "@" + getStaticName(tokens[2]),
            "D=M"
        });
    }
    else if (tokens[1] == "temp") {
        int address = 5;
        address += std::stoi(tokens[2]);
        print({
            "@" + std::to_string(address),
            "D=M"
        });
    }
    else if (tokens[1] == "pointer") {
        string symbol = "THIS";
        if (tokens[2] == "1") symbol = "THAT";
        print({
            "@" + symbol,
            "D=M"
        });
    }
    else {
        string symbol = symbolToSymbolPointer(tokens);
        string offset = tokens[2];
        print({
            "@" + offset,//D = i
            "D=A",
            "@" + symbol,
            "A=D+M",//M is the adress that the symbol points to
            "D=M"//D is now the content of the register the symbol points to
        });
    }
    pushD();
}

void CommandHandler::pop(vector<string> tokens) {
    if (tokens[1] == "static") {
        popToD();
        print({
            "@" + getStaticName(tokens[2]),
            "M=D"
        });
    }
    else if (tokens[1] == "temp") {
        int address = 5;
        address += std::stoi(tokens[2]);
        popToD();
        print({
            "@" + std::to_string(address),
            "M=D"
        });
    }
    else if (tokens[1] == "pointer") {
        string symbol = "THIS";
        if (tokens[2] == "1") symbol = "THAT";
        popToD();
        print({
            "@" + symbol,
            "M=D"
        });
    }
    else {
        string symbol = symbolToSymbolPointer(tokens);
        string offset = tokens[2];
        print({
            "@" + symbol,
            "D=M",//D=the adress of that the symbol points to, so if THIS points to 3000, this is 3000
            "@" + offset,
            "D=D+A",//D = Adress the symbol points to + i
            "@13",//we need this memory segment to save the adress, because we need to reuse A to get the address in the stack and cant access the ram through D
            "M=D"
        });
        popToD();
        writeStackToRamAtD();
    }
}



string CommandHandler::symbolToSymbolPointer(vector<string> tokens) {
    if (tokens[1] == "local") return "LCL";
    else if (tokens[1] == "argument") return "ARG";
    else if (tokens[1] == "this" || (tokens[1] == "pointer" && tokens[2] == "0")) return "THIS";//pointer 0 and pointer 1 will be directed correctly this way
    else if (tokens[1] == "that" || (tokens[1] == "pointer" && tokens[2] == "1")) return "THAT";
    return "ERROR: invalid argument";
}

void CommandHandler::writeStackToRamAtD() {
    print({
        "@13",
        "A=M",//Set A to SymbolPointer + i
        "M=D"//Ram[symbolPointer+i]=D
    });
}

void CommandHandler::incrementSP() {
    print("@SP");
    print("M=M+1");
}

void CommandHandler::decrementSP() {
    print("@SP");
    print("M=M-1");
}

void CommandHandler::popToD() {
    decrementSP();
    print({
        "@SP",
        "A=M",
        "D=M"
    });
}

void CommandHandler::popToA() {
    decrementSP();
    print({
        "@SP",
        "A=M",
        "A=M"
    });
}

void CommandHandler::pushD() {
    print({
        "@SP",
        "A=M",
        "M=D"
        });
    incrementSP();
}

void CommandHandler::pushA() {
    print({
        "D=A",
        "@SP",
        "A=M",
        "M=D"
    });
    incrementSP();
}

//when inserting the operation, D will have the top element of the stack in it, and A the second to top element of
//the stack. After the operation, d will be pushed to the stack.
void CommandHandler::twoToOneElementsWithOperation(vector<string> operation) {
    popToD();
    print({
        "@SP",
        "A=M-1",//A = SP - 1
        "A=M"//A = Ram[SP]

        // For Exmplae add would put "D=D+A" in this place
    });
    print(operation);
    decrementSP();
    pushD();
}

void CommandHandler::compare(string instruction) {
        twoToOneElementsWithOperation(
        { "D=A-D",
          "@TRUE" + std::to_string(nameCounter),
          "D;" + instruction,
          "(FALSE" + std::to_string(nameCounter) + ")",
          "D=0",
          "@END" + std::to_string(nameCounter),
          "0,JMP",
          "(TRUE" + std::to_string(nameCounter) + ")",
          "D=-1",
          "(END" + std::to_string(nameCounter++) + ")"
        }
    );
}

//projekt 8------------------------------------------------------------------------------

void CommandHandler::gotoLabel(string label) {
    print({
        "@" + label,
        "0;JMP"
    });
}

void CommandHandler::conditionalGotoLabel(string label) {
    popToD();
    print({
        "@" + label,
        "D;JNE"
    });
}

void CommandHandler::call(string functionName, string argumentCount) {
    currentFunctionName = functionName;
    //push return adrfess
    print("@" + functionName + "_return" + std::to_string(functionsCalled));//label is below all the pushes
    pushA();
    //push registers
    print("@LCL");//we cant use vector print here because c++ somehow cant differentiate a vector of two strings and a string...
    print("D=M");
    pushD();
    print("@ARG");
    print("D=M");
    pushD();
    print("@THIS");
    print("D=M");
    pushD();
    print("@THAT");
    print("D=M");
    pushD();
    //ARG = SP-nArgs-5
    print({
        "@SP",
        "D=M",
        "@5",
        "D=D-A",
        "@" + argumentCount,
        "D=D-A",
        "@ARG",
        "M=D" 
    });
    //LCL = SP
    print({
        "@SP",
        "D=M",
        "@LCL",
        "M=D"
    });

    //goto function
    gotoLabel(functionName + "_start");
    //return label
    print("(" + functionName + "_return" + std::to_string(functionsCalled++) + ")");
}

void CommandHandler::function(string functionName, string localVariableCount) {
    print({
        "(" + functionName + "_start)",
        "D=0" 
    });//so that we always push 0 in the following code
    for (int i = 0; i < std::stoi(localVariableCount); i++) {
        pushD();
    }
}

void CommandHandler::returnFromFunction(string functionName) {
    print({
        "@LCL",
        "D=M",//D = address in LCL
        "@14",//Frame in 14 (popToD uses 13)
        "M=D",//frame = LCL
        "@5",
        "A=D-A",//A = frame - 5
        "D=M",//D = *(frame -5)
        "@15",//return address into R15
        "M=D"
    });
    //pop to *ARG
    popToD();
    print({
        "@ARG",
        "A=M",
        "M=D",

        //revert registers
        //SP=ARG+1
        "@ARG",
        "D=M+1",
        "@SP",
        "M=D",
        //THAT = *(frame-1)
        "@14",
        "D=M",
        "@1",
        "A=D-A",
        "D=M",
        "@THAT",
        "M=D",
        //THIS = *(frame-2)
        "@14",
        "D=M",
        "@2",
        "A=D-A",
        "D=M",
        "@THIS",
        "M=D",
        //ARG = *(frame-3)
        "@14",
        "D=M",
        "@3",
        "A=D-A",
        "D=M",
        "@ARG",
        "M=D",
        //LCL = *(frame-4)
        "@14",
        "D=M",
        "@4",
        "A=D-A",
        "D=M",
        "@LCL",
        "M=D",
        //return
        "@15",
        "A=M",
        "0,JMP"
    });
}

vector<string> CommandHandler::staticInitialisations() {
    //this map will hold how many static variables the class needs. If static variables 1 and 3 are used, we create variables 1, 2 and 3, because
    //it seems the user wants the second static variable to exist, and maybe he wants to use them in assembly code that he mixes with the vm-translated
    //code. 
    map<string, int> maxStaticIndexForClass;
    int oldLine = parser.getLineNumber();
    while (parser.hasMoreLines()) {
        //for the collective of all functions of the class, find the highest index of static that is read or written. That, plus 1, is the length
        //of that static segment that should be dedicated to this class.
        if (parser.isLineFunctionStart()) {
            vector<string> functionLines = parser.readFunction();

            int highestStaticVariableIndex = -1;
            int currentStaticVariableIndex = -1;
            for (string line : functionLines) {
                if (line.substr(0, 11) == "push static") {//indices 0 to 10
                    currentStaticVariableIndex = std::stoi(line.substr(12, 12));//index 12
                }
                else if (line.substr(0, 10) == "pop static") {//indices 0 to 9
                    currentStaticVariableIndex = std::stoi(line.substr(11, 11));//index 11
                }

                if (currentStaticVariableIndex > highestStaticVariableIndex) {
                    highestStaticVariableIndex = currentStaticVariableIndex;
                }
            }

            actualizeCurrentClass();
            if (maxStaticIndexForClass[currentClass] < highestStaticVariableIndex + 1) {
                maxStaticIndexForClass[currentClass] = highestStaticVariableIndex + 1;
            }
        }
        parser.advance();
    }
    parser.gotoLine(oldLine);

    vector<string> out;
    for (auto entry : maxStaticIndexForClass) {
        for (int i = 0; i < entry.second; i++) {
            out.push_back("@" + entry.first + ".static" + std::to_string(i));
            out.push_back("M=0");//init the variables to 0
        }
    }
    return out;
}