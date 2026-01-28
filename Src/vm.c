#include "jenga.h"

#define PUSH(v) (stack[sp++] = (v))
#define POP() (stack[--sp])
#define PUSH_NUM(n) PUSH(((Value){VAL_NUM, {.num=(n)}}))
#define PUSH_OBJ(o) PUSH(((Value){VAL_OBJ, {.obj=(o)}}))

/* For any contributor this ensures to check overflows
 * If You happen to find a bug Open an issue
 */
#define CHECK_STACK(count) if(sp < (count)) { printf("VM Error: Stack Underflow at IP %d\n", ip); exit(1); }

void init_natives()
{
	strcpy(functions[0].name, "soma_faili");
	functions[0].address = -1;
	functions[0].arg_count = 1;
	strcpy(functions[1].name, "urefu");
	functions[1].address = -2;
	functions[1].arg_count = 1;
	strcpy(functions[2].name, "herufi");
	functions[2].address = -3;
	functions[2].arg_count = 2;
	strcpy(functions[3].name, "unganisha");
	functions[3].address = -4;
	functions[3].arg_count = 2;
	strcpy(functions[4].name, "toka");
	functions[4].address = -5;
	functions[4].arg_count = 1;
	func_count = 5;
}

void run_vm()
{
	int ip = 0;
	bp = -1;
	for (int i = 0; i < MAX_GLOBALS; i++)
		global_mem[i] = (Value) {
		VAL_NUM, {
		0}};

	while (1) {
		int op = code[ip++];
		switch (op) {
		case 0:
			return;	

		case 1:{	
				int val = code[ip++];
				PUSH_NUM(val);
				break;
			}

		case 2:{	
				int idx = code[ip++];
				if (idx < 0 || idx >= MAX_STRINGS) {
					printf
					    ("VM Error: String Index %d out of bounds\n",
					     idx);
					exit(1);
				}
				if (string_pool[idx] == NULL) {
					printf
					    ("VM Error: String Pool[%d] is NULL)",
					     idx);
					exit(1);
				}
				Obj *o = gc_allocate(OBJ_STRING);
				o->as.chars = my_strdup(string_pool[idx]);
				PUSH_OBJ(o);
				break;
			}

		case 3:
			CHECK_STACK(1);
			sp--;
			break;	

		case 4:
			PUSH(global_mem[code[ip++]]);
			break;	
		case 5:
			global_mem[code[ip++]] = POP();
			break;	
		case 6:
			PUSH(stack[bp + 1 + code[ip++]]);
			break;	
		case 7:
			stack[bp + 1 + code[ip++]] = POP();
			break;	

		case 8:{	
				CHECK_STACK(1);
				int size = POP().as.num;
				Obj *o = gc_allocate(OBJ_ARRAY);
				o->as.array.len = size;
				o->as.array.elements =
				    calloc(size, sizeof(Value));
				for (int k = 0; k < size; k++)
					o->as.array.elements[k] = (Value) {
					VAL_NUM, {
					0}};
				PUSH_OBJ(o);
				break;
			}

		case 9:{	
				CHECK_STACK(2);
				Obj *o = POP().as.obj;
				int idx = POP().as.num;
				if (idx < 0 || idx >= o->as.array.len) {
					printf
					    ("Runtime Error: Index %d Out of Bounds\n",
					     idx);
					exit(1);
				}
				PUSH(o->as.array.elements[idx]);
				break;
			}

		case 10:{	
				CHECK_STACK(3);
				Obj *o = POP().as.obj;
				Value v = POP();
				int idx = POP().as.num;
				if (idx < 0 || idx >= o->as.array.len) {
					printf
					    ("Runtime Error: Index %d Out of Bounds\n",
					     idx);
					exit(1);
				}
				o->as.array.elements[idx] = v;
				break;
			}

			
		case 11:{
				CHECK_STACK(2);
				int b = POP().as.num;
				int a = POP().as.num;
				PUSH_NUM(a + b);
				break;
			}
		case 12:{
				CHECK_STACK(2);
				int b = POP().as.num;
				int a = POP().as.num;
				PUSH_NUM(a - b);
				break;
			}
		case 13:{
				CHECK_STACK(2);
				int b = POP().as.num;
				int a = POP().as.num;
				PUSH_NUM(a * b);
				break;
			}
		case 14:{
				CHECK_STACK(2);
				int b = POP().as.num;
				int a = POP().as.num;
				PUSH_NUM(a / b);
				break;
			}
		case 15:{
				CHECK_STACK(2);
				int b = POP().as.num;
				int a = POP().as.num;
				PUSH_NUM(a % b);
				break;
			}

		case 18:{	
				CHECK_STACK(2);
				Value b = POP();
				Value a = POP();
				if (a.type != b.type)
					PUSH_NUM(0);
				else if (a.type == VAL_NUM)
					PUSH_NUM(a.as.num == b.as.num);
				else
					PUSH_NUM(a.as.obj == b.as.obj);	
				break;
			}
		case 19:{
				CHECK_STACK(2);
				int b = POP().as.num;
				int a = POP().as.num;
				PUSH_NUM(a != b);
				break;
			}
		case 20:{
				CHECK_STACK(2);
				int b = POP().as.num;
				int a = POP().as.num;
				PUSH_NUM(a < b);
				break;
			}

		case 27:
			ip = code[ip];
			break;	

		case 28:{
				CHECK_STACK(1);
				int t = code[ip++];
				if (POP().as.num == 0)
					ip = t;
				break;
			}	

		case 29:{	
				int target = code[ip++];
				int ac = code[ip++];
				CHECK_STACK(ac);
				if (target < 0) {
					
					int nid = target;
					Value res = { VAL_NUM, {0} };
					if (nid == -1) {
						Obj *path = POP().as.obj;
						char *c =
						    read_file_content(path->
								      as.chars);
						if (!c)
							c = my_strdup("");
						Obj *o =
						    gc_allocate(OBJ_STRING);
						o->as.chars = c;
						res = (Value) {
							VAL_OBJ, {
							.obj = o}
						};
					} else if (nid == -2) {
						Value v = POP();
						if (v.type == VAL_OBJ)
							res.as.num =
							    (v.as.obj->type ==
							     OBJ_STRING) ?
							    strlen(v.as.obj->
								   as.chars) :
							    v.as.obj->as.
							    array.len;
					} else if (nid == -3) {
						int i = POP().as.num;
						Obj *o = POP().as.obj;
						res.as.num =
						    (int)o->as.chars[i];
					} else if (nid == -4) {
						Obj *o2 = POP().as.obj;
						Obj *o1 = POP().as.obj;
						char *c =
						    malloc(strlen(o1->as.chars)
							   +
							   strlen(o2->
								  as.chars) +
							   1);
						strcpy(c, o1->as.chars);
						strcat(c, o2->as.chars);
						Obj *o =
						    gc_allocate(OBJ_STRING);
						o->as.chars = c;
						res = (Value) {
							VAL_OBJ, {
						.obj = o}};
					} else if (nid == -5) {
						exit(POP().as.num);
					}
					PUSH(res);
					break;
				}

				Value temp_args[20];
				for (int i = 0; i < ac; i++)
					temp_args[ac - 1 - i] = POP();
				PUSH_NUM(ip);
				PUSH_NUM(bp);
				bp = sp - 1;
				for (int i = 0; i < ac; i++)
					PUSH(temp_args[i]);
				ip = target;
				break;
			}

		case 30:{	
				CHECK_STACK(1);
				Value rv = POP();
				sp = bp + 1;
				bp = POP().as.num;
				ip = POP().as.num;
				PUSH(rv);
				break;
			}

		case 31:{	
				CHECK_STACK(1);
				Value v = POP();
				if (v.type == VAL_OBJ) {
					if (v.as.obj->type == OBJ_STRING)
						printf("%s\n",
						       v.as.obj->as.chars);
					else
						printf("[Safu Length: %d]\n",
						       v.as.obj->as.array.len);
				} else
					printf("%d\n", v.as.num);
				break;
			}
		}
	}
}
