#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <termios.h>
#include <stdlib.h>
#include <stdbool.h>

#define ERROR_MSG "Error in system call\n"
#define PATH_FILE "./draw.out"

#define LEFT 'a'
#define DOWN 's'
#define RIGHT 'd'
#define ROTATE 'w'
#define QUIT 'q'

/* ***************** Decelerations ***************** */

static inline void handleError();

char getChar();

bool isGameplayControl(char input);

/* ***************** Implementations ***************** */

static inline void handleError() {
    write(2, ERROR_MSG, 21);
    exit(-1);
}

/**
 * @return the char that the member inserted.
 */
char getChar() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    return (buf);
}

/**
 * Determines if the input is one of the gameplay controls.
 * @param input
 * @return true if it's a gameplay control, and otherwise false.
 */
bool isGameplayControl(char input) {
    return (input == LEFT || input == DOWN || input == RIGHT
            || input == ROTATE || input == QUIT);
}

/* ***************** Main Function ***************** */

int main() {
    int fd[2];
    if (pipe(fd) < 0) {
        handleError();
    }
    pid_t pid = fork();
    if (pid < 0) {
        handleError();
    }
    if (pid == 0) {
        /* Child process */
        close(fd[1]);
        if (dup2(fd[0], 0) < 0) {
            handleError();
        }
        char *args[] = {PATH_FILE, NULL};
        execvp(args[0], args);
        handleError();
    }

    else if (pid > 0) {
        /* Parent process */
        char input;
        close(fd[0]);
        while ((input = getChar()) != QUIT) {
            if (!isGameplayControl(input)) {
                continue;
            }
            if (write(fd[1], &input, sizeof(char)) < 0) {
                handleError();
            }
            if (kill(pid, SIGUSR2) < 0) {
                handleError();
            }
        }
        if (kill(pid, SIGKILL) < 0) {
            handleError();
        }
    }
    return 0;
}