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
		node* oldHead = lst->head;
		newNode->next = oldHead;

		// node* lastNode = (node*)malloc(sizeof(node));
		node* lastNode = oldHead->next;
		// lastNode = newNode->next;
		while (lastNode->next != oldHead) {
			lastNode = lastNode->next;
		}
		
		lastNode->next = newNode;
		lst->head = newNode;
	}
	
	// when index in middle or  end
	else {
		node* previousNode;
		
		previousNode = lst->head;

		for (int i = 0; i < index - 1; i++) {
			previousNode = previousNode->next;
		}
		
		newNode->next = previousNode->next;
		previousNode->next = newNode;

	}	
	
}


// Deletes node at index (counting from head starting from 0).
// Note: index is guarenteed to be valid.
void delete_node_at(list *lst, int index) {
	
	node* nodeToDelete;
	node* nextNode;
	node* headNode = lst->head;


	
	if (headNode == NULL) {
		return;
	}

	else if (headNode == headNode->next) {
		free(headNode);
		lst->head = NULL;
	}

	else if (index == 0) {
		node* nextNode;
		node* lastNode;

		nodeToDelete = headNode;
		nextNode = nodeToDelete->next;
		
		lastNode = nodeToDelete->next;
		while (lastNode->next != headNode) {
			lastNode = lastNode->next;
		}
		
		lst->head = nextNode;
		lastNode->next = nextNode;

		free(nodeToDelete);

	}

	else {
		node* previousNode;
		
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
	node* nextNode = lst->head;
	for (int i = 0; i < offset; i++) {
		nextNode = nextNode->next;
	}
	
	lst->head = nextNode;

}

// Reverses the list, with the original "tail" node
// becoming the new head node.
void reverse_list(list *lst) {

	if (lst->head == NULL) {

	}

	else if (lst->head->next == lst->head) {
				
	}

	else {

		node* prevNode = NULL;
		node* currNode = lst->head;
		node* headNode = lst->head;
		int count = 0;

		while (count == 0 || currNode != headNode) {
			if (currNode == headNode) {
				count++;
			}
			node* nextNode = currNode->next;
			currNode->next = prevNode;
			prevNode = currNode;
			currNode = nextNode;
		}

		headNode->next = prevNode;
		lst->head = prevNode; 
	}

}

// Resets list to an empty state (no nodes) and frees
// any allocated memory in the process
void reset_list(list *lst) {
	
	node* currNode;
	node* nextNode;
	node* headNode;

	headNode = lst->head;

	if (headNode == NULL) {
		return;
	}

	currNode = headNode->next;
	nextNode = currNode->next;
	
	if (currNode == nextNode) {
		free(currNode);
		lst->head = NULL;
	}
	
	else {
		while (currNode != headNode) {
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
	node* headNode = lst->head;
	node* currNode = headNode;

	if (headNode == NULL) {
		return;
	}

	else {
		while(currNode->next != headNode) {
			currNode->data = func(currNode->data);
			currNode = currNode->next;
		}
		
		currNode->data = func(currNode->data);
	}

}

// Traverses list and returns the sum of the data values
// of every node in the list.
long sum_list(list *lst) {

	node* headNode = lst->head;
	node* currNode = headNode->next;

	long result = (long) headNode->data;

	if (headNode == currNode) {
		return result;
	}

	else {
		while (currNode != headNode) {
			long val = currNode->data;
			result = result + val; 
			currNode = currNode->next;
		}
		return result;
	}

}

