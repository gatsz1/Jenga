/* JENGA PROGRAMMING LANGUAGE 1.0
 * Still doesn't support functions 
 * Refer to README.md to see whats supported for now 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum { OBJ_STRING, OBJ_ARRAY } ObjType;

typedef struct sObj {
    ObjType type;
    unsigned char marked; 
    struct sObj* next;    
    
    union {
        char* chars;      
        struct {
            int len;
            int* elements;
        } array; 
    } as;
} Obj;

typedef enum { VAL_NUM, VAL_OBJ } ValType;

typedef struct {
    ValType type;
    union {
        int num;
        Obj* obj;
    } as;
} Value;

#define STACK_MAX 256
Value stack[STACK_MAX];
int sp = 0; 
#define MAX_GLOBALS 256
Value globals[MAX_GLOBALS];
char* global_names[MAX_GLOBALS];
int globals_count = 0;

Obj* heap_head = NULL; 
int num_objects = 0;
int max_objects = 8;   

/*Mark & Sweep garbage collector*/

void mark_object(Obj* obj) {
    if (obj == NULL || obj->marked) return;
    obj->marked = 1;
}

void mark_roots() {
    for (int i = 0; i < sp; i++) {
        if (stack[i].type == VAL_OBJ) {
            mark_object(stack[i].as.obj);
        }
    }
    for (int i = 0; i < 26; i++) {
        if (globals[i].type == VAL_OBJ) {
            mark_object(globals[i].as.obj);
        }
    }
}

void sweep() {
    Obj** object = &heap_head;
    while (*object) {
        if (!(*object)->marked) {
            Obj* unreached = *object;
            *object = unreached->next;
            
            if (unreached->type == OBJ_STRING) {
                free(unreached->as.chars);
            } else if (unreached->type == OBJ_ARRAY) {
                free(unreached->as.array.elements);
            }
            free(unreached);
            num_objects--;
        } else {
            (*object)->marked = 0;
            object = &(*object)->next;
        }
    }
}

void collect_garbage() {
   /*  printf(" %d)\n", num_objects);"/*" Testing if the garbage collector works*/
    mark_roots();
    sweep();
    max_objects = num_objects * 2;
    if (max_objects < 8) max_objects = 8;
    
   /* printf("%d%d)\n", num_objects, max_objects);*/
}

int resolve_global(const char* name) {
    for (int i = 0; i < globals_count; i++) {
        if (!strcmp(global_names[i], name)) return i;
    }
    if (globals_count >= MAX_GLOBALS) {
        printf("Kosa: Too many global variables\n");
        exit(1);
    }
    global_names[globals_count] = strdup(name);
    return globals_count++;
}
Obj* allocate_obj(ObjType type) {
    if (num_objects >= max_objects) collect_garbage();
    
    Obj* obj = (Obj*)malloc(sizeof(Obj));
    obj->type = type;
    obj->marked = 0;
    obj->next = heap_head;
    heap_head = obj;
    num_objects++;
    return obj;
}

/* Lexer*/
typedef enum {
    TOK_EOF, 
    TOK_NAMBA, TOK_MANENO, TOK_SAFU, 
    TOK_ANDIKA, TOK_ANZA, TOK_MALIZA, 
    TOK_KAMA, TOK_SIVYO,             
    TOK_WAKATI, TOK_FANYA,           
    TOK_CHAGUA, TOK_KISA, TOK_KAWAIDA, 
    TOK_KWA,
    TOK_TRUE, TOK_FALSE,
    TOK_ID, TOK_NUM_LIT, TOK_STR_LIT,
    TOK_ASSIGN, TOK_SEMI, TOK_PLUS, TOK_MINUS, 
    TOK_EQ, TOK_LT, TOK_GT, TOK_COLON,
    TOK_LPAREN, TOK_RPAREN, TOK_LBRACKET, TOK_RBRACKET
} TokenType;

typedef struct { TokenType type; char text[128]; int num; } Token;
char* src;
Token current;

void skip_whitespace_and_comments() {
    while (1) {
        while (isspace(*src)) src++;
        if (src[0] == '/' && src[1] == '/') {
            src += 2;
            while (*src != '\n' && *src != '\0') src++;
            continue;
        }
        if (src[0] == '/' && src[1] == '*') {
            src += 2;
            while (*src != '\0' && !(src[0] == '*' && src[1] == '/')) src++;
            if (src[0] == '*' && src[1] == '/') src += 2;
            continue;
        }
        break;
    }
}

void next_token() {
    skip_whitespace_and_comments();
    if (*src == '\0') { current.type = TOK_EOF; return; }

    if (isdigit(*src)) {
        current.type = TOK_NUM_LIT;
        current.num = 0;
        while (isdigit(*src)) current.num = current.num * 10 + (*src++ - '0');
        return;
    }

     if (*src == '"') {
        current.type = TOK_STR_LIT;
        src++;         int i = 0;
        while (*src != '"' && *src != '\0') current.text[i++] = *src++;
        current.text[i] = '\0';
        if (*src == '"') src++;
        return;
    }

    /*keywords*/
    if (isalpha(*src) || *src == '_') {
        int i = 0;
        while (isalnum(*src) || *src == '_') current.text[i++] = *src++;
        current.text[i] = '\0';
        
        if (!strcmp(current.text, "namba")) current.type = TOK_NAMBA;
        else if (!strcmp(current.text, "safu")) current.type = TOK_SAFU;
        else if (!strcmp(current.text, "andika")) current.type = TOK_ANDIKA;
        else if (!strcmp(current.text, "anza")) current.type = TOK_ANZA;
        else if (!strcmp(current.text, "maliza")) current.type = TOK_MALIZA;
        else if (!strcmp(current.text, "kama")) current.type = TOK_KAMA;     // if
        else if (!strcmp(current.text, "sivyo")) current.type = TOK_SIVYO;   // else
        else if (!strcmp(current.text, "wakati")) current.type = TOK_WAKATI; // while
        else if (!strcmp(current.text, "fanya")) current.type = TOK_FANYA;   // do
        else if (!strcmp(current.text, "chagua")) current.type = TOK_CHAGUA; // switch
        else if (!strcmp(current.text, "kisa")) current.type = TOK_KISA;     // case
        else if (!strcmp(current.text, "kawaida")) current.type = TOK_KAWAIDA; // default
        else if (!strcmp(current.text, "kwa")) current.type = TOK_KWA;       // for
        else if (!strcmp(current.text, "kweli")) current.type = TOK_TRUE;    // true
        else if (!strcmp(current.text, "sikweli")) current.type = TOK_FALSE; // false
        else current.type = TOK_ID;
        return;
    }

    /*symbols*/
    switch (*src++) {
        case '=': current.type = (*src == '=') ? (src++, TOK_EQ) : TOK_ASSIGN; break;
        case '+': current.type = TOK_PLUS; break;
        case '-': current.type = TOK_MINUS; break;
        case ';': current.type = TOK_SEMI; break;
        case ':': current.type = TOK_COLON; break;
        case '<': current.type = TOK_LT; break;
        case '>': current.type = TOK_GT; break;
        case '(': current.type = TOK_LPAREN; break;
        case ')': current.type = TOK_RPAREN; break;
        case '[': current.type = TOK_LBRACKET; break;
        case ']': current.type = TOK_RBRACKET; break;
        default: printf("Kosa: %c\n", src[-1]); exit(1);
    }
}

void match(TokenType t) {
    if (current.type == t) next_token();
    else { printf("Syntax Error: Expected %d got %d text: %s\n", t, current.type, current.text); exit(1); }
}

/*compiler and bytecode ---> Basically it kinda like that one in java 
 * python ... :) a simple one tho */

typedef enum {
    OP_HALT, 
    OP_CONST, 
    OP_CONST_STR,
    OP_ALLOC_ARR,
    OP_GET_VAR, 
    OP_SET_VAR,
    OP_GET_IDX, 
    OP_SET_IDX,
    OP_ADD,
    OP_SUB,
    OP_EQ,
    OP_LT, 
    OP_GT,
    OP_PRINT, 
    OP_JMP, 
    OP_JZ, 
    OP_DUP
} OpCode;

int code[2048];
char* str_pool[128]; 
int code_idx = 0;
int str_idx = 0;

void emit(int op) { code[code_idx++] = op; }

int expr();

void primary() {
    if (current.type == TOK_NUM_LIT) {
        emit(OP_CONST); emit(current.num); match(TOK_NUM_LIT);
    } else if (current.type == TOK_TRUE) {
        emit(OP_CONST); emit(1); match(TOK_TRUE);
    } else if (current.type == TOK_FALSE) {
        emit(OP_CONST); emit(0); match(TOK_FALSE);
    } else if (current.type == TOK_STR_LIT) {
        emit(OP_CONST_STR); 
        str_pool[str_idx] = strdup(current.text);
        emit(str_idx++); 
        match(TOK_STR_LIT);
    } else if (current.type == TOK_ID) {
        int var = resolve_global(current.text);
        match(TOK_ID);
        emit(OP_GET_VAR); emit(var);

        if (current.type == TOK_LBRACKET) {
            match(TOK_LBRACKET);
            expr();
            match(TOK_RBRACKET);
            emit(OP_GET_IDX);
        }
    } else if (current.type == TOK_SAFU) {
        match(TOK_SAFU); match(TOK_LPAREN);
        expr(); 
        match(TOK_RPAREN);
        emit(OP_ALLOC_ARR);
    }
}

int expr() {
    primary();
    while (current.type == TOK_PLUS || current.type == TOK_MINUS || 
           current.type == TOK_LT || current.type == TOK_GT || current.type == TOK_EQ) {
        TokenType op = current.type;
        match(op);
        primary();
        if (op == TOK_PLUS) emit(OP_ADD);
        if (op == TOK_MINUS) emit(OP_SUB);
        if (op == TOK_LT) emit(OP_LT);
        if (op == TOK_GT) emit(OP_GT);
        if (op == TOK_EQ) emit(OP_EQ);
    }
    return 0;
}

void block();

void statement() {
    if (current.type == TOK_ANDIKA) {
        match(TOK_ANDIKA);
        match(TOK_LPAREN); 
        expr();
        emit(OP_PRINT);
        match(TOK_RPAREN);
        match(TOK_SEMI);
    }
    else if (current.type == TOK_NAMBA || current.type == TOK_SAFU) {
        if (current.type == TOK_NAMBA) match(TOK_NAMBA);
        else match(TOK_SAFU);
        
        int var = resolve_global(current.text);
        match(TOK_ID);
        match(TOK_ASSIGN);
        expr();
        emit(OP_SET_VAR); emit(var);
        match(TOK_SEMI);
    }
    else if (current.type == TOK_ID) {
        int var = resolve_global(current.text);
        match(TOK_ID);
        
        if (current.type == TOK_ASSIGN) {
            match(TOK_ASSIGN);
            expr();
            emit(OP_SET_VAR); emit(var);
        } else if (current.type == TOK_LBRACKET) {
            emit(OP_GET_VAR); emit(var); 
            match(TOK_LBRACKET);
            expr(); 
            match(TOK_RBRACKET);
            match(TOK_ASSIGN);
            expr(); 
            emit(OP_SET_IDX);
        }
        match(TOK_SEMI);
    }
    else if (current.type == TOK_KWA) {
        match(TOK_KWA); match(TOK_LPAREN);

        if (current.type != TOK_SEMI) {
            if (current.type == TOK_NAMBA || current.type == TOK_SAFU) {
                if (current.type == TOK_NAMBA) match(TOK_NAMBA);
                else match(TOK_SAFU);
                int var = resolve_global(current.text);
                match(TOK_ID);
                match(TOK_ASSIGN);
                expr();
                emit(OP_SET_VAR); emit(var);
                match(TOK_SEMI);
            } else if (current.type == TOK_ID) {
                int var = resolve_global(current.text);
                match(TOK_ID);
                if (current.type == TOK_ASSIGN) {
                    match(TOK_ASSIGN);
                    expr();
                    emit(OP_SET_VAR); emit(var);
                } else if (current.type == TOK_LBRACKET) {
                    emit(OP_GET_VAR); emit(var);
                    match(TOK_LBRACKET);
                    expr();
                    match(TOK_RBRACKET);
                    match(TOK_ASSIGN);
                    expr();
                    emit(OP_SET_IDX);
                } else {
                    printf("Syntax Error: Invalid for-loop init\n");
                    exit(1);
                }
                match(TOK_SEMI);
            } else {
                printf("Syntax Error: Invalid for-loop init\n");
                exit(1);
            }
        } else {
            match(TOK_SEMI);
        }

        int loop_start = code_idx;
        if (current.type != TOK_SEMI) {
            expr();
        } else {
            emit(OP_CONST); emit(1);
        }
        match(TOK_SEMI);

        emit(OP_JZ); int patch_end = code_idx++;
        emit(OP_JMP); int patch_body = code_idx++;
        int post_start = code_idx;

        if (current.type != TOK_RPAREN) {
            if (current.type == TOK_ID) {
                int var = resolve_global(current.text);
                match(TOK_ID);
                if (current.type == TOK_ASSIGN) {
                    match(TOK_ASSIGN);
                    expr();
                    emit(OP_SET_VAR); emit(var);
                } else if (current.type == TOK_LBRACKET) {
                    emit(OP_GET_VAR); emit(var);
                    match(TOK_LBRACKET);
                    expr();
                    match(TOK_RBRACKET);
                    match(TOK_ASSIGN);
                    expr();
                    emit(OP_SET_IDX);
                } else {
                    printf("Syntax Error: Invalid for-loop post\n");
                    exit(1);
                }
            } else {
                printf("Syntax Error: Invalid for-loop post\n");
                exit(1);
            }
        }

        emit(OP_JMP); emit(loop_start);

        int body_start = code_idx;
        code[patch_body] = body_start;
        match(TOK_RPAREN);
        block();
        emit(OP_JMP); emit(post_start);
        code[patch_end] = code_idx;
    }
    else if (current.type == TOK_KAMA) { 
        match(TOK_KAMA); match(TOK_LPAREN); expr(); match(TOK_RPAREN);
        emit(OP_JZ); int patch_else = code_idx++;
        block();
        
        if (current.type == TOK_SIVYO) {
            emit(OP_JMP); int patch_end = code_idx++;
            code[patch_else] = code_idx; 
            match(TOK_SIVYO);
            block();
            code[patch_end] = code_idx;
        } else {
            code[patch_else] = code_idx;
        }
    }
    else if (current.type == TOK_WAKATI) { 
        match(TOK_WAKATI); match(TOK_LPAREN);
        int start = code_idx;
        expr(); match(TOK_RPAREN);
        emit(OP_JZ); int patch = code_idx++;
        block();
        emit(OP_JMP); emit(start);
        code[patch] = code_idx;
    }
    else if (current.type == TOK_FANYA) { 
        match(TOK_FANYA);
        int start = code_idx;
        block();
        match(TOK_WAKATI); match(TOK_LPAREN);
        expr(); match(TOK_RPAREN);
        match(TOK_SEMI);
        emit(OP_JZ); int skip = code_idx++;
        emit(OP_JMP); emit(start);
        code[skip] = code_idx;
    }
    else if (current.type == TOK_CHAGUA) { 
        match(TOK_CHAGUA); match(TOK_LPAREN); 
        int var = resolve_global(current.text);
        match(TOK_ID); match(TOK_RPAREN); match(TOK_ANZA);
        
        int end_jumps[20]; int end_count = 0;
        
        while (current.type == TOK_KISA) {
            match(TOK_KISA);
            emit(OP_GET_VAR); emit(var);
            expr();
            emit(OP_EQ); 
            emit(OP_JZ); int patch_next = code_idx++;
            
            while(current.type != TOK_KISA && current.type != TOK_KAWAIDA && current.type != TOK_MALIZA)
                statement();
                
            emit(OP_JMP); end_jumps[end_count++] = code_idx++;
            code[patch_next] = code_idx; 
            match(TOK_COLON);
        }
        
        if (current.type == TOK_KAWAIDA) {
            match(TOK_KAWAIDA); match(TOK_COLON);
            while(current.type != TOK_MALIZA) statement();
        }
        match(TOK_MALIZA);
        
        for(int i=0; i<end_count; i++) code[end_jumps[i]] = code_idx;
    }
}

void block() {
    match(TOK_ANZA);
    while (current.type != TOK_MALIZA && current.type != TOK_EOF) statement();
    match(TOK_MALIZA);
}


void run_vm() {
    int ip = 0;

    for(int i=0;i<MAX_GLOBALS;i++) globals[i] = (Value){VAL_NUM, {0}};

    while (1) {
        int op = code[ip++];
        
        switch (op) {
            case OP_HALT: return;
            
            case OP_CONST: 
                stack[sp++] = (Value){VAL_NUM, {.num = code[ip++]}}; 
                break;
                
            case OP_CONST_STR: {
                Obj* obj = allocate_obj(OBJ_STRING);
                obj->as.chars = strdup(str_pool[code[ip++]]);
                stack[sp++] = (Value){VAL_OBJ, {.obj = obj}};
                break;
            }
            
            case OP_ALLOC_ARR: {
                int size = stack[--sp].as.num;
                Obj* obj = allocate_obj(OBJ_ARRAY);
                obj->as.array.len = size;
                obj->as.array.elements = calloc(size, sizeof(int));
                stack[sp++] = (Value){VAL_OBJ, {.obj = obj}};
                break;
            }
            
            case OP_GET_VAR: stack[sp++] = globals[code[ip++]]; break;
            
            case OP_SET_VAR: globals[code[ip++]] = stack[--sp]; break;
            
            case OP_GET_IDX: {
                int idx = stack[--sp].as.num;
                Obj* arr = stack[--sp].as.obj;
                if (idx < 0 || idx >= arr->as.array.len) { printf("Kosa: Array Index Out of Bounds\n"); exit(1); }
                stack[sp++] = (Value){VAL_NUM, {.num = arr->as.array.elements[idx]}};
                break;
            }
            
            case OP_SET_IDX: {
                int val = stack[--sp].as.num;
                int idx = stack[--sp].as.num;
                Obj* arr = stack[--sp].as.obj;
                arr->as.array.elements[idx] = val;
                break;
            }
            
            case OP_ADD: {
                Value b = stack[--sp];
                Value a = stack[--sp];
                if (a.type == VAL_NUM && b.type == VAL_NUM) {
                    stack[sp++] = (Value){VAL_NUM, {.num = a.as.num + b.as.num}};
                } else if (a.type == VAL_OBJ && b.type == VAL_OBJ &&
                           a.as.obj->type == OBJ_STRING && b.as.obj->type == OBJ_STRING) {
                    int la = (int)strlen(a.as.obj->as.chars);
                    int lb = (int)strlen(b.as.obj->as.chars);
                    Obj* obj = allocate_obj(OBJ_STRING);
                    obj->as.chars = (char*)malloc(la + lb + 1);
                    memcpy(obj->as.chars, a.as.obj->as.chars, la);
                    memcpy(obj->as.chars + la, b.as.obj->as.chars, lb);
                    obj->as.chars[la + lb] = '\0';
                    stack[sp++] = (Value){VAL_OBJ, {.obj = obj}};
                } else {
                    printf("Kosa: Invalid operands for +\n");
                    exit(1);
                }
                break;
            }

            case OP_SUB: {
                int b = stack[--sp].as.num;
                int a = stack[--sp].as.num;
                stack[sp++] = (Value){VAL_NUM, {.num = a - b}};
                break;
            }
            case OP_LT: {
                int b = stack[--sp].as.num;
                int a = stack[--sp].as.num;
                stack[sp++] = (Value){VAL_NUM, {.num = (a < b)}};
                break;
            }
            case OP_GT: {
                int b = stack[--sp].as.num;
                int a = stack[--sp].as.num;
                stack[sp++] = (Value){VAL_NUM, {.num = (a > b)}};
                break;
            }

            case OP_EQ: {
                int b = stack[--sp].as.num;
                int a = stack[--sp].as.num;
                stack[sp++] = (Value){VAL_NUM, {.num = (a == b)}};
                break;
            }
            
            case OP_PRINT: {
                Value v = stack[--sp];
                if (v.type == VAL_NUM) printf("%d\n", v.as.num);
                else if (v.type == VAL_OBJ && v.as.obj->type == OBJ_STRING) printf("%s\n", v.as.obj->as.chars);
                else printf("[Array]\n");
                break;
            }
            
            case OP_JZ: {
                int target = code[ip++];
                if (stack[--sp].as.num == 0) ip = target;
                break;
            }
            
            case OP_JMP: ip = code[ip++]; break;
        }
    }
}
char* read_file(const char* f) {
    FILE* file = fopen(f, "r");
    if (!file) return NULL;
    fseek(file, 0, SEEK_END); long l = ftell(file); rewind(file);
    char* b = malloc(l+1); fread(b, 1, l, file); b[l]=0; fclose(file);
    return b;
}

int main(int argc, char** argv) {
    if (argc < 2) { printf("Usage: ./Jenga <file.j>\n"); return 1; }
    
    src = read_file(argv[1]);
    if (!src) { printf("Error reading file\n"); return 1; }
    
    next_token();
    block(); 
    emit(OP_HALT);
    
    run_vm();
    return 0;
}
