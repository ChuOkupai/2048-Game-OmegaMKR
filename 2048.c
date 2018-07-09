#include <OmegaMKR.h>
#include <stdio.h>
#include <time.h>
#define RENDER_MODE_SQUARE	1
#define RENDER_MODE_RSQUARE	2
#define RENDER_MODE_CIRCLE	3

typedef struct Data
{
	int		grid[4][4];
	int		score;
	bool	addNumber;
	int		modeCurrent;
	float	modeBackground;
	float	modeSquare;
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

void setNewRenderMode()
{
	if (D.modeCurrent > 2) D.modeCurrent = 1;
	else D.modeCurrent++;
	if (D.modeCurrent == 1)
	{
		D.modeBackground = 0;
		D.modeSquare = 0;
	}
	else if (D.modeCurrent == 2)
	{
		D.modeBackground = 0.04;
		D.modeSquare = 0.04;
	}
	else
	{
		D.modeBackground = 0.25;
		D.modeSquare = 1;
	}
}

void initGame()
{
	start();
	srand(time(NULL));
	if (setScreen(500, 500, "2048")) forceExit();
	SDL_SetWindowMinimumSize(gScreen.window, 400, 400);
	SDL_SetWindowResizable(gScreen.window, true);
	D.addNumber = false;
	D.modeCurrent = 1;
	setNewRenderMode();
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
	D.grid[k][l] = (r >= 10) ? 2 : 4;
}

void newGridEmpty()
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			D.grid[i][j] = 0;
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

void newGridTest1()
{
	D.grid[0][0] = 2;
	D.grid[0][1] = 4;
	D.grid[0][2] = 0;
	D.grid[0][3] = 2;
	D.grid[1][0] = 2;
	D.grid[1][1] = 0;
	D.grid[1][2] = 0;
	D.grid[1][3] = 2;
	D.grid[2][0] = 0;
	D.grid[2][1] = 2;
	D.grid[2][2] = 2;
	D.grid[2][3] = 2;
	D.grid[3][0] = 0;
	D.grid[3][1] = 4;
	D.grid[3][2] = 2;
	D.grid[3][3] = 2;
}

int isGridFull()
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			if (! D.grid[i][j]) return 0;
	return 1;
}

int isGridOver()
{
	if (! isGridFull()) return 0;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (D.grid[i][j])
			{
				if (i < 3 && D.grid[i][j] == D.grid[i + 1][j])
					return 0;
				if (j < 3 && D.grid[i][j] == D.grid[i][j + 1])
					return 0;
			}
		}
	}
	return 1;
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

void renderSquare(Rect r, float rounded, int value)
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
	renderSquare(setRect(p.x, p.y, 4 * D.squareSize + 5 * D.spaceSize, 4 * D.squareSize + 5 * D.spaceSize), D.modeBackground, 0);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (setRenderColor(getSquareColor(D.grid[i][j])))
				endGame();
			p.x = D.borderSize.x + D.squareSize * j + D.spaceSize * j;
			p.y = D.borderSize.y + D.squareSize * i + D.spaceSize * i;
			renderSquare(setRect(p.x, p.y, D.squareSize, D.squareSize), D.modeSquare, D.grid[i][j]);
		}
	}
}

int addValues(int i1, int j1, int i2, int j2)
{
	if (D.grid[i1][j1] || D.grid[i2][j2])
		D.addNumber = true;
	D.grid[i1][j1] += D.grid[i2][j2];
	D.grid[i2][j2] = 0;
	return D.grid[i1][j1];
}

void rotateGridX()
{
	int i, j, grid[4][4];
	
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			grid[i][3 - j] = D.grid[i][j];
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			D.grid[i][j] = grid[i][j];
}

void rotateGridY()
{
	int i, j, grid[4][4];
	
	for (j = 0; j < 4; j++)
		for (i = 0; i < 4; i++)
			grid[3 - i][j] = D.grid[i][j];
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			D.grid[i][j] = grid[i][j];
}

void moveGrid(int i, int j)
{
	if (i > 3 || j > 3) return;
	if (D.move.x)
	{
		if (! D.grid[i][j])
			addValues(i, j, i, j + 1);
		if (j == 2) moveGrid(i + 1, 0);
		else moveGrid(i, j + 1);
	}
	else if (D.move.y)
	{
		if (! D.grid[i][j])
			addValues(i, j, i + 1, j);
		if (i == 2) moveGrid(0, j + 1);
		else moveGrid(i + 1, j);
	}
}

int assembleGrid(int i, int j)
{
	if (i > 3 || j > 3) return 0;
	int n;
	
	n = 0;
	if (D.move.x)
	{
		if (D.grid[i][j] == D.grid[i][j + 1])
			n += addValues(i, j, i, j + 1);
		if (j == 2) n += assembleGrid(i + 1, 0);
		else n += assembleGrid(i, j + 1);
	}
	else if (D.move.y)
	{
		if (D.grid[i][j] == D.grid[i + 1][j])
			n += addValues(i, j, i + 1, j);
		if (i == 2) n += assembleGrid(0, j + 1);
		else n += assembleGrid(i + 1, j);
	}
	return n;
}

void updateGrid()
{
	if (D.move.x == 0 && D.move.y == 0) return;
	if (D.move.x > 0) rotateGridX();
	else if (D.move.y < 0) rotateGridY();
	moveGrid(0, 0);
	moveGrid(0, 0);
	D.score += assembleGrid(0, 0);
	moveGrid(0, 0);
	if (D.move.x > 0) rotateGridX();
	else if (D.move.y < 0) rotateGridY();
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
						setNewRenderMode();
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

void addRandomNumberOnGrid()
{
	int i, j;
	
	i = randi(0, 3);
	j = randi(0, 3);
	while (D.grid[i][j])
	{
		i = randi(0, 3);
		j = randi(0, 3);
	}
	D.grid[i][j] = (randi(0,99) < 10) ? 4 : 2;
	D.addNumber = false;
}

void startGame()
{
	newGridTest1();
	scaleGame();
	while (1)
	{
		renderGame();
		renderSyncedScreen(20);
		if (isGridOver())
			break;
		if (checkEvent()) break;
		updateGrid();
		if (! isGridFull() && D.addNumber)
			addRandomNumberOnGrid();
	}
}

int main()
{
	initGame();
	startGame();
	endGame();
	return 0;
}
