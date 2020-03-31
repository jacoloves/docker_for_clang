#include "9cc.h"
//
// main
//
int main(int argc, char **argv) {
  if (argc != 2) 
    error("%s: invalid number of arguments", argv[0]);

  user_input = argv[1];
  token = tokenize();
  Node *node = expr();

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // Traverse the AST to emit assembly.
  gen(node);

  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}
