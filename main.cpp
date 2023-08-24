#include "krecalc.h"

using namespace std;

int main(int argc, char** argv) {
    try {
        if(argc == 1) throw "Please provide an expression";
        string inputString = argv[1];

        cout << inputString << endl;

        //Turn input string into tokens
        vector<token> tokens = tokenizeString(inputString);
        verifyAndFixTokens(tokens);
        
        vector<buffToken> buffTokens = buildOrderTable(tokens);
        
        //Build Abstract Syntax Tree
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
