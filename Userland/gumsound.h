#ifndef GUMSOUND_GUMSOUND_H
#define GUMSOUND_GUMSOUND_H

struct gumsound;

struct gumsound *gumsound_open(char *filename);
void gumsound_play(struct gumsound *file, void (*callback)(void *), void *data);
void gumsound_free(struct gumsound *file);

#endif //GUMSOUND_GUMSOUND_H
