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
    printf("Сервер отключается...\n");
    if (shmdt(shmaddr) == -1) {
        perror("shmdt");
        exit(1);
    }
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl");
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

    shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    shmaddr = shmat(shmid, NULL, 0);
    if (shmaddr == (void *) -1) {
        perror("shmat");
        exit(1);
    }

    printf("Server is ready to receive data...\n");

    time_t start = time(NULL);
    while (time(NULL) - start < 8) {
        sleep(1);
        int *data = (int *) shmaddr;
        printf("Полученные данные: %d\n", *data);
    }

    if (shmdt(shmaddr) == -1) {
        perror("shmdt");
        exit(1);
    }

    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        exit(1);
    }

    return 0;
}
