#ifndef __COMMON__H__
#define __COMMON__H__

#include <stdint.h>
typedef unsigned char u_char;

#define MEM_ALIGN(p, a)                                                   \
		           (u_char *) (((uintptr_t) (p) + ((uintptr_t) a - 1)) & ~((uintptr_t) a - 1))

#define MIN_SIZE (8)

typedef struct page_info_s page_info_t;

struct page_info_s
{
		void *page_start_addr;
		char page_use_info[128];
		int page_in_use;
		page_info_t *next_page;
};

typedef struct chunk_s
{
		page_info_t *using_pages;
		page_info_t *used_pages;
}chunk_t;

typedef struct mem_pool_s
{
		void *pool_start_addr; //内存池其实地址
		void *pool_align_addr; //对齐的地址
		int pool_size;

		void *pages;
		int page_count;
		chunk_t chunk_info[7];//分别用来定位不同的chunk:8 16 32 64 128 256 512 byte
}mem_pool_t;

void mem_pool_init(mem_pool_t *);
void* mem_pool_alloc(mem_pool_t *,int);
void mem_pool_stat_dump(mem_pool_t *);
#endif
