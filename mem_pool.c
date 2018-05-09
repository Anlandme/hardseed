#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"

void mem_pool_init(mem_pool_t *mem_pool_handle)
{
		int i,page_count;
		page_info_t *pages;
		void *temp;

		mem_pool_handle->pool_start_addr = malloc(mem_pool_handle->pool_size);
		assert(mem_pool_handle->pool_start_addr);

		mem_pool_handle->pool_align_addr = MEM_ALIGN(mem_pool_handle->pool_start_addr,getpagesize());

		page_count = mem_pool_handle->page_count = (mem_pool_handle->pool_start_addr + mem_pool_handle->pool_size - mem_pool_handle->pool_align_addr) / 1024;

		mem_pool_handle->pages = malloc(sizeof(page_info_t) * mem_pool_handle->page_count);

		pages = (page_info_t *)mem_pool_handle->pages;
		temp = mem_pool_handle->pool_align_addr;

		for(i=0;i<page_count;++i)
		{
				pages->page_start_addr = temp;
				memset((void *)pages->page_use_info,'n',128);
				pages->page_in_use = 0;
				pages->next_page = NULL;

				pages++;
				temp += 1024;
		}
		for (i=0;i<7;++i)
		{
				mem_pool_handle->chunk_info[i].using_pages = NULL;
				mem_pool_handle->chunk_info[i].used_pages = NULL;
		}

		return;
}

page_info_t *mem_pool_alloc_page(mem_pool_t *mem_pool_handle)
{
		int i = 0;
		page_info_t *page = (page_info_t *)mem_pool_handle->pages;

		for (;i<mem_pool_handle->page_count;++i)
		{
				if (0 == page->page_in_use)
				{
						page->page_in_use = 1;
						return page;
				}

				page++;
		}

		return NULL;
}

mem_pool_free_page(page_info_t *page)
{
		page->page_in_use = 0;
		memset((void *)page->page_use_info,'n',128);
		page->next_page = NULL;
}

void* mem_pool_alloc(mem_pool_t *mem_pool_handle,int mem_size)
{
		int i = 0;
		int bound = 1;
		int real_size = 8;
		int chunk_index = 0;
		int page_full_flag = 1;
		page_info_t *page = NULL;

		if(mem_size <= 0 || mem_size > 512)
		{
#ifdef DEBUG
				printf("un support mem size!!!");
#endif
				return NULL;
		}

		while (mem_size > real_size)
		{
				real_size <<= 1;
				chunk_index++;
		}
#ifdef DEBUG
		printf("size is:%d,chunk index is:%d\n",mem_size,chunk_index);
#endif
		if (NULL == mem_pool_handle->chunk_info[chunk_index].using_pages)
		{
				if (NULL == (mem_pool_handle->chunk_info[chunk_index].using_pages = mem_pool_alloc_page(mem_pool_handle)))
				{
#ifdef DEBUG
						printf("have no enough page!!!\n");
#endif
						return NULL;
				}
		}
		page = mem_pool_handle->chunk_info[chunk_index].using_pages;

		bound = bound << (7-chunk_index);
		printf("bound:%d\n",bound);
		for (;i<bound;++i)
		{
				//如果当前页有可用的chunk,代表当面页没满，设置相应的flag
				if('n' == page->page_use_info[i])
				{
						page->page_use_info[i] = 'y';
						page_full_flag = 0;
						break;
				}
		}

		if (page_full_flag)
		{
			mem_pool_handle->chunk_info[chunk_index].used_pages = page->next_page;
			mem_pool_handle->chunk_info[chunk_index].used_pages = page;
				
				if (NULL == (mem_pool_handle->chunk_info[chunk_index].using_pages = mem_pool_alloc_page(mem_pool_handle)))
				{
#ifdef DEBUG
						printf("have no enough page!!!\n");
#endif
						return NULL;
				}
				
				return mem_pool_handle->chunk_info[chunk_index].using_pages;
		}

		return  (void *)(page->page_start_addr + i * real_size);
}

void mem_pool_stat_dump(mem_pool_t *mem_pool_handle)
{
		int i = 0,page_count = mem_pool_handle->page_count;
		page_info_t *pages = (page_info_t *)mem_pool_handle->pages;

		if (NULL == mem_pool_handle)
		{
				printf("invalid mem pool handle!!!");
				return;
		}

		printf("mem pool info!\n");
		printf("######################\n");

		printf("mem pool start addr:%p\n",mem_pool_handle->pool_start_addr);
		printf("mem pool align addr:%p\n",mem_pool_handle->pool_align_addr);
		printf("mem pool pool size:%d\n",mem_pool_handle->pool_size);
		printf("mem pool pool page count:%d\n",mem_pool_handle->page_count);
		printf("######################\n");

		printf("mem pool page info!\n");
		for(i=0;i<page_count;++i)
		{
				printf("page id:%d,in use:%c,addr:%p\n",i,pages->page_in_use==1?'y':'n',pages->page_start_addr);
				printf("use info:%s\n",pages->page_use_info);
				printf("\n");

				pages++;
		}

		return ;
}
