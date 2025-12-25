#pragma once

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  TK_RESERVED,
  TK_NUM,
  TK_EOF,
} TokenKind;

typedef struct s_Token Token;
struct s_Token {
  TokenKind kind; // トークンの型
  Token *next;    // 次の入力トークン
  int val;        // kindがTK_NUMの場合、その数値
  char *str;      // トークン文字列
  int len;
};

typedef enum {
  NK_ADD,
  NK_SUB,
  NK_MUL,
  NK_DIV,
  NK_EQ,
  NK_NE,
  NK_LT,
  // NK_GT,
  NK_LE,
  // NK_GE,
  NK_NUM,
} NodeKind;

typedef struct s_Node Node;
struct s_Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;
};

extern Token *token;
extern char *user_input;

Token *tokenize(char *p);
Node *expr(void);
void gen(Node *node);