/* "Let's make 16 games in C++: TETRIS" - by FamTrinli
    ==================================

   Youtube: (see: https://www.youtube.com/watch?v=zH_omFPqMO4)
   This implementation version is upto: 2m11s in the video

   Implementation by Joseph21, oktober 1, 2022
   -------------------------------------------

   Fase 3 - move and rotate tetromino

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

    int dx = 0;
    bool bRotate = false;
    int colorNum = 1;

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

        s = new olc::Sprite( "images/tiles.png" );

        return true;
    }

    bool OnUserUpdate( float fElapsedTime ) override {

        if (GetKey( olc::Key::UP    ).bPressed) bRotate = true;
        if (GetKey( olc::Key::LEFT  ).bPressed) dx = -1;
        if (GetKey( olc::Key::RIGHT ).bPressed) dx = +1;

        // ==========   move   ==========

        for (int i = 0; i < 4; i++) a[i].x += dx;

        // ==========   rotate   ==========

        if (bRotate) {
            Point p = a[1];  // center of rotation
            for (int i = 0; i < 4; i++) {
                int x = a[i].y - p.y;
                int y = a[i].x - p.x;
                a[i].x = p.x - x;
                a[i].y = p.y + y;
            }
        }

        int n = 3; // choose some tetromino for testing
        if (a[0].x == 0) {   // give a[] an initial value if it doesn't have any
            for (int i = 0; i < 4; i++) {
                a[i].x = figures[n][i] % 2;   // get the pattern coordinates for this tetromino in point a[]
                a[i].y = figures[n][i] / 2;
            }
        }

        dx = 0;
        bRotate = false;

        // ==========   draw   ==========

        Clear( olc::WHITE );
        // display this test tetromino
        for (int i = 0; i < 4; i++) {
            // tile size of the sprite is 18x18
            DrawPartialSprite( a[i].x * 18, a[i].y * 18, s, colorNum * 18, 0, 18, 18 );
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

