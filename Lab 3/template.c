#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include "pqueue.h"

char *filename = "queue.dat";

int itemId = 0;

typedef struct item item;
struct item
{
	int id;
};

item *
produce()
{
	int time = rand() % 5 + 1;
	item *it = (item *)malloc(sizeof(item));

	sleep(time);
	it->id = itemId;
	itemId += 1;
	return it;
}

void consume(item *it)
{
	int time = rand() % 5 + 1;
	sleep(time);
	free(it);
}

void producer(pid_t childPid)
{
}

void consumer()
{
}

void createTree(int depth, char *mainPidStr)
{
	// create empty queue
	if (depth == 0)
	{
		execlp("ps", "-u greedann", "--forest", NULL);
		// execlp("pstree", "pstree", "-p", mainPidStr, NULL);
	}
	pid_t pid;
	pid = fork();
	if (pid == 0) // child
	{
		if (depth > 0)
			createTree(depth - 1, mainPidStr);
	}
	else
	{
		fork();
		sleep(1);
	}
}

int main(int argc, char **argv)
{
	int depth = 3;
	if (argc > 1)
	{
		depth = atoi(argv[1]);
	}
	pid_t pid;
	pid = getppid();
	char *mainPidStr = (char *)malloc(10);
	sprintf(mainPidStr, "%d", pid);
	createTree(depth, mainPidStr);

	return 0;
}
