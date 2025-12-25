#include "mycc.h"

Token *token;
char *user_input;
Node *code[100];

bool consume(char *op) {
  if (token->kind != TK_RESERVED ||
      (int)strlen(op) != token->len ||
      memcmp(op, token->str, token->len) != 0) {
    return false;
  }
  token = token->next;
  return true;
}

Token *consume_ident(void) {
  if (token->kind != TK_IDENT) {
    return NULL;
  }
  Token *tok = token;
  token = token->next;
  return tok;
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
        *p == '>' ||
        *p == '=' ||
        *p == ';' ) {
      cur = new_token(TK_RESERVED, cur, p);
      cur->len = 1;
      p++;
      continue;
    }
	if ('a' <= *p && *p <= 'z') {
		cur = new_token(TK_IDENT, cur, p);
    while ('a' <= *p && *p <= 'z') {
      p++;
    }
		cur->len = p - cur->str;
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

typedef struct s_LVar LVar;
struct s_LVar {
  LVar *next;
  char *name;
  int len;
  int offset;
};

LVar *locals;

LVar *find_lvar(Token *tok) {
  for (LVar *var = locals; var; var = var->next) {
    if (var->len == tok->len && memcmp(tok->str, var->name, var->len) == 0) {
      return var;
    }
  }
  return NULL;
}

Node *primary(void) {
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }
  Token *tok = consume_ident();
  if (tok) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = NK_LVAR;

    LVar *lvar = find_lvar(tok);
    if (lvar) {
      node->offset = lvar->offset;
    } else {
      lvar = calloc(1, sizeof(LVar));
      lvar->next = locals;
      lvar->name = tok->str;
      lvar->len = tok->len;
      if (locals == NULL) { // XXX: localsがNULLになっているかもしれないのいやだね
        lvar->offset = 0;
      } else {
        lvar->offset = locals->offset + 8;
      }
      node->offset = lvar->offset;
      locals = lvar;
    }
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

Node *assign(void) {
  Node *node = equality();

  if (consume("=")) {
    return new_node(NK_ASSIGN, node, assign());
  }

  return node;
}

Node *expr(void) {
  return assign();
}

Node *stmt(void) {
  Node *node = assign();

  expect(";");

  return node;
}

void program(void) {
  int i = 0;
  
  while (!at_eof()) {
    code[i++] = stmt();
    if (i >= 100) {
      error_at(token->str, "コードが多すぎます");
    }
  }
  code[i] = NULL;
}
