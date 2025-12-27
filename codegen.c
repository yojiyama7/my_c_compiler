#include "mycc.h"

int stmt_id;

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
  int sid;
  Node *cur;

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
    return;
  case NK_RETURN:
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return ;
  case NK_IF:
    sid = stmt_id++;
    gen(node->cond);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend%d\n", sid);
    gen(node->then);
    printf(".Lend%d:\n", sid);
    return ;
  case NK_IFELSE:
    sid = stmt_id++;
    gen(node->cond);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lelse%d\n", sid);
    gen(node->then);
    printf("  jmp .Lend%d\n", sid);
    printf(".Lelse%d:\n", sid);
    gen(node->els);
    printf(".Lend%d:\n", sid);
    return ;
  case NK_WHILE:
    sid = stmt_id++;
    printf(".Lbegin%d:\n", sid);
    gen(node->cond);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je  .Lend%d\n", sid);
    gen(node->body);
    printf("  jmp .Lbegin%d\n", sid);
    printf(".Lend%d:\n", sid);
    return;
  case NK_FOR:
    sid = stmt_id++;
    gen(node->init);
    printf(".Lbegin%d:\n", sid);
    gen(node->cond);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je  .Lend%d\n", sid);
    gen(node->body);
    gen(node->inc);
    printf("  jmp .Lbegin%d\n", sid);
    printf(".Lend%d:\n", sid);
    return ;
  case NK_BLOCK:
    cur = node->body;
    while (cur) {
      gen(cur);
      printf("  pop rax\n");
      cur = cur->next;
    }
    return ;
  case NK_CALL:
    char *regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
    int i = 0;
    int stack_bytes = 8;
    cur = node->args;
    while (cur && i < 6) {
      gen(cur);                      // stack_bytes += 8
      printf("  pop %s\n", regs[i]); // stack_bytes -= 8
      cur = cur->next;
      i++;
    }
    // 長さ6超過
    if (cur) {
      error("引数が6つ超過あります");
    }
    int rsp_offset = 16 - (stack_bytes % 16);
    printf("  sub rsp, %d\n", rsp_offset);
    printf("  call %.*s\n", node->func_name_len, node->func_name);
    printf("  add rsp, %d\n", rsp_offset);
    printf("  push rax\n");
    return; 
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
