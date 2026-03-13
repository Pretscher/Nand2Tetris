#include <iostream>
#include <algorithm>
#include <fstream> // for file-access
#include "Parser.hpp"
using namespace std;

void Parser::advance() {
    if (currentLineIndex >= -1 && this->hasMoreLines()) {//-1 is the starting line index
        currentLine = lines[++currentLineIndex];
    }
    else {
        std::cout << "Line number " << currentLineIndex << " is out of bounds!";
        std::exit(0);
    }
}

/**
 * @return int 0 if a-instruction, 1 if l-instruction, 2 c-instruction
 */
int Parser::instructionType() const {
    if (currentLine.at(0) == '@') {
        return 0;
    } 
    if (currentLine.at(0) == '(') {
        return 1;//label
    }
    return 2;
}

string Parser::dest() const {
    string out;
    bool equalSignFound = false;//if we dont find an equal sign, there is no destination
    for (char c : currentLine) {
        if (c == '=') equalSignFound = true;
        if (c == '=' || c == ';') break;
        out.push_back(c);
    }
    if (equalSignFound == false) {
        return "null";
    }
    return out;
}

string Parser::comp() const {
    string out;
    bool equalSignFound = false;
    for (char c : currentLine) {
        if (equalSignFound && c == ';') return out;
        if (equalSignFound == true) out.push_back(c);
        if (c == '=') equalSignFound = true;
    }
    //if we didnt find an equalsign, comp is the entire string until ';' or the end
    if (equalSignFound == false) {
        for (char c : currentLine) {
            if (c == ';') return out;
            out.push_back(c);
        }
    }
    return out;//only reached if jmp is empty, so no ';'
}

string Parser::jump() const {
    string out;
    bool write = false;
    for (char c : currentLine) {
        if (write == true) out.push_back(c);
        if (c == ';') write = true;
    }
    if (out.empty()) return "null";
    return out;
}


void Parser::reset() {
    currentLineIndex = -1;
    this->advance();
}

vector<string> Parser::readInput(const char* file) const {
    vector<string> lines;
    std::ifstream infile(file); //open the file

    if (infile.is_open() && infile.good()) {
        string line = "";
        while (getline(infile, line)) {
            
            line.erase(std::remove_if(line.begin(), line.end(), [](unsigned char c) { return std::isspace(c); }), line.end());//remove whitespace

            //erase comments (everything after a //)
            int eraseFromHere = line.find("//");
            if (eraseFromHere != std::string::npos) {
                line.erase(eraseFromHere);
            }
            if (line.empty() == false) {//dont push back comment/whitespace lines
                lines.push_back(line);
            }
        }
    }
    else {
        cout << "Failed to open file..";
        std::exit(0);
    }
    return lines;
}

void Parser::writeFile(string inputFileName, vector<string> lines) const {
    inputFileName.erase(inputFileName.size() - 3);
    inputFileName.append("hack");
    std::ofstream outputFile(inputFileName);

    if (outputFile.is_open()) {
        for (string line : lines) {
            outputFile << line << std::endl;
        }
        outputFile.close();
    }
    else {
        std::cout << "Unable to create the file." << std::endl;
    }
}
