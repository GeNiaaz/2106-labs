#include <stdio.h>
#include <stdlib.h>



// #define SUM_LIST 0
// #define INSERT_AT 1
// #define DELETE_AT 2
// #define ROTATE_LIST 3
// #define REVERSE_LIST 4
// #define RESET_LIST 5
// #define MAP 6

// #define ADD_ONE 0
// #define ADD_TWO 1
// #define MULTIPLY_FIVE 2
// #define SQUARE 3
// #define CUBE 4


int main(int argc, char **argv) {
		if (argc != 2) {
				fprintf(stderr, "Error: expecting 1 argument, %d found\n", argc - 1);
				exit(1);
		}
		
		// We read in the file name provided as argument
		char *fname = argv[1];
		
		printf("hello000\n\n");
		// Update the array of function pointers
		// DO NOT REMOVE THIS CALL
		// (You may leave the function empty if you do not need it)

		// Rest of code logic here
		//if fname != ...		

		FILE *fptr = fopen(fname, "r");
		
		// file check
		if (fptr == NULL) {
				printf("Unable to open file\n");
		}

		// list *lst = (list*)malloc(sizeof(lst));
		// lst->head = NULL;
		int instr;

		while (fscanf(fptr, "%d", &instr) == 1) {
				printf("%d", instr);
				// run_instructions(lst, instr);
		}
		
		// reset_list(lst);
		// free(lst);
}