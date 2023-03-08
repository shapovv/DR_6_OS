#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#define SHM_SIZE 1024

int main() {
    int shmid;
    key_t key;
    char *shm, *s;
    char input[SHM_SIZE];

    // Генерируем ключ для сегмента разделяемой памяти
    if ((key = ftok(".", 'R')) == -1) {
        perror("ftok");
        exit(1);
    }

    // Создаем сегмент разделяемой памяти
    if ((shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT)) == -1) {
        perror("shmget");
        exit(1);
    }

    // Подключаем сегмент разделяемой памяти к адресному пространству процесса
    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }

    // Генерируем случайные числа и передаем их на сервер
    s = shm;
    while (1) {
        printf("Введите число или 'exit' для выхода: ");
        fgets(input, SHM_SIZE, stdin);
        input[strcspn(input, "\n")] = '\0'; // Удаляем символ переноса строки из ввода

        if (strcmp(input, "exit") == 0) {
            break;
        }

        int num = atoi(input);
        sprintf(s, "%d", num);
        s += sizeof(int);
        printf("Отправлено число: %d\n", num);
        sleep(1);
    }

    // Отключаем сегмент разделяемой памяти от адресного пространства процесса
    if (shmdt(shm) == -1) {
        perror("shmdt");
        exit(1);
    }

    return 0;
}
