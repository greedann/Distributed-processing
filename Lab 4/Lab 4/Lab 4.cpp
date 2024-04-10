#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <corecrt_math.h>
#pragma argsused

struct data_thread
{
	int *arr;
	int n;
	int id;
};

int priorytety[3] = {
	THREAD_PRIORITY_BELOW_NORMAL,
	THREAD_PRIORITY_NORMAL,
	THREAD_PRIORITY_ABOVE_NORMAL};


DWORD WINAPI funkcja_watku(void *argumenty)
{
	struct data_thread *my_data = (struct data_thread *)argumenty;
	for (int i = 0; i < my_data->n; i++)
	{
		my_data->arr[i] = my_data->id;
		Sleep(rand() % 200);
	}
	return 0;
}

int main(int argc, char **argv)
{
	srand(time(NULL));
	int n_threads = 10;
	if (argc > 2)
	{
		n_threads = atoi(argv[1]);
	}
	int n_array = 100;
	int tasks_per_thread = ceil(n_array * 1.0 / n_threads);

	HANDLE *watki = new HANDLE[n_threads];
	data_thread *data_threads = new data_thread[n_threads];
	int *data = new int[n_array];

	for (int i = 0; i < n_array; i++)
	{
		data[i] = i;
		printf("%d ", data[i]);
	}

	for (int i = 0; i < n_threads - 1; i++)
	{
		data_threads[i].n = tasks_per_thread;
		data_threads[i].arr = data + tasks_per_thread * i;
		data_threads[i].id = i;
	}
	data_threads[n_threads - 1].n = n_array - tasks_per_thread * (n_threads - 1);
	data_threads[n_threads - 1].arr = data + tasks_per_thread * (n_threads - 1);
	data_threads[n_threads - 1].id = n_threads - 1;
	

	DWORD id;
	clock_t start, end;
	start = clock();
	for (int i = 0; i < n_threads; i++)
	{
		watki[i] = CreateThread(NULL, 0, funkcja_watku, (void *)&data_threads[i], 0, &id);
		if (watki[i] != INVALID_HANDLE_VALUE)
		{
			SetThreadPriority(watki[i], priorytety[2]);
		}
		else
		{
			printf("Error: %d\n", GetLastError());
		}
	}
	WaitForMultipleObjects(n_threads, watki, 1, INFINITE);
	end = clock();

	printf("\n");
	for (int i = 0; i < n_array; i++)
	{
		printf("%d ", data[i]);
	}
	printf("\nCzas: %f\n", (double)(end - start) / CLOCKS_PER_SEC);
	delete[] watki;
	delete[] data_threads;
	delete[] data;
	return 0;
}
