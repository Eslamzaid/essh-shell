#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>

void slice(const char *str, char *result, size_t start, size_t end);
void errorMessage();
#define ANSI_RESET "\x1b[0m"
#define ANSI_BOLD "\x1b[1m"
#define ANSI_UNDERLINE "\x1b[4m"
#define ANSI_RED "\x1b[31m"
#define ANSI_GREEN "\x1b[32m"

int main(int args, char *argc[])
{
    if (args > 2)
    {
        printf("Too many arguments, only 1 allowed (Batch file)\n");
        exit(1);
    }

    //~ printf(ANSI_BOLD "*****Hello, there to! " ANSI_GREEN "essh shell*****" ANSI_RESET "\n");

    if (args == 1)
    { // for interactive
        char *path = "/bin/";
        char end_shell = 0;
        while (end_shell == 0)
        {
            printf("essh> ");
            char *command = NULL;
            size_t size = 0;
            if (getline(&command, &size, stdin) == -1)
            {
                printf("Couldn't read input\n");
                exit(1);
            }

            int len = strlen(command);
            int counter = 0;

            // Count the number of words
            for (int i = 0; i < len; i++)
            {
                if ((command[i] == ' ' && command[i - 1] != ' ') ||
                    (command[i] == '\n' && command[i - 1] != ' '))
                {
                    counter++;
                }
            }

            char **parameters = malloc((counter + 1) * sizeof(char *));
            if (parameters == NULL)
            {
                printf("Error malloc\n");
                //~ errorMessage();
                exit(0);
            }
            int word_len = 0;
            int index = 0;
            int start = 0;

            for (int i = 0; i <= len; i++)
            {
                if (!isspace(command[i]) && command[i] != '\0')
                {
                    if (word_len == 0)
                    {
                        start = i; // Set the start of the word
                    }
                    word_len++;
                }
                else
                {
                    if (word_len > 0)
                    {
                        parameters[index] = malloc((word_len + 1) * sizeof(char));
                        if (parameters[index] == NULL)
                        {
                            printf("Error inner malloc\n");
                            //~ errorMessage();
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

            parameters[index] = NULL; // Null-terminate the parameters array

            if (strcmp(parameters[0], "exit") == 0)
            {
                printf("Exiting\n");
                // Free the allocated memory for each parameter
                for (int i = 0; i < index; i++)
                {
                    free(parameters[i]);
                }
                // Free the parameters array and command string
                free(parameters);
                free(command);
                end_shell = 1;
                exit(0); // Exit the program
            }

            if (index > 0)
            {
                __pid_t pid = fork();
                if (pid == 0)
                {
                    char *full_path = malloc(strlen(parameters[0] + strlen(path) + 1));
                    strcpy(full_path, path);
                    strcat(full_path, parameters[0]);
                    execv(full_path, parameters);
                    perror("execv failed");
                    // ~ e errorMessage();
                    exit(1);
                }
                else if (pid < 0)
                {
                    perror("fork failed");
                    // ~ e errorMessage();
                }
                else
                {
                    int status;
                    waitpid(pid, &status, 0);
                }
            }

            //* Print the results for debugging
            
            for (int i = 0; i < index; i++)
            {
                printf("Parameter %d: %s\n", i, parameters[i]);
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


//TODO: Implement the Path, cd, build-in functions
//TODO: Implement the redirection
//TODO: Implement Parellel commands
//TODO: Change the README description
