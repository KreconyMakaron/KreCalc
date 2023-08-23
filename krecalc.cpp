#include"krecalc.h"
#include <cstdio>

bool isNumber(const char c) {
    return ('0' <= c && c <= '9') || (c == '.');
}

bool isInteger(const f64 x) {
    f64 integerPart;
    return std::modf(x, &integerPart) == 0;
}

bool isText(const char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

std::vector<token> tokenizeString(std::string str) {
    std::vector<token> tokens;
    std::string tempString;
    token tempToken;
    
    for(i32 i = 0; i < (i32)str.size(); ++i) {
        switch(str[i]) {
            case ' ': continue; break;
            case Addition:          tempToken = {Addition, "+"}; break;
            case Subtraction:       tempToken = {Subtraction, "-"}; break;
            case Multiplication:    tempToken = {Multiplication, "*"}; break;
            case Division:          tempToken = {Division, "/"}; break;
            case Power:             tempToken = {Power, "^"}; break;
            case Modulo:            tempToken = {Modulo, "%"}; break;
            case Factorial:         tempToken = {Factorial, "!"}; break;
            case LeftBracket:       tempToken = {LeftBracket, "("}; break;
            case RightBracket:      tempToken = {RightBracket, ")"}; break;

            default:
                tempString = str[i];
                
                if(isText(str[i])) {
                    while(i+1 < (int)str.size() && isText(str[i+1])) tempString += str[++i];     //Text
                    tempToken = {Text, tempString};
                    break;
                }
                else if(isNumber(str[i])) {
                    while(i+1 < (int)str.size() && isNumber(str[i+1])) tempString += str[++i];   //Number
                    tempToken = {Number, tempString};
                    break;
                }
                
                tempToken = {None, ""}; // No Token Matches
                // throw Something
                break;
        }
        tokens.emplace_back(tempToken);
    }

    return tokens;
}

bool verifyAndFixTokens(std::vector<token> &tokens) {
    std::vector<int> v;
    bool* removed = new bool[tokens.size()];

    //Used a lambda just for fun
    auto check = [removed, tokens](int index, std::vector<tokenType> v) {
        for(tokenType elem : v) {
            if(removed[index] || tokens[index].type != elem) return 0;
            ++index;
        }
        return 1;
    };
    
    for(i32 i = 0; i+2 < (i32)tokens.size(); ++i) {
        if(check(i, {Number, Text, Number}) && tokens[i+1].value == "e") {
            tokens[i].value += tokens[i+1].value + tokens[i+2].value;
            removed[i+1] = 1; removed[i+2] = 1;
            i += 2;
        }
        else if((check(i, {Number, Text, Addition, Number}) || check(i, {Number, Text, Subtraction, Number})) && tokens[i+1].value == "e") {
            tokens[i].value += tokens[i+1].value + tokens[i+2].value + tokens[i+3].value;
            removed[i+1] = 1; removed[i+2] = 1; removed[i+3] = 1;
            i += 3;
        }
    }

    for(i32 i = 0; i+1 < (i32)tokens.size(); ++i) {
        if(check(i, {Number, LeftBracket}))
    }

    std::vector<token> new_tokens;
    for(i32 i = 0; i < (i32)tokens.size(); ++i) if(!removed[i]) new_tokens.emplace_back(tokens[i]);
    tokens = new_tokens;
    
    return 1;
}
















