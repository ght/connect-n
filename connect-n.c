/* connect-n game */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG 1
#define MAX_COLS 9
#define MAX_ROWS 9
#define MAX_PLAYERS 2


enum Errors {
	E_OK = 0, /* Just so this is 0 based for simpler extension */
	E_ALLOC,
	E_INVALID_INPUT,
};

struct GameState {
	unsigned int width;
	unsigned int height;
	unsigned char* grid;
	unsigned char* moves;
	unsigned int moves_n;
	unsigned char players_n;
	char tiles[MAX_PLAYERS + 1]; /* Chars to use for empty tiles and the players */
};


int game_init(struct GameState*, unsigned char* const, unsigned char* const,
		const char* const players, const unsigned int, const unsigned int);
int game_move(struct GameState*, const unsigned char);
int game_print(struct GameState*);


int
game_init(struct GameState* gs, unsigned char* const grid, unsigned char* const moves,
		const char* const tiles, const unsigned int width, const unsigned int height)
{
	if (width > MAX_COLS || height > MAX_ROWS || strlen(tiles) > MAX_PLAYERS + 1)
		return -E_INVALID_INPUT;

	gs->width = width;
	gs->height = height;
	gs->grid = grid;
	gs->moves = moves;
	gs->moves_n = 0;
	gs->players_n = 0;
	for (; gs->players_n <= MAX_PLAYERS && tiles[gs->players_n]; gs->players_n++)
		gs->tiles[gs->players_n] = tiles[gs->players_n];
	gs->players_n--; /* Correct for empty tile */

	if (DEBUG) printf(">> game_init() gs: {width: %u, height: %u, players_n: %u, tiles: \"%s\"}\n",
			gs->width, gs->height, gs->players_n, tiles);

	return 0;
}

int
game_move(struct GameState* gs, const unsigned char col)
{
	unsigned int free_tiles = gs->height;
	if (col > gs->width - 1)
		return -E_INVALID_INPUT;

	while (free_tiles) {
		if (gs->grid[gs->height * (col + 1) - free_tiles] == 0) {
			gs->grid[gs->height * (col + 1) - free_tiles] =
				(gs->moves_n + 1) % gs->players_n + 1;
			break;
		}
		--free_tiles;
	}

	if (!free_tiles)
		return -E_INVALID_INPUT;

	gs->moves[gs->moves_n++] = col + 1;

	if (DEBUG) printf(">> game_move() player %d played column %d\n", gs->moves_n % gs->players_n + 1, col);

	//TODO Check for win and return player number on win

	return 0;
}

int
game_print(struct GameState* gs)
{
	char* str_buf;
	size_t len = (gs->width + 2) * (gs->height + 1);
	if (!(str_buf = malloc(len)))
		return -E_ALLOC;

	size_t str_pos = 0;
	size_t i;
	size_t row;
	size_t col;

	/* Print grid */
	for (row = gs->height; row > 0; --row) {
		i = row - 1; /* row is 1-indexed */
		str_buf[str_pos++] = (char) row + '0'; /* print row number */
		for (col = 0; col < gs->width; ++col) {
			str_buf[str_pos++] = gs->tiles[gs->grid[i]]; /* print tile */
			i += gs->height;
		}
		str_buf[str_pos++] = '\n';
	}

	/* Print column numbers */
	str_buf[str_pos++] = '/';
	for (i = 0; i < gs->width; ++i)
		str_buf[str_pos++] = (char) i + '1';

	str_buf[len - 1] = '\0';

	printf("%s\n", str_buf);

	return 0;
}

int
main(void)
{
	struct GameState gs;
	int move_status;

	unsigned char grid[MAX_COLS * MAX_ROWS] = {0};
	unsigned char moves[MAX_COLS * MAX_ROWS] = {0};
	int in_ch;
	unsigned char col;

	//TODO Let player chose dimensions and better help message
	printf("Enter 'q' to quit. Enter digit to play that column. Confirm with enter.\n");
	game_init(&gs, grid, moves, " X@", 7, 6);

	while ((in_ch = getchar())) {
		if (in_ch == 'q')
			break;
		if (in_ch == '\n')
			continue;

		/* Try to move and give feedback */
		col = (unsigned char) in_ch - '1';
		move_status = game_move(&gs, col);
		if (move_status < 0) /* Error */
			printf("game_move(&gs, %d) returned %d\n", col, move_status);
		else if (move_status == 0) /* Normal move */
			game_print(&gs);
		else { /* Win */
			printf("Player TODO won\n");
			break;
		}
	}

	return 0;
}
