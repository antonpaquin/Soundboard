//
// Created by paquin on 12/11/17.
//

#include <stdio.h>
#include "gumsound.h"
#include "stdlib.h"
#include "string.h"
#include "pthread.h"

#define filename_max_length 100


void *gumsound_play_thread(void *gumsound_ptr);


struct gumsound {
    char name[filename_max_length];
};

struct gumsound_thread_args {
    void *data;
    void (*callback)(void *);
    struct gumsound *sound;
};

struct gumsound *gumsound_open(char *filename) {
    struct gumsound *res = malloc(sizeof(struct gumsound));

    // Anton: What is the initial state of res?
    memset(res->name, 0, filename_max_length);
    strcpy(res->name, filename);

    return res;
}

void gumsound_play(struct gumsound *file, void (*callback)(void *), void *data) {
    struct gumsound_thread_args *gg;
    pthread_t sound_thread;

    gg = (struct gumsound_thread_args *) malloc(sizeof(struct gumsound_thread_args));
    gg->data = data;
    gg->callback = callback;
    gg->sound = file;

    pthread_create(&sound_thread, NULL, &gumsound_play_thread, gg);
}

void *gumsound_play_thread(void *gumsound_ptr) {
    struct gumsound_thread_args gg;
    char cmd[filename_max_length + 40];

    gg = *(struct gumsound_thread_args *)gumsound_ptr;

    sprintf(cmd, "padsp ./bin/madplay %s", gg.sound->name);
    printf("%s\n", cmd);

    system(cmd);

    if (gg.callback) {
        (*(gg.callback))(gg.data);
    }

    free(gumsound_ptr);
    return NULL;
}

void gumsound_free(struct gumsound *file) {
    // Anton: Clean up any pointers in file
    free(file);
}
