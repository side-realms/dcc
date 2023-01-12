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

typedef struct Token Token;

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

typedef struct Node Node;

struct Node{
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
};


    // 構造体を変数するときは 
    // struct Token Token;
    // とする必要があるが，めんどくさいので 
    // typedef struct Token Token
    // とすることで Token ~~~ みたいな変数ができる．

Token *token;
char *user_input;
Node *equality();
Node *relational();
Node *add();
Node *expr();
Node *mul();
Node *primary();
Node *unary();

// 参考：https://ez-net.jp/article/E3/CQ4fxR9H/br4mR3gSb_sE/
// vsprintf は sprintf と似ていて，
// vsprintf(ストリームバッファ，引数，va_list 構造体) となる
// loc は現在見ているバイト，user_input は先頭バイトを指している
void error_at(char *loc, char *fmt, ...) { // 可変個の引数を持つ
    va_list ap; // 可変長引数を操作するための構造体
    va_start(ap, fmt); // 可変長を操作するための必要なマクロ(?)
    
    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " ");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
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
bool consume(char *op){
    if(token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len)){
        return false;
    }
    token = token->next;
    return true;
}

bool startswith(char *p, char *q) {
  return memcmp(p, q, strlen(q)) == 0;
}

// expect
    // 次が期待した文字が確かめ，エラーを返す
void expect(char *op){
    if(token->kind != TK_RESERVED || strlen(op) != token->len || memcmp(token->str, op, token->len)){
        error_at(token->str, "not '%c'", op);
    }
    token = token->next;
}

// expect_number
    // 次のトークンが数字であるか判定する
int expect_number(){
    if(token->kind != TK_NUM){
        error_at(token->str, "token is not number");
    }
    int val = token->val;
    token = token->next;
    return val;
}

Token *new_token(TokenKind kind, Token *cur, char *str, int len){
    Token *tok = calloc(1, sizeof(Token));
        // メモリ初期化に Token のサイズを使っている
        // malloc だと char 1 つ分になる．
    tok->kind = kind;
    cur->next = tok;
    tok->str = str;
    tok->len = len;
    return tok;
}

Node *new_node(NodeKind kind){
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    return node;
}

Node *new_binary(NodeKind kind, Node *lhs, Node *rhs){
    Node *node = new_node(kind);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val){
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

Node *expr(){
    return equality();
}

Node *equality(){
    Node *node = relational();

    for(;;){
        if(consume("==")){
            node = new_binary(ND_EQ, node, relational());
        }else if(consume("!=")){
            node = new_binary(ND_NE, node, relational());
        }else{
            return node;
        }
    }
}

Node *relational(){
    Node *node = add();

    for (;;) {
        if (consume("<"))
          node = new_binary(ND_LT, node, add());
        else if (consume("<="))
          node = new_binary(ND_LE, node, add());
        else if (consume(">"))
          node = new_binary(ND_LT, add(), node);
        else if (consume(">="))
          node = new_binary(ND_LE, add(), node);
        else
          return node;
    }
}

Node *add(){
    Node *node = mul();

    for(;;){
        if(consume("+")){
            node = new_binary(ND_ADD, node, mul());
        }else if(consume("-")){
            node = new_binary(ND_SUB, node, mul());
        }else{
            return node;
        }
    }
}

Node *mul(){
    Node *node = unary();

    for(;;){
        if(consume("*")){
            node = new_binary(ND_MUL, node, unary());
        }else if(consume("/")){
            node = new_binary(ND_DIV, node, unary());
        }else{
            return node;
        }
    }
}

Node *unary(){
    if(consume("+"))
        return unary();
    if (consume("-"))
        return new_binary(ND_SUB, new_node_num(0), unary());
    return primary();
}


Node *primary(){
    if(consume("(")){
        Node *node = expr();
        expect(")");
        return node;
    }
    return new_node_num(expect_number());
}

void gen(Node *node){
    if(node->kind == ND_NUM){
        printf("  push %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch(node->kind){
        case ND_ADD:
            printf("  add rax, rdi\n");
            break;
        case ND_SUB:
            printf("  sub rax, rdi\n");
            break;
        case ND_MUL:
            printf("  imul rax, rdi\n");
            break;
        case ND_DIV:
            printf("  cqo\n");
            printf("  idiv rdi\n");
            break;
        case ND_EQ:
            printf("  cmp rax, rdi\n");
            printf("  sete al");
            printf("  movzb rax, al");
        case ND_NE:
            printf("  cmp rax, rdi\n");
            printf("  setne al");
            printf("  movzb rax, al");
        case ND_LE:
            printf("  cmp rax, rdi\n");
            printf("  setle al");
            printf("  movzb rax, al");
        case ND_LT:
            printf("  cmp rax, rdi\n");
            printf("  setl al");
            printf("  movzb rax, al");
    }
    printf("  push rax\n");
}

// tokenize
    // トークナイズする

Token *tokenize(){
    char *p = user_input;
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while(*p){
        if(isspace(*p)){
            p++;
            continue;
        }

        if (startswith(p, "==") || startswith(p, "!=") || startswith(p, "<=") || startswith(p, ">=")) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
    }

        if(strchr("+-*/()<>", *p)){
            cur = new_token(TK_RESERVED, cur, p, 1);
            p++;
            continue;
        }
        if(isdigit(*p)){
            cur = new_token(TK_NUM, cur, p,0);
            char *q = p;
            cur->val = strtol(p, &p, 10);
            cur->len = p - q;
            continue;
        }

        error_at(p, "cannot tokenize");
    }
    new_token(TK_EOF, cur, p, 0);
    return head.next; // 連続の先頭を返す
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません");
    return 1;
  }

  user_input = argv[1];
  //fprintf(stdout, "user input is %s\n",user_input);
  token = tokenize(user_input);
  Node *node = expr();

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  gen(node);

  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}