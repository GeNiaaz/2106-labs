#include "userswap.h"
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/queue.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>



#define NON 0
#define READ 1
#define WRITE 2

#define OUT_OF_SWAP 5
#define IN_SWAP 10

#define OCCUPIED 1
#define UNOCCUPIED 0

#define FILE_UNOPENED 0
#define FILE_OPEN 1

// variables

size_t MAX_PAGES = 2106;
int CURR_PAGES = 0;
int PAGE_SIZE = 4096;
int file_d;
int file_open_status = 0;
int partition_size = 10000;
int partitions[10000];

int eviction_counter = 0;
int counter_2 = 0;
int fault_counter = 0;


typedef struct PAGE {
   void *page_addr;
   void *region_addr;
   int status;
   int in_swap;
   int index_swap_arr;
   int swap_offset;
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

// swap structs

typedef struct swap{
  int offset_in_swap;
  void *page_addr;
  int occupied;
  struct swap *next;
} swap;

typedef struct {
  swap *head;
  // region *tail;
} List_swap;

// Queue implementation

struct tailq_entry {
  void *page_addr;
  page *page_itself;

	TAILQ_ENTRY(tailq_entry) entries;
};

TAILQ_HEAD(, tailq_entry) my_tailq_head;


// list declarations
// List_swap *lst_swap;
List2 *lst2;

int init_status = 0;

void init() {
  lst2 = (List2*)malloc(sizeof(List2));
  lst2->head = NULL;
  // lst_swap = (List_swap*)malloc(sizeof(List_swap));
  // lst_swap->head = NULL;

  for (int i = 0; i < partition_size; i++) {
    partitions[i] = UNOCCUPIED;
  }

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
    arr_pages[i].status = NON;
    arr_pages[i].page_addr = i * 4096 + start_address;
    // printf("%p\n", arr_pages[i].page_addr);
    arr_pages[i].region_addr = start_address;
    arr_pages[i].in_swap = OUT_OF_SWAP;
  }
  // exit(1);

}


region* remove_from_list_2(void *addr) {
  region *region_to_delete;
  region *prev_region;
  region *head_region = lst2->head;

  //int length_return = -1;
  region_to_delete = NULL; 
  // case for if list is empty
  // region_to_delete = head_region;
  if (head_region == NULL) {
    printf("wth why is this empty??\n");

  } else if (head_region->next == NULL) {
    if (head_region->start_address == addr) {
      //length_return = head_region->num_pages;
      region_to_delete = head_region;
      head_region = NULL;
      //free (head_region);
    }

  } else {
    prev_region = head_region;
    region_to_delete = head_region->next;

    // if head node is to be deleted
    if (head_region->start_address == addr) {
      lst2->head = head_region->next;
      //length_return = head_region->num_pages;
      region_to_delete = head_region;
      //free(head_region);
    } else {

      while (region_to_delete->start_address != addr) {
        prev_region = region_to_delete;
        region_to_delete = region_to_delete->next;
      }

      prev_region->next = region_to_delete->next;
      //length_return = region_to_delete->num_pages;
      //free(region_to_delete);
    }
  }
  return region_to_delete;
  //return length_return;

}

// 
page *get_page(void *addr) {
  // find range
  region *curr_region = lst2->head;
  // page *curr_page;

  //printf("ptr: %p\n", addr);
  page* result = NULL;
  while (curr_region != NULL) {
    //printf("loop\n");
    void *low = curr_region->start_address;
    void *high = curr_region->end_address;

    int page_index = 0;

    if (addr >= low && addr < high) {
      // break;
      // // int remainder = addr - low;
      // // if (remainder % 4096 == 0) {
      // //   page_index = remainder / 4096;
      // // } else {
      // //   page_index = remainder / 4096 + 1;
      // // }
      // printf("loop2\n");
      page_index = (addr - low)/PAGE_SIZE;
      page *pages = curr_region->arr_pages;
      result = &pages[page_index];
      break;
      //return &pages[page_index];
     

    } else {
      curr_region = curr_region->next;
      printf("regiin thing %p\n", curr_region->next->start_address);
      // continue;
    }
  }

  return result;

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

// void page_set_prot_none(void *addr) {
//   void *addr_to_handle = addr;
//   page *curr_page = get_page(addr_to_handle);

//   curr_page->status = NON;

//   mprotect(addr_to_handle, PAGE_SIZE, PROT_NONE);

// }

// append to back of tailq
void add_to_tailq(void *addr, page *page_ptr) {
  struct tailq_entry *item;
  item = malloc(sizeof(*item));

  item->page_addr = addr;
  item->page_itself = page_ptr;

  TAILQ_INSERT_TAIL(&my_tailq_head, item, entries);

}


// add condition to check if file alr exists or not
void create_file() {

    if (file_open_status == FILE_OPEN) {
      return;
    }
    int pid = getpid();
    char* filename = (char *)malloc(13); 
    sprintf(filename, "%d", pid);
    strcat(filename, ".swap");

    file_d = open(filename, O_RDWR | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);

}

/* SWAP SHIT HERE */

int get_next_free_swap_offset() {
  
  int index = -1;

  for (int i = 0; i < 10000; i++) {
    if (partitions[i] == UNOCCUPIED) {
      partitions[i] = OCCUPIED;
      index = i;
      break;
    }
    if (i == 10000) {
      printf("array limit exceeded\n");
      exit(1);
    }
  }

  return index;
  
}

void load_from_swap(page *curr_page, int offset_to_load_from) {
  
  // create new page
  // page *swap_page = (page*)malloc(sizeof(page));

  // file related
  create_file();

  // JIM FIX GYM
  mprotect(curr_page->page_addr, PAGE_SIZE, PROT_WRITE | PROT_READ);
  int success = pread(file_d, curr_page->page_addr, PAGE_SIZE, offset_to_load_from);

  if (success == -1) {
    printf("failed on line 354\n");
    exit(1);
  }

  // return swap_page;
}

void write_to_swap(page *curr_page) {
  int offset_to_write_to = get_next_free_swap_offset();
  curr_page->swap_offset = offset_to_write_to * PAGE_SIZE;
  curr_page->index_swap_arr = offset_to_write_to;

  // write here
  create_file();

  // JIM FIX GYM
  // mprotect(page_addr, sizeof(page), PROT_WRITE | PROT_READ);
  int success = pwrite(file_d, curr_page->page_addr, PAGE_SIZE, offset_to_write_to);

  // printf("%p\n", curr_page->page_addr);
  // printf("%d\n", offset_to_write_to);


  if (success == -1) {
    printf("failed on line 375\n");
    exit(1);
  }

}

// remove page from front of tail queue
void evict_page_from_queue() {

  eviction_counter++;

  struct tailq_entry *first_item;
  first_item = TAILQ_FIRST(&my_tailq_head);

  TAILQ_REMOVE(&my_tailq_head, first_item, entries);

}

void free_physical_page(void *addr) {
  madvise(addr, PAGE_SIZE, MADV_DONTNEED);
  mprotect(addr, PAGE_SIZE, PROT_NONE);
}

// do the swap shit here,IF DIRTY
void evict_page_to_swap() {

  // get top of queue
  struct tailq_entry *first_item;
  first_item = TAILQ_FIRST(&my_tailq_head);

  page *curr_page = first_item->page_itself;

  if (curr_page->status == WRITE) {
    // exit(0);
    if (curr_page->in_swap == IN_SWAP) {
      // load_from_swap(curr_page, curr_page->swap_offset);

      int success = pwrite(file_d, curr_page->page_addr, PAGE_SIZE, curr_page->swap_offset);
      if (success == -1) {
        printf("error 54");
        exit(1);
      }

    } else {
      write_to_swap(curr_page);
    }

    // READ 
  } 
  curr_page->status = NON;
  free_physical_page(curr_page->page_addr);
  // create_file();

}


void page_fault_handler(void *addr) {
  
  // TO DELETE
  // printf("fault counter: %d\n", fault_counter);
  fault_counter++;

  void *addr_to_handle = addr; //make it page aligned
  // int size_to_use = get_size(addr_to_handle);

  page *curr_page = get_page(addr_to_handle);

  if (curr_page == NULL) {
    return;
  }

  int page_status = curr_page->status;

  //printf("this page has %d\n", curr_page->status);

  if (page_status == NON) {
    curr_page->status = READ;
    mprotect(curr_page->page_addr, PAGE_SIZE, PROT_READ);

    /* add to tail queue */

    // evict 1 page
    if (CURR_PAGES == (int)MAX_PAGES) {
      
      evict_page_to_swap();
      evict_page_from_queue();
    } else {
      CURR_PAGES++;
    }


    // in swap, load 
    if (curr_page->in_swap == IN_SWAP) {

      // load into the same memory from the offset
      // page *tmp_swap_page; 
      // mprotect(curr_page->page_addr, PAGE_SIZE, PROT_WRITE | PROT_READ);
      load_from_swap(curr_page, curr_page->swap_offset);
      // curr_page = tmp_swap_page;

      curr_page->status = READ;

    // OUT OF SWAP
    } else if (curr_page->in_swap == OUT_OF_SWAP) {
      curr_page->in_swap = IN_SWAP;
      write_to_swap(curr_page);
    } else {
      printf("error: line 498");
      exit(1);
    }

    add_to_tailq(addr_to_handle, curr_page);

  } else if (page_status == READ) {
    curr_page->status = WRITE;
    mprotect(curr_page->page_addr, PAGE_SIZE, PROT_READ | PROT_WRITE);
    // printf("pg st %d\n", curr_page->status);

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

  // initialise if not already done
  if (init_status == 0) {
    init();
  }

  // open file
  create_file();
  file_open_status = FILE_OPEN;

  // sigsev handler
  struct sigaction sa;
  sa.sa_sigaction = handler;
  sa.sa_flags = SA_SIGINFO;
  sigaction(SIGSEGV, &sa, NULL);

  void *start_addr;
  // int multiple;

  // round up to next multiple of page size
  int multiple = 0;

  if (size % 4096 == 0) {
    multiple = size / PAGE_SIZE;
  } else {
    multiple = (size / PAGE_SIZE) + 1;
  }

  int remainder = size % PAGE_SIZE;
  int page_multiple_size = size;
  if (remainder != 0) {
    page_multiple_size = size + PAGE_SIZE - remainder;
  }
  start_addr = mmap (NULL, page_multiple_size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  add_to_list_2(multiple, page_multiple_size, start_addr);
  printf("ptrrr %p\n", start_addr);

  return start_addr;

}

// clear up alloc'd things
void userswap_free(void *mem) {
  region* region = remove_from_list_2(mem);

  printf("eviction count: %d\n", eviction_counter);
  printf("%d\n", region->num_pages);
  exit(1);


  if (region != NULL) {
    for (int i = 0; i < region ->num_pages; i++) {
      // printf("num pages: %d\n", region->num_pages);
      printf("%d\n", i);
      partitions[i] = UNOCCUPIED;
    }
    //remove_from_swap(mem);
    munmap(mem, region->num_pages * PAGE_SIZE);
  } else {
    printf("WHYYYYYY\n");
    exit(1);
  }

}

void *userswap_map(int fd, size_t size) {
  return NULL;
}

// void remove_from_swap(void *addr) {

//   printf("line 446\n");
//   //page *curr_page = get_page(addr);
//   printf("GOT HEREEEEEE\n");
//   region* current_node;

//   partitions[curr_page->index_swap_arr] = UNOCCUPIED;

// }




  // swap *curr_swap;

  // curr_swap = lst_swap->head;
  // printf("wrong\n");
  // while (curr_swap != NULL && curr_swap->page_addr != curr_page->page_addr) {
  //   printf("stuck loop \n");
  //   curr_swap = curr_swap->next;
  // }

  // if(curr_swap != NULL) {
  //   curr_swap->occupied = UNOCCUPIED;
  // }


  // int offset = curr_page->swap_offset;
  // swap *curr_swap;

  // curr_swap = (swap*)malloc(sizeof(swap));

  // // JIM FIX GYM
  // // mprotect(curr_page, sizeof(swap), PROT_WRITE | PROT_READ);
  // int success = pread(file_d, curr_swap, PAGE_SIZE, offset);

  // if (success == -1) {
  //   printf("failed on line 386\n");
  //   exit(1);
  // }

  // curr_swap->occupied = UNOCCUPIED;

  // // JIM FIX GYM
  // // mprotect(curr_swap, sizeof(swap), PROT_WRITE | PROT_READ);
  // int success_2 = pwrite(file_d, curr_swap, sizeof(page), offset);

  // if (success_2 == -1) {
  //   printf("failed on line 397\n");
  //   exit(1);
  // }

    // swap *new_swap;
  // swap *curr_swap;

  // // list empty
  // if (lst_swap->head == NULL) {

  //   curr_swap = (swap*)malloc(sizeof(swap));
  //   lst_swap->head = curr_swap;

  //   curr_swap->next = NULL;
  //   curr_swap->occupied = OCCUPIED;
  //   curr_swap->offset_in_swap = 0;

  //   return 0;

  // } else {

  //   curr_swap = (swap*)malloc(sizeof(swap));
    
  //   // check if any swap empty
  //   curr_swap = lst_swap->head;
  //   int curr_offset = curr_swap->offset_in_swap;

  //   while (curr_swap->next != NULL) {
  //     if (curr_swap->occupied == UNOCCUPIED) {
  //       curr_offset = curr_swap->offset_in_swap;
  //       curr_swap->occupied = OCCUPIED;
  //       return curr_offset;
  //     } 
  //     curr_swap = curr_swap->next;
  //   }

  //   // for last item in offset list
  //   curr_offset = curr_swap->offset_in_swap;

  //   if (curr_swap->occupied == UNOCCUPIED) {
  //     curr_swap->occupied = OCCUPIED;

  //     return curr_offset;
  //   } 

  //   // offset to last
  //   curr_offset += PAGE_SIZE;

  //   // nothing free, create new offset entry to list
  //   new_swap = (swap*)malloc(sizeof(swap));

  //   // set this back of the list
  //   curr_swap->next = new_swap;

  //   new_swap->next = NULL;
  //   new_swap->occupied = OCCUPIED;
  //   new_swap->offset_in_swap = curr_offset;

  //   return curr_offset;

  // int total_pages_needed() {
//   int size = 0;
//   node_t *curr = lst->head;
//   while (curr != NULL) {
//       size += curr->size;
//       curr = curr->next;
//   }
//   return size / PAGE_SIZE;
// }

// int get_partition() {
//   for (int i = 0; i < partition_size; i++) {
//       if (partitions[i] == FREE) {
//           partitions[i] = TAKEN;
//           return i;
//       }
//   }

//   int result = partition_size;
//   partition_size = total_pages_needed();
//   partitions = (int *)realloc(partitions, sizeof(int) * partition_size);
//   partitions[result] = TAKEN;
//   return result;
// }

// int create_file_partitions_array() {
//   int num_of_pages = total_pages_needed();
//   int num_of_partitions_needed = num_of_pages;
//   if (num_of_partitions_needed > 0) {
//       partitions = (int *)malloc(sizeof(int) * num_of_partitions_needed);
//       return num_of_partitions_needed;
//   }
//   return 0;
// }