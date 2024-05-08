#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#define FIFO_PATH "/tmp/myfifo"
#define BUFFER_SIZE 256

void *changeCase(void *arg)
{
    char *buf = (char *)arg;
    for (int i = 0; buf[i] != '\0'; ++i)
    {
        if (buf[i] >= 'a' && buf[i] <= 'z')
        {
            buf[i] -= ('a' - 'A');
        }
    }
    printf("Modified text: %s", buf);
    pthread_exit(NULL);
}

void *filterDigits(void *arg)
{
    char *buf = (char *)arg;
    for (int i = 0; buf[i] != '\0'; ++i)
    {
        if (buf[i] >= '0' && buf[i] <= '9')
        {
            buf[i] = ' ';
        }
    }
    printf("Filtered text: %s", buf);
    pthread_exit(NULL);
}

int main()
{
    int fd;
    char buf[BUFFER_SIZE];
    pthread_t threads[2];

    mkfifo(FIFO_PATH, 0666);

    while (1)
    {
        printf("Enter message: ");
        fgets(buf, sizeof(buf), stdin);

        if (pthread_create(&threads[0], NULL, changeCase, (void *)buf) != 0)
        {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
        if (pthread_create(&threads[1], NULL, filterDigits, (void *)buf) != 0)
        {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < 2; ++i)
        {
            if (pthread_join(threads[i], NULL) != 0)
            {
                perror("pthread_join");
                exit(EXIT_FAILURE);
            }
        }
        fd = open(FIFO_PATH, O_WRONLY);
        if (fd == -1)
        {
            perror("open");
            exit(EXIT_FAILURE);
        }

        write(fd, buf, sizeof(buf));
        close(fd);
    }

    return 0;
}
