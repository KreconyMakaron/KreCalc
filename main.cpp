#include "krecalc.h"

using namespace std;

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
        i32 minimum = INT_MAX, operatorIndex = 0;
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
    /* if(tree[idx].Token.type == Number) return tree[idx].Token.value; */
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
            if(!isInteger(leftAns) || !isInteger(rightAns)) throw "Modulus doesnt work like that mate";
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
        for(token t : tokens) cout << t.value << ' ';
        
        verifyAndFixTokens(tokens);
        
        cout << endl;
        for(token t : tokens) cout << t.value << ' ';
        
        return 0;
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
