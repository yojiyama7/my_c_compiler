#include "mycc.h"

Token *token;
char *user_input;

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

bool consume(char *op) {
  if (token->kind != TK_RESERVED ||
      (int)strlen(op) != token->len ||
      memcmp(op, token->str, token->len) != 0) {
    return false;
  }
  token = token->next;
  return true;
}

void expect(char *op) {
  if (token->kind != TK_RESERVED ||
      (int)strlen(op) != token->len ||
      memcmp(op, token->str, token->len) != 0) {
    error_at(token->str, "'%s'ではありません", op);
  }
  token = token->next;
}

int expect_number(void) {
  if (token->kind != TK_NUM) {
    error_at(token->str, "数ではありません");
  }
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof(void) {
  return token->kind == TK_EOF;
}

Token *new_token(TokenKind kind, Token *cur, char *str) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
}

Token *tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }
    if (memcmp(p, "==", 2) == 0 ||
        memcmp(p, "!=", 2) == 0 ||
        memcmp(p, "<=", 2) == 0 ||
        memcmp(p, ">=", 2) == 0 ) {
      cur = new_token(TK_RESERVED, cur, p);
      cur->len = 2;
      p += 2;
      continue;
    }
    if (*p == '+' ||
        *p == '-' ||
        *p == '*' ||
        *p == '/' ||
        *p == '(' ||
        *p == ')' ||
        *p == '<' ||
        *p == '>' ) {
      cur = new_token(TK_RESERVED, cur, p);
      cur->len = 1;
      p++;
      continue;
    }
    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error_at(p, "トークナイズできません");
  }
  new_token(TK_EOF, cur, p);
  // for (Token* t = head.next; t->kind != TK_EOF; t = t->next) {
  //   fprintf(stderr, "%s\n", t->str);
  // }
  return head.next;
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = NK_NUM;
  node->val = val;
  return node;
}

Node *primary(void) {
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  return new_node_num(expect_number());
}

Node *unary(void) {
	if (consume("+")) {
		return primary();
	}
	if (consume("-")) {
		return new_node(NK_SUB, new_node_num(0), primary());
	}
	return primary();
}

Node *mul(void) {
  Node *node = unary();

  while (true) {
    if (consume("*")) {
      node = new_node(NK_MUL, node, unary());
    } else if (consume("/")) {
      node = new_node(NK_DIV, node, unary());
    } else {
      return node;
    }
  }
}

Node *add(void) {
  Node *node = mul();
  while (true) {
    if (consume("+")) {
      node = new_node(NK_ADD, node, mul());
    } else if (consume("-")) {
      node = new_node(NK_SUB, node, mul());
    } else {
      return node;
    }
  }
}

Node *relational(void) {
  Node *node = add();
  if (consume("<=")) {
    return new_node(NK_LE, node, add());
  }
  if (consume(">=")) {
    return new_node(NK_LE, add(), node);
  }
  if (consume("<")) {
		return new_node(NK_LT, node, add());
	}
	if (consume(">")) {
    return new_node(NK_LT, add(), node);
  }
  return node;
}

Node *equality(void) {
  Node *node = relational();
  if (consume("==")) {
    return new_node(NK_EQ, node, add());
  }
  if (consume("!=")) {
    return new_node(NK_NE, node, add());
  }
  return node;
}

Node *expr(void) {
  return equality();
}
