#include "chibi.h"

// Scope for LOcal variables, global variables, typedef
// or enum constants
typedef struct VarScope VarScope;
struct VarScope {
  VarScope *next;
  char *name;
  int depth;

  Var *var;
  Type *type_def;
  Type *enum_ty;
  int enum_val;
};

// Scope for struct or enum tags
typedef struct TagScope TagScope;
struct TagScope {
  TagScope *next;
  char *name;
  int depth;
  Type *ty;
};

typedef struct {
  VarScope *var_scope;
  TagScope *tag_scope;
} Scope;

// All local variable onstances created during parsing are
// accunmulated to this list.
static VarList *locals;

// Lokewise, global variables are accumulated to this list.
static VarList *globals;

// C has two block scopes; one is for variables/typedefes and
// the other is for struct/union/enum tags.
static VarScope *var_scope;
static TagScope *tag_scope;
static int scope_depth;

// Points to a node representing a switch if we are parsing
// a switch statement. Otherwise. NULL.
static Node *current_switch;

// begin a block scope
static Scope *entere_scope(void) {
  Scope *sc = calloc(1, sizeof(Scope));
  sc->var_scope = var_scope;
  sc->tag_scope = tag_scope;
  scope_depth++;
  return sc;
}

// End a block scope
static void leave_scope(Scope *sc) {
  var_scope = sc->var_scope;
  tag_scope = sc->tag_scope;
  scope_depth--;
}

// FInd a variable or a typedef by name
static VarScope *find_var(Token *tok) {
  for (VarScope *sc = var_scope; sc; sc = sc->next)
    if (strlen(sc->name) == tok->len && !strncmp(tok->str, sc->name, tok->len))
      return sc;
  return NULL;
}

static TagScope *find_tag(Token *tok) {
  for (TagScope *sc = tag_scope; sc; sc = sc->next)
    if (strlen(sc->name) == tok->len && !strncmp(tok->str, sc->name, tok->len))
      return sc;
  return NULL;
}

static Node *new_node(NodeKind kind, Token *tok) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->tok = tok;
  return node;
}

static Node *new_binary(NodeKind kind, Node *lhs, Node *rhs, Token *tok) {
  Node *node = new_node(kind, tok);
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

static Node *new_unary(NodeKind kind, Node *expr, Token *tok) {
  Node *node = new_node(kind, tok);
  node->lhs = expr;
  return node;
}

static Node *new_num(long val, Token *tok) {
  Node *node = new_node(ND_NUM, tok);
  node->val = val;
  node->ty = int_type;
  return node;
}

static Node *new_var_node(Var *var, Token *tok) {
  Node *node = new_node(ND_VAR, tok);
  node->var = var;
  return node;
}

static VarScope *push_scope(char *name) {
  VarScope *sc = calloc(1, sizeof(VarScope));
  sc->name = name;
  sc->next = var_scope;
  sc->depth = scope_depth;
  var_scope = sc;
  return sc;
}

static Var *new_var(char *name, Type *ty, bool is_local) {
  Var *var = calloc(1, sizeof(Var));
  var->name = name;
  var->ty = ty;
  var->is_local = is_local;
  return var;
}

static Var *new_lvar(char *name, Type *ty) {
  Var *var = new_var(name, ty, true);
  push_scope(name)->var = var;

  VarList *vl = calloc(1, sizeof(VarList));
  vl->var = var;
  vl->next = locals;
  locals = vl;
  return var;
}

static Var *new_gvar(char *name, Type *ty, bool is_static, bool emit) {
  Var *var = new_var(name, ty, false);
  var->is_static = is_static;
  push_scope(name)->var = var;

  if (emit) {
    VarList *vl = calloc(1, sizeof(VarList));
    vl->var = var;
    vl->next = globals;
    globals = vl;
  }

  return var;
}

static Type *find_typedef(Token *tok) {
  if (tok->kind == TK_IDENT) {
    VarScope *sc = find_var(tok);
    if (sc)
      return sc->type_def;
  }
  return NULL;
}

static char *new_label(void) {
  static int cnt = 0;
  char buf[20];
  sprintf(buf, ".L.data.%d", cnt++);
  return strndup(buf, 20);
}

typedef enum {
  TYPEDEF = 1 << 0,
  STATIC = 1 << 1,
  EXTERN = 1 << 2,
} StorageClass;

static Function *function(void);
static Type *basetype(StorageClass *sclass);
static Type *declarator(Type *ty, char **name);
static Type *abstract_declarator(Type *ty);
static Type *type_suffix(Type *ty);
static Type *type_name(void);
static Type *struct_decl(void);
static Type *enum_specifier(void);
static Member *struct_member(void);
static void global_var(void);
static Node *declaration(void);
static bool is_typename(void);
static Node *stmt(void);
static Node *stmt2(void);
static Node *expr(void);
static long eval(Node *node);
static long eval2(Node *node, Var **var);
static long const_expr(void);
static Node *assign(void);
static Node *conditional(void);
static Node *lofor(void);
static Node *logand(void);
static Node *bitand(void);
static Node *bitor(void);
static Node *bitxor(void);
static Node *equality(void);
static Node *relational(void);
static Node *shift(void);
static Node *new_add(Node *lhs, Node *rhs, Token *tok);
static Node *add(void);
static Node *mul(void);
static Node *cast(void);
static Node *unary(void);
static Node *postfix(void);
static Node *compound_literal(void);
static Node *primary(void);

// Determine whether the next top-level time is a function
// or a global variab;e by looking ahead input tokens.
static bool is_function(void) {
  Token *tok = token;
  bool isfunc = false;

  StorageClass sclass;
  Type *ty = basetype(&sclass);

  if (!consume(";")) {
    char *name = NULL;
    declarator(ty, &name);
    isfunc = name && consume("(");
  }

  token = tok;
  return isfunc;
}

// program = (global - var | function)
Program *program(void) {
  Function head = {};
  Function *cur = &head;
  globals = NULL;

  while (!at_eof()) {
    if (is_function()) {
      Function *fn = function();
      if (!fn)
	continue;
      cur->next = fn;
      cur = cur->next;
      continue;
    }

    global_var();
  }

  Program *prog = calloc(1, sizeof(Program));
  prog->globals = globals;
  prog->fns = head.next;
  return prog;
}


// basetype = builtin-type | struct-decl | typedef-name | enum-specifier
//
// builtin-type = "void" | "_Bool" | "char" | "short" | "int"
//              | "long" | "long" "long"
//
// Note that "typedef" and "static" can appear anywhere in a basetype.
// "int" can appear anywhere if type is short, long or long long.
// "signed" can appear anywhere if type is short, int, long or long long.
static Type *basetype(StorageClass *sclass) {
  if (!is_typename())
    error_tok(token, "typename expected");

  enum {
    VOID   = 1 << 0,
    BOOL   = 1 << 2,
    CHAR   = 1 << 4,
    SHORT  = 1 << 6,
    INT    = 1 << 8,
    LONG   = 1 << 10,
    OTHER  = 1 << 12, 
    SIGNED = 1 << 13,
  };

  Type *ty = int_type;
  int counter = 0;

  if (sclass)
    *sclass = 0;

  while (is_typename()) {
    Token *tok = token;

    // Handle storage class specifiers.
    if (peek("typedef") || peek("static") || peek("extern")) {
      if (!sclass)
	error_tok(tok, "storage class specifier is not allowed");

      if (consume("typedef"))
	*sclass |= TYPEDEF;
      else if (consume("static"))
	*sclass |= STATIC;
      else if (consume("extern"))
	*sclass |= EXTERN;

      if (*sclass & (*sclass - 1))
	error_tok(tok, "typedef, static and extern may not be used together");
      continue;
    }

    // Handle user-defined types.
    if (!peek("void") && !peek("_Bool") && !peek("char") &&
	!peek("short") && !peek("int") && !peek("long") &&
	!peek("signed")) {
      if (counter)
	break;

      if (peek("struct")) {
	ty = struct_decl();
      } else if (peek("enum")) {
	ty = enum_specifier();
      } else {
	ty = find_typedef(token);
	assert(ty);
	token = token->next;
      }

      counter |= OTHER;
      continue;
    }

    // Handle built-in types.
    if (consume("void"))
      counter += VOID;
    else if (consume("_Bool"))
      counter += BOOL;
    else if (consume("char"))
      counter += CHAR;
    else if (consume("short"))
      counter += SHORT;
    else if (consume("int"))
      counter += INT;
    else if (consume("long"))
      counter += LONG;
    else if (consume("signed"))
      counter |= SIGNED;

    switch (counter) {
    case VOID:
      ty = void_type;
      break;
    case BOOL:
      ty = bool_type;
      break;
    case CHAR:
    case SIGNED + CHAR:
      ty = char_type;
      break;
    case SHORT:
    case SHORT + INT:
    case SIGNED + SHORT:
    case SIGNED + SHORT + INT:
      ty = short_type;
      break;
    case INT:
    case SIGNED:
    case SIGNED + INT:
      ty = int_type;
      break;
    case LONG:
    case LONG + INT:
    case LONG + LONG + INT:
    case SIGNED + LONG:
    case SIGNED + LONG + INT:
    case SIGNED + LONG + LONG:
    case SIGNED + LONG + LONG + INT:
      ty = long_type;
      break;
    default:
      error_tok(tok, "invalid type");
    }
  }

  return ty;
}
