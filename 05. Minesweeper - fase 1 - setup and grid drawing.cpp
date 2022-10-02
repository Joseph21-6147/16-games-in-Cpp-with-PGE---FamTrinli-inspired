/* "Let's make 16 games in C++: MINESWEEPER" - by FamTrinli
    =================================

   Youtube: (see: https://youtu.be/c8wswUEfnrQ?list=PLB_ibvUSN7mzUffhiay5g5GUHyJRO4DYr)
   This implementation version is upto: 0m47s in the video

   Implementation by Joseph21, october 2, 2022
   -------------------------------------------

   Fase 1 - setup and grid drawing

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
#define SCREEN_X    400
#define SCREEN_Y    400
#define PIXEL_X       1
#define PIXEL_Y       1


class MineSweeper : public olc::PixelGameEngine {

public:
    MineSweeper() {
        sAppName = "Minesweeper!";
    }

private:
    int w = 32;           // tile size of (square) tiles in sprite
    int grid[12][12];
    int sgrid[12][12];    // for showing

    olc::Sprite *s = nullptr;

public:
    bool OnUserCreate() override {

        srand( time( 0 ));

        // read sprite with all the necessary tiles
        s = new olc::Sprite( "images/tiles.png" );
        // initialize the show model of the grid (= playing board)
        for (int i = 1; i <= 10; i++) {
            for (int j = 1; j <= 10; j++) {
                sgrid[i][j] = 10;        // 10 is the tile index of the closed cell
            }
        }

        return true;
    }

    bool OnUserUpdate( float fElapsedTime ) override {

        // ==========   draw   ==========

        Clear( olc::WHITE );

        for (int i = 1; i <= 10; i++) {
            for (int j = 1; j <= 10; j++) {
                DrawPartialSprite( i * w, j * w, s, sgrid[i][j] * w, 0, w, w );
            }
        }

        return true;
    }
};

int main()
{
	MineSweeper game;
	if (game.Construct( SCREEN_X / PIXEL_X, SCREEN_Y / PIXEL_Y, PIXEL_X, PIXEL_Y ))
		game.Start();

	return 0;
}

