#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BOARD_WIDTH 6
#define BOARD_HEIGHT 5
#define ERASE_CHAIN_COUNT 3

int board[BOARD_HEIGHT][BOARD_WIDTH];
bool toErase[BOARD_HEIGHT][BOARD_WIDTH];

typedef struct {
	int x, y;
}vec2;

vec2 cursorPos = {};

bool holding = false;

bool animating = false;

int combo = 0;

enum {
	DROP_NONE,
	DROP_FIRE,	//‰Î
	DROP_WATER,//…
	DROP_WOOD,//–Ø
	DROP_LIGHT,//Œõ
	DROP_DARK,//ˆÅ
	DROP_HEAL, //‰ñ•œ
	DROP_MAX
};

enum {
	DROP_STATUS_DEFAULT,
	DROP_STATUS_HOLD,
	DROP_STATUS_MAX
};

//ü¢¤ž™ 
char dropAA[DROP_MAX][DROP_STATUS_MAX][2 + 1] = {
	{"  ","  "},//DROP_NONE,
	{"¤","¥"},//DROP_FIRE,	
	{"¢","£"},//DROP_WATER,
	{"ž","Ÿ"},//DROP_WOOD,
	{"ü","œ"},//DROP_LIGHT,
	{"™","š"},//DROP_DARK,
	{" ","¡"}//DROP_HEAL
};

enum
{
	DIRECTION_RIGHT,
	DIRECTION_DOWN,
	DIRECTION_MAX
};

vec2 directions[DIRECTION_MAX] = {
	{1,0},
	{0,1}
};

int checkChain(vec2 _pos, int _dir, bool _erase=false)
{
		if (board[_pos.y][_pos.x] == DROP_NONE)return 0;

		if (_erase)
			toErase[_pos.y][_pos.x] = true;

		int chain = 1;
		vec2 pos = {
			_pos.x + directions[_dir].x,
			_pos.y + directions[_dir].y
		};

		while (true) {
			if(pos.x>=BOARD_WIDTH
				|| pos.y>=BOARD_HEIGHT
				|| (board[pos.y][pos.x] != board[_pos.y][_pos.x]))break;

			if (_erase)
				toErase[pos.y][pos.x] = true;

			chain++;
			pos.x += directions[_dir].x;
			pos.y += directions[_dir].y;
		}

		return chain;
}

void eraseDrops() 
{
	memset(toErase, 0, sizeof(toErase));
	for (int y = 0; y < BOARD_HEIGHT; y++) {
		for (int x = 0; x < BOARD_WIDTH; x++) {
			for(int i = 0; i < DIRECTION_MAX; i++){
				vec2 pos = { x, y };
				int chain = checkChain(pos, i);

				if (chain >= ERASE_CHAIN_COUNT) {
					checkChain(pos, i, true);
					animating = true;
				}
			}
		}
	}

	for (int y = 0; y < BOARD_HEIGHT; y++) {
		for (int x = 0; x < BOARD_WIDTH; x++) {
			if (toErase[y][x])
				board[y][x] = DROP_NONE;
		}
	}
}

int main()
{
	srand((unsigned int)time(NULL));

	for (int y = 0; y < BOARD_HEIGHT; y++) {
		for (int x = 0; x < BOARD_WIDTH; x++) {
			board[y][x] = 1 + rand() % (DROP_MAX-1);
		}
	}

	while(1) {
		system("cls");
		for (int y = 0; y < BOARD_HEIGHT; y++) {
			for (int x = 0; x < BOARD_WIDTH; x++) {
				int status = DROP_STATUS_DEFAULT;
				if (holding
					&& (x == cursorPos.x)
					&& (y == cursorPos.y))status = DROP_STATUS_HOLD;

				printf("%s", dropAA[board[y][x]][status]);
			}
			if(!animating)if (y == cursorPos.y)printf("©");
			printf("\n");
		}
		for (int x = 0; x < BOARD_WIDTH; x++) {
			if(!animating)if (x == cursorPos.x)printf("ª");
			else printf("  ");
		}

		vec2 lastCursorPos = cursorPos;

		if (!animating) {
			switch (_getch()) {
			case'w':cursorPos.y--; if (cursorPos.y < 0)cursorPos.y = 0; break;
			case'a':cursorPos.x--; if (cursorPos.x < 0)cursorPos.x = 0; break;
			case's':cursorPos.y++; if (cursorPos.y >= BOARD_HEIGHT)cursorPos.y = BOARD_HEIGHT - 1; break;
			case'd':cursorPos.x++; if (cursorPos.x >= BOARD_WIDTH)cursorPos.x = BOARD_WIDTH - 1; break;
			case 'q':return 1; break;
			default:holding = !holding;
				if (!holding) {
					eraseDrops();
				}
				break;
			};

			if (holding) {
				int tmp = board[cursorPos.y][cursorPos.x];
				board[cursorPos.y][cursorPos.x] = board[lastCursorPos.y][lastCursorPos.x];
				board[lastCursorPos.y][lastCursorPos.x] = tmp;
			}
		}
		else {
			time_t t = time(NULL);
			while (time(NULL) == t);

			bool dropped = false;
			for (int y = BOARD_HEIGHT - 2; y >= 0; y--) {
				for (int x = 0; x < BOARD_WIDTH; x++) {
					if ((board[y][x] != DROP_NONE)
						&& (board[y + 1][x] == DROP_NONE)){
						board[y + 1][x] = board[y][x];
						board[y][x] = DROP_NONE;
						dropped = true;
					}
				}
			}
			for (int x = 0; x < BOARD_WIDTH; x++) {
				if (board[0][x] == DROP_NONE) {
					board[0][x] = 1 + rand() % (DROP_MAX - 1);
					dropped = true;
				}
			}

			if (!dropped) {
				animating = false;
				eraseDrops();
			}
		}
	}
}