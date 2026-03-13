#pragma once
#include <string>
#include <vector>
#include "Utility.hpp"
using namespace std;

class Parser {
public:
    Parser(const char* file) : lines(readInput(file)) {
        currentLineIndex = -1;//we do it like that so that we dont have to check in two places if lines[0] is out of bounds
        this->advance();
    }

    bool hasMoreLines() const {
        return currentLineIndex < lines.size();
    }

    void advance();

    string getLine() const {
        return currentLine;
    }

    int getLineNumber() const {
        return currentLineIndex;
    }

    bool isLineFunctionStart() {
        return currentLine.substr(0, 8) == "function";
    }

    vector<string> readFunction() {
        vector<string> out;
        if (isLineFunctionStart() == false) {
            std::cout << "current line is not the start of a function!";
            std::exit(0);
        }
        int savedLine = currentLineIndex;

        //if the program returns (ends in an infinite loop, Thats needed for sys.init), we can stop reading the function. 
        while (currentLine != "return" && !nowEndsInInfiniteLoop()) {
            out.push_back(currentLine);
            advance();
        }
        out.push_back(currentLine);
        gotoLine(savedLine);
        return out;
    }

    bool nowEndsInInfiniteLoop() {
        string label;
        if (currentLine.substr(0, 5) == "label") {
            label = currentLine.substr(6);
            if (lines.size() > currentLineIndex + 1 && lines[currentLineIndex + 1] == "goto " + label) {
                return true;
            }
        }
        return false;
    }

    vector<string> getAllLines() {
        return lines;
    }

    void gotoLine(int line) {
        currentLineIndex = line;
        currentLine = lines[currentLineIndex];
    }

    void reset();
    void writeFile(vector<string> lines) const;

    string makeUniform(string line) const;
    vector<string> getTokens();

    vector<string> readSingleFile(string file);
    vector<string> readFolder(string path);

    bool hasMultipleFiles() {
        return multipleFiles;
    }
private:
    bool multipleFiles = false;
    string fileNameToWrite;
    vector<string> lines;
    int currentLineIndex;
    string currentLine;
    vector<string> readInput(const char* file);
};