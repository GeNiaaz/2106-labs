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
        if (curr->status == 0) {
            int new_status;

            // check for fail
            if (waitpid(curr->pid, &new_status, WNOHANG) == 1) {
                EXIT_FAILURE;
            }

            // newly exited processes
            if (WIFEXITED(new_status)) {
                printf("[%d] Exited %d\n", curr->pid, new_status);
            }
             
            // process still running
            else {
                printf("[%d] Running\n", curr->pid);
            }
        }

        // for exited processes
        else if (curr->status == 1) {
            printf("[%d] Exited %d\n", curr->pid, curr->exit_status);
        }

        else {
            printf("THIS SHOULD NOT SHOW UP, LINE 92\n");
        }
    
        curr = curr->next;
    }

}

void my_quit(void) {
    // Clean up function, called after "quit" is entered as a user command
    printf("Goodbye!\n");
}

void handle_info(void) {
    print_statuses();
    return;
}

void handle_background(size_t* num_tokens, int* child_pid, char **tokens) {
    if (*child_pid == -1) {
        EXIT_FAILURE;
    }
    insert_node_at(*child_pid, 0, 0);

    tokens[(int)*num_tokens - 2] = NULL;
    if (*child_pid == 0) {
        execv(tokens[0], tokens);
        EXIT_FAILURE;
    }
    else {
        printf("Child[%d] in background\n", *child_pid);
        return;
    }
}

void handle_process(int* child_pid, char **tokens) {
    if (*child_pid == -1) {
        EXIT_FAILURE;
    }
    insert_node_at(*child_pid, 0, 0);
    if (*child_pid == 0) {
        execv(tokens[0], tokens);
        EXIT_FAILURE;
    }
    else {
        return;
    }
}

void my_process_command(size_t num_tokens, char **tokens) {
    // Your code here, refer to the lab document for a description of the arguments


    // & symbol logic
    int _print_child_pid_status = strcmp(tokens[num_tokens - 2], "&");

    // info
    if (strcmp(tokens[0], "info") == 0) {
        handle_info();
        return;
    }

    // Execute program
    int exit_status = -5;
    int child_pid = fork();

    if (_print_child_pid_status == 0) {
        handle_background(&num_tokens, &child_pid, tokens);
    }

    else {
        handle_process(&child_pid, tokens);
        waitpid(child_pid, &exit_status, 0);

        update_node(child_pid, 1, exit_status);

    }

}