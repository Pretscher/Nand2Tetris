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
        return currentLineIndex + 1 < (int) lines.size();
    }

    void advance();

    /**
     * @return int 0 if a-instruction, 1 if l-instruction, 2 c-instruction, 3 comment or white space
     */
    int instructionType() const;

    bool isSymbol() const {
        return instructionType() == 0 && Utility::isLetter(currentLine.at(1));
    }

    string symbol() const {
        string out = currentLine;
        return out.erase(0);
    }

    string dest() const;

    string comp() const;

    string jump() const;

    string getLine() const {
        return currentLine;
    }

    int getLineNumber() const {
        return currentLineIndex;
    }

    void reset();
    void writeFile(string inputFileName, vector<string> lines) const;
private:
    vector<string> lines;
    int currentLineIndex;
    string currentLine;
    vector<string> readInput(const char* file) const;
};