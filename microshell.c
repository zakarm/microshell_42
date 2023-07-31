#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main_executable(char **av, int i, int tmp_fd, char **env);
void ft_putstr_fd(char *s, char *a, int fd)
{
    int i = 0;
    while (s[i])
    {
        write(fd, &s[i], 1);
        i++;
    }
    if (a)
    {
        i = 0;
        while (a[i])
        {
            write(fd, &a[i], 1);
            i++;
        }
    }
    write(2, "\n", 1);
}

int main(int ac, char **av, char **env) {
    (void)ac;
    int i = 0;
    int fd[2];
    int tmp_fd = dup(STDIN_FILENO);
    while (av[i] && av[i + 1])
    {
        av = &av[i + 1];
        i = 0;
        while (av[i] && strcmp(av[i], ";") && strcmp(av[i], "|"))
            i++;
        if (strcmp(av[0], "cd") == 0)
        {
            if (i != 2)
                ft_putstr_fd("error: cd: bad arguments", NULL, 2);
            else if (chdir(av[1]) != 0)
                ft_putstr_fd("error: cd: cannot change directory to ", av[1], 2);
        }
        else if(i != 0 && (av[i] == NULL || strcmp(av[i], ";") == 0))
        {
            if (fork() == 0){
                if (main_executable(av, i, tmp_fd, env))
                    return 1;
            }
            else{
                close(tmp_fd);
                while(waitpid(-1, NULL, WUNTRACED) != -1);
                tmp_fd = dup(STDIN_FILENO);
            }
        }
        else if (i != 0 && strcmp(av[i], "|") == 0)
        {
            pipe(fd);
            if (fork() == 0){
                dup2(fd[1], STDOUT_FILENO);
                close(fd[0]);
                close(fd[1]);
                if (main_executable(av, i, tmp_fd, env))
                    return (1);
            }
            else{
                close(fd[1]);
                close(tmp_fd);
                tmp_fd = fd[0];
            }
        }
    }
    close(tmp_fd);
    return 0;
}

int main_executable(char *av[], int i, int tmp_fd, char **env)
{
    av[i] = NULL;
    dup2(tmp_fd, STDIN_FILENO);
    close(tmp_fd);
    execve(av[0], av, env);
    ft_putstr_fd("error: cannot execute ", av[0], 2);
    return (1);
}
