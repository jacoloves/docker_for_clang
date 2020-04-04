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
static Scope *enter_scope(void) {
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

// declarator = "*"* ("(" declarator ")" | ident) type-suffix
static Type *declarator(Type *ty, char **name) {
  while (consume("*"))
    ty = pointer_to(ty);

  if (consume("()")) {
    Type *placeholder = calloc(1, sizeof(Type));
    Type *new_ty = declarator(placeholder, name);
    expect(")");
    memcpy(placeholder, type_suffix(ty), sizeof(Type));
    return new_ty;
  }

  *name = expect_ident();
  return type_suffix(ty);
}

// abstract-declarator = "*"* ("(" abstract-declarator ")")? type-suffix
static Type *abstract_declarator(Type *ty) {
  while (consume("*"))
    ty = pointer_to(ty);

  if (consume("()")) {
    Type *placeholder = calloc(1, sizeof(Type));
    Type *new_ty = abstract_declarator(placeholder);
    expect(")");
    memcpy(placeholder, type_suffix(ty), sizeof(Type));
    return new_ty;
  }
  return type_suffix(ty);
}

// type-suffix = ("[" const-expr? "]" type-suffix)?
static Type *type_suffix(Type *ty) {
  if (!consume("["))
    return ty;

  int sz = 0;
  bool is_incomplete = true;
  if (!consume("]")) {
    sz = const_expr();
    is_incomplete = false;
    expect("]");
  }

  Token *tok = token;
  ty = type_suffix(ty);
  if (ty->is_incomplete)
    error_tok(tok, "incomplete element type");

  ty = array_of(ty, sz);
  ty->is_incomplete = is_incomplete;
  return ty;
}

// type-name = basetype abstract-declarator type-suffix
static Type *type_name(void) {
  Type *ty = basetype(NULL);
  ty = abstract_declarator(ty);
  return type_suffix(ty);
}

static void push_tag_scope(Token *tok, Type *ty) {
  TagScope *sc = calloc(1, sizeof(TagScope));
  sc->next = tag_scope;
  sc->name = strndup(tok->str, tok->len);
  sc->depth = scope_depth;
  sc->ty = ty;
  tag_scope = sc;
}

// type-name = basetype abstract-declarator type-suffix
static Type *struct_decl(void) {
  // Read a struct tag.
  expect("struct");
  Token *tag = consume_ident();
  if (tag && !peek("{")) {
    TagScope *sc = find_tag(tag);

    if (!sc) {
      Type *ty = struct_type();
      push_tag_scope(tag, ty);
      return ty;
    }

    if (sc->ty->kind != TY_STRUCT)
      error_tok(tag, "not a struct tag");
    return sc->ty;
  }
  
  // Although it looks weird, "struct *foo" is legal C that defines
  // foo as a pointer to an unnamed incomplete struct type.
  if (!consume("{"))
    return struct_type();

  Type *ty;

  TagScope *sc = NULL;
  if (tag)
    sc = find_tag(tag);

  if (sc && sc->depth == scope_depth) {
    // If there's an existing struct type having the same tag name in
    // the same block scope, this is a redefinition.
    if (sc->ty->kind != TY_STRUCT)
      error_tok(tag, "not a struct tag");
    ty = sc->ty;
  } else {
    // Register a struct type as an incomplete type early, so that you
    // can write recursive structs such as
    // "struct T { struct T *next; }".
    ty = struct_type();
    if (tag)
      push_tag_scope(tag, ty);
  }

  // Read struct members.
  Member head = {};
  Member *cur = &head;

  while (!consume("}")) {
    cur->next = struct_member();
    cur = cur->next;
  }

  ty->members = head.next;

  // Assign offsets within the struct to members.
  int offset = 0;
  for (Member *mem = ty->members; mem; mem = mem->next) {
    if (mem->ty->is_incomplete)
      error_tok(mem->tok, "incomplete struct member");

    offset = align_to(offset, mem->ty->align);
    mem->offset = offset;
    offset += mem->ty->size;

    if (ty->align < mem->ty->align)
      ty->align = mem->ty->align;
  }
  ty->size = align_to(offset, ty->align);

  ty->is_incomplete = false;
  return ty;


}

// Some types of list can end with an optional "," followed by "}"
// to allow a trailing comma. This function returns true if it looks
// like we are at the end of such list.
static bool consume_end(void) {
  Token *tok = token;
  if (consume("}") || (consume(",") && consume("}")))
    return true;
  token = tok;
  return false;
}

static bool peek_end(void) {
  Token *tok = token;
  bool ret = consume("}") || (consume(",") && consume("}"));
  token = tok;
  return ret;
}

static void expect_end(void) {
  if (!consume_end())
    expect("}");
}

// enum-specifier = "enum" ident
//                | "enum" ident? "{" enum-list? "}"
//
// enum-list = enum-elem ("," enum-elem)* ","?
// enum-elem = ident ("=" const-expr)?
static Type *enum_specifier(void) {
  expect("enum");
  Type *ty = enum_type();
  
  // read an enum tag.
  Token *tag = consume_ident();
  if (tag&& !peek("{}")) {
    TagScope *sc = find_tag(tag);
    if (!sc)
      error_tok(tag, "unknown enum type");
    if (sc->ty->kind != TY_ENUM)
      error_tok(tag, "not an enum tag");
    return sc->ty;
  }

  expect("{");

  // Read enum-list.
  int cnt = 0;
  for(;;) {
    char *name = expect_ident();
    if (consume("="))
      cnt = const_expr();

    VarScope *sc = push_scope(name);
    sc->enum_ty  = ty;
    sc->enum_val = cnt++;

    if (consume_end())
      break;
    expect(",");
  }

  if (tag)
    push_tag_scope(tag, ty);
  return ty;
}

// struct-member = basetype declarator type-suffix ";"
static Member *struct_member(void) {
  Type *ty = basetype(NULL);
  Token *tok = token;
  char *name = NULL;
  ty = declarator(ty, &name);
  ty = type_suffix(ty);
  expect(";");

  Member *mem = calloc(1, sizeof(Member));
  mem->name = name;
  mem->ty = ty;
  mem->tok = tok;
  return mem;
}

static VarList *read_func_param(void) {
  Type *ty = basetype(NULL);
  char *name = NULL;
  ty = declarator(ty, &name);
  ty = type_suffix(ty);

  // "array of T" is converted to "pointer to T" only in the parameter
  // context. For example, *argv[] is converted to **argv by this.
  if (ty->kind == TY_ARRAY)
    ty = pointer_to(ty->base);

  VarList *vl = calloc(1, sizeof(VarList));
  vl->var = new_lvar(name, ty);
  return vl;
}

static void read_func_params(Function *fn) {
  if (consume(")"))
    return;

  Token *tok = token;
  if (consume("void") && consume(")"))
    return;
  token = tok;

  fn->params = read_func_param();
  VarList *cur = fn->params;

  while(!consume(")")) {
    expect(",");

    if (consume("...")) {
      fn->has_varargs = true;
      expect(")");
      return;
    }

    cur->next = read_func_param();
    cur = cur->next;
  }
}

// function = basetype declarator "(" params? ")" ("{" stmt* "}" | ";")
// params   = param ("," param)* | "void"
// param    = basetype declarator type-suffix
static Function *function(void) {
  locals = NULL;

  StorageClass sclass;
  Type *ty = basetype(&sclass);
  char *name = NULL;
  ty = declarator(ty, &name);

  // Add a function type to to, he scope
  new_gvar(name, func_type(ty), false, false);

  // Construct a function object
  Function *fn = calloc(1, sizeof(Function));
  fn->name = name;
  fn->is_static = (sclass == STATIC);
  expect("(");

  Scope *sc = enter_scope();
  read_func_params(fn);

  if (consume(";")) {
    leave_scope(sc);
    return NULL;
  }

  // Read function body
  Node head = {};
  Node *cur = &head;
  expect("{");
  while (!consume("}")) {
    cur->next = stmt();
    cur = cur->next;
  }
  leave_scope(sc);

  fn->node = head.next;
  fn->locals = locals;
  return fn;
}

// global-var = basetype declarator type-suffix ";"
static Initializer *new_init_val(Initializer *cur, int sz, int val) {
  Initializer *init = calloc(1, sizeof(Initializer));
  init->sz = sz;
  init->val = val;
  cur->next = init;
  return init;
}

static Initializer *new_init_label(Initializer *cur, char *label, long addend) {
  Initializer *init = calloc(1, sizeof(Initializer));
  init->label = label;
  init->addend = addend;
  cur->next = init;
  return init;
}

static Initializer *new_init_zero(Initializer *cur, int nbytes) {
  for (int i = 0; i <nbytes; i++)
    cur = new_init_val(cur, 1, 0);
  return cur;
}

static Initializer *gvar_init_string(char *p, int len) {
  Initializer head = {};
  Initializer *cur = &head;
  for (int i = 0; i < len; i++)
    cur = new_init_val(cur, i, p[i]);
  return head.next;
}

static Initializer *emit_struct_padding(Initializer *cur, Type *parent, Member *mem) {
  int start = mem->offset + mem->ty->size;
  int end = mem->next ? mem->next->offset : parent->size;
  return new_init_zero(cur, end - start);
}

static void skip_excess_elements2(void) {
  for(;;) {
    if (consume("{"))
      skip_excess_elements2();
    else
      assign();

    if (consume_end())
      return;
    expect(",");
  }
}

static void skip_excess_elements(void) {
  expect(",");
  warn_tok(token, "excess elements in initializer");
  skip_excess_elements2();
}

// gvar-initializer2 = assign
//                   | "{" (gvar-initializer2 ("," gvar-initializer2)* ","?)? "}"
//
// A gvar-initializer represents an initialization expression for
// a global variable. Since global variables are just mapped from
// a file to memory before the control is passed to main(), their
// contents have to be fixed at link-time. Therefore, you cannot
// write an expression that needs to be initialized at run-time.
// For example, the following global variable definition is illegal:
//
//   int foo = bar(void);
//
// If the above definition were legal, someone would have to call
// bar() before main(), but such initialization mechanism doesn't
// exist in the C execution model.
//
// Only the following expressions are allowed in an initializer:
//
//  1. A constant such as a number or a string literal
//  2. An address of another global variable with an optional addend
//
// It is obvious that we can embed (1) to an object file as static data.
// (2) may not be obvious why that can result in static data, but
// the linker supports an expression consisting of a label address
// plus/minus an addend, so (2) is allowed.
static Initializer *gvar_initializer2(Initializer *cur, Type *ty) {
  Token *tok = token;

  if (ty->kind == TY_ARRAY && ty->base->kind == TY_CHAR &&
      token->kind == TK_STR) {
    token = token->next;

    if (ty->is_incomplete) {
      ty->size = tok->cont_len;
      ty->array_len = tok->cont_len;
      ty->is_incomplete = false;
    }

    int len = (ty->array_len < tok->cont_len)
      ? ty->array_len : tok->cont_len;

    for (int i = 0; i < len; i++)
      cur = new_init_val(cur, 1, tok->contents[i]);
    return new_init_zero(cur, ty->array_len - len);
  }

  if (ty->kind == TY_ARRAY) {
    bool open = consume("{");
    int i = 0;
    int limit = ty->is_incomplete ? INT_MAX : ty->array_len;

    if (!peek("}")) {
      do {
	cur = gvar_initializer2(cur, ty->base);
	i++;
      } while (i < limit && !peek_end() && consume(","));
    }
  

    if (open && !consume_end())
      skip_excess_elements();

    // Set excess array elements to zero.
    cur = new_init_zero(cur, ty->base->size * (ty->array_len - i));

    if (ty->is_incomplete) {
      ty->size = ty->base->size * i;
      ty->array_len  = i;
      ty->is_incomplete = false;
    }
    return cur;
  }
  
  if (ty->kind == TY_STRUCT) {
    bool open = consume("{");
    Member *mem = ty->members;

    if (!peek("}")) {
      do {
	cur = gvar_initializer2(cur, mem->ty);
	cur = emit_struct_padding(cur, ty, mem);
	mem = mem->next;
      } while (mem && !peek_end() && consume(","));
    }

    if (open && !consume_end())
      skip_excess_elements();

    // Set excess struct elements to zero.
    if (mem)
      cur = new_init_zero(cur, ty->size - mem->offset);
    return cur;
  }

  bool open = consume("{");
  Node *expr = conditional();
  if (open)
    expect_end();

  Var *var = NULL;
  long addend = eval2(expr, &var);

  if (var) {
    int scale = (var->ty->kind == TY_ARRAY)
      ? var->ty->base->size : var->ty->size;
    return new_init_label(cur, var->name, addend * scale);
  }
  return new_init_val(cur, ty->size, addend);
}

static Initializer *gvar_initializer(Type *ty) {
  Initializer head = {};
  gvar_initializer2(&head, ty);
  return head.next;
}

// global-var = basetype declarator type-suffix ("=" gvar-initializer)? ";"
static void global_var(void) {
  StorageClass sclass;
  Type *ty = basetype(&sclass);
  if (consume(";"))
    return;

  char *name = NULL;
  Token *tok = token;
  ty = declarator(ty, &name);
  ty = type_suffix(ty);

  if (sclass == TYPEDEF) {
    expect(";");
    push_scope(name)->type_def = ty;
    return;
  }

  Var *var = new_gvar(name, ty, sclass == STATIC, sclass != EXTERN);

  if (sclass == EXTERN) {
    expect(";");
    return ;
  }

  if (consume("=")) {
    var->initializer = gvar_initializer(ty);
    expect(";");
    return;
  }

  if (ty->is_incomplete)
    error_tok(tok, "incomplete type");
  expect(";");
}
typedef struct Designator Designator;
struct Designator {
  Designator *next;
  int idx;
  Member *mem;
};

// Creates a node for an array access. For example, if var represents
// a variable x and desg represents indices 3 and 4, this function
// returns a node representing x[3][4].
static Node *new_desg_node2(Var *var, Designator *desg, Token *tok) {
  if (!desg)
    return new_var_node(var, tok);

  Node *node = new_desg_node2(var, desg->next, tok);

  if (desg->mem) {
    node = new_unary(ND_MEMBER, node, desg->mem->tok);
    node->member = desg->mem;
    return node;
  }

  node = new_add(node, new_num(desg->idx, tok), tok);
  return new_unary(ND_DEREF, node, tok);
}

static Node *new_desg_node(Var *var, Designator *desg, Node *rhs) {
  Node *lhs = new_desg_node2(var, desg, rhs->tok);
  Node *node = new_binary(ND_ASSIGN, lhs, rhs, rhs->tok);
  return new_unary(ND_EXPR_STMT, node, rhs->tok);
}

static Node *lvar_init_zero(Node *cur, Var *var, Type *ty, Designator *desg)  {
  if (ty->kind == TY_ARRAY) {
    for (int i = 0; i < ty->array_len; i++) {
      Designator desg2 = {desg, i++};
      cur = lvar_init_zero(cur, var, ty->base, &desg2);
    }
    return cur;
  }

  cur->next = new_desg_node(var, desg, new_num(0, token));
  return cur->next;
}
