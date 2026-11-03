#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024


void error_msg(const char *msg) {
    int size = 0;
    while (msg[size] != '\0') size++;

    write(STDERR_FILENO, msg, size);
}

int is_vowel(char c) {
    if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u') return 1;
    if (c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U') return 1;

    return 0;
}

int erase_vowels(char *str) {
    int pos = 0, w_pos = 0;
    char c;

    while ((c = str[pos]) != '\0') {
        if (!is_vowel(c)) {
            str[w_pos] = c;
            w_pos++;
        }

        pos++;
    }

    str[w_pos] = '\0';

    return w_pos;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        error_msg("Error: no filename provided\n");
        exit(EXIT_FAILURE);
    }

    int file = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file == -1) {
        error_msg("Error: failed to open file\n");
        exit(EXIT_FAILURE);
    }
    if (dup2(file, STDOUT_FILENO) == -1) {
        error_msg("Error: dup2 failed at child1.c\n");
        close(file);
        exit(EXIT_FAILURE);
    }
    close(file);

    char buffer[BUFFER_SIZE];
    char line[BUFFER_SIZE];
    int len = 0;
    while (1) {
        int bytes = read(STDIN_FILENO, buffer, BUFFER_SIZE);
        if (bytes < 0) {
            error_msg("Error: failed to read from pipe in child1.c\n");
            exit(EXIT_FAILURE);
        }
        if (bytes == 0) break;

        for (int i = 0; i < bytes; i++) {
            if (buffer[i] == '\n') {
                line[len] = '\0';
                len = erase_vowels(line);
                
                write(STDOUT_FILENO, line, len);
                write(STDOUT_FILENO, "\n", 1);
                len = 0;
            } else {
                line[len] = buffer[i];
                len++;
            }
        }
    }

    if (len > 0) {
        line[len] = '\0';
        len = erase_vowels(line);
        write(STDOUT_FILENO, line, len);
        write(STDOUT_FILENO, "\n", 1);
    }

    return 0;
}