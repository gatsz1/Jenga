#include "jenga.h"

char* string_pool[MAX_STRINGS];
int str_count = 0;
struct GlobalVar globals[MAX_GLOBALS];
int global_count = 0;
Function functions[MAX_FUNCS];
int func_count = 0;
Local locals[MAX_LOCALS];
int local_count = 0;
int scope_depth = 0;
int code[MAX_CODE];
int pc = 0;
char* src;
Token current;

/*Virtual Machine State */
Value stack[MAX_STACK];
int sp = 0;
int bp = 0;
Value global_mem[MAX_GLOBALS];

/* Garbage Collector State*/
Obj* objects = NULL;
int num_objects = 0;
int max_objects = 8;
