#include "unistd.h"
#include "stdio.h"

int main()
{
	int fd[2];
	pipe(fd);
	int fd2[2];
	pipe(fd2);
	int id = fork();
	if (id == -1)
	{
		perror("fork error");
		return -1;
	}
	else if (id == 0)
	{
		printf("[%d] It's child\n", getpid());
		fflush(stdout);
		int x, y;
		close(fd[1]);
		close(fd2[0]);
		read(fd[0], &x, sizeof(int));
		read(fd[0], &y, sizeof(int));
		
		int res = x + y;
		write(fd2[1], &res, sizeof(int));
		close(fd[0]);
		close(fd2[1]);
	}
	else
	{
		printf("[%d] It's parent. Child id: %d\n", getpid(), id);
		fflush(stdout);
		int x = 123;
		int y = 5;
		close(fd[0]);
		close(fd2[1]);
		write(fd[1], &x, sizeof(int));
		write(fd[1], &y, sizeof(int));
		
		int res;
		read(fd2[0], &res, sizeof(int));
		printf("[%d] Result from child: %d\n", getpid(), res);
		fflush(stdout);
		close(fd[1]);
		close(fd2[0]);
	}
	return 0;
}
