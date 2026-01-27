#include "jenga.h"

char *my_strdup(const char *s)
{
	if (s == NULL)
		return NULL;
	size_t len = strlen(s);
	char *copy = malloc(len + 1);
	if (copy)
		strcpy(copy, s);
	return copy;
}

int add_string(char *s)
{
	if (str_count >= MAX_STRINGS) {
		printf("Compiler Error: String pool overflow\n");
		exit(1);
	}
	if (s == NULL) {
		printf("Compiler Error: Attempted to add NULL string\n");
		exit(1);
	}

	string_pool[str_count] = my_strdup(s);
	if (string_pool[str_count] == NULL) {
		printf("Compiler Error: Failed to allocate string '%s'\n", s);
		exit(1);
	}

	return str_count++;
}

int find_global(char *name)
{
	for (int i = 0; i < global_count; i++)
		if (strcmp(globals[i].name, name) == 0)
			return i;
	return -1;
}

char *read_file_content(const char *f)
{
	FILE *file = fopen(f, "r");
	if (!file)
		return NULL;
	fseek(file, 0, SEEK_END);
	long l = ftell(file);
	rewind(file);
	char *b = malloc(l + 1);
	if (b) {
		fread(b, 1, l, file);
		b[l] = 0;
	}
	fclose(file);
	return b;
}

void emit(int op)
{
	if (pc >= MAX_CODE) {
		printf("Compiler Error: Code overflow\n");
		exit(1);
	}
	code[pc++] = op;
}

void match(TokenType t)
{
	if (current.type == t)
		next_token();
	else {
		printf("Syntax Error: Expected token %d, got %d. Text: '%s'\n",
		       t, current.type, current.text);
		exit(1);
	}
}
