#include "userswap.h"
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/queue.h>



#define NON 0
#define READ 1
#define WRITE 2

#define DIRTY 5;
#define CLEAN 10;

// variables

size_t MAX_PAGES = 2106;
int CURR_PAGES = 0;
int PAGE_SIZE = 4096;
int eviction_counter = 0;


typedef struct PAGE {
   void *offset;
   void *region_addr;
   int status;
   int is_dirty;
} page;

typedef struct REGION{
  void *start_address;
  void *end_address;
  int num_pages;
  struct REGION *next;
  page *arr_pages;
} region;

typedef struct {
  region *head;
  region *tail;
} List2;


// Queue implementation

struct tailq_entry {
  void *page_addr;
  page *page_itself;

	TAILQ_ENTRY(tailq_entry) entries;
};

TAILQ_HEAD(, tailq_entry) my_tailq_head;


List2 *lst2;

int init_status = 0;

void init() {
  lst2 = (List2*)malloc(sizeof(List2));
  init_status = 1;

  TAILQ_INIT(&my_tailq_head);
  
}

void add_to_list_2(int multiple, int size, void *start_address) {

  region *new_region;
  page *arr_pages;

  new_region = (region*)malloc(sizeof(region));

  if (lst2->head == NULL) {
    lst2->head = new_region;
    lst2->tail = new_region;
  } else {
    lst2->tail->next = new_region;
    lst2->tail = new_region;
  }

  new_region->start_address = start_address;
  new_region->num_pages = multiple;
  new_region->end_address = start_address + size;

  arr_pages = (page*)malloc(sizeof(page) * multiple);

  new_region->arr_pages = arr_pages;

  for (int i = 0; i < multiple; i++) {
    arr_pages[i].status = 0;
    arr_pages[i].offset = i * 4096 + start_address;
    arr_pages[i].region_addr = start_address;
    arr_pages[i].is_dirty = CLEAN;
  }

}


int remove_from_list_2(void *addr) {
  region *region_to_delete;
  region *prev_region;
  region *head_region = lst2->head;

  int length_return = -1;


  // case for if list is empty
  if (head_region == NULL) {
    printf("wth why is this empty??\n");

  } else if (head_region->next == NULL) {
    if (head_region->start_address == addr) {
      length_return = head_region->num_pages;
      free (head_region);
    }

  } else {
    prev_region = head_region;
    region_to_delete = head_region->next;

    // if head node is to be deleted
    if (head_region->start_address == addr) {
      lst2->head = head_region->next;
      length_return = head_region->num_pages;
      free(head_region);
    } else {

      while (region_to_delete->start_address != addr) {
        prev_region = region_to_delete;
        region_to_delete = region_to_delete->next;
      }

      prev_region->next = region_to_delete->next;
      length_return = region_to_delete->num_pages;
      free(region_to_delete);
    }
  }

  return length_return;

}

// 
void *get_page(void *addr) {
  // find range
  region *curr_region = lst2->head;
  page *curr_page;

  while (curr_region != NULL) {
    void *low = curr_region->start_address;
    void *high = curr_region->end_address;

    int page_index = 0;

    if (addr >= low && addr <= high) {
      int remainder = addr - low;
      if (remainder % 4096 == 0) {
        page_index = remainder / 4096;
      } else {
        page_index = remainder / 4096 + 1;
      }

      curr_page = &curr_region->arr_pages[page_index];

      return curr_page;
      break;

    } else {
      curr_region = curr_region->next;
      continue;
    }
  }

  return NULL;

}


int get_size(void *addr) {
  region *curr_region;
  curr_region = lst2->head;

  while (curr_region->next != NULL) {

    if (curr_region->start_address == addr) {
      return curr_region->num_pages;
    } else {
      
      // FAULT HERE FIX THIS
      // printf("%d\n", curr_region->next->num_pages);
      curr_region = curr_region->next;
    }
  } 

  if (curr_region->start_address == addr) {
      return curr_region->num_pages;
  } else {
    exit(0);
    return -1;
  }
  
}

void page_set_prot_none(void *addr) {
  void *addr_to_handle = addr;
  page *curr_page = get_page(addr_to_handle);

  curr_page->status = NON;

  mprotect(addr_to_handle, PAGE_SIZE, PROT_NONE);

}

// append to back of tailq
void add_to_tailq(void *addr, page *page_ptr) {
  struct tailq_entry *item;
  item = malloc(sizeof(*item));

  item->page_addr = addr;
  item->page_itself = page_ptr;

  TAILQ_INSERT_TAIL(&my_tailq_head, item, entries);

}


// do the swap shit here
void evict_page_to_swap(page *curr_page) {

}

// remove page from front of tail queue
void evict_page_from_queue() {

  eviction_counter++;
  printf("%d\n", eviction_counter);

  struct tailq_entry *first_item;
  first_item = TAILQ_FIRST(&my_tailq_head);

  evict_page_to_swap(first_item->page_itself);

  TAILQ_REMOVE(&my_tailq_head, first_item, entries);

}

void page_fault_handler(void *addr) {

  void *addr_to_handle = addr;
  // int size_to_use = get_size(addr_to_handle);

  page *curr_page = get_page(addr_to_handle);
  int page_status = curr_page->status;

  if (page_status == NON) {
    curr_page->status = READ;

    /* add to tail queue */

    // evict 1 page
    if (CURR_PAGES == (int)MAX_PAGES) {
      evict_page_from_queue();
    } else {
      CURR_PAGES++;
    }

    add_to_tailq(addr, curr_page);
    mprotect(addr_to_handle, PAGE_SIZE, PROT_READ);

  } else if (page_status == READ) {
    curr_page->status = WRITE;
    mprotect(addr_to_handle, PAGE_SIZE, PROT_READ | PROT_WRITE);
  } else if (page_status == WRITE) {
    
  } else {
    printf("thou shall not be here");
  }

}

void handler(int sig, siginfo_t *info, void *ucontext) {
  page_fault_handler(info->si_addr);
}


// ================================== GIVEN FUNCTIONS BELOW ===============================================

void userswap_set_size(size_t size) {
  MAX_PAGES = size;
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
    multiple = size / PAGE_SIZE;
  } else {
    multiple = (size / PAGE_SIZE) + 1;
  }
  start_addr = mmap (NULL, PAGE_SIZE * multiple, PROT_NONE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);


  add_to_list_2(multiple, size, start_addr);

  return start_addr;

}

// clear up alloc'd things
void userswap_free(void *mem) {
  int size_unmap = remove_from_list_2(mem);
  munmap(mem, size_unmap);

}

void *userswap_map(int fd, size_t size) {
  return NULL;
}


// when do i do the check on max pages.

// after the page fault handler or...
