#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#define readsize 15
int main(int argc, char **argv)
{
	int fd;
	struct stat sb;
	
	if(argc != 2)
	{
		printf("wrong usage !\n");
		return -1;
	}

	if((fd = open(argv[1], O_RDONLY)) <= 0)
	{
		printf("can't open file !\n");
		return -1;
	}

	fstat(fd, &sb);
	char buf[(long long)sb.st_size+1];
	
	if(read(fd, buf, (long long)sb.st_size) == -1)
	{
		printf("can't read\n");
		return -1;
	}
	buf[(long long)sb.st_size + 1] = '\0';
	
	printf("read successfully size : %lld  !\n file :\n %s\n",(long long)sb.st_size,buf);
	

	return 0;
}



























