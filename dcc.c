#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークンの種類を列挙型で定義する，文字・数字・終端
// トークン型の構造体を定義する，トークン型↑・次のトークン・val・文字列
// エラー専用の print を定義しておく
// 次のトークンが数値の場合，

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
};

typedef struct Token Token;
    // 構造体を変数するときは 
    // struct Token Token;
    // とする必要があるが，めんどくさいので 
    // typedef struct Token Token
    // とすることで Token ~~~ みたいな変数ができる．

Token *token;

// 参考：https://ez-net.jp/article/E3/CQ4fxR9H/br4mR3gSb_sE/
// vsprintf は sprintf と似ていて，
// vsprintf(ストリームバッファ，引数，va_list 構造体) となる
void error(char *fmt, ...) { // 可変個の引数を持つ
    va_list ap; // 可変長引数を操作するための構造体
    va_start(ap, fmt); // 可変長を操作するための必要なマクロ(?)
    vsprintf(stderr, fmt, ap);
    va_end(ap);
    exit(1);
}

// at_eof
    // 次が EOF か判定する
bool at_eof(){
    return token->kind == TK_EOF; // 今見ているトークンが EOF か？
}

// consume
    // 次が期待した文字化確かめ，真偽を返す
bool consume(char op){
    if(token->kind != TK_RESERVED || token->str[0] != op){
        return false;
    }
    token = token->next;
    return true;
}

// expect
    // 次が期待した文字が確かめ，エラーを返す
void expext(char op){
    if(token->kind != TK_RESERVED || token -> str[0] != op){
        error("not '%c'", op);
    }
    token = token->next;
}

// expect_number
    // 次のトークンが数字であるか判定する
int expect_number(){
    if(token->kind != TK_NUM){
        error("token is not number");
    }
    int val = token->val;
    token = token->next;
    return val;
}

Token *new_token(TokenKind kind, Token *cur, char *str){
    Token *tok = calloc(1, sizeof(Token));
        // メモリ初期化に Token のサイズを使っている
        // malloc だと char 1 つ分になる．
    tok->kind = kind;
    cur->next = tok;
    tok->str = str;
    return tok;
}

// tokenize
    // トークナイズする

Token *tokenize(char *p){
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while(*p){
        if(isspace(*p)){
            p++;
            continue;
        }
        if(*p == '+' || *p == '-'){
            cur = new_token(TK_RESERVED, cur, p);
            p++;
            continue;
        }
        if(isdigit(*p)){
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error('cannot tokenize');
    }
    new_token(TK_EOF, cur, p);
    return head.next; // 連続の先頭を返す
}

int main(int argc, char **argv) {
  if (argc != 2) {
    error("引数の個数が正しくありません");
    return 1;
  }

  token = tokenize(argv[1]);

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  printf("  mov rax, %d\n", expect_number()); 

  while (!at_eof()) {
    if (consume('+')) {
      printf("  add rax, %d\n", expect_number());
      continue;
    }

    expect('-');
    printf("  sub rax, %d\n", expect_number());
  }

  printf("  ret\n");
  return 0;
}