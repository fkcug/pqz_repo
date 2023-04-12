#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

int main(int argc, char **argv)
{
	int fd_old, fd_new, fd_new2;
	char *addr = NULL;
	char *addr2 = NULL;
	int readnum;
	struct stat sb;//必须先声明stat结构体，再fstat(,&) ,不能直接声明stat指针
	//struct stat *sb2;

	if(argc != 4)
	{	
		printf("wrong usage !\n");
		return -1;
	}	
	fd_old = open(argv[1], O_RDWR);
	if(fd_old <=0)
	{
		printf("can't open old file ! \n");
		return -1;
	}
	
	printf("integer sb = %d \n",fstat(fd_old, &sb));
	
	//printf("integer sb2 = %d \n",fstat(fd_old, sb2)); //fstat(fd_old, sb2)=-1
	printf("point sb = %p \n",&sb);
	printf("the sizeof old file is %lld bytes \n",(long long)sb.st_size);
	
	fd_new = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	// 参数O_CREAT有的话 后面的S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH 必须有，不然文件存在无法覆盖
	if(fd_new <=0)
	{
		printf("can't create new file ! \n");
		return -1;
	}
	
	addr = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd_old, 0);
	/**内存映射
	 *void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
	 *文件映射将一个文件的一部分直接映射到调用进程的虚拟内存中。一旦一
	 *个文件被映射之后就可以通过在相应的内存区域中操作字节来访问文件内容了。
	 */
	printf("point addr = %p \n",addr);

	if(write(fd_new, addr, sb.st_size) != sb.st_size)
	{
		printf("can't write ! \n");
		return -1;
	}
	
	//addr2 = mmap(NULL, sb.st_size-32, PROT_READ, MAP_SHARED, fd_old, 32);
	//printf("point addr2 = %p \n",addr2);
	//fd_new2 = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	/*
	if(fd_new2 <=0)
		{
			printf("can't create new2 file ! \n");
			return -1;
		}

		if(write(fd_new2, addr2, sb.st_size-32) != (sb.st_size-32))
		{
			printf("can't write ! \n");
			return -1;
		}
		printf("write successfully !\n");

	*/
	
	
	return 0;
}



























