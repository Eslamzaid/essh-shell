#include <stdio.h>
#include <malloc.h>
#include "essh.h"

extern char **paths;

int get_path(int count, char *path_arr[])
{

    if (count == 1)
    {
        paths = NULL;
        return 1;
    };
    paths = malloc((count + 1) * sizeof(char *));
    
    int word_len = 0;
    int start = 0;
    int index = 0;
    char skipFirst = 0;

    for (int j = 0; j < count; j++) {
        short len = strlen(path_arr[j]);
        if(skipFirst == 0) {
            skipFirst = 1;
            continue;
        }
        for (int i = 0; i <= len; i++) {
            if (!isspace(path_arr[j][i]) && path_arr[j][i] != '\0') {
                if (word_len == 0)
                    start = i; // Set the start of the word
                word_len++;
            }
            else {
                if (word_len > 0) {
                    paths[index] = malloc((word_len + 1) * sizeof(char));
                    if (paths[index] == NULL) {
                        printf("Error inner malloc\n");
                        //~ errorMessage();
                        free(paths);
                        exit(0);
                    }
                    slice(path_arr[j], paths[index], start, start + word_len);
                    paths[index][word_len] = '\0'; // Null-terminate the string
                    index++;
                    word_len = 0;
                }
            }
        }
    }

    return 0;
}


void cd(char* name)
{
    if(chdir(name) != 0) {
        perror("Error occurred");
    } else {
        printf("Switched to %s\n", name);
    }
}
