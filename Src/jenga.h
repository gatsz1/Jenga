#ifndef JENGA_H
#define JENGA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Configs*/
#define MAX_SRC 50000
#define MAX_CODE 20000
#define MAX_STACK 5000
#define MAX_LOCALS 500
#define MAX_GLOBALS 500
#define MAX_FUNCS 200
#define MAX_STRINGS 1000

typedef struct sObj Obj;

typedef enum { VAL_NUM, VAL_OBJ } ValType;

typedef struct {
    ValType type;
    union {
        int num;    
        Obj* obj;   
    } as;
} Value;

typedef enum { OBJ_STRING, OBJ_ARRAY } ObjType;

struct sObj {
    ObjType type;
    unsigned char marked;
    struct sObj* next;
    union {
        char* chars;
        struct { 
            int len; 
            Value* elements; 
        } array;
    } as;
};

typedef enum {
    TOK_EOF, TOK_ID, TOK_NUM, TOK_STR,
    TOK_NAMBA, TOK_SAFU, TOK_ANDIKA, TOK_KAZI, TOK_ANZA,
    TOK_KAMA, TOK_SIVYO, TOK_WAKATI, TOK_RUDISHA,
    TOK_LBRACE, TOK_RBRACE, TOK_LPAREN, TOK_RPAREN, 
    TOK_LBRACKET, TOK_RBRACKET, TOK_SEMI, TOK_COMMA, TOK_ASSIGN,
    TOK_PLUS, TOK_MINUS, TOK_MUL, TOK_DIV, TOK_MOD,
    TOK_EQ, TOK_NEQ, TOK_LT, TOK_GT, TOK_LEQ, TOK_GEQ,
    TOK_AND, TOK_OR, TOK_BIT_AND, TOK_BIT_OR
} TokenType;

typedef struct { TokenType type; char text[256]; int val; } Token;

typedef enum {
    OP_HALT, OP_PUSH, OP_PUSH_STR, OP_POP,
    OP_GET_GLOBAL, OP_SET_GLOBAL,
    OP_GET_LOCAL,  OP_SET_LOCAL, 
    OP_ALLOC_ARR, OP_GET_ARR, OP_SET_ARR,
    OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD,
    OP_BIT_AND, OP_BIT_OR,
    OP_EQ, OP_NEQ, OP_LT, OP_GT, OP_LEQ, OP_GEQ,
    OP_AND, OP_OR, OP_NOT,
    OP_JMP, OP_JZ, 
    OP_CALL, OP_RET, OP_PRINT
} OpCode;

typedef struct { char name[64]; int address; int arg_count; } Function;
typedef struct { char name[64]; int depth; int index; } Local;

extern char* string_pool[MAX_STRINGS];
extern int str_count;

extern struct GlobalVar { char name[64]; } globals[MAX_GLOBALS];
extern int global_count;

extern Function functions[MAX_FUNCS];
extern int func_count;

extern Local locals[MAX_LOCALS];
extern int local_count;
extern int scope_depth;

extern int code[MAX_CODE];
extern int pc;
extern char* src;
extern Token current;

extern Value stack[MAX_STACK]; 
extern int sp;
extern int bp;
extern Value global_mem[MAX_GLOBALS];

extern Obj* objects;
extern int num_objects;
extern int max_objects;

int add_string(char* s);
int find_global(char* name);
char* read_file_content(const char* f);
char* my_strdup(const char* s);
void emit(int op);
void next_token();
void match(TokenType t);
void compile();
Obj* gc_allocate(ObjType type);
void gc_collect();
void run_vm();
void init_natives();

#endif
