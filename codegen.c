#include "dcc.h"

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