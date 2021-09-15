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
                newNode->next = newNode;
                lst->head = newNode;
        }

        // when index in middle or  end
        else {
                node* previousNode;

                previousNode = lst->head;

                while (previousNode->next != lst->head) {
                        previousNode = previousNode->next;
                }

                newNode->next = lst->head;
                previousNode->next = newNode;
        }

}

void update_node(int pid, int status, int exit_status) {
        node* currNode;
    currNode = lst->head;

    printf("GOT HERE!!\n");


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
    do {
        if (curr->pid == 0) {
            printf("[%d] Running\n", curr->pid);
        }

        else if (curr->pid == 1) {

            printf("[%d] Exited %d\n", curr->pid, curr->exit_status);
        }

        else {
            printf("THIS SHOULD NOT SHOW UP, LINE 92\n");
        }

        curr = curr->next;
    } while (curr != lst->head);
}

void my_quit(void) {
    // Clean up function, called after "quit" is entered as a user command
    printf("Goodbye!\n");
    exit(0);
}

void handle_info(void) {
    print_statuses();
    return;
}

void handle_background(size_t* num_tokens, int* child_pid, char **tokens) {
    if (*child_pid == -1) {
        EXIT_FAILURE;
    }
    tokens[(int)*num_tokens - 2] = NULL;
    if (*child_pid == 0) {
        int pid = getpid();
        insert_node_at(pid, 0, 0);
        printf("Child[%d] in background", *child_pid);
        execv(tokens[0], tokens);
        EXIT_FAILURE;
    }
}

void handle_process(int* child_pid, char **tokens) {
    if (*child_pid == -1) {
        EXIT_FAILURE;
    }
    if (*child_pid == 0) {
        int pid = getpid();
        insert_node_at(pid, 0, 0);

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
    }

    // Execute program
    int status;
    int child_pid = fork();

    if (_print_child_pid_status == 0) {
        handle_background(&num_tokens, &child_pid, tokens);
    }

    else {
        handle_process(&child_pid, tokens);
        waitpid(child_pid, &status, 0);
        // PROBLEM IS HERREEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
        printf("first is: %d\n", lst->head->pid);
        update_node(child_pid, 1, status);

        printf("exit status is: %d\n", status);
        print_statuses();
    }

}