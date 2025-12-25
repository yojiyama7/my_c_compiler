#include "mycc.h"

void gen(Node *node) {
  if (node->kind == NK_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
  case NK_ADD:
    printf("  add rax, rdi\n");
    break;
  case NK_SUB:
    printf("  sub rax, rdi\n");
    break;
  case NK_MUL:
    printf("  imul rax, rdi\n");
    break;
  case NK_DIV:
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  case NK_EQ:
    printf("  cmp rax, rdi\n");
    printf("  sete al\n");
    printf("  movzx rax, al\n");
    break;
  case NK_NE:
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");
    printf("  movzx rax, al\n");
    break;
  case NK_LT:
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzx rax, al\n");
    break;
  case NK_LE:
    printf("  cmp rax, rdi\n");
    printf("  setle al\n");
    printf("  movzx rax, al\n");
    break;
  default:
    break;
  }

  printf("  push rax\n");
}
