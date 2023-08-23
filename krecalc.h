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
    Number,
    Addition        = '+',
    Subtraction     = '-',
    Multiplication  = '*',
    Division        = '/',
    Power           = '^',
    Modulo          = '%',
    LeftBracket     = '(',
    RightBracket    = ')',
    Factorial       = '!',
    Function,
    Text,
    None
};

struct token {
    tokenType type = None;
    std::string value;
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

bool isNumber(const char c);
bool isInteger(const f64 x);
bool isText(const char c);

std::vector<token> tokenizeString(std::string str);
bool verifyAndFixTokens(std::vector<token> &tokens);
#endif

// TODO:
// input flags - main focus of main.cpp
// 
// error handing:
// - Modulo ints
// - No expression provided
// - Invalid token
// - Brackets not matching
// - dot at the end of numebr
// - operator after operator
