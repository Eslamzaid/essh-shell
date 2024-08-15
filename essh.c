#include <stdio.h>
#include <errno.h>
#include <sys/wait.h>
#include "buildInFunc.h"
#include "essh.h"

void errorMessage();
#define ANSI_RESET "\x1b[0m"
#define ANSI_BOLD "\x1b[1m"
#define ANSI_UNDERLINE "\x1b[4m"
#define ANSI_RED "\x1b[31m"
#define ANSI_GREEN "\x1b[32m"


char **paths;
static int pthSize = 0;
static int after_Redi = 0;

int main(int args, char *argc[])
{
    if (args > 2)
    {
        printf("Too many arguments, only 1 allowed (Batch file)\n");
        exit(1);
    }

    //~ printf(ANSI_BOLD "*****Hello, there to! " ANSI_GREEN "essh shell*****" ANSI_RESET "\n");

    if (args == 1) { // for interactive

        while (1) {
            printf("essh> ");
            char *command = NULL;
            size_t size = 0;
            if (getline(&command, &size, stdin) == -1) {
                printf("Couldn't read input\n");
                exit(1);
            }

            int len = strlen(command);

            if (len == 1) {
                free(command);
                continue;
            };

            // Count the number of words && ! place for performance improvement (mixing it with setStringToArray if possible) 
            int counter = 0;
            char letter_found = 0;
            for (int i = 0; i < len; i++) {
                if ((command[i] == ' ' && command[i - 1] != ' ') ||
                    (command[i] == '\n' && command[i - 1] != ' '))
                    counter++;
            }
            for(int i = 0; i < len-1; i++) {
                if(command[i] != ' ') {
                    letter_found = 1;
                    break;
                }
            }
            
            if(letter_found == 0) {
                free(command);
                printf("Enter a command\n");
                continue;
            }

            char **parameters = malloc((counter + 1) * sizeof(char *));
            if (parameters == NULL) {
                printf("Error malloc\n");
                //~ errorMessage();
                exit(0);
            }

            int index = setStringToArray(len, command, parameters);
            if(index == -1) {
                printf("Command not supported.\n");
                for (int i = 0; i < index; i++) {
                    free(parameters[i]);
                }
                free(parameters);
                free(command);
                continue;
            }

            
            parameters[index] = NULL; // Null-terminate the parameters array


            // exit commands
            if (strcmp(parameters[0], "exit") == 0)
            {
                printf("Exiting\n");
                for (int i = 0; i < index; i++)
                    free(parameters[i]);

                for (int i = 0; i < pthSize; i++)
                    free(paths[i]);
                
                free(parameters);
                if(pthSize != 0) free(paths);
                free(command);
                exit(0);
            }

            // setting path command
            else if (strcmp(parameters[0], "path") == 0)
            {
                if(pthSize == 0 && index != 1) {
                    pthSize = index - 1 ;
                } else {
                    if(index == 1) {
                        if(pthSize >= 1) {
                            for (int i = 0; i < pthSize; i++)
                                free(paths[i]);
                            free(paths);
                            pthSize = 0;
                            continue;
                        }
                        continue;
                    };
                    for (int i = 0; i < pthSize; i++)
                        free(paths[i]);
                    free(paths);
                    pthSize = index - 1;
                }
                get_path(index, parameters);
            }
            
            // cd command
            else if (strcmp(parameters[0], "cd") == 0) {
                if(index <= 1) printf("Need path\n");
                else cd(parameters[1]);
            }
            
            
            else if(after_Redi == 1) {
                printf("Finished parsing now executing!.\n");
            }
            
            // Shell commands go here
            else if (index > 0) {
                __pid_t pid = fork();
                if (pid == 0) {
                    //! will change to adopt many paths, or non
                    for(int i = 0; i < pthSize; i++) {
                        char *full_path = malloc(strlen(strlen(paths[i]) + parameters[0] + 1));
                        if (full_path == NULL) {
                            perror("malloc failed");
                            exit(1);
                        }
                        strcpy(full_path, paths[i]);
                        strcat(full_path, parameters[0]);
                        execv(full_path, parameters);
                        perror("execv failed with path: ");
                        // ~ e errorMessage();
                        free(full_path);
                    }
                    exit(1);
                }
                else if (pid < 0) {
                    perror("fork failed");
                    // ~ e errorMessage();
                }
                else {
                    int status;
                    waitpid(pid, &status, 0);
                }
            }

            //* Print the results for debugging
            for (int i = 0; i < index; i++) {
                // printf("Parameter %d: %s\n", i, parameters[i]);
                free(parameters[i]);
            }
            free(parameters);
            free(command);
        }
    }
}

void slice(const char *str, char *result, size_t start, size_t end)
{
    strncpy(result, str + start, end - start);
    result[end - start] = '\0'; // Ensure the result is null-terminated
}

void errorMessage()
{
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

int setStringToArray(int len, char *command, char **parameters)
{
    int word_len = 0;
    int start = 0;
    int index = 0;
    char redir = 0;

    for (int i = 0; i <= len; i++)
    {
        if (!isspace(command[i]) && command[i] != '\0') {
            if (word_len == 0)
                start = i; // Set the start of the word
            word_len++;
        }
        else {
            if (word_len > 0) {
                if((command[start] == '>' && command[start+1] == ' ')) {
                    if(index == 0) return -1;
                    after_Redi = 1;    
                }
                else if(after_Redi == 1) {
                    if(++redir == 2) return -1;
                };
                parameters[index] = malloc((word_len + 1) * sizeof(char));
                if (parameters[index] == NULL) {
                    printf("Error inner malloc\n");
                    //~ errorMessage();
                    for(int i = 0; i < index; i++) free(parameters[i]);
                    free(parameters);
                    exit(0);
                }
                
                slice(command, parameters[index], start, start + word_len);
                parameters[index][word_len] = '\0'; // Null-terminate the string
                index++;
                word_len = 0;
                

            }
        }
    }
    return index;
}

//* TODO: Implement the Path, cd, build-in functions
//@ TODO_MINI: implement the default path.
// TODO: Implement the redirection
// TODO: Implement Parellel commands
// TODO: Change the README description
// & ADDITIONS:
/**  
 * Print ecch>{current path name, where are we located}
 */