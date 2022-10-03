/* "Let's make 16 games in C++: 15-PUZZLE" - by FamTrinli
    =====================================

   Youtube: (see: https://youtu.be/NJZ2K6HPpl4?list=PLB_ibvUSN7mzUffhiay5g5GUHyJRO4DYr)
   This implementation version is upto: 0m50s in the video

   Implementation by Joseph21, october 3, 2022
   -------------------------------------------

   Fase 1 - setup, drawing and swapping cells

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

// keep the screen dimensions constant and vary the resolution by adapting the pixel size
#define SCREEN_X    256
#define SCREEN_Y    256
#define PIXEL_X       1
#define PIXEL_Y       1


class ShiftingPuzzle : public olc::PixelGameEngine {

public:
    ShiftingPuzzle() {
        sAppName = "15-Puzzle!";
    }

private:
    // the sprite sheet with all the 15 elements in it (and the empty cell)
    olc::Sprite *t = nullptr;

    int w = 64; // tile size?
    int grid[6][6] = { 0 };
    olc::Sprite *sprite[20] = { nullptr };  // contains pointers to the partial sprites

public:
    bool OnUserCreate() override {

        // load the sprite sheet
        t = new olc::Sprite( "images/15.png" );

        // initialize the grid and the sprite array
        int n = 0;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                n++;
                sprite[n] = t->Duplicate( olc::vi2d( i * w, j * w ), olc::vi2d( w, w ));
                grid[i + 1][j + 1] = n;
            }
        }

        return true;
    }

    bool OnUserUpdate( float fElapsedTime ) override {

        // be sensitive to left mouse click
        if (GetMouse( 0 ).bPressed) {
            olc::vi2d pos = GetMousePos();
            int x = pos.x / w + 1;
            int y = pos.y / w + 1;

            int dx = 0;
            int dy = 0;

            // determine which way to move the cell that's clicked
            if (grid[x + 1][y    ] == 16) { dx =  1; dy =  0; }
            if (grid[x    ][y + 1] == 16) { dx =  0; dy =  1; }
            if (grid[x    ][y - 1] == 16) { dx =  0; dy = -1; }
            if (grid[x - 1][y    ] == 16) { dx = -1; dy =  0; }

            // swap the empty cell and the clicked cell
            int nSwap = grid[x][y];
            grid[x     ][y     ] = 16;
            grid[x + dx][y + dy] = nSwap;
        }

        // draw
        Clear( olc::WHITE );
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                int n = grid[i + 1][j + 1];
                DrawSprite( i * w, j * w, sprite[n] );
            }
        }

        return true;
    }
};

int main()
{
	ShiftingPuzzle game;
	if (game.Construct( SCREEN_X / PIXEL_X, SCREEN_Y / PIXEL_Y, PIXEL_X, PIXEL_Y, false, true ))
		game.Start();

	return 0;
}

