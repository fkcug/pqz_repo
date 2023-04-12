#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

// ./ledtest /dev/myled on
// ./ledtest /dev/myled off

int main(int argc, char **argv)
{
	int fd;
	char status = 0;
	
	if(argc != 3)
	{
		printf("wrong usage !\n");
		return -1;
	}
	
	fd = open(argv[1], O_RDWR);
	if(fd == -1)
	{
		printf("can not open device !\n");
		return -1;
	}

	//write
	if(0 == strcmp(argv[2], "on") )
	{
		status = 1;
	}
	else
	{
		status = 0;
	}
	
	write(fd, &status, 1);

	
	return 0;
}







