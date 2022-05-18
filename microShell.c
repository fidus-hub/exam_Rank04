#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

void ft_putstr2(char *str)
{
	int i = 0;
	
	while(str[i])
		i++;
	write(2, str, i);
}


int execc(char *argv[], int i, char *env[])
{
	argv[i] = NULL;
	execve(argv[0], argv, env);
	ft_putstr2("error: cannot execute ");
	ft_putstr2(argv[0]);
	write(2, "\n",1);
	return(1);
}


int main(int argc, char *argv[], char *env[])
{
	int i = 0;
	int pid = 0;
	int fd[2];
	int tmp_fd;
	(void)argc;

	tmp_fd = dup(0);
	while (argv[i] && argv[i + 1])
	{
		argv = &argv[i +1];
		i = 0;
		while(argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
			i++;
		if(strcmp(argv[0], "cd") == 0)
		{
			if (i != 2)
				ft_putstr2("error: cd: bad arguments\n");
			else if (chdir(argv[1]) != 0)
			{
				ft_putstr2("error: cd: cannot change directory to ");
				ft_putstr2(argv[1]);
				write(2, "\n", 1);
			}
		}
		else if (i != 0 && (argv[i] == NULL || strcmp(argv[i], ";") == 0))
		{
			pid = fork();
			if (pid == 0)
			{
				dup2(tmp_fd, 0);
				close(tmp_fd);
				if(execc(argv, i, env))
					return(1);
			}
			else
			{
				close(tmp_fd);
				while(waitpid(-1, NULL, WUNTRACED) != -1)
					;
				tmp_fd = dup(0);
				
			}
		}
		else if (i != 0 && strcmp(argv[i], "|") == 0)
		{
			pipe(fd);
			pid = fork();
			if(pid == 0)
			{
				dup2(fd[1], 1);
				dup2(tmp_fd, 0);
				close(fd[0]);
				close(fd[1]);
				close(tmp_fd);
				if(execc(argv, i, env))
					return (1);
			}
			else
			{
				close(tmp_fd);
				close(fd[1]);
				tmp_fd = fd[0];
			}
		}
	}
	close(tmp_fd);
	return(0);
}
