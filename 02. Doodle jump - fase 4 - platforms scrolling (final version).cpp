/* "Let's make 16 games in C++: DOODLE JUMP" - by FamTrinli
    =======================================

   Youtube: (see: https://youtu.be/7Vf_vQIUk5Q?list=PLB_ibvUSN7mzUffhiay5g5GUHyJRO4DYr)
   This implementation version is upto the end of the video - this is the final version

   Implementation by Joseph21, oktober 1, 2022
   -------------------------------------------

   Fase 4 - platforms scrolling

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
#define SCREEN_Y    533
#define PIXEL_X       1
#define PIXEL_Y       1

#define NR_PLATFORMS   10

class DoodleJump : public olc::PixelGameEngine {

public:
    DoodleJump() {
        sAppName = "Doodle Game!";
    }

private:
    // put your class variables here
    olc::Sprite *sBackground = nullptr;
    olc::Sprite *sPlat       = nullptr;
    olc::Sprite *sPers       = nullptr;

    struct point {
        int x = 0;
        int y = 0;
    };

    point plat[NR_PLATFORMS];

    int x = 100;    // location of the doodle
    int y = 100;
    int h = 200;    // used for scrolling of the platforms

    float dx = 0.0f;
    float dy = 0.0f;

    bool bLookingLeft = false;

    bool bGameOver    = false;
    bool bGameStarted = false;

    int nScore = 0;

public:
    bool OnUserCreate() override {

        // initialize your assets here
        srand( time( 0 ));

        // load all necessary sprites
        std::string sPath = "images/";
        sBackground = new olc::Sprite( sPath + "background - new.png" );
        sPlat       = new olc::Sprite( sPath +   "platform - new.png" );
        sPers       = new olc::Sprite( sPath +     "doodle - new.png" );

        // initialize positions for the platforms
        for (int i = 0; i < NR_PLATFORMS; i++) {
            plat[i].x = rand() % (SCREEN_X - sPlat->width );   // use platform size to keep coordinates within window
            plat[i].y = rand() % (SCREEN_Y - sPlat->height);
        }

        return true;
    }

    bool OnUserUpdate( float fElapsedTime ) override {

        if (bGameOver) {
            DrawString( 25, 25, "GAME OVER...", olc::MAGENTA, 2 );
            return true;
        }

        // move doodle left or right
        if (GetKey( olc::Key::RIGHT ).bHeld) { x += 3; bLookingLeft = false; }
        if (GetKey( olc::Key::LEFT  ).bHeld) { x -= 3; bLookingLeft = true;  }

        // make doodle bounce off the ground
        dy += 0.2f;
        y += dy;
        if (y > (ScreenHeight() - sPers->height / 2)) {
            // allow 1 bounce on the ground, and have it trigger game over after that
            if (bGameStarted) {
                bGameOver = true;
            } else {
                bGameStarted = true;
                dy = -10.0f;
            }
        }

        // have the platforms scroll under the jumping doodle, re-init them if
        // the scroll off the bottom of the screen
        if (y < h) {
            nScore += (h - y);
                y = h;
            for (int i = 0; i < NR_PLATFORMS; i++) {
                plat[i].y = plat[i].y - dy;
                if (plat[i].y > SCREEN_Y) {
                    plat[i].y = 0;
                    plat[i].x = rand() % (SCREEN_X - sPlat->width );
                }
            }
        }

        // have the doodle bounce up off the platforms
        for (int i = 0; i < NR_PLATFORMS; i++) {
            if ((x + 50 > plat[i].x) && (x + 20 < plat[i].x + 68) &&
                (y + 70 > plat[i].y) && (y + 70 < plat[i].y + 14) && (dy > 0)) {
                    bGameStarted = true;
                    dy = -10;
                }
        }

        // ==========   draw   ==========

        DrawSprite( 0, 0, sBackground );
        SetPixelMode( olc::Pixel::MASK );
        DrawSprite( x, y, sPers, 1, (bLookingLeft ? olc::Sprite::Flip::HORIZ : olc::Sprite::Flip::NONE));

        for (int i = 0; i < NR_PLATFORMS; i++) {
            DrawSprite( plat[i].x, plat[i].y, sPlat );
        }
        SetPixelMode( olc::Pixel::NORMAL );

        DrawString( 5, 5, "Score: " + std::to_string( nScore ), olc::MAGENTA, 2 );

        return true;
    }
};

int main()
{
	DoodleJump game;
	if (game.Construct( SCREEN_X / PIXEL_X, SCREEN_Y / PIXEL_Y, PIXEL_X, PIXEL_Y, false, true ))
		game.Start();

	return 0;
}
