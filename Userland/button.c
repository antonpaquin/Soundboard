//
// Created by paquin on 12/11/17.
//

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "button.h"

int init = 0;
char listen_file[200];
void (*button_callback)(int);

void handle_signal(int signum);

void button_listen_file(char *filename, void (*callback)(int)) {
    FILE *fp;

    if (!init) {
        init = 1;
        signal(SIGUSR1, handle_signal);
        fp = fopen("pidfile", "w");
        fprintf(fp, "%d\n", getpid());
        fclose(fp);
    }

    strcpy(listen_file, filename);
    button_callback = callback;
}

void handle_signal(int signum) {
    FILE *fp;
    int button_num;
    char read_buf[100];

    memset(read_buf, 0, 100);

    fp = fopen(listen_file, "r");
    fgets(read_buf, 100, fp);
    button_num = atoi(read_buf);
    fclose(fp);

    (*button_callback)(button_num);
}

