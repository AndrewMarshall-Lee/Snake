#ifndef UNICODE
#define UNICODE
#endif


#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
using namespace std;

#include <math.h>
#include <stdio.h>
#include <Windows.h>
#include <winuser.h>
#include <algorithm>
#include <list>


const int Width = 50;
const int Height = 30;
const int ScrnWidth = 60;
const int ScrnHeight = 60;
const int offset = 3;

//empty arrays to store all empty frame, and active frame data
unsigned char *pField = nullptr;

struct Segment{
    int x;
    int y;
};


int main()
{
     //create buffer screen
    wchar_t *screen = new wchar_t [ScrnHeight*ScrnWidth];
    for(int i = 0; i < ScrnWidth*ScrnHeight; i++) screen[i] = L' ';
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;




    //create background (boundary and nothing else) screen
    pField = new unsigned char[Width*Height];
    for (int x = 0; x < Width; x++){ // draw boundary
        for (int y = 0; y < Height; y++){
            pField[y*Width + x] = (x == 0 || x == Width - 1 || y == 0 || y == Height -1) ? 4 : 0;
        }
    }


    //initial variables
    int px = Width / 2;
    int py = Height / 2;
    int fruitx = rand() % Width;
    int fruity = rand() % Height;
    int score = 0, Direction = 0;
    bool GameOver = false, GameWin = false;
    bool AddPoints = false;
    list<Segment> snake = {{px, py}, {px+1, py}, {px+2, py}, {px+3, py}, {px+4, py}, {px+5, py}};

    //change direction
    bool KeyLeft = false, KeyRight = false, KeyUp = false, KeyDown = false;


    while(!GameOver && !GameWin)
    {

        //Game timing
        this_thread::sleep_for(150ms);
        //Get key input
        KeyRight = (0x8000 & GetAsyncKeyState((unsigned char)('\x27'))) != 0;
        KeyLeft = (0x8000 & GetAsyncKeyState((unsigned char)('\x25'))) != 0;
        KeyUp = (0x8000 & GetAsyncKeyState((unsigned char)('\x26'))) != 0;
        KeyDown = (0x8000 & GetAsyncKeyState((unsigned char)('\x28'))) != 0;


        if (KeyLeft){
            Direction = 4;
        }
        else if (KeyUp){
            Direction = 1;
        }
        else if (KeyDown){
            Direction = 3;
        }
        else if (KeyRight){
            Direction = 2;
        }

        //Move Snake
        // Update Snake Position, place a new head at the front of the list in
        // the right direction
        if (Direction > 0)
        {
            switch (Direction)
            {
            case 1: // UP
                snake.push_front({ snake.front().x, snake.front().y - 1 });
                break;
            case 2: // RIGHT
                snake.push_front({ snake.front().x + 1, snake.front().y });
                break;
            case 3: // DOWN
                snake.push_front({ snake.front().x, snake.front().y + 1 });
                break;
            case 4: // LEFT
                snake.push_front({ snake.front().x - 1, snake.front().y });
                break;
            }
            snake.pop_back();
        }




        //Game Logic
            //Check if snake is is eating a fruit
        if (snake.front().x == fruitx && snake.front().y == fruity)
        {
            score++;



            fruitx = rand() % Width ;
            fruity = rand() % Height;




            switch (Direction)
            {
            case 1: // UP
                snake.push_front({ snake.front().x, snake.front().y - 1 });
                break;
            case 2: // RIGHT
                snake.push_front({ snake.front().x + 1, snake.front().y });
                break;
            case 3: // DOWN
                snake.push_front({ snake.front().x, snake.front().y + 1 });
                break;
            case 4: // LEFT
                snake.push_front({ snake.front().x - 1, snake.front().y });
                break;
            }
        }

        for (list<Segment>::iterator i = snake.begin(); i != snake.end(); i++)
        {
            if (i != snake.begin() && i -> x == snake.front().x && i -> y == snake.front().y)
                GameOver = true;
        }

        //check if hitting wall
        if (snake.front().x <= offset || snake.front().x >= (Width+offset-1)){
            GameOver = true;
        }
        if (snake.front().y <= offset || snake.front().y >= (Height+offset-1)){
            GameOver = true;
        }







        //Render Output
           //Draw frame
        for (int x = 0; x < Width; x++){
            for (int y = 0; y < Height; y++){
            //each tile is drawn based on value in array and a set of characters, e.g. 0 = " ", 1 = "A"
                screen[(y+offset)*ScrnWidth + (x+offset)]= L" Hx%#"[pField[y*Width + x]];
            }
        }


    //Draw snake
        for (auto s : snake)
            screen[s.y * ScrnWidth + s.x] = GameOver ? L'x' : L'z';
    //draw head
        for (auto s : snake)
            screen[snake.front().y * ScrnWidth + snake.front().x] = GameOver ? L'X' : L'H';

    //draw fruit
        screen[fruity * ScrnWidth + fruitx] = L'%';

     //Display Frame
        wsprintf(&screen[ScrnWidth + 5],  L"Score = %d", score);
        if (Direction == 0)
            wsprintf(&screen[(py-1) * ScrnWidth + 10],  L"Press an Arrow Key to Start!");

        wsprintf(&screen[ ScrnWidth + 20],  L"GameOver = %d", GameOver);
        wsprintf(&screen[2* ScrnWidth + 40], L"Pos = (%d, %d)", snake.front().x, snake.front().y);

        WriteConsoleOutputCharacter(hConsole, screen, ScrnWidth*ScrnHeight, { 0,0 }, &dwBytesWritten);




        // Wait for space

    }
    wsprintf(&screen[((Height/2)) * ScrnWidth + (Width/2)],  L"Game over, press Space to end");
    while ((0x8000 & GetAsyncKeyState((unsigned char)('\x20'))) == 0);
    WriteConsoleOutputCharacter(hConsole, screen, ScrnWidth*ScrnHeight, { 0,0 }, &dwBytesWritten);
    return 0;
}
