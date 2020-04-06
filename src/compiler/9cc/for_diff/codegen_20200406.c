#include "chibi.h"

static char *argreg1[] = {"dil", "sil", "dl", "cl", "r8b", "r9b"};
static char *argreg2[] = {"di", "si", "dx", "cx", "r8w", "r9w"};
static char *argreg4[] = {"edi", "esi", "edx", "ecx", "r8d", "r9d"};
static char *argreg8[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

static int labelseq = 1;
static int brkseq;
static int contseq;
static char *funcname;

static void gen(Node *node);

// Pushes the given node's address to the stack.
static void gen_addr(Node *node) {
  switch (node->kind) {
  case ND_VAR: {
    if (node->init)
      gen(node->init);

    Var *var = node->var;
    if (var->is_local) {
      printf("  lea rax, [rdp-%d]\n", var->offset);
      printf("  push rax\n");
    } else {
      printf("  push offset %s\n", var->name);
    }
    return;
  }
  case ND_DEREF:
    gen(node->lhs);
    return;
  case ND_MEMBER:
    gen_addr(node->lhs);
    printf("  pop rax\n");
    printf("  add rax, %d\n", node->member->offset);
    printf("  push rax\n");
    return;
  }

  error_tok(node->tok, "not an lvalue");
}

static void gen_lval(Node *node) {
  if (node->ty->kind == TY_ARRAY)
    error_tok(node->tok, "not an lvalue");
  gen_addr(node);
}

static void load(Type *ty) {
  printf("  pop rax\n");

  if (ty->size == 1) {
    printf("  movsx rax, byte ptr [rax]\n");
  } else if (ty->size == 2) {
    printf("  movsx rax, word ptr [rax]\n");
  } else if (ty->size == 4) {
    printf("  movsxd rax, dword ptr [rax]\n");
  } else {
    assert(ty->size == 8);
    printf("  mov rax, [rax]\n");
  }

  printf("  push rax\n");
}

static void store(Type *ty) {
  printf("  pop rdi\n");
  printf("  pop rax\n");

  if (ty->kind == TY_BOOL) {
    printf("  cmp rdi, 0\n");
    printf("  setne dil\n");
    printf("  movzb rdi, dil\n");
  }

  if (ty->size == 1) {
    printf("  mov [rax], dil\n");
  } else if (ty->size == 2) {
    printf("  mov [rax], di\n");
  } else if (ty->size == 4) {
    printf("  mov [rax], edi\n");
  } else {
    assert(ty->size == 8);
    printf("  mov [rax], rdi\n");
  }

  printf("  push rdi\n");
}

static void truncate(Type *ty) {
  printf("  pop rax\n");

  if (ty->kind == TY_BOOL) {
    printf("  cmp rax, 0\n");
    printf("  setne al\n");
  }

  if (ty->size == 1) {
    printf("  movsx rax, al\n");
  } else if (ty->size == 2) {
    printf("  movsx rax, ax\n");
  } else if (ty->size == 4) {
    printf("  movsxd rax, eax\n");
  }
  printf("  push rax\n");
}

static void inc(Type *ty) {
  printf("  pop rax\n");
  printf("  add rax, %d\n", ty->base ? ty->base->size : 1);
  printf("  push rax\n");
}

static void dec(Type *ty) {
  printf("  pop rax\n");
  printf("  sub rax, %d\n", ty->base ? ty->base->size : 1);
  printf("  push rax\n");
}

static void gen_binary(Node *node) {
  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
  case ND_ADD:
  case ND_ADD_EQ:
    printf("  add rax, rdi\n");
    break;
  case ND_PTR_ADD:
  case ND_PTR_ADD_EQ:
    printf("  imul rdi, %d\n", node->ty->base->size);
    printf("  add rax, rdi\n");
    break;
  case ND_SUB:
  case ND_SUB_EQ:
    printf("  sub rax, rdi\n");
    break;
  case ND_PTR_SUB:
  case ND_PTR_SUB_EQ:
    printf("  imul rdi, %d\n", node->ty->base->size);
    printf("  sub rax, rdi\n");
    break;
  case ND_PTR_DIFF:
    printf("  sub rax, rdi\n");
    printf("  cqo\n");
    printf("  mov rdi, %d\n", node->lhs->ty->base->size);
    printf("  idiv rdi\n");
    break;
  case ND_MUL:
  case ND_MUL_EQ:
    printf("  imul rax, rdi\n");
    break;
  case ND_DIV:
  case ND_DIV_EQ:
    printf("  cqo\n");
    printf(" idiv rdi\n");
    break;
  case ND_BITAND:
  case ND_BITAND_EQ:
    printf("  and rax, rdi\n");
    break;
  case ND_BITOR:
  case ND_BITOR_EQ:
    printf("  or rax, rdi\n");
    break;
  case ND_BITXOR:
  case ND_BITXOR_EQ:
    printf("  xor rax, rdi\n");
    break;
  case ND_SHL:
  case ND_SHL_EQ:
    printf("  mov cl, dil\n");
    printf("  shl rax, cl\n");
    break;
  case ND_SHR:
  case ND_SHR_EQ:
    printf("  mov cl, dil\n");
    printf("  sar rax, cl\n");
    break;
  case ND_EQ:
    printf("  cmp rax, rdi\n");
    printf("  sete al\n");
    printf("  movz rax, al\n");
    break;
  case ND_NE:
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
    break;
  }
}
