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
    
    for(int i = 0; i < (int)str.size(); ++i) {
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

bool match(std::vector<token>::iterator it, std::vector<tokenType> matchType, std::vector<std::string> matchValue) {
    int k = 0;
    for(int i = 0; i < (int)matchType.size(); ++i) {
        if(it->type != matchType[i]) return 0;
        if(it->type == Text && it->value != matchValue[k++]) return 0;
        ++it;
    }
    return 1;   
}

void addToValue(std::vector<token> &t, std::vector<token>::iterator &it, int count) {
    std::vector<token>::iterator temp = it; it++;
    while(count--) {
        temp->value += it->value;
        it = t.erase(it);
    }
}

bool verifyAndFixTokens(std::vector<token> &t) {
    std::vector<token>::iterator it;
    
    //eg. 2e4
    it = t.begin();
    while(it != t.end()) {
        if(match(it, {Number, Text, Number}, {"e"})) addToValue(t, it, 2);
        else it++;
    }
    
    /* //eg. 2e+4 or 2e-4 */
    it = t.begin();
    while(it != t.end()) {
        if(match(it, {Number, Text, Addition, Number}, {"e"}) || match(it, {Number, Text, Subtraction, Number}, {"e"})) addToValue(t, it, 3);
        else it++;
    }

    //eg. 2(...) -> 2*(...)
    it = t.begin();
    while(it != t.end()) {
        if(match(it, {Number, LeftBracket}, {})) t.insert(++it, {Multiplication, "*"});
        else it++;
    }

    int bracketDiff = 0;
    for(int i = 0; i < (int)t.size(); ++i) {
        if(t[i].type == LeftBracket) bracketDiff++;
        if(t[i].type == RightBracket) bracketDiff--;
    }
    
    if(bracketDiff != 0) {
        //Throw Something
        return 0;
    }
    
    return 1;
}
















