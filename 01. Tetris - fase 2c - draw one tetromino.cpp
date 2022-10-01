/* "Let's make 16 games in C++: TETRIS" - by FamTrinli
    ==================================

   Youtube: (see: https://www.youtube.com/watch?v=zH_omFPqMO4)
   This implementation version is upto: 1m31s in the video

   Implementation by Joseph21, oktober 1, 2022
   -------------------------------------------

   Fase 2c - draw one tetromino

   This implementation file is part of a series of progressive implementation files. These files follow the compilation
   instances from the video. The best way to study this is to follow the video, look at the associated implementation file and
   check on differences between two subsequent implementation files using some difference analysis tool (for instance
   WinMerge).

   Dependencies:
     * FamTrinli uses SFML, I implemented this game using olcPixelGameEngine.h - (olc::PixelGameEngine header file) by JavidX9
       (see: https://github.com/OneLoneCoder/olcPixelGameEngine)
     * In the header of the Youtube video you can find the location where any resources can be found, as well as the original
       code by FamTrinli

     Have fun!
 */

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define M  20         // field dimensions
#define N  10

class Tetris : public olc::PixelGameEngine {

public:
    Tetris() {
        sAppName = "The Game!";
    }

private:
    // put your class variables here
    olc::Sprite *s = nullptr;

    int field[M][N] = { 0 };  // the playing field (board)

    struct Point {
        int x = 0, y = 0;     // added initialisation of x and y
    } a[4], b[4];

    int figures[7][4] = {     // tetromino definitions
        1, 3, 5, 7,    // I
        2, 4, 5, 7,    // Z
        3, 5, 4, 6,    // S
        3, 5, 4, 7,    // T
        2, 3, 5, 7,    // L
        3, 5, 7, 6,    // J
        2, 3, 4, 5,    // O
    };

public:
    bool OnUserCreate() override {

        // initialize your assets here
        s = new olc::Sprite( "images/tiles.png" );

        return true;
    }

    bool OnUserUpdate( float fElapsedTime ) override {

        // your update code per frame here

        int n = 3; // choose some tetromino for testing
        for (int i = 0; i < 4; i++) {
            a[i].x = figures[n][i] % 2;   // get the pattern coordinates for this tetromino in point a[]
            a[i].y = figures[n][i] / 2;
        }

        Clear( olc::WHITE );
        for (int i = 0; i < 4; i++) {
            DrawPartialSprite( a[i].x * 18, a[i].y * 18, s, 0, 0, 18, 18 );
        }

        return true;
    }
};

// keep the screen dimensions constant and vary the resolution by adapting the pixel size
#define SCREEN_X    320
#define SCREEN_Y    480
#define PIXEL_X       1
#define PIXEL_Y       1

int main()
{
	Tetris game;
	if (game.Construct( SCREEN_X / PIXEL_X, SCREEN_Y / PIXEL_Y, PIXEL_X, PIXEL_Y ))
		game.Start();

	return 0;
}

