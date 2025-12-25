#include "mycc.h"

void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  vfprintf(stderr, fmt, ap);
  exit(1);
}

void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, " ");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

int main(int argc, char **argv) {
	if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
		return 1;
	}

  user_input = argv[1];
  token = tokenize(argv[1]);
  program();

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  // プロローグ
  printf("  push rbp\n"); // rpbの保持
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 208\n"); // rbpから208バイト分(8 * 26)関数フレームを拡大する

  for (int i = 0; code[i]; i++) {
    gen(code[i]);
    printf("  pop rax\n"); // [^1] 最後のstmtに対してpopした値が返り値になる
  }

  // エピローグ
  printf("  mov rsp, rbp\n"); // 関数フレーム(or stack)をrbpの示すアドレスまで縮小
  printf("  pop rbp\n");
  printf("  ret\n"); // ^1でpopした値が返る
	return 0;
}
