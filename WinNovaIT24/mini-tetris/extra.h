#ifndef EXTRA_H
#define EXTRA_H

// ----------------------------------------------------------------
// imports & macros
// ----------------------------------------------------------------

	#include <raylib.h>


// ----------------------------------------------------------------
// structs & enums
// ----------------------------------------------------------------

	typedef struct {
		int x;
		int y;
	} iVec2;


	typedef struct {
		Rectangle rec;
		const char* text;
		bool active;
	} Button;


	typedef enum {
		NONE=0,
		GAME,
		MENU,
		SCORE
	} gameState;


// ----------------------------------------------------------------
// func declaration
// ----------------------------------------------------------------

	Button CreateButton(const char* text, float x, float y, float w, float h);
	int ButtonUpdate(Button*);
	void ButtonDraw(Button*);


#endif


// ----------------------------------------------------------------
// func implementation
// ----------------------------------------------------------------


#ifdef EXTRA_I

	Button CreateButton(const char* text, float x, float y, float w, float h)
	{
		Button b = { 0 };

		b.rec = (Rectangle){x, y, w, h};
		b.text = text;
		b.active = false;

		return b;
	}


	int ButtonUpdate(Button* b)
	{
		int result = 0;

		if(CheckCollisionPointRec(GetMousePosition(), b->rec))
		{
			b->active = true;

			if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
			{
				b->active = false;
				result = 1;
			}
		}
		else
		{
			b->active = false;
		}

		return result;
	}


	void ButtonDraw(Button* b)
	{

		if(b->active)
		{
			DrawRectangleRec(b->rec, GRAY);
		}

		DrawRectangleLinesEx(b->rec, 1, WHITE);

		DrawText(b->text, b->rec.x + (b->rec.width / 10), b->rec.y + (b->rec.height / 10), (b->rec.height / 3), WHITE);
	}


#endif
