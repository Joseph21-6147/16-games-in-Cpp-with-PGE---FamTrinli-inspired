/* "Let's make 16 games in C++: ARKANOID" - by FamTrinli
    ====================================

   Youtube: (see: https://youtu.be/U7vSIvLLZi0?list=PLB_ibvUSN7mzUffhiay5g5GUHyJRO4DYr)
   This implementation version is upto: 0m54s in the video

   Implementation by Joseph21, october 2, 2022
   -------------------------------------------

   Fase 1 - setup and sprite drawing

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


class Arkanoid : public olc::PixelGameEngine {

public:
    Arkanoid() {
        sAppName = "Arkanoid Game!";
    }

private:
    // put your class variables here
    olc::Sprite *sBlockSprite      = nullptr;  // pointers to the necessary sprites
    olc::Sprite *sBackgroundSprite = nullptr;
    olc::Sprite *sBallSprite       = nullptr;
    olc::Sprite *sPaddleSprite     = nullptr;

    // in SFML you can pass the position as an attribute to the sprite. I
    // defined this struct to implement that
    struct sObjectRecord {
        olc::Sprite *pTexture = nullptr;
        int x = 0;
        int y = 0;
    };
    typedef struct sObjectRecord Object;

    Object block[1000];      // 1000 is overkill, but 100 is too little :)
    Object sBall;
    Object sPaddle;

    int n = 0;    // keeps track of the number of blocks in the array

    void InitObject( Object &obj, olc::Sprite *pSpr, int nx, int ny ) {
        obj.pTexture = pSpr;
        obj.x = nx;
        obj.y = ny;
    }

    void MoveObject( Object &obj, int dx, int dy ) {
        obj.x += dx;
        obj.y += dy;
    }

public:
    bool OnUserCreate() override {

        // initialize your assets here

        // load the necessary sprites
        std::string sPath = "images/";
        sBlockSprite      = new olc::Sprite( sPath +    "block01.png" );
        sBackgroundSprite = new olc::Sprite( sPath + "background.png" );
        sBallSprite       = new olc::Sprite( sPath +       "ball.png" );
        sPaddleSprite     = new olc::Sprite( sPath +     "paddle.png" );

        InitObject( sPaddle, sPaddleSprite, 300, 440 );
        InitObject( sBall  , sBallSprite  , 300, 300 );

        // set the positions of all the blocks in the game
        int xOffset = (ScreenWidth()  - 11 * 44 ) * 0.5f;
        int yOffset = (ScreenHeight() - 11 * 22 ) * 0.15f;
        for (int i = 0; i <= 10; i++) {
            for (int j = 0; j <= 10; j++) {
                block[n].pTexture = sBlockSprite;
                block[n].x = xOffset + i * 44;
                block[n].y = yOffset + j * 22;
                n += 1;
            }
        }

        return true;
    }

    bool OnUserUpdate( float fElapsedTime ) override {

        // your update code per frame here

        // Drawing
        Clear( olc::BLACK );     // this call is not needed
        DrawSprite( 0, 0, sBackgroundSprite );
        SetPixelMode( olc::Pixel::Mode::MASK );
        DrawSprite(   sBall.x,   sBall.y,   sBall.pTexture );
        DrawSprite( sPaddle.x, sPaddle.y, sPaddle.pTexture );
        SetPixelMode( olc::Pixel::Mode::NORMAL );

        for (int i = 0; i < n; i++) {
            DrawSprite( block[i].x, block[i].y, block[i].pTexture );
        }
        return true;
    }
};

// keep the screen dimensions constant and vary the resolution by adapting the pixel size
#define SCREEN_X    520
#define SCREEN_Y    450
#define PIXEL_X       1
#define PIXEL_Y       1

int main()
{
	Arkanoid game;
	if (game.Construct( SCREEN_X / PIXEL_X, SCREEN_Y / PIXEL_Y, PIXEL_X, PIXEL_Y, false, true ))
		game.Start();

	return 0;
}

