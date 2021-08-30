/*************************************
 * Lab 1 Exercise 3
 * Name: Muhammad Niaaz Wahab
 * Student No: A0200161E
 * Lab Group: B04
 *************************************/

#include <stdio.h>
#include <stdlib.h>

#include "function_pointers.h"
#include "node.h"

// The runner is empty now! Modify it to fulfill the requirements of the
// exercise. You can use ex2.c as a template

// DO NOT initialize the func_list array in this file. All initialization
// logic for func_list should go into function_pointers.c.

// Macros
#define SUM_LIST 0
#define INSERT_AT 1
#define DELETE_AT 2
#define ROTATE_LIST 3
#define REVERSE_LIST 4
#define RESET_LIST 5
#define MAP 6

#define PRINT_LIST 7

// #define ADD_ONE 0
// #define ADD_TWO 1
// #define MULTIPLY_FIVE 2
// #define SQUARE 3
// #define CUBE 4

void run_instructions(FILE *fptr, list *lst, int instr);
void print_list(list *lst);

int main(int argc, char **argv) {
		if (argc != 2) {
				fprintf(stderr, "Error: expecting 1 argument, %d found\n", argc - 1);
				exit(1);
		}
		
		// We read in the file name provided as argument
		char *fname = argv[1];

		// Update the array of function pointers
		// DO NOT REMOVE THIS CALL
		// (You may leave the function empty if you do not need it)
		update_functions();

		// Rest of code logic here
		//if fname != ...		

		FILE *fptr = fopen(fname, "r");
		
		// file check
		if (fptr == NULL) {
				printf("Unable to open file\n");
		}

		list *lst = (list*)malloc(sizeof(list));
		lst->head = NULL;
		int instr;
		while (fscanf(fptr, "%d", &instr) == 1) {
				run_instructions(fptr, lst, instr);
		}
		
		reset_list(lst);
		free(lst);
}

void run_instructions(FILE *fptr, list *lst, int instr) {
		int index, data, offset;
		long sumResult;

		switch (instr) {

				case SUM_LIST:
						sumResult = sum_list(lst);
						printf("%ld\n", sumResult);
						break;
				case INSERT_AT:
						fscanf(fptr, "%d %d", &index, &data);
						insert_node_at(lst, index, data);
						break;
				case DELETE_AT:
						fscanf(fptr, "%d", &index);
						delete_node_at(lst, index);
						break;
						rotate_list(lst, offset);
						break;
				case REVERSE_LIST:
						reverse_list(lst);
						break;
				case RESET_LIST:
						reset_list(lst);
						break;
				case MAP:
						fscanf(fptr, "%d", &data);
						map(lst, func_list[data]);
						break;
				case PRINT_LIST:
						print_list(lst);
		}
}

// REMOVE
void print_list(list *lst) {
    if (lst->head == NULL) {
        printf("[ ]\n");
        return;
    }

    printf("[ ");
    node *curr = lst->head;
    do {
        printf("%d ", curr->data);
        curr = curr->next;
    } while (curr != lst->head);
    printf("]\n");
}
						/*
						switch (data) {
								case ADD_ONE:
										map(lst, func_list[0]);
										break;
								case ADD_TWO:
										map(lst, func_list[1]);
										break;
								case MULTIPLY_FIVE:
										map(lst, func_list[2]);
										break;
								case SQUARE:
										map(lst, func_list[3]);
										break;
								case CUBE:
										map(lst, func_list[4]);
						*/
						

/*
void print_list(list *lst) {
		if (lst->head == NULL) {
				printf("[ ]\n");
				return;
		}

		printf("[ ");
		node *curr = lst->head;

		do {
				printf("%d ", curr->data);
				curr = curr->next;
		}

		printf("]\n");
}

*/
