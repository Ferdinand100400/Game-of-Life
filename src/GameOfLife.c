#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

#define HEIGHT 25
#define WIDTH 80

void read_pattern(FILE *f, int **field, int line, int row);
int get_index(int i, int max);
int count_neighbors(int **field, int y, int x);
void update(int **current_field, int line, int row, int *end);
void draw(int **field, int line, int row);
void control(int *speed, int *pause);
int check_stab_konfig(int **current_field, int **new_field, int line, int row);
int check_all_dead(int **next_field, int line, int row);
int **malloc_matric(int line, int row);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s pattern.txt\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        perror("Failed to open pattern");
        return 1;
    }

    int **current_field;
    int N = HEIGHT;
    int M = WIDTH;
    current_field = malloc_matric(N, M);
    if (current_field == NULL) {
        printf("n/a");
        free(current_field);
        return 0;
    }

    int speed = 200;
    int pause = 1;
    int end = 1;

    read_pattern(f, current_field, N, M);
    fclose(f);

    initscr();
    noecho();
    cbreak();
    curs_set(0);
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    while (end) {
        draw(current_field, N, M);

        control(&speed, &pause);
        while (pause) {
            control(&speed, &pause);
        }

        update(current_field, N, M, &end);
        napms(speed);
    }

    free(current_field);
    napms(5000);
    endwin();
    return 0;
}

void control(int *speed, int *pause) {
    int ch = getch();
    if (ch != ERR) {
        if (((ch == 'a') || (ch == 'A')) && (*pause == 0)) {
            if (*speed > 50) *speed -= 50;
        } else if (((ch == 'z') || (ch == 'Z')) && (*pause == 0)) {
            if (*speed < 1000) *speed += 50;
        } else if (ch == ' ') {
            *pause = !*pause;
        }
    }
}

void read_pattern(FILE *f, int **field, int line, int row) {
    char line1[row + 2];
    for (int y = 0; y < line; y++) {
        if (fgets(line1, sizeof(line1), f) != NULL) {
            for (int x = 0; x < row; x++) {
                field[y][x] = (line1[x] == '1') ? 1 : 0;
            }
        } else {
            for (int x = 0; x < row; x++) {
                field[y][x] = 0;
            }
        }
    }
}

int get_index(int i, int max) {
    if (i < 0) return max - 1;
    if (i >= max) return 0;
    return i;
}

int count_neighbors(int **field, int y, int x) {
    int count = 0;
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue;
            int ny = get_index(y + dy, HEIGHT);
            int nx = get_index(x + dx, WIDTH);
            count += field[ny][nx];
        }
    }
    return count;
}

void update(int **current_field, int line, int row, int *end) {
    int **next_field = malloc_matric(line, row);
    if (next_field != NULL) {
        for (int y = 0; y < line; y++) {
            for (int x = 0; x < row; x++) {
                int n = count_neighbors(current_field, y, x);
                if (current_field[y][x] == 1 && (n == 2 || n == 3)) {
                    next_field[y][x] = 1;
                } else if (current_field[y][x] == 0 && n == 3) {
                    next_field[y][x] = 1;
                } else {
                    next_field[y][x] = 0;
                }
            }
        }

        if ((check_all_dead(next_field, line, row) == line * row) ||
            (check_stab_konfig(current_field, next_field, line, row) == line * row)) {
            *end = 0;
        }

        for (int y = 0; y < line; y++) {
            for (int x = 0; x < row; x++) {
                current_field[y][x] = next_field[y][x];
            }
        }

        free(next_field);
        next_field = NULL;
    } else {
        free(next_field);
        printf("n/a");
    }
}

// Проверка на стабильную конфигурацию
int check_stab_konfig(int **current_field, int **new_field, int line, int row) {
    int count = 0;
    for (int i = 0; i < line; i++) {
        for (int j = 0; j < row; j++) {
            if (new_field[i][j] == current_field[i][j]) {
                count++;
            }
        }
    }
    return count;
}

// Проверка на то что все клетки мертвые
int check_all_dead(int **next_field, int line, int row) {
    int count = 0;
    for (int i = 0; i < line; i++) {
        for (int j = 0; j < row; j++) {
            if (next_field[i][j] == 0) {
                count++;
            }
        }
    }
    return count;
}

void draw(int **field, int line, int row) {
    clear();
    for (int y = 0; y < line; y++) {
        for (int x = 0; x < row; x++) {
            mvaddch(y, x, field[y][x] ? '*' : ' ');
        }
    }
    refresh();
}

int **malloc_matric(int line, int row) {
    int **field = (int **)malloc(line * sizeof(int *));
    int *data = (int *)malloc(line * row * sizeof(int));
    if ((field != NULL) && (data != NULL)) {
        for (int i = 0; i < line; i++) {
            field[i] = data + row * i;
        }
        for (int i = 0; i < line; i++) {
            for (int j = 0; j < row; j++) {
                field[i][j] = 0;
            }
        }
        return field;
    } else {
        free(data);
        free(field);
        return NULL;
    }
}