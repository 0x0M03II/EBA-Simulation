#include "ops.h"
#include "memory_pinning.h"


/**
 * The memory manager is responsible for initializing one, of the currently
 * two supported types of memory:
 * 
 *      1. Using ioctl to allocate memory reserves on an I/O device
 *      2. Use system RAM
 * 
 * The memory manager should always allocate available memory first because
 * it invokes an the mlockall() function with the appropriate flags to ensure
 * current and future allocions are pinned.
 * 
 * Memory pinning is necessary to ensure alocated pages (whether claimed by a 
 * user program or not) remain available for the duration of the parent process
 * (i.g. manager).  This ensures inactivate pages are not swapped, allowing us
 * to avoid page faults.
 * 
 */

static void shared_memory_allocation_matrix(manmemory_manager_state *state);
static inline unsigned int calculate_buffer_percentages(int percentage);
static void buffer_generate_block_count(int bcount, size_t bsize);
static buffer_ent *allocate_sized_block(size_t buff_size);

static void init_memory_handler(int init_error)
{   // ascertain error and close descriptor

    switch (init_error) {
        case EPERM:     fprintf(stderr, "Insufficient priviliges to use MFD_HUGETLB\n"); break;
        case EFAULT:    fprintf(stderr, "Address points to invalid memory!\n"); break;
        case EINVAL:    fprintf(stderr, "EINVAL: See Man page for possibilities.\n"); break;
        case EMFILE:    fprintf(stderr, "Per-process limit on number of file descriptors met\n"); break;
        case ENOMEM:    fprintf(stderr, "Insufficient memory\n"); break;

        // Something is very wrong if this is printed
        default:        fprinf(stderr, "Unkown Error\n"), break;
    }

    close(memfd);   // memfd is globalized
    exit(1);        // there may be a better way to handle our exit
}

static void device_memory_initializer(memory_manager_state *state char *dev); // do NOT leave as prototype

static void shared_memory_initializer(memory_manager_state *state, char *name)
{

    // sealed memory creation for use as shared memory 
    memfd = memfd_create(name, HUGEPAGE_FLAG);
    if (memfd == -1) { perror("memfd_create"); return 1; }

    // The manager should initially claim all memory on the system.
    if (ftruncate(memfd, MEM_SIZE) == -1) {init_memory_handler(errno); }

    // map sealed memory and set up current and future memory pinning
    shared_memory = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, 
                                                    MAP_SHARED, memfd, 0);

    if (addr == MAP_FAILED) { init_memory_handler(errno); }
    if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0) { init_memory_handler(errno); }

    // allow matrix allocator to generate buffers
    shared_memory_allocation_matrix(state);
}

static int initialize_memory(memory_manager_state *state, int allocation_deck)
{       // Initialize the memory state struct

    switch (allocation_deck) {
        case 0: 
            state->memory_type.init_memory_shared = shared_memory_initializer;
            shared_memory_initializer(state, SHM_NAME);
            state->ctx_operations = shmops;
        case 1:
            state->memory_type.init_device_memory = device_memory_initializer;
            device_memory_initializer(state, DEVICESHM);
            state->ctx_operations = devops;
    }

    return 0;
}

static inline unsigned int calculate_buffer_percentages(int percentage)
{       // compute buffer percentages
    
    return (MAX_BUFFERS * percentage) / 100;
}

static buffer_ent *allocate_sized_block(size_t buff_size)
{       // allocate buffers

    if (alloc_offset + buff_size > MEM_SIZE)
        return NULL;

    /* structs are on heap outside of shared & sealed region
        the member buffaddr points to sealed region */
    buffer_ent *temp = (buffer_ent *)malloc(sizeof(buffer_ent));
    if (temp == NULL) { init_memory_handler(errno); }

    temp->buffsize      = buff_size;
    temp->buffaddr      = (char *)shared_memory + alloc_offset;
    temp->is_available  = BUFFER_AVAIL;

    // memory assigned to buffaddr is in shared region; no error handling needed
    alloc_offset += buff_size;

    return temp; 
}

static void buffer_generate_block_count(int bcount, size_t bsize)
{       // Generate buffer memory for count bcount 

    for (int k = 0; k < bcount; ++k)
    {
        buffer_ent *buffer = allocate_sized_block(bsize);
        if (buffer == NULL) {
            return -1;
        }

        bufftab->allocated_buffers[bufftab->no_entries++] = buffer;
    }
}

static void shared_memory_allocation_matrix(manmemory_manager_state *state)
{       // Allocate memory buffers and register in the table

    // prefer to allocate more blocks of size
    int c514    = calculate_buffer_percentages(BUFFER_COUNT_514);
    int c1024   = calculate_buffer_percentages(BUFFER_COUNT_1024);
    int c2048   = calculate_buffer_percentages(BUFFER_COUNT_2048);
    int c4096   = calculate_buffer_percentages(BUFFER_COUNT_4096);

    int total_allocated = c514 + c1024 + c2048 + c4096;
    if (total_allocated < MAX_BUFFERS) {
        printf("Allocating %d leftover buffers to size 514.\n",
            MAX_BUFFERS - total_allocated);

        c514 += MAX_BUFFERS - total_allocated;
    }

    // create total_allocated blocks, honoring the count
    if (bufftab->no_entries == 0 || bufftab->no_entries == NULL) {
        bufftab->no_entries = 0;

        memset(bufftab->allocated_buffers, 0, 
                sizeof(bufftab->allocated_buffers));
    }

    state->tbl_index = &bufftab;

    buffer_generate_block_count(c514,   BUFFER_514);    // 514  bytes
    buffer_generate_block_count(c1024,  BUFFER_1024);   // 1024 bytes
    buffer_generate_block_count(c2048,  BUFFER_2048);   // 2048 bytes
    buffer_generate_block_count(c4096,  BUFFER_4096);   // 4096 bytes

}