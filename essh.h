#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

int setStringToArray(int len, char *command, char **parameters);
void slice(const char *str, char *result, size_t start, size_t end);