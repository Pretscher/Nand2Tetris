#include <iostream>
#include <algorithm>
#include <fstream> // for file-access
#include "Parser.hpp"
#include "Utility.hpp"
#include <filesystem>
namespace fs = std::filesystem;
using namespace std;

void Parser::advance() {
    currentLineIndex++;
    if (currentLineIndex < lines.size()) {//we may advance if there are no more lines, but then the string will be empty.
        currentLine = lines[currentLineIndex];
    }
    else {
        currentLine = "";
    }
}

void Parser::reset() {
    currentLineIndex = -1;
    this->advance();
}

vector<string> Parser::readInput(const char* path) {
    string pathStr = string(path);
    if (pathStr.length() >= 3) {
        std::string lastThreeChars = pathStr.substr(pathStr.length() - 3);
        if (lastThreeChars == ".vm") {
            return readSingleFile(pathStr);
        }
    }
    multipleFiles = true;
    return readFolder(pathStr);
}

vector<string> Parser::readSingleFile(string file) {
    vector<string> lines;
    fileNameToWrite = file;
    fileNameToWrite.erase(fileNameToWrite.end() - 3, fileNameToWrite.end());
    std::ifstream infile(file); //open the file
    if (infile.is_open() && infile.good()) {
        string line = "";
        while (getline(infile, line)) {
            line = makeUniform(line);//remove unneccesary whitespace and comments

            if (line != "") {//then the line would have been whitespace or a comment
                lines.push_back(line);
            }
        }
    }
    return lines;
}

vector<string> Parser::readFolder(string path) {
    vector<string> lines;
    vector<string> sys;//sys file is inserted at the beginning of lines
    for (const auto& entry : fs::directory_iterator(path)) {
        //we want to name the files after the cmps for easier testing with the cpu emulator
        if (entry.path().extension() == ".cmp") {
            string fileName = entry.path().string();
            fileName.erase(fileName.end() - 4, fileName.end());
            fileNameToWrite = fileName;
        }
        if (entry.path().extension() == ".vm") {
            //check if the file ends with Sys.vm. If so, put this at the start for readability of the assembly output
            bool isSys = false;
            if (entry.path().string().substr(entry.path().string().length() - 6) == "Sys.vm") {
                isSys = true;
            }

            std::ifstream file(entry.path());
            std::string line;
            while (std::getline(file, line)) {
                line = makeUniform(line);//remove unneccesary whitespace and comments
                if (line != "") {//then the line would have been whitespace or a comment

                    if (!isSys) {
                        lines.push_back(line);
                    }
                    else {
                        sys.push_back(line);
                    }
                }
            }

            file.close();
        }
    }
    lines.insert(lines.begin(), sys.begin(), sys.end());
    return lines;
}

void Parser::writeFile(vector<string> lines) const {
    string fileName = fileNameToWrite;
    fileName.append(".asm");
    std::ofstream outputFile(fileName);

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

//removes unnecessary whitespace and all comments from line
string Parser::makeUniform(string line) const {
    bool lastWasWhitespace = true;//set to true because if we begin with whitespace, we want to delete that
    for (int i = 0; i < line.size(); i++) {
        string sub = line.substr(i, 1);
        if (Utility::isWhitespace(sub)) {
            if (lastWasWhitespace == true) {
                line.erase(i, 1);
                i--;
                continue;
            }
            lastWasWhitespace = true;
        }
        else {
            lastWasWhitespace = false;
        }
    }

    //erase comments (everything after a //)
    int eraseFromHere = line.find("//");
    if (eraseFromHere != std::string::npos) {
        line.erase(eraseFromHere);
    }
    return line;
}

vector<string> Parser::getTokens() {
    if (currentLine == "") return vector<string>();
    vector<string> out;
    string currentWord;
    for (int i = 0; i < currentLine.size(); i++) {
        if (currentLine.at(i) == ' ') {
            out.push_back(currentWord);
            currentWord = "";
            continue;
        }
        currentWord.push_back(currentLine.at(i));
    }
    out.push_back(currentWord);
    return out;
}