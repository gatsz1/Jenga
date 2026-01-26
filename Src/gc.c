#include "jenga.h"

void mark_object(Obj* obj) {
    if (obj == NULL || obj->marked) return;
    
    obj->marked = 1;

    if (obj->type == OBJ_ARRAY) {
        for (int i = 0; i < obj->as.array.len; i++) {
            Value v = obj->as.array.elements[i];
            if (v.type == VAL_OBJ) {
                mark_object(v.as.obj);
            }
        }
    }
}

void mark_roots() {
    for (int i = 0; i < sp; i++) {
        if (stack[i].type == VAL_OBJ) {
            mark_object(stack[i].as.obj);
        }
    }
    for (int i = 0; i < MAX_GLOBALS; i++) {
        if (global_mem[i].type == VAL_OBJ) {
             mark_object(global_mem[i].as.obj);
        }
    }
}

void sweep() {
    Obj** object = &objects;
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

void gc_collect() {
    mark_roots();
    sweep();
    max_objects = num_objects * 2;
    if (max_objects < 8) max_objects = 8;
}

Obj* gc_allocate(ObjType type) {
    if (num_objects >= max_objects) gc_collect();
    
    Obj* obj = (Obj*)malloc(sizeof(Obj));
    obj->type = type;
    obj->marked = 0;
    obj->next = objects;
    objects = obj;
    num_objects++;
    return obj;
}
