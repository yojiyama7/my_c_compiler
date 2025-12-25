#include "mycc.h"

// nodeの持つoffsetが指し示す変数のアドレスをスタックにpushする
void gen_lval(Node *node) {
  if (node->kind != NK_LVAR) {
    error("代入の左辺値が変数ではありません");
  }

  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

void gen(Node *node) {
  switch (node->kind) {
  case NK_NUM:
    printf("  push %d\n", node->val);
    return;
  // LVAR単体を右辺値として処理する
  case NK_LVAR:
    gen_lval(node);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  case NK_ASSIGN:
    gen_lval(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n"); // 代入式は右辺値と同じ値として評価される
  default:
    break;
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
