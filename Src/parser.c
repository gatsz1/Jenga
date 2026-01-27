#include "jenga.h"

int resolve_local(char *name)
{
	for (int i = local_count - 1; i >= 0; i--) {
		if (strcmp(locals[i].name, name) == 0)
			return locals[i].index;
	}
	return -1;
}

int declare_local(char *name)
{
	if (local_count >= MAX_LOCALS) {
		printf("Error: Too many locals\n");
		exit(1);
	}
	Local *l = &locals[local_count++];
	strcpy(l->name, name);
	l->depth = scope_depth;
	l->index = local_count - 1;
	return l->index;
}

int expr();
void statement();
void block();

void primary()
{
	if (current.type == TOK_NUM) {
		emit(OP_PUSH);
		emit(current.val);
		match(TOK_NUM);
	} else if (current.type == TOK_STR) {
		int idx = add_string(current.text);
		emit(OP_PUSH_STR);
		emit(idx);
		match(TOK_STR);
	} else if (current.type == TOK_ID) {
		char name[64];
		strcpy(name, current.text);
		match(TOK_ID);
		if (current.type == TOK_LPAREN) {
			match(TOK_LPAREN);
			int fidx = -1;
			for (int i = 0; i < func_count; i++)
				if (!strcmp(functions[i].name, name))
					fidx = i;
			if (fidx == -1) {
				printf("Error: Undefined func '%s'\n", name);
				exit(1);
			}
			int argc = 0;
			if (current.type != TOK_RPAREN)
				do {
					expr();
					argc++;
					if (current.type == TOK_COMMA)
						match(TOK_COMMA);
					else
						break;
				} while (1);
			match(TOK_RPAREN);
			emit(OP_CALL);
			emit(functions[fidx].address);
			emit(argc);
		} else if (current.type == TOK_LBRACKET) {
			match(TOK_LBRACKET);
			expr();
			match(TOK_RBRACKET);
			int idx = resolve_local(name);
			if (idx != -1) {
				emit(OP_GET_LOCAL);
				emit(idx);
			} else {
				idx = find_global(name);
				emit(OP_GET_GLOBAL);
				emit(idx);
			}
			emit(OP_GET_ARR);
		} else {
			int idx = resolve_local(name);
			if (idx != -1) {
				emit(OP_GET_LOCAL);
				emit(idx);
			} else {
				idx = find_global(name);
				emit(OP_GET_GLOBAL);
				emit(idx);
			}
		}
	} else if (current.type == TOK_LPAREN) {
		match(TOK_LPAREN);
		expr();
		match(TOK_RPAREN);
	}
}

int expr()
{
	primary();
	while (current.type >= TOK_PLUS && current.type <= TOK_BIT_OR) {
		TokenType op = current.type;
		match(op);
		primary();
		if (op == TOK_PLUS)
			emit(OP_ADD);
		if (op == TOK_MINUS)
			emit(OP_SUB);
		if (op == TOK_MUL)
			emit(OP_MUL);
		if (op == TOK_DIV)
			emit(OP_DIV);
		if (op == TOK_MOD)
			emit(OP_MOD);
		if (op == TOK_LT)
			emit(OP_LT);
		if (op == TOK_GT)
			emit(OP_GT);
		if (op == TOK_EQ)
			emit(OP_EQ);
		if (op == TOK_NEQ)
			emit(OP_NEQ);
		if (op == TOK_AND)
			emit(OP_AND);
		if (op == TOK_OR)
			emit(OP_OR);
		if (op == TOK_BIT_AND)
			emit(OP_BIT_AND);
		if (op == TOK_BIT_OR)
			emit(OP_BIT_OR);
	}
	return 0;
}

void statement()
{
	if (current.type == TOK_NAMBA || current.type == TOK_SAFU) {
		int is_arr = (current.type == TOK_SAFU);
		match(current.type);
		char name[64];
		strcpy(name, current.text);
		match(TOK_ID);
		match(TOK_ASSIGN);
		if (is_arr) {
			match(TOK_SAFU);
			match(TOK_LPAREN);
			expr();
			match(TOK_RPAREN);
			emit(OP_ALLOC_ARR);
		} else
			expr();
		match(TOK_SEMI);
		if (scope_depth > 0)
			declare_local(name);
		else {
			strcpy(globals[global_count].name, name);
			emit(OP_SET_GLOBAL);
			emit(global_count++);
		}
	} else if (current.type == TOK_ID) {
		char name[64];
		strcpy(name, current.text);
		match(TOK_ID);
		if (current.type == TOK_LPAREN) {
			match(TOK_LPAREN);
			int argc = 0;
			if (current.type != TOK_RPAREN)
				do {
					expr();
					argc++;
					if (current.type == TOK_COMMA)
						match(TOK_COMMA);
					else
						break;
				} while (1);
			match(TOK_RPAREN);
			match(TOK_SEMI);
			int fidx = -1;
			for (int i = 0; i < func_count; i++)
				if (!strcmp(functions[i].name, name))
					fidx = i;
			emit(OP_CALL);
			emit(functions[fidx].address);
			emit(argc);
			emit(OP_POP);
		} else if (current.type == TOK_LBRACKET) {
			match(TOK_LBRACKET);
			expr();
			match(TOK_RBRACKET);
			match(TOK_ASSIGN);
			expr();
			match(TOK_SEMI);
			int idx = resolve_local(name);
			if (idx != -1) {
				emit(OP_GET_LOCAL);
				emit(idx);
			} else {
				idx = find_global(name);
				emit(OP_GET_GLOBAL);
				emit(idx);
			}
			emit(OP_SET_ARR);
		} else {
			match(TOK_ASSIGN);
			expr();
			match(TOK_SEMI);
			int idx = resolve_local(name);
			if (idx != -1) {
				emit(OP_SET_LOCAL);
				emit(idx);
			} else {
				idx = find_global(name);
				emit(OP_SET_GLOBAL);
				emit(idx);
			}
		}
	} else if (current.type == TOK_ANDIKA) {
		match(TOK_ANDIKA);
		match(TOK_LPAREN);
		expr();
		match(TOK_RPAREN);
		match(TOK_SEMI);
		emit(OP_PRINT);
	} else if (current.type == TOK_RUDISHA) {
		match(TOK_RUDISHA);
		expr();
		match(TOK_SEMI);
		emit(OP_RET);
	} else if (current.type == TOK_WAKATI) {
		match(TOK_WAKATI);
		match(TOK_LPAREN);
		int s = pc;
		expr();
		match(TOK_RPAREN);
		emit(OP_JZ);
		int p = pc++;
		block();
		emit(OP_JMP);
		emit(s);
		code[p] = pc;
	} else if (current.type == TOK_KAMA) {
		match(TOK_KAMA);
		match(TOK_LPAREN);
		expr();
		match(TOK_RPAREN);
		emit(OP_JZ);
		int p = pc++;
		block();
		if (current.type == TOK_SIVYO) {
			emit(OP_JMP);
			int pe = pc++;
			code[p] = pc;
			match(TOK_SIVYO);
			block();
			code[pe] = pc;
		} else
			code[p] = pc;
	} else if (current.type == TOK_LBRACE)
		block();
}

void block()
{
	match(TOK_LBRACE);
	scope_depth++;
	while (current.type != TOK_RBRACE && current.type != TOK_EOF)
		statement();
	scope_depth--;
	match(TOK_RBRACE);
}

void compile_function()
{
	match(TOK_KAZI);
	char name[64];
	strcpy(name, current.text);
	match(TOK_ID);
	strcpy(functions[func_count].name, name);
	functions[func_count].address = pc;
	match(TOK_LPAREN);
	local_count = 0;
	scope_depth = 1;
	int args = 0;
	if (current.type != TOK_RPAREN)
		do {
			if (current.type == TOK_NAMBA
			    || current.type == TOK_SAFU)
				next_token();
			char an[64];
			strcpy(an, current.text);
			match(TOK_ID);
			declare_local(an);
			args++;
			if (current.type == TOK_COMMA)
				match(TOK_COMMA);
			else
				break;
		} while (1);
	functions[func_count].arg_count = args;
	func_count++;
	match(TOK_RPAREN);
	block();
	emit(OP_PUSH);
	emit(0);
	emit(OP_RET);
}

void compile()
{
	emit(OP_JMP);		
	int main_jump = pc++;	

	while (current.type != TOK_EOF) {
		if (current.type == TOK_KAZI)
			compile_function();
		else if (current.type == TOK_ANZA) {
			match(TOK_ANZA);
			code[main_jump] = pc;	
			scope_depth = 1;
			local_count = 0;
			block();
			emit(OP_HALT);
		} else
			statement();
	}
}
