#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
		int i = 1;
		void *p = NULL;
		mem_pool_t *mem_pool_handle = (mem_pool_t *)malloc(sizeof(mem_pool_t));
		memset((void *)mem_pool_handle,0,sizeof(mem_pool_t));

		mem_pool_handle->pool_size = 1024 * 1024;

		mem_pool_init(mem_pool_handle);

		mem_pool_stat_dump(mem_pool_handle);
		i=128;
		while (1)
		{
			p = mem_pool_alloc(mem_pool_handle,7);

			if (NULL == p)
			{
					break;
			}
			printf("addr:%p\n",p);

		}

		return 0;
}

