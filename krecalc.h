#ifndef KREC
#define KREC

#include<cmath>
#include<string>
#include<vector>
#include<iostream>
#include<algorithm>

typedef long double f64;
typedef long long i64;
typedef int i32;

const i32 INT_MAX = 2147483647;
const i32 BRACKET_MULTIPLIER = 10;

enum tokenType {
    Addition        = '+',
    Subtraction     = '-',
    Multiplication  = '*',
    Division        = '/',
    Power           = '^',
    Modulo          = '%',
    LeftBracket     = '(',
    RightBracket    = ')',
    Factorial       = '!',
    Number,
    Function,
    Text,
    None
};

struct token {
    tokenType type = None;
    std::string value;

    bool isOperator() {
        switch(type) {
            case Number:
            case Text:
            case Function:
            case Factorial:
            case LeftBracket:
            case RightBracket:
            case None:
                return 0;
            default:
                return 1;
        }
    }
};

struct node {
    token Token;
    i32 leftChild = -1;
    i32 rightChild = -1;
};

struct buffToken {
    token Token;
    i32 order;
};

bool isNumber(const char c);
bool isInteger(const f64 x);
bool isInteger(const f64 x, f64 &integerPart);
bool isText(const char c);

std::vector<token> tokenizeString(std::string str);
void verifyAndFixTokens(std::vector<token> &tokens);
f64 calculateAnswer(i32 idx, std::vector<node> tree);
i32 buildTree(i32 left, i32 right, std::vector<node> &tree, std::vector<buffToken> buffTokens);
std::vector<buffToken> buildOrderTable(std::vector<token> tokens);
f64 factorial(std::string str);
#endif
