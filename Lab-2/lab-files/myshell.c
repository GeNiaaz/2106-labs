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
#include <fcntl.h>

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

// Declare functions
int check_redirect(int num_tokens, char **tokens);
int handle_redirect(int num_tokens, char **tokens);


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
            if (result > 0) {
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

int check_chain(int num_tokens, char **tokens) {
    for (int i = 0; i < (int) num_tokens - 1; i++) {
        if (strcmp(tokens[i], "&&") == 0) { 
            return 1;
        }
    }
    return 0;
}

int handle_process(int num_tokens, char **tokens) {

    // check if background process 
    int background_process_status = strcmp(tokens[num_tokens - 2], "&");

    int r = check_redirect(num_tokens, tokens);
    if (r == 1) {
        int w = handle_redirect(num_tokens, tokens);
        return w;
    }

    // check if command is there
    if(access( tokens[0], F_OK ) == -1 ) {
        printf("%s not found\n", tokens[0]);
        return 1;
    }

    // FORKED here
    int child_pid = fork();

    if (background_process_status == 0) {
        tokens[num_tokens - 2] = NULL;
    }

    // only child execute
    if (child_pid == 0) {
        execv(tokens[0], tokens);
    } 

    // parent process part
    else {

        insert_node_at(child_pid, RUNNING, 0);

        // background process quits here
        if (background_process_status == 0) {
            printf("Child[%d] in background\n", child_pid);
            return 1;
        }

        int exit_status;
        int status_code;

        waitpid(child_pid, &exit_status, 0);
        
        if (WIFEXITED(exit_status)) { 
            status_code = WEXITSTATUS(exit_status);
        }

        update_node(child_pid, EXITED, status_code);

        // for failed command execution
        if (exit_status != 0) {
            printf("%s failed\n", tokens[0]);
            return 1;
        }
    }
    return 0;

    
}

void handle_chain(int num_tokens, char **tokens) {

    // outer loop
    char *curr_commands[num_tokens];
    int arr_index = 0;

    for (int i = 0; i < num_tokens - 1; i++) {
        
        if (strcmp(tokens[i], "&&") == 0) {

            // redirect part
            int r = check_redirect(arr_index, tokens);
            if (r == 1) {
                int break_status = handle_redirect(arr_index, curr_commands);
                if (break_status == 1) {
                    return;
                }
            } else {
                curr_commands[arr_index] = NULL;
                int failure = handle_process(arr_index, curr_commands);
                if (failure == 1) {
                    return;
                }
            }
            arr_index = 0;
        }
        
        else {
            char *command = tokens[i];
            curr_commands[arr_index] = command;
            arr_index = arr_index + 1;
        }

    }

    // redirect part
    // printf("this: %s\n", curr_commands[0]);
    // printf("this: %s\n", curr_commands[1]);
    // printf("this: %s\n", curr_commands[2]);
    // printf("this: %s\n", curr_commands[3]);
    // printf("this: %s\n", curr_commands[4]);

    int r = check_redirect(arr_index, curr_commands);
    if (r == 1) {
        int break_status = handle_redirect(arr_index, curr_commands);
        if (break_status == 1) {
            arr_index = 0;
            return;
        }
        // printf(" did another redirect processsss\n");
    } else {
        curr_commands[arr_index] = NULL;
        int failure = handle_process(arr_index, curr_commands);
        if (failure == 1) {
            return;
        }
        arr_index = 0;
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

int check_redirect(int num_tokens, char **tokens) {
    for (int i = 0; i < (int) num_tokens - 1; i++) {
        if ((strcmp(tokens[i], "<") == 0) || (strcmp(tokens[i], ">") == 0) || (strcmp(tokens[i], "2>") == 0)) { 
            return 1;
        }
    }
    return 0;
}

int handle_redirect(int num_tokens, char **tokens) {
    // int background_status = 0;

    int background_process_status = strcmp(tokens[num_tokens - 2], "&");
    char *to_execute[num_tokens];

    // build the input array
    for (int i = 0; i < (int) num_tokens - 1; i++) {
        if (strcmp(tokens[i], "<") == 0 || strcmp(tokens[i], ">") == 0 || strcmp(tokens[i], "2>") == 0) { 
            to_execute[i] = NULL;
            break;
        } else {
            to_execute[i] = tokens[i];
        }
    }

    // check that file exists
    for (int i = 0; i < (int) num_tokens - 1; i++) {
        if (strcmp(tokens[i], "<") == 0) { 
            // input_status = 1;

            char *inp = tokens[i + 1];
            if( access( inp, F_OK ) != 0 ) {
                printf("%s does not exist\n", inp);
                return 1;
            }
        }
    }

    int child_pid = fork();
    if (child_pid == 0){

        for (int i = 0; i < (int) num_tokens - 1; i++) {
            if (strcmp(tokens[i], "<") == 0) { 
                // input_status = 1;

                char *input_file = tokens[i + 1];

                int in = open(input_file, O_RDONLY);
                dup2(in, 0);

            } else if (strcmp(tokens[i], ">") == 0) { 
                // output_status = 1;
                char *output_file = tokens[i + 1];
                int out = open(output_file, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
                dup2(out, 1);          
            } else if (strcmp(tokens[i], "2>") == 0) { 
                // error_status = 1;

                char *error_file = tokens[i + 1];
                int err = open(error_file, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
                dup2(err, 2);
            // } else if (strcmp(tokens[i], "&") == 0) { 
            //     background_status = 1;
            } else {}
        }

        
        execv(to_execute[0], to_execute);
    }
    
    // check whether successful or not
    insert_node_at(child_pid, RUNNING, 0);

    // background process quits here
    if (background_process_status == 0) {
        printf("Child[%d] in background\n", child_pid);
        return 0;
    }

    int exit_status;
    int status_code;

    waitpid(child_pid, &exit_status, 0);
    
    if (WIFEXITED(exit_status)) { 
        status_code = WEXITSTATUS(exit_status);
    }

    update_node(child_pid, EXITED, status_code);

    // for failed command execution
    if (exit_status != 0) {
        printf("%s failed\n", tokens[0]);
        return 1;
    }
    
    return 0;


}

// ORIGINAL PROCESS COMMAND

void my_process_command(size_t num_tokens, char **tokens) {
    // Your code here, refer to the lab document for a description of the arguments

    // <, >, 2> handling logic boolean
    if (check_chain((int)num_tokens, tokens) == 1) {
        handle_chain((int)num_tokens, tokens);
        return;
    }

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

    handle_process((int)num_tokens, tokens);

}

// FIX QUIT