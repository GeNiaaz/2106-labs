/*************************************
* Lab 1 Exercise 3
* Name:
* Student No:
* Lab Group:
*************************************/

#include "node.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Copy in your implementation of the functions from ex2.
// There is one extra function called map which you have to fill up too.
// Feel free to add any new functions as you deem fit.

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
	node* nextNode = (node*)malloc(sizeof(node));
	for (int i = 0;i < offset; i++) {
		nextNode = lst->head;
		lst->head = nextNode->next;
	}
}

// Reverses the list, with the original "tail" node
// becoming the new head node.
void reverse_list(list *lst) {
	if (lst->head->next == lst->head) {
				
	}

	else {
		node* prevNode = NULL;
		node* currNode = lst->head;
		node* nextNode;
		node* headNode = lst->head;

		while (nextNode != headNode) {
			nextNode = currNode->next;
			currNode->next = prevNode;
			prevNode = currNode;

			currNode = nextNode;
			nextNode = currNode->next;
		}
		
		currNode->next = prevNode;
		headNode->next = currNode;
		
		lst->head = currNode;


	}

}

// Resets list to an empty state (no nodes) and frees
// any allocated memory in the process
void reset_list(list *lst) {
	node* currNode;
	node* nextNode;
	node* headNode;

	headNode = lst->head;
	currNode = headNode->next;
	nextNode = currNode->next;
	
	if (currNode == nextNode) {
		free(currNode);
		lst->head = NULL;
	}
	
	else {
		while (nextNode != headNode) {
			free(currNode);
			currNode = nextNode;
			nextNode = nextNode->next;
		}

		free(headNode);
		lst->head = NULL;
	}
	

}

// Traverses list and applies func on data values of
// all elements in the list.
void map(list *lst, int (*func)(int)) {
	

}

// Traverses list and returns the sum of the data values
// of every node in the list.
long sum_list(list *lst) {

	node* headNode = lst->head;
	node* currNode = headNode->next;

	long result = headNode->data;

	if (headNode == currNode) {
		return result;
	}

	else {
		while (currNode != headNode) {
			result = result + currNode->data;
			currNode = currNode->next;
		}
		return result;
	}

}

