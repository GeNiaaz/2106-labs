#include "userswap.h"
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


// use sigaction
// calls page fault handler


// list struct
typedef struct NODE {
   void* start_address;
   size_t size;
   struct NODE *next;
} node;

typedef struct {
    node *head;
} List;

// declare list
List *lst;

int init_status = 0;
int counter = 0;
// struct node *first_node;
// node* first_node = (node*)malloc(sizeof(node));


void init() {
  lst = (List*)malloc(sizeof(List));
  init_status = 1;
  
  // DELETE LATER
  counter = 0;
}

void add_to_list(int size, void *start_address) {
  node *curr_node;
  node *new_node;

  curr_node = lst->head;
  new_node = (node*)malloc(sizeof(node));
  
  new_node->next = NULL;
  new_node->start_address = start_address;
  new_node->size = size;

  // case for if list is empty
  if (curr_node == NULL) {
    lst->head = new_node;

  } else {

    printf("reached the add to list while loop\n");
    while (curr_node->next != NULL) {
      curr_node = curr_node->next;
    }

    curr_node->next = new_node;

  }

}

int remove_from_list(void *addr) {
  node *node_to_delete;
  node *prev_node;
  node *head_node = lst->head;

  int length_return = -1;

  // case for if list is empty
  if (head_node == NULL) {
    printf("wth why is this empty??\n");

  } else if (head_node->next == NULL) {
    if (head_node->start_address == addr) {
      length_return = head_node->size;
      free (head_node);
    }

  } else {
    prev_node = head_node;
    node_to_delete = head_node->next;

    // if head node is to be deleted
    if (head_node->start_address == addr) {
      lst->head = head_node->next;
      length_return = head_node->size;
      free(head_node);
    } else {

      while (node_to_delete->start_address != addr) {
        prev_node = node_to_delete;
        node_to_delete = node_to_delete->next;
      }

      prev_node->next = node_to_delete->next;
      length_return = node_to_delete->size;
      free(node_to_delete);
    }
  }

  return length_return;

}

int get_size(void *addr) {
  node *curr_node;
  curr_node = lst->head;

  while (curr_node->next != NULL) {
    if (curr_node->start_address == addr) {
      return curr_node->size;
    } else {
      curr_node = curr_node->next;
    }
  } 

  if (curr_node->start_address == addr) {
      return curr_node->size;
  } else {
    exit(0);
    return -1;
  }

  
}

// what to pass in???
void handler(int sig, siginfo_t *info, void *ucontext) {
  counter++;
  void *addr_to_handle = info->si_addr;
  int size_to_use = get_size(addr_to_handle);

  mprotect(addr_to_handle, size_to_use, PROT_READ);
  // printf("called handler %d\n", counter);
}

// ================================== GIVEN FUNCTIONS BELOW ===============================================

void userswap_set_size(size_t size) {
}

void *userswap_alloc(size_t size) {

  // sigsev handler
  struct sigaction sa;
  sa.sa_sigaction = handler;
  sa.sa_flags = SA_SIGINFO;
  sigaction(SIGSEGV, &sa, NULL);

  void *start_addr;
  int multiple;

  // initialise if not already done
  if (init_status == 0) {
    init();
  }

  // round up to next multiple of page size
  if (size % 4096 == 0) {
    multiple = size / 4096;
  } else {
    multiple = size / 4096 + 1;
  }
  start_addr = mmap (NULL, 4096 * multiple, PROT_NONE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

  add_to_list(multiple * 4096, start_addr);

  return start_addr;

}

// clear up alloc'd things
void userswap_free(void *mem) {
  int size_unmap = remove_from_list(mem);
  munmap(mem, size_unmap);
}

void *userswap_map(int fd, size_t size) {
  return NULL;
}
