#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;

class Utility {
public:
    static bool isUpperCase(char c) {
        return c >= 65 && c <= 90;
    }

    static bool isLowerCase(char c) {
        return c >= 97 && c <= 122;
    }

    static bool isLetter(char c) {
        return isLowerCase(c) || isUpperCase(c);
    }

    static bool isNumber(char c) {
        return c >= 48 && c <= 57;
    }

    static bool isWhitespace(const std::string& str) {
        for (char c : str) {
            if (!std::isspace(c)) {
                return false;
            }
        }
        return true;
    }

    static string toBinary(int number) {
        string binary;
        for (int power = 0; power < 15; power++) {//maximum 15 bits for the number, because we need 1 bit to signal that it's an a-instruction
            binary.insert(0, std::to_string(number % 2));
            number /= 2;
        }
        return binary;
    }

    static void append(vector<string>& vec, vector<string>& append) {
        vec.insert(vec.end(), append.begin(), append.end());
    }

    static void append(vector<string>& vec, vector<string>&& append) {
        vec.insert(vec.end(), append.begin(), append.end());
    }
};