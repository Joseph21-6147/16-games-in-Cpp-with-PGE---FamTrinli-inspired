/* "Let's make 16 games in C++: ARKANOID" - by FamTrinli
    ====================================

   Youtube: (see: https://youtu.be/U7vSIvLLZi0?list=PLB_ibvUSN7mzUffhiay5g5GUHyJRO4DYr)
   This implementation version is upto: 2m00s in the video

   Implementation by Joseph21, october 2, 2022
   -------------------------------------------

   Fase 4 - ball bouncing off paddle & paddle movements

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
    olc::Sprite *sBlockSprite[5]   = { nullptr };  // pointers to the necessary sprites
    olc::Sprite *sBackgroundSprite = nullptr;
    olc::Sprite *sBallSprite       = nullptr;
    olc::Sprite *sPaddleSprite     = nullptr;

    // in SFML you can pass the position as an attribute to the sprite. I
    // created this struct and typedef to implement that
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

    float dx = 5.0f, dy = 4.0f;

    void InitObject( Object &obj, olc::Sprite *pSpr, int nx, int ny ) {
        obj.pTexture = pSpr;
        obj.x = nx;
        obj.y = ny;
    }

    void MoveObject( Object &obj, int dx, int dy ) {
        obj.x += dx;
        obj.y += dy;
    }

    olc::vf2d GetPosition( Object &obj ) {
        olc::vf2d result = { (float)obj.x, (float)obj.y };
        return result;
    }

    void SetPosition( Object &obj, int nx, int ny ) {
        obj.x = nx;
        obj.y = ny;
    }

    bool IsCollide( Object &obj1, Object &obj2 ) {
        int x1_left = obj1.x; int x1_rght = obj1.x + obj1.pTexture->width;
        int x2_left = obj2.x; int x2_rght = obj2.x + obj2.pTexture->width;
        int y1_top  = obj1.y; int y1_bot  = obj1.y + obj1.pTexture->height;
        int y2_top  = obj2.y; int y2_bot  = obj2.y + obj2.pTexture->height;

        return  !(x1_rght < x2_left || x2_rght < x1_left ||
                  y1_bot  < y2_top  || y2_bot  < y1_top);
    }

public:
    bool OnUserCreate() override {

        srand( time( 0 ));

        // load the necessary sprites
        std::string sPath = "images/";
        sBlockSprite[0]   = new olc::Sprite( sPath +    "block01.png" );
        sBlockSprite[1]   = new olc::Sprite( sPath +    "block02.png" );
        sBlockSprite[2]   = new olc::Sprite( sPath +    "block03.png" );
        sBlockSprite[3]   = new olc::Sprite( sPath +    "block04.png" );
        sBlockSprite[4]   = new olc::Sprite( sPath +    "block05.png" );

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
                olc::Sprite *aux;
                if (j == 0 || j == 10) aux = sBlockSprite[4]; else
                if (i == 0 || i == 10) aux = sBlockSprite[4]; else
                if (j == 1 || j ==  9) aux = sBlockSprite[3]; else
                if (i == 1 || i ==  9) aux = sBlockSprite[3]; else
                if (j == 2 || j ==  8) aux = sBlockSprite[2]; else
                if (i == 2 || i ==  8) aux = sBlockSprite[2]; else
                if (j == 3 || j ==  7) aux = sBlockSprite[1]; else
                if (i == 3 || i ==  7) aux = sBlockSprite[1]; else
                aux = sBlockSprite[0];

                InitObject( block[n], aux, xOffset + i * 44, yOffset + j * 22 );
                n += 1;
            }
        }
        return true;
    }

    bool OnUserUpdate( float fElapsedTime ) override {

        // split up movement in pure horizontal and vertical component to handle collisions with blocks
        MoveObject( sBall, dx, 0 );
        for (int i = 0; i < n; i++) {
            // if a collision occurs, remove the block from the visible world, and bounce
            if (IsCollide( sBall, block[i] )) {
                SetPosition( block[i], -100, 0 );
                dx = -dx;
            }
        }
        MoveObject( sBall, 0, dy );
        for (int i = 0; i < n; i++) {
            if (IsCollide( sBall, block[i] )) {
                SetPosition( block[i], -100, 0 );
                dy = -dy;
            }
        }

        olc::vf2d b = GetPosition( sBall );
        // bounce off the borders of the window
        if (b.x < 0 || b.x > 520) dx = -dx;
        if (b.y < 0 || b.y > 450) dy = -dy;

        if (GetKey( olc::Key::RIGHT).bHeld) MoveObject( sPaddle,  8, 0 );
        if (GetKey( olc::Key::LEFT ).bHeld) MoveObject( sPaddle, -8, 0 );

        // bounce off the paddle with a slight change in angle
        if (IsCollide( sPaddle, sBall )) dy = -(rand() % 5 + 2);


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

