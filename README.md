# Partial Tetris game
Assignment as part of operating systems course. I was asked to implement it by using pipes, signals, and alarms.

## Files
There are two files: 
* ex51.c - receives data from the keyboard and passes the data to the process generated from the second file (ex52.c). 
* ex52.c - displays the Tetris board on the screen and updates the board according to the data received.

## Instructions
#### Compliation
```
gcc ex52.c -o draw.out
```
```
gcc ex51.c -o a.out
```
#### Run
```
./a.out
```
#### Gameplay Controls
| Operation     | Control  |
| ------------- |:--------:|
| Move Down     | s        |
| Move Left     | a        |
| Move Right    | d        |
| Rotate        | w        | 
| Quit          | q        |
