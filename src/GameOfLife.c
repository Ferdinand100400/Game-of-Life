#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

#define HEIGHT 25
#define WIDTH 80

void read_pattern(FILE *f, int grid[HEIGHT][WIDTH]);
int get_index(int i, int max);
int count_neighbors(int grid[HEIGHT][WIDTH], int y, int x);
void update(int grid[HEIGHT][WIDTH]);
void draw(int grid[HEIGHT][WIDTH]);
int to_lower(int c);

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

    int grid[HEIGHT][WIDTH];
    read_pattern(f, grid);
    fclose(f);

    initscr();
    noecho();
    cbreak();
    curs_set(0);
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    int delay = 200;

    while (1) {
        draw(grid);

        int ch = getch();
        if (ch != ERR) {
            ch = to_lower(ch);
            if (ch == 'a') {
                if (delay > 50) delay -= 50;
            } else if (ch == 'z') {
                if (delay < 1000) delay += 50;
            } else if (ch == ' ') {
                break;
            }
        }

        update(grid);
        napms(delay);
    }

    endwin();
    return 0;
}

void read_pattern(FILE *f, int grid[HEIGHT][WIDTH]) {
    char line[WIDTH + 2];
    for (int y = 0; y < HEIGHT; y++) {
        if (fgets(line, sizeof(line), f) != NULL) {
            for (int x = 0; x < WIDTH; x++) {
                grid[y][x] = (line[x] == '1') ? 1 : 0;
            }
        } else {
            for (int x = 0; x < WIDTH; x++) {
                grid[y][x] = 0;
            }
        }
    }
}

int get_index(int i, int max) {
    if (i < 0) return max - 1;
    if (i >= max) return 0;
    return i;
}

int count_neighbors(int grid[HEIGHT][WIDTH], int y, int x) {
    int count = 0;
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue;
            int ny = get_index(y + dy, HEIGHT);
            int nx = get_index(x + dx, WIDTH);
            count += grid[ny][nx];
        }
    }
    return count;
}

void update(int grid[HEIGHT][WIDTH]) {
    int next[HEIGHT][WIDTH] = {0};
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int n = count_neighbors(grid, y, x);
            if (grid[y][x] == 1 && (n == 2 || n == 3)) {
                next[y][x] = 1;
            } else if (grid[y][x] == 0 && n == 3) {
                next[y][x] = 1;
            }
        }
    }
    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++) grid[y][x] = next[y][x];
}

void draw(int grid[HEIGHT][WIDTH]) {
    clear();
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            mvaddch(y, x, grid[y][x] ? '*' : ' ');
        }
    }
    refresh();
}

int to_lower(int c) {
    if (c >= 'A' && c <= 'Z') return c + ('a' - 'A');
    return c;
}
