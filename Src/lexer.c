#include "jenga.h"

void next_token() {
    while(*src) {
        if(isspace(*src)) { src++; continue; }
        if(*src == '/' && *(src+1) == '/') { 
            while(*src && *src != '\n') src++; 
            continue; 
        }
        if(*src == '#') { 
            while(*src && *src != '\n') src++; 
            continue; 
        }
        break;
    }

    if(*src == '\0') { current.type = TOK_EOF; return; }

    if(*src == '"') {
        src++; 
        int i = 0;
        while(*src && *src != '"') {
            current.text[i++] = *src++;
        }
        if(*src == '"') src++; 
        current.text[i] = '\0';
        current.type = TOK_STR;
        return;
    }

    if(isdigit(*src)) {
        current.type = TOK_NUM; current.val = 0;
        while(isdigit(*src)) current.val = current.val * 10 + (*src++ - '0');
        return;
    }

    if(isalpha(*src) || *src == '_') {
        int i = 0;
        while(isalnum(*src) || *src == '_') current.text[i++] = *src++;
        current.text[i] = '\0';
        
        if(!strcmp(current.text, "namba")) current.type = TOK_NAMBA;
        else if(!strcmp(current.text, "safu")) current.type = TOK_SAFU;
        else if(!strcmp(current.text, "kazi")) current.type = TOK_KAZI;
        else if(!strcmp(current.text, "anza")) current.type = TOK_ANZA;
        else if(!strcmp(current.text, "andika")) current.type = TOK_ANDIKA;
        else if(!strcmp(current.text, "kama")) current.type = TOK_KAMA;
        else if(!strcmp(current.text, "sivyo")) current.type = TOK_SIVYO;
        else if(!strcmp(current.text, "wakati")) current.type = TOK_WAKATI;
        else if(!strcmp(current.text, "rudisha")) current.type = TOK_RUDISHA;
        else current.type = TOK_ID;
        return;
    }

    switch(*src++) {
        case '{': current.type = TOK_LBRACE; break;
        case '}': current.type = TOK_RBRACE; break;
        case '(': current.type = TOK_LPAREN; break;
        case ')': current.type = TOK_RPAREN; break;
        case '[': current.type = TOK_LBRACKET; break;
        case ']': current.type = TOK_RBRACKET; break;
        case ';': current.type = TOK_SEMI; break;
        case ',': current.type = TOK_COMMA; break;
        case '+': current.type = TOK_PLUS; break;
        case '-': current.type = TOK_MINUS; break;
        case '*': current.type = TOK_MUL; break;
        case '/': current.type = TOK_DIV; break;
        case '%': current.type = TOK_MOD; break;
        case '&': if(*src=='&'){src++; current.type=TOK_AND;} else current.type=TOK_BIT_AND; break;
        case '|': if(*src=='|'){src++; current.type=TOK_OR;} else current.type=TOK_BIT_OR; break;
        case '=': if(*src=='='){src++; current.type=TOK_EQ;} else current.type=TOK_ASSIGN; break;
        case '!': if(*src=='='){src++; current.type=TOK_NEQ;} break; 
        case '<': if(*src=='='){src++; current.type=TOK_LEQ;} else current.type=TOK_LT; break;
        case '>': if(*src=='='){src++; current.type=TOK_GEQ;} else current.type=TOK_GT; break;
        default: printf("Unknown char: %c\n", *(src-1)); exit(1);
    }
}
