#include "krecalc.h"

using namespace std;

int main(int argc, char** argv) {
    try {
        if(argc == 1) throw "Please provide an expression";
        string inputString = argv[1];

        //Turn input string into tokens
        vector<token> tokens = tokenizeString(inputString);
        verifyAndFixTokens(tokens);


        vector<orderedToken> orderedTokens = buildOrderTable(tokens);
        cout << "Token Count: " << tokens.size() << endl;
        cout << "Token Order: ";
        for(orderedToken& ot :orderedTokens) cout << ot.order << ' ';
        cout << endl;

        vector<node> tree;
        makeTree(0, orderedTokens.size()-1, tree, orderedTokens); 

        cout << "Node Count: " << tree.size() << endl;
        
        cout << calculateAnswer(tree.size()-1, tree) << endl;
    }
    catch (std::string msg){
        cout << "Error: " << msg << endl;
    }
    catch (const char* msg){
        cout << "Error: " << msg << endl;
    }
}
