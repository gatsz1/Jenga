#include "jenga.h"

int main(int argc, char** argv) {
    if(argc < 2) { printf("Usage: ./Jenga file.j\n"); return 1; }
    
    init_natives();
    
    src = read_file_content(argv[1]);
    if(!src) { printf("Error: Could not read file.\n"); return 1; }
    
    next_token();
    compile();
    run_vm();
    
    return 0;
}
