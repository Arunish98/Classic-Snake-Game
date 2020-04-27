#include<iostream>
#include<list>
#include<Windows.h>
#include<thread>
using namespace std;

int nScreenWidth = 120;
int nScreenHeight = 30;

struct sSnakeSegment
{
	int x;
	int y;
};


int main()
{
	// Create Screen Buffer
	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	for (int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i] = L' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;
	while (1)
	{
		list<sSnakeSegment> snake = { {60,15},{61,15}, {62,15}, {63,15}, {64,15}, {65,15}, {66,15}, {67,15}, {68,15}, {69,15} };
		int nFoodx = 30;
		int nFoody = 15;
		int nScore = 0;
		int nSnakeDirection = 3;
		bool isDead = false;
		bool bKeyLeft = false, bKeyRight = false, bKeyLeftOld = false, bKeyRightOld = false;

		while (!isDead)
		{
			//Timing & Input
			auto t1 = chrono::system_clock::now();
			//GetInput
			while ((chrono::system_clock::now() - t1) < ((nSnakeDirection % 2 == 1) ? 120ms : 200ms))
			{
				bKeyRight = (0x8000 & GetAsyncKeyState((unsigned char)('\x27'))) != 0;
				bKeyLeft = (0x8000 & GetAsyncKeyState((unsigned char)('\x25'))) != 0;

				if (bKeyRight && !bKeyRightOld)
				{
					nSnakeDirection++;
					if (nSnakeDirection == 4) nSnakeDirection = 0;
				}

				if (bKeyLeft && !bKeyLeftOld)
				{
					nSnakeDirection--;
					if (nSnakeDirection == -1) nSnakeDirection = 3;
				}

				bKeyRightOld = bKeyRight;
				bKeyLeftOld = bKeyLeft;
			}
			//Game Logic

			switch (nSnakeDirection)
			{
			case 0: // UP
				snake.push_front({ snake.front().x,snake.front().y - 1 });
				break;
			case 1: //Right
				snake.push_front({ snake.front().x + 1,snake.front().y });
				break;
			case 2: // Down
				snake.push_front({ snake.front().x,snake.front().y + 1 });
				break;
			case 3: //Left
				snake.push_front({ snake.front().x - 1,snake.front().y });
				break;
			}
			//collision detection
			//collision with v world
			if (snake.front().x < 0 || snake.front().x > nScreenWidth) isDead = true;
			if (snake.front().y < 3 || snake.front().y > nScreenHeight) isDead = true;

			//collision with food
			if (snake.front().x == nFoodx && snake.front().y == nFoody)
			{
				nScore++;
				while (screen[nFoody * nScreenWidth + nFoodx] != L' ')
				{
					nFoodx = rand() % nScreenWidth;
					nFoody = (rand() % (nScreenHeight - 3)) + 3;
				}

				for (int i = 0; i < 5; i++)
					snake.push_back({ snake.back().x, snake.back().y });
			}

			//collision with snake V snake
			for (list<sSnakeSegment>::iterator i = snake.begin(); i != snake.end(); i++)
				if (i != snake.begin() && i->x == snake.front().x && i->y == snake.front().y)
					isDead = true;


			snake.pop_back();

			//Display To Player
			//clear screen
			for (int i = 0; i < nScreenHeight * nScreenWidth; i++) screen[i] = L' ';

			for (int i = 0; i < nScreenWidth; i++)
			{
				screen[i] = L'=';
				screen[2 * nScreenWidth + i] = L'=';
			}
			wsprintf(&screen[nScreenWidth + 5], L" S N A K E ! !                SCORE: %d", nScore);

			//Draw snake body
			for (auto s : snake)
			{
				screen[s.y * nScreenWidth + s.x] = isDead ? L'+' : L'O';
			}

			//Draw snake head
			screen[snake.front().y * nScreenWidth + snake.front().x] = isDead ? L'X' : L'@';

			//Draw food
			screen[nFoody * nScreenWidth + nFoodx] = L'$';
			if (isDead)
				wsprintf(&screen[15 * nScreenWidth + 40], L"    PRESS 'SPACE' TO PLAY AGAIN    ");

			//Display To Console
			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
		}

		// Wait for space
		while ((0x8000 & GetAsyncKeyState((unsigned char)('\x20'))) == 0);
	}
}
