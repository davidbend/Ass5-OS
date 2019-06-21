#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>

#define ALARM_TIME 1
#define SCREEN_WIDTH 20
#define SCREEN_HEIGHT 20
#define SHAPE_SIGN '-'
#define BORDER '*'
#define BLANK ' '
#define SHAPE_SIZE 3
#define BLOCKS_TO_MOVE 1
#define X_START (SCREEN_WIDTH / 2)
#define Y_START 0

/* ***************** Gameplay Controls ***************** */

#define MOVE_RIGHT 'd'
#define MOVE_LEFT 'a'
#define MOVE_DOWN 's'
#define ROTATE_SHAPE 'w'
#define QUIT_GAME 'q'

/* ***************** Structs ***************** */

struct Point {
    int x;
    int y;
} typedef Point;

struct Shape {
    Point location;
    bool isHorizontal;
} typedef Shape;

struct GameBoard {
    char board[SCREEN_HEIGHT][SCREEN_WIDTH];
    Shape shape;
    bool shouldStop;
} typedef GameBoard;

/* ***************** Globals ***************** */

GameBoard game;

/* ***************** Decelerations ***************** */

static inline void clearScreen();

void initTetrisBoard();

void addShapeToScreen();

void removeShape();

void printBoard();

bool validateMove(Point point);

void moveShapeDown();

void moveShapeRight();

void moveShapeLeft();

void rotateShape();

void signalHandler(char btn);

/* ***************** Implementations ***************** */

/**
 * Clears the console.
 */
static inline void clearScreen() {
    system("clear");
}

/**
 * Initialize the game's components.
 */
void initTetrisBoard() {
    Point pos;
    Shape shape;
    pos.x = X_START;
    pos.y = Y_START;
    shape.location = pos;
    shape.isHorizontal = true;
    game.shape = shape;
    game.shouldStop = false;
    int i, j;
    for (i = 0; i < SCREEN_HEIGHT; ++i) {
        for (j = 0; j < SCREEN_WIDTH; ++j) {
            if (j == 0 || j == SCREEN_WIDTH - 1 || i == SCREEN_HEIGHT - 1) {
                game.board[i][j] = BORDER;
            } else {
                game.board[i][j] = BLANK;
            }
        }
    }
}

/**
 * Adds the shape to the screen.
 */
void addShapeToScreen() {
    int x, y;
    Point point = game.shape.location;
    if (game.shape.isHorizontal) {
        for (x = point.x - 1; x <= point.x + 1; x++) {
            game.board[point.y][x] = SHAPE_SIGN;
        }
    } else if (!game.shape.isHorizontal) {
        for (y = point.y - 1; y <= point.y + 1; y++) {
            game.board[y][point.x] = SHAPE_SIGN;
        }
    }
}

/**
 * Removes the shape from the screen.
 */
void removeShape() {
    int x, y;
    Point point = game.shape.location;
    if (game.shape.isHorizontal) {
        for (x = point.x - 1; x <= point.x + 1; x++) {
            game.board[point.y][x] = BLANK;
        }
    } else if (!game.shape.isHorizontal) {
        for (y = point.y - 1; y <= point.y + 1; y++) {
            game.board[y][point.x] = BLANK;
        }
    }
}

/**
 * Prints the game's board.
 */
void printBoard() {
    int i, j;
    for (i = 0; i < SCREEN_HEIGHT; ++i) {
        for (j = 0; j < SCREEN_WIDTH; ++j) {
            printf("%c", game.board[i][j]);
        }
        printf("\n");
    }
}

/**
 * Executes one frame of the game.
 */
void doOneFrame() {
    clearScreen();
    addShapeToScreen();
    printBoard();
    removeShape();
}

/**
 * Determines if we can move the shape.
 * @param point is the location of the shape.
 * @return true if we can, and false otherwise.
 */
bool validateMove(Point point) {
    if (game.shape.isHorizontal) {
        return ((point.x >= SHAPE_SIZE - 1 &&
                 point.x <= SCREEN_WIDTH - SHAPE_SIZE) &&
                (point.y > 0 && point.y < SCREEN_HEIGHT - 1));
    } else if (!game.shape.isHorizontal) {
        return ((point.x > 0 && point.x < SCREEN_WIDTH - 1) &&
                (point.y >= SHAPE_SIZE - 1 &&
                 point.y <= SCREEN_HEIGHT - SHAPE_SIZE));
    }
}

/**
 * Moves the shape down.
 */
void moveShapeDown() {
    Point point = game.shape.location;
    point.y += BLOCKS_TO_MOVE;
    bool isLegal = validateMove(point);
    if (isLegal) {
        game.shape.location.y = point.y;
    } else {
        game.shape.location.x = X_START;
        game.shape.location.y = Y_START;
        game.shape.isHorizontal = true;
    }
}

/**
 * Moves the shape right.
 */
void moveShapeRight() {
    Point point = game.shape.location;
    point.x += BLOCKS_TO_MOVE;
    bool isLegal = validateMove(point);
    if (isLegal) {
        game.shape.location.x = point.x;
    }
}

/**
 * Moves the shape left.
 */
void moveShapeLeft() {
    Point point = game.shape.location;
    point.x -= BLOCKS_TO_MOVE;
    bool isLegal = validateMove(point);
    if (isLegal) {
        game.shape.location.x = point.x;
    }
}

/**
 * Rotates the shape.
 */
void rotateShape() {
    Point point = game.shape.location;
    bool isLegal = validateMove(point);
    bool isHorizontal = game.shape.isHorizontal;
    if (isLegal && isHorizontal) {
        game.shape.isHorizontal = false;
    } else if (isLegal) {
        game.shape.isHorizontal = true;
    }
}

/**
 * Moves the shape according to the input.
 * @param btn is the input from the keyboard.
 */
void signalHandler(char btn) {
    switch (btn) {
        case (MOVE_DOWN):
            moveShapeDown();
            break;
        case (MOVE_RIGHT):
            moveShapeRight();
            break;
        case (MOVE_LEFT):
            moveShapeLeft();
            break;
        case (ROTATE_SHAPE):
            rotateShape();
            break;
        case (QUIT_GAME):
            game.shouldStop = true;
            return;
    }
    doOneFrame();
}

/**
 * Handles the input, and executes the signal handler.
 */
void inputHandler() {
    signal(SIGUSR2, inputHandler);
    char btn;
    read(STDIN_FILENO, &btn, sizeof(btn));
    signalHandler(btn);
}

/**
 * Runs the game.
 */
void alarmHandler() {
    signal(SIGALRM, alarmHandler);
    alarm(ALARM_TIME);
    doOneFrame();
    moveShapeDown();
}

/* ***************** Main Function ***************** */

int main() {
    initTetrisBoard();
    signal(SIGUSR2, inputHandler);
    signal(SIGALRM, alarmHandler);
    alarm(ALARM_TIME);
    while (!game.shouldStop) {
        pause();
    }
    return 0;
}