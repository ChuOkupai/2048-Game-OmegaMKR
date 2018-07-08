#include <OmegaMKR.h>
#include <stdio.h>
#include <time.h>

typedef struct Data
{
	int		grid[4][4];
	int		score;
	bool	circleMode;
	int		spaceSize;
	int		squareSize;
	Point	borderSize;
	Point	move;
	Font	*f1;
	Font	*f2;
	Font	*f3;
}	Data;

Data D;

int randi(int min, int max)
{
	return (rand() % (max - min + 1)) + min;
}

void endGame()
{
	if (D.f1) closeFont(D.f1);
	if (D.f2) closeFont(D.f2);
	if (D.f3) closeFont(D.f3);
	end();
}

void initGame()
{
	start();
	srand(time(NULL));
	if (setScreen(500, 500, "2048")) forceExit();
	SDL_SetWindowMinimumSize(gScreen.window, 400, 400);
	SDL_SetWindowResizable(gScreen.window, true);
	D.circleMode = false;
	D.move = setPoint(0, 0);
	D.f1 = NULL;
	D.f2 = NULL;
	D.f3 = NULL;
	renderSplashScreen();
}

void newGrid()
{
	int i, j, k, l, r;
	
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			D.grid[i][j] = 0;
	r = randi(0, 99);
	i = randi(0, 3);
	k = randi(0, 3);
	j = randi(0, 3);
	l = randi(0, 3);
	while (k == i && l == j)
	{
		k = randi(0, 3);
		l = randi(0, 3);
	}
	D.grid[i][j] = (r == 0) ? 4 : 2;
	D.grid[k][l] = (r >= 20) ? 2 : 4;
}

void newGridDebug()
{
	int n;
	
	n = 2;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			D.grid[i][j] = n;
			n *= 2;
		}
	}
}

void scaleGame()
{
	SDL_GetWindowSize(gScreen.window, &gScreen.width, &gScreen.height);
	if (gScreen.width == gScreen.height)
	{
		D.borderSize.x = gScreen.width / 10;
		D.borderSize.y = gScreen.height / 10;
		D.spaceSize = (gScreen.width - gScreen.width / 5) / 35;
		D.squareSize = (gScreen.width - gScreen.width / 5 - 3 * D.spaceSize) / 4;
	}
	else if (gScreen.width < gScreen.height)
	{
		D.borderSize.x = gScreen.width / 10;
		D.spaceSize = (gScreen.width - gScreen.width / 5) / 35;
		D.squareSize = (gScreen.width - gScreen.width / 5 - 3 * D.spaceSize) / 4;
		D.borderSize.y = gScreen.height / 10 + D.borderSize.x;
	}
	else
	{
		D.borderSize.y = gScreen.height / 10;
		D.spaceSize = (gScreen.height - gScreen.height / 5) / 35;
		D.squareSize = (gScreen.height - gScreen.height / 5 - 3 * D.spaceSize) / 4;
		D.borderSize.x = gScreen.width / 10 + D.borderSize.y;
	}
	if (D.f1) closeFont(D.f1);
	D.f1 = loadFont("ClearSans-Bold.ttf", D.squareSize / 2);
	if (! D.f1) endGame();
	if (D.f2) closeFont(D.f2);
	D.f2 = loadFont("ClearSans-Bold.ttf", D.squareSize / 3);
	if (! D.f2) endGame();
	if (D.f3) closeFont(D.f3);
	D.f3 = loadFont("ClearSans-Bold.ttf", D.squareSize / 3.5);
	if (! D.f3) endGame();
}

void renderSquareValue(Point p, int n)
{
	char value[11];
	Color c;
	
	sprintf(value, "%d", n);
	if (n <= 4) c = setColor(119, 110, 101, 255);
	else c = setColor(249, 246, 242, 255);
	if (setRenderColor(c)) endGame();
	if (n <= 64)
		renderCenteredText(D.f1, value, p);
	else if (n <= 8192)
		renderCenteredText(D.f2, value, p);
	else
		renderCenteredText(D.f3, value, p);
}

void renderPercentSquare(Rect r, float rounded, int value)
{
	float d;
	
	d = r.w / 2.0 * rounded;
	renderFillRectangle(setRect(r.x, r.y + d, r.w, r.h - 2 * d));
	renderFillRectangle(setRect(r.x + d, r.y, r.w - 2 * d, r.h));
	renderFillCircle(setPoint(r.x + d, r.y + d), d);
	renderFillCircle(setPoint(r.x + d, r.y + r.h - d), d);
	renderFillCircle(setPoint(r.x + r.w - d, r.y + r.h - d), d);
	renderFillCircle(setPoint(r.x + r.w - d, r.y + d), d);
	if (value > 0)
		renderSquareValue(setPoint(r.x + D.squareSize / 2, r.y + D.squareSize / 2), value);
}

void renderSquare(Rect r, int radius, int value)
{
	renderFillRectangle(setRect(r.x + D.squareSize / radius, r.y, r.w - 2 * D.squareSize / radius, r.h));
	renderFillRectangle(setRect(r.x, r.y + D.squareSize / radius, D.squareSize / radius, r.h - 2 * D.squareSize / radius));
	renderFillRectangle(setRect(r.x + (radius - 1) * D.squareSize / radius, r.y + D.squareSize / radius, r.w - (radius - 1) * D.squareSize / radius, r.h - 2 * D.squareSize / radius));
	renderFillCircle(setPoint(r.x + D.squareSize / radius, r.y + D.squareSize / radius), D.squareSize / radius);
	renderFillCircle(setPoint(r.x + D.squareSize / radius, r.y + (radius - 1) * D.squareSize / radius), D.squareSize / radius);
	renderFillCircle(setPoint(r.x + (radius - 1) * D.squareSize / radius, r.y + D.squareSize / radius), D.squareSize / radius);
	renderFillCircle(setPoint(r.x + (radius - 1) * D.squareSize / radius, r.y + (radius - 1) * D.squareSize / radius), D.squareSize / radius);
	if (value > 0)
		renderSquareValue(setPoint(r.x + D.squareSize / 2, r.y + D.squareSize / 2), value);
}

Color getSquareColor(int n)
{
	switch (n)
	{
		case 0		: return setColor(205, 193, 180, 255);
		case 2		: return setColor(238, 228, 218, 255);
		case 4		: return setColor(237, 224, 200, 255);
		case 8		: return setColor(242, 177, 121, 255);
		case 16		: return setColor(245, 149,  99, 255);
		case 32		: return setColor(246, 124,  95, 255);
		case 64		: return setColor(246,  94,  59, 255);
		case 128	: return setColor(237, 207, 114, 255);
		case 256	: return setColor(237, 204,  99, 255);
		case 512	: return setColor(237, 200,  80, 255);
		case 1024	: return setColor(237, 197,  63, 255);
		case 2048	: return setColor(236, 193,  45, 255);
		default		: return setColor( 61,  58,  49, 255);
	}
}

void renderGame()
{
	Point p;
	
	if (setRenderColor(setColor(250, 248, 239, 255))) endGame();
	renderClear();
	if (setRenderColor(setColor(187, 173, 160, 255))) endGame();
	p = setPoint(D.borderSize.x - D.spaceSize, D.borderSize.y - D.spaceSize);
	renderPercentSquare(setRect(p.x, p.y, 4 * D.squareSize + 5 * D.spaceSize, 4 * D.squareSize + 5 * D.spaceSize), (D.circleMode) ? 0.25 : 0.03, 0);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (setRenderColor(getSquareColor(D.grid[i][j])))
				endGame();
			p.x = D.borderSize.x + D.squareSize * j + D.spaceSize * j;
			p.y = D.borderSize.y + D.squareSize * i + D.spaceSize * i;
			renderPercentSquare(setRect(p.x, p.y, D.squareSize, D.squareSize), (D.circleMode) ? 1 : 0.03, D.grid[i][j]);
		}
	}
}

int checkEvent()
{
	Event e;
	
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
			case SDL_QUIT:
				return 1;
			case SDL_KEYUP:
				D.move = setPoint(0, 0);
				break;
			case SDL_KEYDOWN:
				if (! e.key.repeat)
				{
					if (e.key.keysym.sym == KEY_LEFT)
						D.move.x = -1;
					else if (e.key.keysym.sym == KEY_RIGHT)
						D.move.x = 1;
					else
						D.move.x = 0;
					if (e.key.keysym.sym == KEY_DOWN)
						D.move.y = -1;
					else if (e.key.keysym.sym == KEY_UP)
						D.move.y = 1;
					else
						D.move.y = 0;
					if (e.key.keysym.sym == KEY_C)
						D.circleMode = (D.circleMode) ? false : true;
					if (e.key.keysym.sym == KEY_ESCAPE)
						return 1;
				}
				else D.move = setPoint(0, 0);
				break;
			case SDL_WINDOWEVENT:
				if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
					scaleGame();
				break;
		}
	}
	return 0;
}

void startGame()
{
	newGrid();
	//newGridDebug();
	scaleGame();
	while (1)
	{
		renderGame();
		renderSyncedScreen(20);
		if (checkEvent()) break;
	}
}

int main()
{
	initGame();
	startGame();
	endGame();
	return 0;
}
