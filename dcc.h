#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Token Token;
typedef struct Node Node;

// トークンの種類を列挙型で定義する，文字・数字・終端
// トークン型の構造体を定義する，トークン型↑・次のトークン・val・文字列
// エラー専用の print を定義しておく
// 次のトークンが数値の場合

    // 構造体を変数するときは 
    // struct Token Token;
    // とする必要があるが，めんどくさいので 
    // typedef struct Token Token
    // とすることで Token ~~~ みたいな変数ができる．

typedef enum{ // トークンの種類を列挙型で定義する
    TK_RESERVED, // 文字
    TK_NUM, // 数字
    TK_EOF, // 終端文字
}TokenKind;

struct Token{ // Token 型
    TokenKind kind; // トークンの種類(TokenKind)
    Token *next; // 次のトークンへのポインタ
    int val; // 数値トークンのときの値
    char *str; // 文字トークンのときの文字
    int len;
};

typedef enum{
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NUM,
    ND_EQ,
    ND_NE,
    ND_LT,
    ND_LE,
}NodeKind;

struct Node{
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
};

Token *token;
char *user_input;