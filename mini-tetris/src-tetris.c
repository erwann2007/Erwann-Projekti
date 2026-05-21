// ---------------------------------------------------------------------
// imports & macros
// ---------------------------------------------------------------------
#include <raylib.h>

#define EXTRA_I
#include "extra.h"

#define TILESIZE 30
#define TG_W 12
#define TG_H 20


// ---------------------------------------------------------------------
// global variables
// ---------------------------------------------------------------------

// other
gameState state = MENU;

double fallTime = 0.0f;
Vector2 sp; // screen position

// buttons
Button start;
Button cntn; // continue

int score = 0;

// tilegrid
int** tg;

// player
int** p;
iVec2 pPos;
int pLs; // last piece


// ---------------------------------------------------------------------
// func declaration
// ---------------------------------------------------------------------

// player
void PlayerInit();
void PlayerQuit();
void PlayerGetPiece();
void PlayerRotate(int);
void PlayerDraw();

// tilegrid
void TileGridInit();
void TileGridQuit();
void TileGridClear(int);
void TileGridDraw();
void TileGridDrawLines();

// tilegrid + player
bool PlayerCollision();
void TileGridPutPlayer();
void ClearCompleteLines();

// game
void GameRun();
void MenuRun();
void ScoreRun();
void AppDraw();


// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// MAIN
int main()
{
	// setup ----------------------------
	// other
	SetTraceLogLevel(LOG_FATAL);
	SetTargetFPS(60);

	// window
	InitWindow(600, 800, "tetris");


	// assets ----------------------------
	sp = (Vector2){GetScreenWidth()/2-(TILESIZE*TG_W)/2, GetScreenHeight()/2-(TILESIZE*TG_H)/2};
	TraceLog(LOG_INFO, "SP: %f, %f", sp.x, sp.y);

	TileGridInit();

	PlayerInit();
	PlayerGetPiece();

	start = CreateButton("START", GetScreenWidth()/3+35, (GetScreenHeight()/4)*3, TILESIZE*4, TILESIZE*2);
	cntn = CreateButton("CONTINUE", GetScreenWidth()/3+10, (GetScreenHeight()/4)*3, TILESIZE*5, TILESIZE*2);


	// loop ----------------------------
	while(!WindowShouldClose())
	{
		// update ----------------------------
		switch(state)
		{
			case GAME: GameRun(); break;
			case MENU: MenuRun(); break;
			case SCORE: ScoreRun(); break;
			default: break;
		}


		// draw ----------------------------
		BeginDrawing();
			ClearBackground(BLACK);

			AppDraw();

			//DrawFPS(10, 10);
		EndDrawing();
	}


	// quit ----------------------------
	TileGridQuit();
	PlayerQuit();
	CloseWindow();

	return 0;
}
// |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


// ---------------------------------------------------------------------
// func implementation
// ---------------------------------------------------------------------

// player --------------------------------------------
void PlayerInit()
{
	// 2D arr
	p = MemAlloc(4 * sizeof(int*));
	for(int i = 0; i < 4; i++) p[i] = MemAlloc(4 * sizeof(int));

	pPos = (iVec2){(TG_W/2)-2, 0}; // pos
	pLs = 0; // last piece

	// clear
	for(int y = 0; y < 4; y++){
	for(int x = 0; x < 4; x++){
		p[y][x] = 0;
	}
	}
}


void PlayerQuit()
{
	for(int i = 0; i < 4; i++) MemFree(p[i]);
	MemFree(p);
}


void PlayerGetPiece()
{
	// get piece
	int random = 0;
	while(random == pLs || random == 0) random = GetRandomValue(1, 7);
	pLs = random;

	// clear
	for(int y = 0; y < 4; y++){
	for(int x = 0; x < 4; x++){
		p[y][x] = 0;
	}
	}

	// build piece
	switch(random)
	{
		case 1: p[1][1]=1; p[1][2]=1; p[2][1]=1; p[2][2]=1; break; // square
		case 2: p[0][1]=2; p[1][1]=2; p[2][1]=2; p[3][1]=2; break; // straight
		case 3: p[0][1]=3; p[1][1]=3; p[2][1]=3; p[2][2]=3; break; // L
		case 4: p[0][2]=3; p[1][2]=3; p[2][2]=3; p[2][1]=3; break; // inverted L
		case 5: p[0][1]=4; p[1][1]=4; p[1][2]=4; p[2][2]=4; break; // S
		case 6: p[0][2]=4; p[1][2]=4; p[1][1]=4; p[2][1]=4; break; // inverted S
		case 7: p[0][1]=5; p[1][0]=5; p[1][1]=5; p[1][2]=5; break; // half cross
		default: break;
	}
}


void PlayerRotate(int direction)
{
	int aux;

	// clock wise
	if(direction == 1)
	{
		// borders
		aux = p[0][0];
		p[0][0] = p[3][0];
		p[3][0] = p[3][3];
		p[3][3] = p[0][3];
		p[0][3] = aux;

		// side left
		aux = p[0][1];
		p[0][1] = p[2][0];
		p[2][0] = p[3][2];
		p[3][2] = p[1][3];
		p[1][3] = aux;

		// side right
		aux = p[0][2];
		p[0][2] = p[1][0];
		p[1][0] = p[3][1];
		p[3][1] = p[2][3];
		p[2][3] = aux;

		// center
		aux = p[1][1];
		p[1][1] = p[2][1];
		p[2][1] = p[2][2];
		p[2][2] = p[1][2];
		p[1][2] = aux;
	}
	// counter clock wise
	else if(direction == -1)
	{
		// borders
		aux = p[0][0];
		p[0][0] = p[0][3];
		p[0][3] = p[3][3];
		p[3][3] = p[3][0];
		p[3][0] = aux;

		// side left
		aux = p[0][1];
		p[0][1] = p[1][3];
		p[1][3] = p[3][2];
		p[3][2] = p[2][0];
		p[2][0] = aux;

		// side right
		aux = p[0][2];
		p[0][2] = p[2][3];
		p[2][3] = p[3][1];
		p[3][1] = p[1][0];
		p[1][0] = aux;

		// center
		aux = p[1][1];
		p[1][1] = p[1][2];
		p[1][2] = p[2][2];
		p[2][2] = p[2][1];
		p[2][1] = aux;
	}
}


void PlayerDraw()
{
	Rectangle rec = {0, 0, TILESIZE, TILESIZE};

	for(int y = 0; y < 4; y++){
	for(int x = 0; x < 4; x++){
		rec.x = (x*TILESIZE) + (pPos.x*TILESIZE) + sp.x;
		rec.y = (y*TILESIZE) + (pPos.y*TILESIZE) + sp.y;

		switch(p[y][x])
		{
			case 1: DrawRectangleRec(rec, RED); break;
			case 2: DrawRectangleRec(rec, GREEN); break;
			case 3: DrawRectangleRec(rec, BLUE); break;
			case 4: DrawRectangleRec(rec, YELLOW); break;
			case 5: DrawRectangleRec(rec, PINK); break;
			default: break;
		}
	}
	}
}


// tilegrid --------------------------------------------
void TileGridInit()
{
	// 2D arr
	tg = MemAlloc(TG_H * sizeof(int*));
	for(int i = 0; i < TG_H; i++) tg[i] = MemAlloc(TG_W * sizeof(int));

	// clear
	for(int y = 0; y < TG_H; y++){
	for(int x = 0; x < TG_W; x++){
		tg[y][x] = 0;
	}
	}
}


void TileGridQuit()
{
	for(int i = 0; i < TG_H; i++) MemFree(tg[i]);
	MemFree(tg);
}


void TileGridClear(int newVal)
{
	for(int y = 0; y < TG_H; y++){
	for(int x = 0; x < TG_W; x++){
		tg[y][x] = newVal;
	}
	}
}


void TileGridDraw()
{
	Rectangle rec = {0, 0, TILESIZE, TILESIZE};

	for(int y = 0; y < TG_H; y++){
	for(int x = 0; x < TG_W; x++){
		rec.x = x*TILESIZE + sp.x;
		rec.y = y*TILESIZE + sp.y;

		switch(tg[y][x])
		{
			case 1: DrawRectangleRec(rec, RED); break;
			case 2: DrawRectangleRec(rec, GREEN); break;
			case 3: DrawRectangleRec(rec, BLUE); break;
			case 4: DrawRectangleRec(rec, YELLOW); break;
			case 5: DrawRectangleRec(rec, PINK); break;
			default: break;
		}
	}
	}
}


void TileGridDrawLines()
{
	// --
	for(int y = 0; y < TG_H+1; y++) DrawLine(sp.x, y*TILESIZE+sp.y, TG_W*TILESIZE+sp.x, y*TILESIZE+sp.y, WHITE);

	// ||
	for(int x = 0; x < TG_W+1; x++) DrawLine(x*TILESIZE+sp.x, sp.y, x*TILESIZE+sp.x, TG_H*TILESIZE+sp.y, WHITE);
}


// tilegrid + player --------------------------------------------
bool PlayerCollision()
{
	bool result = false;

	for(int y = 0; y < 4; y++){
	for(int x = 0; x < 4; x++){
		// check if current tile is full
		if(p[y][x] != 0)
		{
			// check if current tile is in bounds
			if(x+pPos.x > -1 && x+pPos.x < TG_W &&
			y+pPos.y > -1 && y+pPos.y < TG_H )
			{
				// check if current tile place if full
				if(tg[y+pPos.y][x+pPos.x] != 0) result = true;
			}
			else result = true;
		}
	}
	}

	return result;
}


void TileGridPutPlayer()
{
	for(int y = 0; y < 4; y++){
	for(int x = 0; x < 4; x++){
		if(p[y][x] != 0) tg[y+pPos.y][x+pPos.x] = p[y][x];
	}
	}
}


void ClearCompleteLines()
{
	int count;

	for(int y = 0; y < TG_H; y++)
	{
		// count full tiles in a line
		for(int x = 0; x < TG_W; x++) if(tg[y][x] != 0) count += 1;

		// if full
		if(count == TG_W)
		{
			// move upper lines downwards
			for(int nY = y; nY > 0; nY--){
			for(int x = 0; x < TG_W; x++){
				tg[nY][x] = tg[nY-1][x];
			}
			}

			// clear first line
			for(int i = 0; i < TG_W; i++) tg[0][i] = 0;

			// score
			score += 1;
		}

		count = 0;
	}
}


// GAME --------------------------------------------

void GameRun()
{
	// go to menu
	if(IsKeyPressed(KEY_ESCAPE))
	{
		// player
		pPos = (iVec2){(TG_W/2)-2, 0};
		PlayerGetPiece();
		TileGridClear(0);

		// game
		score = 0;
		state = MENU;
		SetExitKey(KEY_ESCAPE);
	}

	// player side movement
	if(IsKeyPressed(KEY_LEFT))
	{
		pPos.x -= 1;
		if(PlayerCollision()) pPos.x += 1;
	}
	else if(IsKeyPressed(KEY_RIGHT))
	{
		pPos.x += 1;
		if(PlayerCollision()) pPos.x -= 1;
	}
	// player rotation
	if(IsKeyPressed(KEY_UP))
	{
		PlayerRotate(1);
		if(PlayerCollision()) PlayerRotate(-1);
	}
	// player fast fall
	if(IsKeyPressed(KEY_DOWN))
	{
		while(!PlayerCollision()) pPos.y += 1;
		pPos.y -= 1;
	}

	// player falling
	if(fallTime+1 < GetTime())
	{
		fallTime = GetTime();

		// player reaches floor
		pPos.y += 1;
		if(PlayerCollision())
		{
			pPos.y -= 1;
			// update tilegrid
			TileGridPutPlayer();
			ClearCompleteLines();

			// reset player
			pPos = (iVec2){(TG_W/2)-2, 0};
			PlayerGetPiece();
			if(PlayerCollision())
			{
				TileGridClear(0);
				state = SCORE;

				//SetExitKey(KEY_ESCAPE);
			}
		}
	 }
}


void MenuRun()
{
	if(ButtonUpdate(&start))
	{
		state = GAME;
		score = 0;

		SetExitKey(KEY_NULL);
	}
}


void ScoreRun()
{
	if(ButtonUpdate(&cntn))
	{
		state = GAME;
		score = 0;
	}
	else if(IsKeyPressed(KEY_ESCAPE))
	{
		state = MENU;

		SetExitKey(KEY_ESCAPE);
	}
}


void AppDraw()
{
	switch(state)
	{
		case GAME:
			PlayerDraw();
			TileGridDraw();
			TileGridDrawLines();
			DrawText(TextFormat("SCORE: %d", score), 10, 750, 30, WHITE);
			break;

		case MENU:
			DrawText("TETRIS", 100, 100, 100, WHITE);
			ButtonDraw(&start);
			break;

		case SCORE:
			DrawText(TextFormat("your score was: %d", score), 40, 350, 30, WHITE);
			ButtonDraw(&cntn);
			break;

		default: break;
	}
}
