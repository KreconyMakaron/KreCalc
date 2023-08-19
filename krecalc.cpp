#include<cmath>
#include<string>
#include<vector>
#include<iostream>

using namespace std;

typedef long double f64;
typedef long long i64;
typedef long long i32;

const i32 I32_MAX = 2147483647;
const i32 BRACKET_MULTIPLIER = 10;

enum tokenType {
    Number,
    Addition,
    Subtraction,
    Multiplication,
    Division,
    Power,
    Modulo,
    LeftBracket,
    RightBracket,
    None
};

struct token {
    tokenType type = None;
    f64 value;
};

struct node {
    token Token;
    i32 leftChild = -1;
    i32 rightChild = -1;
};

struct buffToken {
    token Token;
    i32 order;
    bool isOperator;
};

bool isNumber(const char c) {
    return ('0' <= c && c <= '9');
}

bool isAni32eger(f64 x) {
    f64 i32egerPart;
    return modf(x, &i32egerPart) == 0;
}

vector<token> tokenizeString(string str) {
    vector<token> result;
    string tempString = "";
    token tempToken, previousToken;
    i32 leftBracketCount = 0, rightBracketCount = 0;
    bool negative = 0;
    
    //Remove whitespaces
    for(char c : str) if(c != ' ') tempString += c;
    str = tempString;

    //Tokenize string
    for(i32 idx = 0; idx < (i32)str.size(); ++idx) {
        tempToken.value = 0;
        
        if(isNumber(str[idx])) {
            tempString = str[idx];
            
            while(isNumber(str[idx+1]) || str[idx+1] == '.') tempString += str[++idx];
            tempToken = {Number, stold(tempString)};
            if(negative) {
                tempToken.value *= -1;
                negative = 0;
            }
            if(tempString[tempString.size()-1] == '.') throw "Number '" + tempString + "' ends with a dot, perhaps you misspelt?";
        }
        else switch(str[idx]) {
            case '+': tempToken.type = Addition; break;
            case '-': 
                //Handle negative numbers
                //Though this relies on the assumption that the next token is a number
                //This can and probably will cause bugs
                //Will refactor it at some poi32 when adding new features
                if(previousToken.type != Number && previousToken.type != RightBracket) negative = 1;
                tempToken.type = Subtraction; 
                break;
            case '*': tempToken.type = Multiplication; break;
            case '/': tempToken.type = Division; break;
            case '(': 
                //Add multiplication token between a number and left bracket ex. 2(3+4) -> 2*(3+4)
                if(previousToken.type == Number) result.push_back({Multiplication, 0}); 
                
                tempToken.type = LeftBracket; 
                leftBracketCount++;
                break;
            case ')': 
                tempToken.type = RightBracket; 
                rightBracketCount++;
                break;
            case '%': tempToken.type = Modulo; break;
            case '^': tempToken.type = Power; break;
            default: 
                tempString = str[idx];
                throw "Expression '" + tempString + "' is not a valid token";
                break;
        }
        previousToken = tempToken;
        //This is hacky af please fix
        if(!negative) result.push_back(tempToken);
    }
    
    if(leftBracketCount != rightBracketCount) {
        if(abs(leftBracketCount - rightBracketCount) == 1) throw "It seems that a bracket isn't closed";
        else throw "It seems that " + to_string(abs(leftBracketCount - rightBracketCount)) + " brackets aren't closed";
    }
    
    return result;
}

vector<buffToken> buildOrderTable(vector<token> tokens) {
    vector<buffToken> order;
    i32 bracketAdditional = 0, orderNum = 0;
    
    for(token t : tokens) {
        if(t.type == LeftBracket) bracketAdditional += BRACKET_MULTIPLIER;
        else if(t.type == RightBracket) bracketAdditional -= BRACKET_MULTIPLIER;
        else if(t.type == Number) order.push_back({t, -1, 0});
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
            order.push_back({t, orderNum + bracketAdditional, 1});
        }
    }

    return order;
}

i32 buildTree(i32 left, i32 right, vector<node> &tree, vector<buffToken> buffTokens) {
    node temp;
    if(left == right) temp.Token = buffTokens[left].Token;
    else {
        i32 minimum = I32_MAX, operatorIndex = 0;
        for(i32 i = right; i >= left; --i) if(buffTokens[i].isOperator) minimum = min(minimum, buffTokens[i].order);
        for(i32 i = right; i >= left; --i) if(buffTokens[i].isOperator && buffTokens[i].order == minimum) {
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

f64 calculateAnswer(i32 idx, vector<node> tree) {
    if(tree[idx].Token.type == Number) return tree[idx].Token.value;
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
            if(!isAni32eger(leftAns) || !isAni32eger(rightAns)) throw "The modulo operation cannot be calculated for non-i32egers: " + to_string(leftAns) + " and " + to_string(rightAns);
            return (i64)leftAns % (i64)rightAns;
            break;
        default:
            return 0;
    }
}

int main(int argc, char** argv) {
    try {
        if(argc == 1) throw "Please provide an expression";
        string inputString = argv[1];

        //Turn the string i32o tokens
        vector<token> tokens = tokenizeString(inputString);
        
        //Create an array that represents what priority operations have
        vector<buffToken> buffTokens = buildOrderTable(tokens);
        
        //Build the Abstract Syntax Tree
        vector<node> AST;
        buildTree(0, buffTokens.size()-1, AST, buffTokens);
        
        cout << calculateAnswer(AST.size()-1, AST) << endl;
    }
    catch (std::string msg){
        cout << "Error: " << msg << endl;
    }
    catch (const char* msg){
        cout << "Error: " << msg << endl;
    }
}
