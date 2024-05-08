#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#define FIFO_PATH "/tmp/myfifo"
#define BUFFER_SIZE 256

void *readFifo(void *arg)
{
    int fd;
    char buf[BUFFER_SIZE];

    fd = open(FIFO_PATH, O_RDONLY);
    if (fd == -1)
    {
        perror("open");
        pthread_exit(NULL);
    }

    while (1)
    {
        if (read(fd, buf, sizeof(buf)) > 0)
        {
            printf("Received: %s", buf);
        }
    }

    close(fd);
    pthread_exit(NULL);
}

int main()
{
    pthread_t thread;

    if (pthread_create(&thread, NULL, readFifo, NULL) != 0)
    {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    if (pthread_join(thread, NULL) != 0)
    {
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }

    return 0;
}
