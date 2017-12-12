#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include "gumsound.h"
#include "button.h"

#define FILE_NUM 36
#define FILENAME_MAX_LEN 128
#define MEDIA_PATH "./media/"


static struct gumsound* fileStruct[FILE_NUM];

void sound_on_stop(void* data);
void button_listen( int num );
int checkForFiles(char *names[]);

void soundfiles_open(int count, char *names[]);
void soundfiles_close(int count);


int main(int argc, char** argv){
    char *names[FILE_NUM];
    int ii;
    int count;

    //init everything
    for (ii = 0; ii < FILE_NUM; ii++){
        names[ii] = (char *) malloc(sizeof(char) * FILENAME_MAX_LEN);
        memset(names[ii], 0, FILENAME_MAX_LEN);
    }

    // load files into struct
    count = checkForFiles(names);
  
    // gumsound_open on everything
    soundfiles_open(count, names);
 
    button_listen_file("./button_dev", &button_listen);

    // Hang, let threads take care of things
    while (1) {
        sleep(2000);
    }

    soundfiles_close(count);
    return 0;
}


void soundfiles_open(int count, char* names[]) {
    int ii;
    for (ii = 0; ii < count; ii++){
        fileStruct[ii] = gumsound_open(names[ii]);
    }
}


void soundfiles_close(int count){
    int ii;
    for (ii = 0; ii < count; ii++){
        gumsound_free(fileStruct[ii]);
    }
}


int checkForFiles(char *names[]){
    DIR *dir;
    struct dirent *dp;
    int ii;

    printf("Entering checkForFiles\n");
    if ( (dir = opendir(MEDIA_PATH)) == NULL ) {
        printf("Cannot open .\n");
        return -1;
    }
    printf("Opened %s\n", MEDIA_PATH);

    ii = 0;

    while ((dp = readdir(dir)) != NULL){
        printf("Iterating: %u\n", ii);
        if (    (strcmp(dp->d_name, "." ) != 0) &&
                (strcmp(dp->d_name, "..") != 0)) {

            printf("File found: %s\n", dp->d_name);
            sprintf(names[ii], "%s%s", MEDIA_PATH, dp->d_name);
	        ii++;

	        if (ii >= FILE_NUM) {
                return FILE_NUM;
            }
        }
    }

    free(dir);

    return ii;

}


void sound_on_stop(void *data){
    long index = (long) data;
    printf("Stopping index %u\n", index);
    //something to do with turning off the LED
}


void button_listen(int num){
    long index = (long) num;
    if (fileStruct[index]) {
        gumsound_play(fileStruct[index], &sound_on_stop, (void *) index);
    }
}
