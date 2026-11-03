#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>

#define MAX_LINE_LEN 1024


size_t strlen(const char *str) {
    size_t cnt = 0;
    for (size_t i = 0; str[i] != '\0'; i++)
        cnt++;
    
    return cnt;
}

void error_msg(const char *msg) {
    write(STDERR_FILENO, msg, strlen(msg));
}

int read_line(char *line, int max_size) {
    int pos = 0;
    while (pos < max_size - 1) {
        char c;
        int res = read(STDIN_FILENO, &c, 1);
        
        if (res < 0) return -1;
        if (res == 0) {
            if (pos == 0) return 0;

            break;
        }
        if (c == '\n') break;

        line[pos] = c;
        pos++;
    }

    line[pos] = '\0';
    return 1;
}

int write_all(int fd, char *text, int size) {
    int written = 0;
    while (written < size) {
        int res = (int)write(fd, text + written, size - written);
        if (res <= 0) return -1;

        written += res;
    }

    return 0;
}

int main(void) {
    srand(time(NULL));

    char file1[MAX_LINE_LEN];
    if (read_line(file1, MAX_LINE_LEN) != 1) {
        error_msg("Error: failed to read 1st file\n");
        exit(EXIT_FAILURE);
    }

    char file2[MAX_LINE_LEN];
    if (read_line(file2, MAX_LINE_LEN) != 1) {
        error_msg("Error: failed to read 2nd file\n");
        exit(EXIT_FAILURE);
    }

    int pipe1[2];
    if (pipe(pipe1) == -1) {
        error_msg("Error: failed to create pipe1\n");
        exit(EXIT_FAILURE);
    }

    int pipe2[2];
    if (pipe(pipe2) == -1) {
        error_msg("Error: failed to create pipe2\n");
        close(pipe1[0]);
        close(pipe1[1]);
        exit(EXIT_FAILURE);
    }

    pid_t child1 = fork();
    if (child1 == -1) {
        error_msg("Error: failed to create child1\n");
        exit(EXIT_FAILURE);
    } else if (child1 == 0) {
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);

        if (dup2(pipe1[0], STDIN_FILENO) == -1) {
            error_msg("Error: dup2 child1 failed\n");
            exit(EXIT_FAILURE);
        }
        close(pipe1[0]);

        char *args[] = {"./child1", file1, NULL};
        execv("./build/child1", args);
        error_msg("Error: execv child1 failed\n");
        exit(EXIT_FAILURE);
    }

    pid_t child2 = fork();
    if (child2 == -1) {
        error_msg("Error: failed to create child2\n");
        exit(EXIT_FAILURE);
    } else if (child2 == 0) {
        close(pipe2[1]);
        close(pipe1[0]);
        close(pipe1[1]);

        if (dup2(pipe2[0], STDIN_FILENO) == -1) {
            error_msg("Error: dup2 child2 failed\n");
            exit(EXIT_FAILURE);
        }
        close(pipe2[0]);

        char *args[] = {"./child2", file2, NULL};
        execv("./build/child2", args);
        error_msg("Error: execv child2 failed\n");
        exit(EXIT_FAILURE);
    }

    close(pipe1[0]);
    close(pipe2[0]);

    char line[MAX_LINE_LEN];
    while (1) {
        int res = read_line(line, MAX_LINE_LEN);
        if (res == 0) break;
        if (res == -1) {
            error_msg("Error: failed to read line\n");
            exit(EXIT_FAILURE);
        }

        int len = strlen(line);
        int percent = rand() % 100;

        line[len] = '\n';
        len++;

        if (percent < 80) {
            if (write_all(pipe1[1], line, len) == -1) {
                error_msg("Error: failed to write to pipe1\n");
                exit(EXIT_FAILURE);
            }
        } else {
            if (write_all(pipe2[1], line, len) == -1) {
                error_msg("Error: failed to write to pipe2\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    close(pipe1[1]);
    close(pipe2[1]);

    waitpid(child1, NULL, 0);
    waitpid(child2, NULL, 0);

    return 0;
}