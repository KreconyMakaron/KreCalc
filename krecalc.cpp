#include"krecalc.h"

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

// Debugging tool
std::ostream& operator<<(std::ostream& o, const token& t)
{
    o << "type: " << t.type << ", Value: " << t.value;
    if(!t.extra_data.empty()) 
    {
        o << ", extra_data: ";
        for(std::vector<token> field : t.extra_data) {
            o << "{";
            for(token tk : field) {
                o << tk << "; ";
            }
            o << "}; ";
        }
    }
    return o;
}

std::vector<token> tokenizeString(std::string str) {
    std::vector<token> tokens;
    std::string tempString;
    token tempToken;
    
    for(int i = 0; i < (int)str.size(); ++i) {
        tempToken = {None, ""};
        switch(str[i]) {
            case ' ': continue; break;
            case Addition:          tempToken.type = Addition; break;
            case Subtraction:       tempToken.type = Subtraction; break;
            case Multiplication:    tempToken.type = Multiplication; break;
            case Division:          tempToken.type = Division; break;
            case Power:             tempToken.type = Power; break;
            case Modulo:            tempToken.type = Modulo; break;
            case Factorial:         tempToken.type = Factorial; break;
            case LeftBracket:       tempToken.type = LeftBracket; break;
            case RightBracket:      tempToken.type = RightBracket; break;
            case Comma:             tempToken.type = Comma; break;

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
bool match(std::vector<token>::iterator it, std::vector<token>::iterator end, std::vector<tokenType> matchType, std::vector<std::string> matchValue = {}) {
    int k = 0;
    for(int i = 0; i < (int)matchType.size(); ++i) {
        if(it == end) return 0;
        if(it->type != matchType[i]) return 0;
        if(it->type == Text && !matchValue.empty() && it->value != matchValue[k++]) return 0;
        ++it;
    }
    return 1;   
}

void addValueAndRemove(std::vector<token> &t, std::vector<token>::iterator &it, int count) {
    std::vector<token>::iterator temp = it; it++;
    while(count--) {
        temp->value += it->value;
        it = t.erase(it);
    }
}

void computeFunction(std::vector<token> &t) {
    /* for(token& tk : t) std::cout << tk << std::endl; */
    /* std::cout << "\n-----\n"; */
    std::vector<token>::iterator it = t.begin(), functionIterator;
    bool foundAny = 0;
    
    if(t.empty()) return;
    while(it != t.end()) {
        if(match(it, t.end(), {Text, LeftBracket})) {
            foundAny = 1;
            functionIterator = it;
            functionIterator->type = Function;
            
            it++; it = t.erase(it);
            int bracketCountInFunction = 1;

            std::vector<token> functionSlot = {};
            while(it != t.end() && bracketCountInFunction != 0) {
                switch(it->type) {
                    case LeftBracket: 
                        bracketCountInFunction++; 
                        functionSlot.push_back(*it);
                        break;
                    case RightBracket: 
                        bracketCountInFunction--;
                        
                        if(bracketCountInFunction == 0) {
                            functionIterator->extra_data.push_back(functionSlot);
                            functionSlot.clear();
                            break;
                        }
                        
                        functionSlot.push_back(*it);
                        break;
                    case Comma: 
                        if(bracketCountInFunction > 1) {
                            functionSlot.push_back(*it);
                            break;
                        }
                        functionIterator->extra_data.push_back(functionSlot);
                        functionSlot.clear();
                        break;
                    default:
                        functionSlot.push_back(*it);
                        break;
                }
               
                it = t.erase(it);
            }
            if(it == t.end()) break;
            it++;
        }
        else it++;
    }

    //Compute Functions in Function 💀
    if(!foundAny) return;
    for(std::vector<token> &data : functionIterator->extra_data) computeFunction(data);
}

void verifyAndFixTokens(std::vector<token> &t) {
    std::vector<token>::iterator it;
    
    int bracketDiff = 0;
    for(int i = 0; i < (int)t.size(); ++i) {
        if(bracketDiff < 0) throw "Not all brackets are closed";
        if(t[i].type == LeftBracket) bracketDiff++;
        if(t[i].type == RightBracket) bracketDiff--;
    }
    if(bracketDiff != 0) throw "Not all brackets are closed";
    
    //eg. 2e4
    it = t.begin();
    while(it != t.end()) {
        if(match(it, t.end(), {Number, Text, Number}, {"e"})) addValueAndRemove(t, it, 2);
        else it++;
    }
    
    /* //eg. 2e+4 or 2e-4 */
    it = t.begin();
    while(it != t.end()) {
        if(match(it, t.end(), {Number, Text, Addition, Number}, {"e"})
            || match(it, t.end(), {Number, Text, Subtraction, Number}, {"e"})) addValueAndRemove(t, it, 3);
        else it++;
    }

    //eg. 2(...) -> 2*(...)
    it = t.begin();
    while(it != t.end()) {
        if(match(it, t.end(), {Number, LeftBracket})) t.insert(++it, {Multiplication, "*"});
        else it++;
    }

    //Factorial
    for(it = t.begin(); it != t.end();) {
        if(it->type == Factorial) {
            std::vector<token> data;
            it--;
            
            int bracketCount = (it->type == RightBracket);
            data.push_back(*it);
            
            while(it != t.begin() && bracketCount) {
                it--;
                
                if(it->type == RightBracket) bracketCount++;
                else if(it->type == LeftBracket) bracketCount--;
                
                data.push_back(*it);
            }

            while(it->type != Factorial && it != t.end()) it = t.erase(it);

            std::reverse(data.begin(), data.end());

            it->value = "Factorial";
            it->type = Function;
            it->extra_data.push_back(data);
            
            it++;
        }
        else it++;
    }
    
    //Negative Numbers
    it = t.begin();
    if(match(it, t.end(), {Subtraction, Number})) { // First Token is -x
        it = t.erase(it);
        it->value = "-" + it->value;
        it++;
    }
    while(it != t.end()) {
        if(it->isOperator()) {
            it++;
            if(match(it, t.end(), {Subtraction, Number})) {
                it = t.erase(it);
                it->value = "-" + it->value;
                it++;
            }
        }
        else it++;
    }


    //text ( data1 , data2 , datan )
    //Functions
    computeFunction(t);

    //Two operators next to each other
    for(int i = 0; i+1 < (int)t.size(); ++i) {
        if(t[i].isOperator() && t[i+1].isOperator()) throw "Tokens " + t[i].value + " and " + t[i+1].value + " cannot be next to each other";
    }

    for(int i = 0; i < (int)t.size(); ++i) if(t[i].type == Text || t[i].type == None) throw t[i].value + " is not a valid token";
}

std::vector<orderedToken> buildOrderTable(std::vector<token> &tokens) {
    std::vector<orderedToken> order;
    int bracketCount = 0, orderNum = 0;

    for(token t : tokens) {
        if(t.type == LeftBracket) bracketCount += BRACKET_MULTIPLIER;
        else if(t.type == RightBracket) bracketCount -= BRACKET_MULTIPLIER;
        else if(t.type == Number || t.type == Function) order.push_back({t, 100000}); //Asign a big numbur to non-operators
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
            order.push_back({t, orderNum + bracketCount});
        }
    }
    return order;
}

int makeTree(int left, int right, std::vector<node> &tree, std::vector<orderedToken> tokens) {
    // New node pushed onto the structure
    node tempNode;
    
    // Either a number or a function of some kind
    if(left == right) {
        if(tokens[left].t.type == Function) {
            tempNode.type = Function; 
            tempNode.value = tokens[left].t.value;
            
            for(std::vector<token> &data : tokens[left].t.extra_data) {
                tempNode.children.push_back(makeTree(0, data.size()-1, tree, buildOrderTable(data)));
            }
        }
        else if(tokens[left].t.type == Number) {
            tempNode.type = Number;
            tempNode.value = tokens[left].t.value;
        }
    }
    // The token is an operator
    // The next position is going to be the first token with the minimum order
    else {
        int minimum = INT_MAX, nextPosition = 0; 
        for(int i = right; i >= left; --i) minimum = std::min(minimum, tokens[i].order);
        for(int i = right; i >= left; --i) if(tokens[i].order == minimum) {
            nextPosition = i;
            break;
        }

        tempNode.type = tokens[nextPosition].t.type;
        tempNode.value = tokens[nextPosition].t.value;
        
        tempNode.children.push_back(makeTree(left, nextPosition-1, tree, tokens));  // Left Branch
        tempNode.children.push_back(makeTree(nextPosition+1, right, tree, tokens)); // Right Branch
    }
    
    tree.push_back(tempNode);
    return tree.size()-1;
}

f64 calculateAnswer(int idx, std::vector<node> tree) {
    node tempNode = tree[idx];
    if(tempNode.type == Number) return stold(tempNode.value);
    else if(tempNode.type == Factorial) return factorial(tempNode.value);
    f64 leftAns = calculateAnswer(tempNode.children[0], tree);
    f64 rightAns = calculateAnswer(tempNode.children[1], tree);

    switch(tempNode.type) {
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
