#include <stdio.h>
#include <string.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>

void main()
{
	int fd = -1;
	fd = open("/dev/input/event0" , O_RDWR);
	if(fd < 0)
	{
		printf("open touch panel fail\n");
		return ;
	}
	else 
	{
		printf("open touch panel ok");
		close(fd);
	}
}
