/**
 * CS2106 AY21/22 Semester 1 - Lab 2
 *
 * This file contains function definitions. Your implementation should go in
 * this file.
 */

#include "myshell.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define RUNNING 1
#define EXITED 2
#define TERMINATING 3

typedef struct NODE {
    int pid;
    int status;
    int exit_status;
    struct NODE *next;
} node;

typedef struct {
    node *head;
} List;

// Declare list
List *lst;

void insert_node_at(int pid, int status, int exit_status) {
        // instantiate new node, add data

        node* newNode = (node*)malloc(sizeof(node));
        newNode->pid = pid;
        newNode->status = status;
        newNode->exit_status = exit_status;

        // when list empty
        if (lst->head == NULL) {

            newNode->next = NULL;
            lst->head = newNode;
        }

        // when index in middle or  end
        else {
                node* previousNode;

                previousNode = lst->head;

                while (previousNode->next != NULL) {
                        previousNode = previousNode->next;
                }

                newNode->next = NULL;
                previousNode->next = newNode;
        }

}

void delete_node(int pid) {
    node* nodeToDelete;
	node* headNode = lst->head;

	if (headNode == NULL) {
		return;
	}

	else if (headNode->next == NULL) {
		free(headNode);
		lst->head = NULL;
	}

	else if (headNode->pid == pid) {
		node* nextNode;
		node* lastNode;

		nodeToDelete = headNode;
		nextNode = nodeToDelete->next;
		
		lastNode = nodeToDelete->next;
		while (lastNode->next != headNode) {
			lastNode = lastNode->next;
		}
		
		lst->head = nextNode;

		free(nodeToDelete);

	}

	else {
		node* nextNode;
        node* previousNode;
		
		nextNode = lst->head;

		while (nextNode->pid != pid){
			previousNode = nextNode;
            nextNode = nextNode->next;
		}

		previousNode->next = nextNode->next;

		free(nextNode);

	}
}

void update_node(int pid, int status, int exit_status) {
    node* currNode;
    currNode = lst->head;

    while (currNode->pid != pid) {
            currNode = currNode->next;
    }

    currNode->status = status;
    currNode->exit_status = exit_status;

}


void my_init(void) {
    // Initialize what you need here
    lst = (List*)malloc(sizeof(List));
}


void print_statuses(void) {
    if (lst->head == NULL) {
        return;
    }

    node *curr = lst->head;
    while (curr != NULL) {
        
        // if running process, check whether exit or not
        if (curr->status == RUNNING) {
            int new_status;
            // check for fail
            int result = waitpid(curr->pid, &new_status, WNOHANG);
            if (result == -1) {
                printf("EXIT FAILUREEEE\n");
                EXIT_FAILURE; 
            }

            // newly exited processes
            if (WIFEXITED(new_status)) {
                update_node(curr->pid, EXITED, WEXITSTATUS(new_status));
                printf("[%d] Exited %d\n", curr->pid, WEXITSTATUS(new_status));

            }
             
            // process still running
            else {
                printf("[%d] Running\n", curr->pid);
                
            }
        }

        else if (curr->status == TERMINATING) {
            int new_status;

            // check for fail
            int result = waitpid(curr->pid, &new_status, WNOHANG);
            if (result == -1) {
                EXIT_FAILURE;
            }

            // newly exited processes
            if (result > 0) {
                update_node(curr->pid, EXITED, WEXITSTATUS(new_status));
                printf("[%d] Exited %d\n", curr->pid, WEXITSTATUS(new_status));

            }

            else {
                printf("[%d] Terminating\n", curr->pid);
            }
        }

        // for exited processes
        else if (curr->status == EXITED) {
            printf("[%d] Exited %d\n", curr->pid, curr->exit_status);
        }

        else {
            printf("THIS SHOULD NOT SHOW UP, LINE 144\n");
        }
    
        curr = curr->next;
    }

}

void my_quit(void) {
    // Clean up function, called after "quit" is entered as a user command

    node *curr;
    int pid;
    int new_status;

    curr = lst->head;
    
    while (curr != NULL) {
        pid = curr->pid;
        
        int result = waitpid(pid, &new_status, WNOHANG);
        if (result == 0) {
            if (!WIFEXITED(new_status)) {
                kill(pid, SIGTERM);
                int s = waitpid(pid, &new_status, 0);
                s += 1;
            }
        }
        curr = curr->next;

    }
    printf("Goodbye!\n");
}

int check_chain(size_t* num_tokens, char **tokens) {
    for (int i = 0; i < (int) *num_tokens - 1; i++) {
        if (strcmp(tokens[i], "&&") == 0) { 
            return 1;
        }
    }
    return 0;
}

void handle_process(int* child_pid, char **tokens) {
    if (*child_pid == -1) {
        EXIT_FAILURE;
    }
    
    insert_node_at(*child_pid, 0, 0);
    if (*child_pid == 0) {
        execv(tokens[0], tokens);
        // printf("%s not found\n", tokens[0]);
    }
    else {
        return;
    }
}

void handle_chain(size_t* num_tokens, char **tokens) {

    // outer loop
    char *curr_commands[(int)*num_tokens];
    int arr_index = 0;

    for (int i = 0; i < (int)*num_tokens - 1; i++) {
        
        if (strcmp(tokens[i], "&&") == 0) {

            curr_commands[arr_index] = NULL;
            arr_index = 0;

            if(access( curr_commands[0], F_OK ) == -1 ) {
                printf("%s not found\n", curr_commands[0]);
                return;
            }
        
            int child_pid = fork();
            handle_process(&child_pid, curr_commands);

            int exit_status;
            waitpid(child_pid, &exit_status, 0);
            int status_code;

            if (WIFEXITED(exit_status)) { 
                status_code = WEXITSTATUS(exit_status);
            }

            update_node(child_pid, EXITED, status_code);

            if (exit_status != 0) {
                return;
            }

        }
        
        else {
            char *command = tokens[i];
            curr_commands[arr_index] = command;
            arr_index = arr_index + 1;
        }

    }

    curr_commands[arr_index] = NULL;

    if(access( curr_commands[0], F_OK ) == -1 ) {
        printf("%s not found\n", curr_commands[0]);
        return;
    }

    int child_pid = fork();
    handle_process(&child_pid, curr_commands);

    int exit_status;
    waitpid(child_pid, &exit_status, 0);
    int status_code;

    // if (exit_status != 0) {
    //     printf("NODE DELETED\n");
    //     delete_node(child_pid);
    //     return;
    // }

    if (WIFEXITED(exit_status)) { 
        status_code = WEXITSTATUS(exit_status);
    }

    update_node(child_pid, EXITED, status_code);

    if (exit_status != 0) {
        return;
    }

}

void handle_info(void) {
    print_statuses();
    return;
}

void handle_wait(char pid[]) {
    int p = atoi(pid);

    int exit_status;
    waitpid(p, &exit_status, 0);
    int status_code;
    status_code = WEXITSTATUS(exit_status);
    update_node(p, EXITED, status_code);

}

void handle_terminate(char pid[]) {
    int p = atoi(pid);
    int new_status;

    // check for fail
    int result = waitpid(p, &new_status, WNOHANG);

    // Terminate process
    if (result == 0) {
        if (!WIFEXITED(new_status)) {
            update_node(p, TERMINATING, new_status);
            kill(p, SIGTERM);
        }
    }
    
}

void handle_background(size_t* num_tokens, int* child_pid, char **tokens) {
    if (*child_pid == -1) {
        EXIT_FAILURE;
    }
    insert_node_at(*child_pid, RUNNING, 0);

    tokens[(int)*num_tokens - 2] = NULL;
    if (*child_pid == 0) {
        execv(tokens[0], tokens);
        printf("%s not found\n", tokens[0]);
    }
    else {
        printf("Child[%d] in background\n", *child_pid);
        return;
    }
}

// ORIGINAL PROCESS COMMAND

void my_process_command(size_t num_tokens, char **tokens) {
    // Your code here, refer to the lab document for a description of the arguments

    // && symbol chaining logic 
    if (check_chain(&num_tokens, tokens) == 1) {
        handle_chain(&num_tokens, tokens);
        return;
    }

    // & symbol logic
    int _print_child_pid_status = strcmp(tokens[num_tokens - 2], "&");

    // info
    if (strcmp(tokens[0], "info") == 0) {
        handle_info();
        return;
    }

    // wait
    if (strcmp(tokens[0], "wait") == 0) {
        handle_wait(tokens[1]);
        return;
    } 

    // terminate
    if (strcmp(tokens[0], "terminate") == 0) {
        handle_terminate(tokens[1]);
        return;
    }

    // Execute program
    int exit_status = -5;

    // if file does not exist
    if(access( tokens[0], F_OK ) == -1 ) {
        printf("%s not found\n", tokens[0]);
        return;
    }

    int child_pid = fork();

    // handle background process
    if (_print_child_pid_status == 0) {
        handle_background(&num_tokens, &child_pid, tokens);
    }

    // handle regular process
    else {
        handle_process(&child_pid, tokens);
        waitpid(child_pid, &exit_status, 0);

        if (exit_status != 0) {
            delete_node(child_pid);
            return;
        }
        
        int status_code;
        if (WIFEXITED(exit_status)) { 
            status_code = WEXITSTATUS(exit_status);
        }

        update_node(child_pid, EXITED, status_code);

    }

}

// FIX QUIT