#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <assert.h>
#include <unistd.h>

typedef struct {
	int id;
	sem_t * dataCopied;
	sem_t * footman;
	sem_t * fork[5];
} Data;

int left(int i) {
	return i;
}

int right(int i) {
	return (i + 1) % 5;
}

int philosopher(void *inputData) {
	int id;
	Data data = *((Data*)inputData);
	id = data.id;

	assert(sem_post(data.dataCopied) == 0);
	while (1) {
		usleep(rand() % 100);

		assert(sem_wait(data.footman) == 0);
		assert(sem_wait(data.fork[right(id)]) == 0);
		assert(sem_wait(data.fork[left(id)]) == 0);

		printf("Eating philosoph id: %i\n", id);
		usleep(rand() % 100);

		assert(sem_post(data.fork[left(id)]) == 0);
		assert(sem_post(data.fork[right(id)]) == 0);
		assert(sem_post(data.footman) == 0);
	}
}

int main() {
	int i = 0;
	pthread_t id[4];

	Data data;
	sem_t dataCopied, footman;
	sem_t fork[5];

	assert(sem_init(&dataCopied, 0, 0) == 0);
	data.dataCopied = &dataCopied;
	assert(sem_init(&footman, 0, 4) == 0);
	data.footman = &footman;

	for (i = 0; i < 5; i++) {
		assert(sem_init(&fork[i], 0, 1) == 0);
	}

	for (i = 0; i < 4; i++) {
		data.id = i;
		assert(pthread_create(&id[i], NULL, philosopher, (void*)&data) == 0);
		assert(sem_wait(&dataCopied) == 0);
	}

	for (i = 0; i < 4; i++) {
		assert(pthread_join(id[i], NULL) == 0);
	}

	return 0;
}
