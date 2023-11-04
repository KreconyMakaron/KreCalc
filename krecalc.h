#ifndef KREC
#define KREC

#include<cmath>
#include<string>
#include<vector>
#include<iostream>
#include<algorithm>

typedef long double f64;
typedef long long i64;

const int INT_MAX = 2147483647;
const int BRACKET_MULTIPLIER = 10;

enum tokenType {
    None,
    Number,
    Text,
    Function,
    Addition        = '+',
    Subtraction     = '-',
    Multiplication  = '*',
    Division        = '/',
    Power           = '^',
    Modulo          = '%',
    LeftBracket     = '(',
    RightBracket    = ')',
    Factorial       = '!',
    Comma           = ',',
    //43
    //45
    //42
    //47
    //94
    //37
    //40
    //41
    //33
    //44
};

struct token {
    tokenType type = None;
    std::string value = "";
    std::vector<std::vector<token>> extra_data = {};
    
    bool isOperator() {
        switch(type) {
            case Addition:
            case Subtraction:
            case Multiplication:
            case Division:
            case Power:
            case Modulo:
                return 1;
                break;
            default:
                return 0;
                break;
        }
    }
};

struct orderedToken {
    token t;
    int order;
};

struct node {
    tokenType type = None;
    std::string value = "";
    std::vector<int> children = {}; // Index of the child node
};

int makeTree(int left, int right, std::vector<node> &tree, std::vector<orderedToken> tokens);

bool isNumber(const char c);
bool isInteger(const f64 x);
bool isInteger(const f64 x, f64 &integerPart);
bool isText(const char c);

std::vector<token> tokenizeString(std::string str);
void verifyAndFixTokens(std::vector<token> &tokens);
std::vector<orderedToken> buildOrderTable(std::vector<token> &tokens);

f64 factorial(std::string str);
f64 calculateAnswer(int idx, std::vector<node> tree);

std::ostream& operator<<(std::ostream& o, const token& t);
#endif
