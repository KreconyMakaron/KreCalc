#include"krecalc.h"
#include <cstdio>

bool isNumber(const char c) {
    return ('0' <= c && c <= '9') || (c == '.');
}

bool isText(const char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

bool isInteger(const f64 x) {
    f64 integerPart;
    return std::modf(x, &integerPart) == 0;
}

bool isInteger(const f64 x, f64 &integerPart) {
    return std::modf(x, &integerPart) == 0;
}

f64 factorial(std::string str) {
    f64 cnt = stold(str), res = 1;
    if(!isInteger(cnt, cnt)) throw "Factorial cannot be computed for non-integers";
    for(int i = 2; i <= (int)cnt; ++i) res *= i;
    return res;
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
                
                //Get all characters in Text
                if(isText(str[i])) {
                    while(i+1 < (int)str.size() && isText(str[i+1])) tempString += str[++i];
                    tempToken = {Text, tempString};
                    break;
                }
                //Get all digits of a number
                else if(isNumber(str[i])) {
                    while(i+1 < (int)str.size() && isNumber(str[i+1])) tempString += str[++i];
                    tempToken = {Number, tempString};
                    break;
                }
                
                throw tempString + " is not a valid token";
                break;
        }
        tokens.emplace_back(tempToken);
    }

    return tokens;
}

//Match a pattern
bool match(std::vector<token>::iterator it, std::vector<tokenType> matchType, std::vector<std::string> matchValue) {
    int k = 0;
    for(int i = 0; i < (int)matchType.size(); ++i) {
        if(it->type != matchType[i]) return 0;
        if(it->type == Text && it->value != matchValue[k++]) return 0;
        ++it;
    }
    return 1;   
}

void addToValueAndRemove(std::vector<token> &t, std::vector<token>::iterator &it, int count) {
    std::vector<token>::iterator temp = it; it++;
    while(count--) {
        temp->value += it->value;
        it = t.erase(it);
    }
}

void verifyAndFixTokens(std::vector<token> &t) {
    std::vector<token>::iterator it;
    
    //eg. 2e4
    it = t.begin();
    while(it != t.end()) {
        if(match(it, {Number, Text, Number}, {"e"})) addToValueAndRemove(t, it, 2);
        else it++;
    }
    
    /* //eg. 2e+4 or 2e-4 */
    it = t.begin();
    while(it != t.end()) {
        if(match(it, {Number, Text, Addition, Number}, {"e"}) || match(it, {Number, Text, Subtraction, Number}, {"e"})) addToValueAndRemove(t, it, 3);
        else it++;
    }

    //eg. 2(...) -> 2*(...)
    it = t.begin();
    while(it != t.end()) {
        if(match(it, {Number, LeftBracket}, {})) t.insert(++it, {Multiplication, "*"});
        else it++;
    }

    //Factorial
    it = t.begin();
    while(it != t.end()) {
        if(match(it, {Number, Factorial}, {})) {
            it->type = Factorial; it++;
            it = t.erase(it);
        }
        else it++;
    }

    //Negative Numbers
    it = t.begin();
    
    // First Token is -x
    if(match(it, {Subtraction, Number}, {})) { 
        it = t.erase(it);
        it->value = "-" + it->value;
        it++;
    }
    while(it != t.end()) {
        if(it->isOperator()) {
            it++;
            if(match(it, {Subtraction, Number}, {})) {
                it = t.erase(it);
                it->value = "-" + it->value;
                it++;
            }
        }
        else it++;
    }

    int bracketDiff = 0;
    for(int i = 0; i < (int)t.size(); ++i) {
        if(bracketDiff < 0) throw "Not all brackets are closed";
        if(t[i].type == LeftBracket) bracketDiff++;
        if(t[i].type == RightBracket) bracketDiff--;
    }
    if(bracketDiff != 0) throw "Not all brackets are closed";

    for(int i = 0; i+1 < (int)t.size(); ++i) {
        if(t[i].isOperator() == t[i+1].isOperator() == 1) throw "Tokens " + t[i].value + " and " + t[i+1].value + " cannot be next to each other";
    }

    for(int i = 0; i < (int)t.size(); ++i) if(t[i].type == Text || t[i].type == None) throw t[i].value + " is not a valid token";
}

std::vector<buffToken> buildOrderTable(std::vector<token> tokens) {
    std::vector<buffToken> order;
    i32 bracketAdditional = 0, orderNum = 0;
    
    for(token t : tokens) {
        if(t.type == LeftBracket) bracketAdditional += BRACKET_MULTIPLIER;
        else if(t.type == RightBracket) bracketAdditional -= BRACKET_MULTIPLIER;
        else if(t.type == Number || t.type == Factorial) order.push_back({t, -1});
        else {
            switch(t.type) {
                case Addition:
                case Subtraction:
                    orderNum = 0;
                    break;
                case Multiplication: case Division:
                case Modulo:
                    orderNum = 1;
                    break;
                case Power:
                    orderNum = 2;
                    break;
                default: break;
            }
            order.push_back({t, orderNum + bracketAdditional});
        }
    }

    return order;
}

i32 buildTree(i32 left, i32 right, std::vector<node> &tree, std::vector<buffToken> buffTokens) {
    node temp;
    if(left == right) temp.Token = buffTokens[left].Token;
    else {
        i32 minimum = INT_MAX, operatorIndex = 0;
        for(i32 i = right; i >= left; --i) if(buffTokens[i].Token.isOperator()) minimum = std::min(minimum, buffTokens[i].order);
        for(i32 i = right; i >= left; --i) if(buffTokens[i].Token.isOperator() && buffTokens[i].order == minimum) {
            operatorIndex = i;
            break;
        }

        temp.Token = buffTokens[operatorIndex].Token;
        temp.leftChild = buildTree(left, operatorIndex-1, tree, buffTokens);
        temp.rightChild = buildTree(operatorIndex+1, right, tree, buffTokens);
    }
    tree.push_back(temp);
    return tree.size()-1;
}

f64 calculateAnswer(i32 idx, std::vector<node> tree) {
    if(tree[idx].Token.type == Number) return stold(tree[idx].Token.value);
    else if(tree[idx].Token.type == Factorial) return factorial(tree[idx].Token.value);
    f64 leftAns = calculateAnswer(tree[idx].leftChild, tree);
    f64 rightAns = calculateAnswer(tree[idx].rightChild, tree);

    switch(tree[idx].Token.type) {
        case Addition:
            return leftAns + rightAns;
            break;
        case Subtraction:
            return leftAns - rightAns;
            break;
        case Multiplication:
            return leftAns * rightAns;
            break;
        case Division:
            return leftAns / rightAns;
            break;
        case Power:
            return pow(leftAns, rightAns);
            break;
        case Modulo:
            if(!isInteger(leftAns) || !isInteger(rightAns)) throw "Modulus operation only works for integers";
            return (i64)leftAns % (i64)rightAns;
            break;
        default:
            return 0;
    }
}


