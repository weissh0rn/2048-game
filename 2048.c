#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <unistd.h>

typedef unsigned int u_int;

#define DETERMINE_STEP(A) ((A) == 0 ? 1 : -1)
#define DIM 4
#define MAX_NUMBER 2048

int max = 4;

bool is_place(const u_int array[DIM][DIM]);
u_int generate_new_number(void);
void add_new_number(u_int array[DIM][DIM]);

//function for move along Y
void get_column(const u_int array[DIM][DIM], u_int tmp[DIM], int x, int start);
void set_column(u_int array[DIM][DIM], u_int tmp[DIM], int x, int start);
void move_along_y(u_int array[DIM][DIM], char direction, u_int *score);

//function for move along X
void get_row(const u_int array[DIM][DIM], u_int tmp[DIM], int y, int start);
void set_row(u_int array[DIM][DIM], u_int tmp[DIM], int y, int start);
void move_along_x(u_int array[DIM][DIM], char direction, u_int *score);

void add(u_int tmp[DIM], u_int *score);
u_int *remove_zeros(u_int array[DIM]);
bool is_game_over(const u_int array[DIM][DIM]);
bool was_movement(const u_int first[DIM][DIM], const u_int second[DIM][DIM]);

void show(const u_int array[DIM][DIM], u_int score);
void show_menu(u_int old_record);
void show_manual(void);
void quit(void);

void save_record_to_file(u_int score);
u_int get_old_record(void);
void game_over(int score, char result);

void game(void);

int main(void)
{
	srand(time(NULL));

	initscr();
	keypad(stdscr, 1);
	noecho();
	curs_set(0);
        start_color();

        init_pair(1, COLOR_WHITE, COLOR_BLACK);
        init_pair(2, COLOR_WHITE, COLOR_GREEN);
        init_pair(3, COLOR_WHITE, COLOR_MAGENTA);
        init_pair(4, COLOR_WHITE, COLOR_RED);

	char key;
	do {
		show_menu(get_old_record());
		game();
		max = 4;
	} while ((key = getch()) == 'r');

	refresh();
	endwin();
	return 0;
}

void game(void)
{
        u_int score = 0;

        u_int array[DIM][DIM] =
        {
                {0, 0, 0, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0}
        };
        u_int previous_array[DIM][DIM];

        add_new_number(array);
        show(array, score);

	int key;
	while (!is_game_over(array)) {
                memcpy(previous_array, array, sizeof(array));
		key = getch();
		switch(key) {
		case 'h': move_along_x(array, key, &score);
			  break;
		case 'j': move_along_y(array, key, &score);
			  break;
		case 'k': move_along_y(array, key, &score);
			  break;
		case 'l': move_along_x(array, key, &score);
			  break;
                case 'm': show_manual();
                          break;
		case 'q': quit();
			  break;
		}

                if (was_movement(array, previous_array)) {
		        add_new_number(array);
                }

		show(array, score);
		if (max == MAX_NUMBER) {
			goto win;
		}
	}
	save_record_to_file(score);
	game_over(score, 'L');

	return ;

win:
	game_over(score, 'W');
	return ;
}

bool is_place(const u_int array[DIM][DIM])
{
	for (u_int x = 0; x < DIM; ++x) {
		for (u_int y = 0; y < DIM; ++y) {
			if (array[y][x] == 0) {
				return 1;
			}
		}
	}
	return 0;
}

u_int generate_new_number(void)
{
        if ((rand() % 100 + 1) > 90) {
                return 4;
        } else {
                return 2;
        }
}

void add_new_number(u_int array[DIM][DIM])
{
	if (!is_place(array)) {
		return ;
	}

        u_int count = rand() % 16 + 1;
        u_int x, y;

        while (1) {
                for (y = 0; y < DIM; ++y) {
                        for (x = 0; x < DIM; ++x) {
                                if (array[y][x] == 0) {
                                        --count;
                                }
                                if (count == 0) {
                                        goto insert_number;
                                }
                        }
                }
        }

insert_number:
        array[y][x] = generate_new_number();
}

void get_column(const u_int array[DIM][DIM], u_int tmp[DIM], int x, int start)
{
        int step = DETERMINE_STEP(start);
        for (u_int count = 0; count < DIM; ++count) {
                tmp[count] = array[start][x];
                start += step;
        }
}

void set_column(u_int array[DIM][DIM], u_int tmp[DIM], int x, int start)
{
        int step = DETERMINE_STEP(start);
        for (u_int count = 0; count < DIM; ++count) {
                array[start][x] = tmp[count];
                start += step;
        }
}

void move_along_y(u_int array[DIM][DIM], char direction, u_int *score)
{
        u_int tmp[DIM];
        int start;
        if (direction == 'k') {
                start = 0;
        } else {
                start = DIM - 1;
        }

        for (u_int x = 0; x < DIM; ++x) {
                get_column(array, tmp, x, start);
                add(tmp, score);
                set_column(array, tmp, x, start);
        }
}

void get_row(const u_int array[DIM][DIM], u_int tmp[DIM], int y, int start)
{
	int step = DETERMINE_STEP(start);
	for (u_int count = 0; count < DIM; ++count) {
		tmp[count] = array[y][start];
		start += step;
	}
}

void set_row(u_int array[DIM][DIM], u_int tmp[DIM], int y, int start)
{
	int step = DETERMINE_STEP(start);
	for (u_int count = 0; count < DIM; ++count) {
		array[y][start] = tmp[count];
		start += step;
	}
}

void move_along_x(u_int array[DIM][DIM], char direction, u_int *score)
{
	unsigned int tmp[DIM];
	int start;
	if (direction == 'h') {
		start = 0;
	} else {
		start = DIM - 1;
	}

	for (u_int y = 0; y < DIM; ++y) {
		get_row(array, tmp, y, start);
		add(tmp, score);
		set_row(array, tmp, y, start);
	}
}

void add(u_int tmp[DIM], u_int *score)
{
        tmp = remove_zeros(tmp);
        for (u_int i = 0; i < DIM; ++i) {
                if (tmp[i] == tmp[i + 1]) {
                        tmp[i] *= 2;
                        tmp[i + 1] = 0;
                        *score += tmp[i];
                        if (tmp[i] > max) {
                                max = tmp[i];
                        }
                }
        }
        tmp = remove_zeros(tmp);
}

u_int *remove_zeros(u_int array[DIM])
{
        u_int tmp[DIM] = {0, 0, 0, 0};
        u_int j = 0;

        for (u_int i = 0; i < DIM; ++i) {
                if (array[i] != 0) {
                        tmp[j] = array[i];
                        ++j;
                }
        }

        memcpy(array, tmp, sizeof(tmp));

        return array;
}

void show(const u_int array[DIM][DIM], u_int score)
{
        attron(COLOR_PAIR(1));
        move(0, 0);
	printw("+----+----+----+----+\n");
	for (size_t y = 0; y < DIM; ++y) {
		printw("|");
		for (size_t x = 0; x < DIM; ++x) {
			if (array[y][x] == 0) {
				printw("    |");
			} else {
                                if (array[y][x] == max) {
                                        attron(COLOR_PAIR(2));
                                }
                                if (array[y][x] == max / 2) {
                                        attron(COLOR_PAIR(3));
                                }
                                if (array[y][x] == max / 4) {
                                        attron(COLOR_PAIR(4));
                                }

				printw("%4d", array[y][x]);
                                attron(COLOR_PAIR(1));
                                printw("|");
			}
		}
		printw("\n+----+----+----+----+\n");
	}
        move(10, 8);
        printw("%d", score);
        refresh();
}

bool is_game_over(const u_int array[DIM][DIM])
{
        for (size_t y = 0; y < DIM; ++y) {
                for (size_t x = 0; x < DIM; ++x) {
                        if (array[y][x] == 0) {
                                return 0;
                        }

                        if (y != DIM - 1) {
                                if (array[y][x] == array[y + 1][x]) {
                                        return 0;
                                }
                        }

                        if (x != DIM - 1) {
                                if (array[y][x] == array[y][x + 1]) {
                                        return 0;
                                }
                        }
                }
        }

        return 1;
}

void show_menu(u_int old_record)
{
	move(9, 0);
	printw("                       ");
        move(10, 0);
        printw("Score:");
        move(11, 0);
        printw("Record:");
	move(11, 8);
	printw("%d", old_record);
        move(12, 0);
        printw("Enter < m > for manual");

        refresh();
}

void show_manual(void)
{
        static bool is_manual_show = 0;
        if (!is_manual_show) {
                for (u_int i = 0; i < 20; ++i) {
                        move(14, i);
                        printw("_");
                        move(22, i);
                        printw("_");
                }

                move(14, 6);
                printw("CONTROLS");
                move(17, 0);
                printw("for move:");
                move(16, 14);
                printw("k");
                move(17, 12);
                printw("h");
                move(17, 16);
                printw("l");
                move(18, 14);
                printw("j");
                move(20, 0);
                printw("for exit:");
                move(20, 12);
                printw("< q >");
                is_manual_show = 1;
        } else {
                for (u_int i = 0; i < 12; ++i) {
                        move(14 + i, 0);
                        printw("                         ");
                }
                is_manual_show = 0;
        }
        refresh();
}

bool was_movement(const u_int first[DIM][DIM], const u_int second[DIM][DIM])
{
        for (u_int y = 0; y < DIM; ++y) {
                for (u_int x = 0; x < DIM; ++x) {
                        if (first[y][x] != second[y][x]) {
                        return 1;
                        }
                }
        }
        return 0;
}

void save_record_to_file(u_int score)
{
	FILE *f = fopen("record.txt", "r");

	u_int previous_record;
	fscanf(f, "%d", &previous_record);

	if (score > previous_record) {
		remove("record.txt");
		f = fopen("record.txt", "w");
		fprintf(f, "%d", score);
		fclose(f);

		return ;
	}

	fclose(f);
	return ;
}

u_int get_old_record(void)
{
	FILE *f = fopen("record.txt", "r");

	if (!f) {
		f = fopen("record.txt", "w");
		fprintf(f, "0");
		fclose(f);
		return 0;
	} else {
		u_int old_record;
		fscanf(f, "%d", &old_record);
		fclose(f);
		return old_record;
	}
}

void game_over(int score, char result)
{
	move(2, 4);
	printw("             ");
	move(3, 4);
	printw("             ");
	move(4, 4);
	if (result == 'L') {
		printw("  YOU  LOSE  ");
	} else {
		printw("   YOU WIN   ");
	}
	move(5, 4);
	printw("             ");
	move(6, 4);
	printw("             ");
	move(9, 0);
	printw("Enter < r > for restart");
}

void quit(void)
{
	move(9, 0);
	printw("Enter < y > for quit");
	int k = getch();
	if (k == 'y') {
		refresh();
		endwin();
		exit(0);
	}
	move(9, 0);
	printw("                    ");
	return ;
}




