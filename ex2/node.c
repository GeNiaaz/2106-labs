/*************************************
* Lab 1 Exercise 2
* Name: Muhammad Niaaz Wahab
* Student No: A0200161E
* Lab Group: B04
*************************************/

#include "node.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Add in your implementation below to the respective functions
// Feel free to add any headers you deem fit (although you do not need to)

// Inserts a new node with data value at index (counting from head
// starting at 0).
// Note: index is guaranteed to be valid.
void insert_node_at(list *lst, int index, int data) {
	

	// instantiate new node, add data 
	node* newNode = (node*)malloc(sizeof(node));
	newNode->data = data;
	
	// when list empty
	if (lst->head == NULL) {
		newNode->next = newNode;
		lst->head = newNode;
	}

	// when index referenced is 0
	else if (index == 0) {
		newNode->next = lst->head;
		// lst->head = newNode;

		node* lastNode = (node*)malloc(sizeof(node));
		lastNode = newNode->next;
		while (lastNode->next != lst->head) {
			lastNode = lastNode->next;
		}
		
		lst->head = newNode;
		lastNode->next = newNode;
	}
	
	// when index in middle or  end
	else {
		node* previousNode = (node*)malloc(sizeof(node));
		node* nextNode = (node*)malloc(sizeof(node));
		
		previousNode = lst->head;

		for (int i = 0; i < index - 1; i++) {
			previousNode = previousNode->next;
		}
		
		nextNode = previousNode->next;

		newNode->next = nextNode;
		previousNode->next = newNode;
	}	
	
}

// Deletes node at index (counting from head starting from 0).
// Note: index is guarenteed to be valid.
void delete_node_at(list *lst, int index) {
	
	node* nodeToDelete = (node*)malloc(sizeof(node));
	
	if (lst->head == NULL) {
		int i = 1;
	}

	else if (lst->head == lst->head->next) {
		free(lst->head);
		lst->head = NULL;
	}

	else if (index == 0) {
		node* nextNode = (node*)malloc(sizeof(node));
		node* lastNode = (node*)malloc(sizeof(node));

		nodeToDelete = lst->head;
		nextNode = nodeToDelete->next;
		
		lastNode = nodeToDelete->next;
		while (lastNode->next != lst->head) {
			lastNode = lastNode->next;
		}
		
		lst->head = nextNode;
		lastNode->next = nextNode;

		free(nodeToDelete);

	}

	else {
		node* previousNode = (node*)malloc(sizeof(node));
		node* nextNode = (node*)malloc(sizeof(node));
		
		previousNode = lst->head;

		for (int i = 0; i < index - 1; i++) {
			previousNode = previousNode->next;
		}

		nodeToDelete = previousNode->next;
		nextNode = nodeToDelete->next;

		previousNode->next = nextNode;

		free(nodeToDelete);

	}
}

// Rotates list by the given offset.
// Note: offset is guarenteed to be non-negative.
void rotate_list(list *lst, int offset) {
}

// Reverses the list, with the original "tail" node
// becoming the new head node.
void reverse_list(list *lst) {
}

// Resets list to an empty state (no nodes) and frees
// any allocated memory in the process
void reset_list(list *lst) {
}


int main() {
	
	printf("running...\n");
	list* lst = (list*)malloc(sizeof(list));
	
	//node* tmp = (node*)malloc(sizeof(node));
	//tmp->data = 5;
	//lst->head = tmp;

	insert_node_at(lst, 0, 1);
	insert_node_at(lst, 1, 5);
	insert_node_at(lst, 2, 8);
	insert_node_at(lst, 3, 11);

	delete_node_at(lst, 0);
	delete_node_at(lst, 2);

	node* curr = lst -> head;
	while (curr->next != lst->head)  {
		int data = curr -> data;
		printf("%d\n", data);
		curr = curr->next;
	}

	int data = curr-> data;
	printf("%d\n", data);

	return 0;
}










