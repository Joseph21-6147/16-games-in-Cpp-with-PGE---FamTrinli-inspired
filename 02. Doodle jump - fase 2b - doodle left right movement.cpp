/* "Let's make 16 games in C++: DOODLE JUMP" - by FamTrinli
    =======================================

   Youtube: (see: https://youtu.be/7Vf_vQIUk5Q?list=PLB_ibvUSN7mzUffhiay5g5GUHyJRO4DYr)
   This implementation version is upto: 1m06s in the video

   Implementation by Joseph21, oktober 1, 2022
   -------------------------------------------

   Fase 2b - doodle horizontal movement

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

    point plat[20];

    int x = 100, y = 100, h = 200;
    float dx = 0.0f, dy = 0.0f;

public:
    bool OnUserCreate() override {

        // initialize your assets here
        srand( time( 0 ));

        sBackground = new olc::Sprite( "images/background.png" );
        sPlat       = new olc::Sprite( "images/platform.png" );
        sPers       = new olc::Sprite( "images/doodle.png" );

        for (int i = 0; i < 10; i++) {
            plat[i].x = rand() % SCREEN_X;   // use platform size to keep coordinates within window
            plat[i].y = rand() % SCREEN_Y;
        }
        return true;
    }

    bool OnUserUpdate( float fElapsedTime ) override {

        // your update code per frame here

        if (GetKey( olc::Key::RIGHT ).bHeld) x += 3;
        if (GetKey( olc::Key::LEFT  ).bHeld) x -= 3;

        // make doodle bounce
        dy += 0.2f;
        y += dy;
        if (y > 500.0f) dy = -10.0f;    // use screen size icw doodle size iso 500.0f

        DrawSprite( 0, 0, sBackground );
        SetPixelMode( olc::Pixel::MASK );
        DrawSprite( x, y, sPers );
        for (int i = 0; i < 10; i++) {
            DrawSprite( plat[i].x, plat[i].y, sPlat );
        }
        SetPixelMode( olc::Pixel::NORMAL );

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
