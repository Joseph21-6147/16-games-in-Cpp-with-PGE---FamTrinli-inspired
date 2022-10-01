/* "Let's make 16 games in C++: TETRIS" - by FamTrinli
    ==================================

   Youtube: (see: https://www.youtube.com/watch?v=zH_omFPqMO4)
   This implementation version is upto: 0m57s in the video

   Implementation by Joseph21, oktober 1, 2022
   -------------------------------------------

   Fase 2b - draw partial sprite (for building tetrominos)

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

class Tetris : public olc::PixelGameEngine {

public:
    Tetris() {
        sAppName = "The Game!";
    }

private:
    // put your class variables here
    olc::Sprite *s = nullptr;

public:
    bool OnUserCreate() override {

        // initialize your assets here
        s = new olc::Sprite( "images/tiles.png" );

        return true;
    }

    bool OnUserUpdate( float fElapsedTime ) override {

        // your update code per frame here
        Clear( olc::WHITE );
        DrawPartialSprite( 0, 0, s, 0, 0, 16, 16 );

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

