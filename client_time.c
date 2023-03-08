#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

#define SHM_SIZE 1024

int shmid;
void *shmaddr;

void sigint_handler(int signum) {
    printf("Клиент отключается...\n");
    if (shmdt(shmaddr) == -1) {
        perror("shmdt");
        exit(1);
    }
    exit(0);
}

int main() {
    signal(SIGINT, sigint_handler);

    key_t key = ftok("server_time.c", 'R');
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    shmid = shmget(key, SHM_SIZE, 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    shmaddr = shmat(shmid, NULL, 0);
    if (shmaddr == (void *) -1) {
        perror("shmat");
        exit(1);
    }

    srand(time(NULL));
    while (1) {
        int data = rand() % 100;
        int *ptr = (int *) shmaddr;
        *ptr = data;
        printf("Отправить данные: %d\n", data);
        sleep(1);
    }

    return 0;
}
