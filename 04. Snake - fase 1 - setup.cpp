/* "Let's make 16 games in C++: SNAKE" - by FamTrinli
    =================================

   Youtube: (see: https://youtu.be/OBBrp43TX3A?list=PLB_ibvUSN7mzUffhiay5g5GUHyJRO4DYr)
   This implementation version is upto: 0m37 in the video

   Implementation by Joseph21, october 2, 2022
   -------------------------------------------

   Fase 1 - setup

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

#define N     30      // width of playing field
#define M     20      // height
#define size  16      // (square) cell size in pixels

// keep the screen dimensions constant and vary the resolution by adapting the pixel size
#define SCREEN_X   (size * N)
#define SCREEN_Y   (size * M)
#define PIXEL_X       1
#define PIXEL_Y       1


class Snake : public olc::PixelGameEngine {

public:
    Snake() {
        sAppName = "Snake Game!";
    }

private:
    // ==========   (animated / sprite) Object   ==========

    // in SFML you can pass the position as an attribute to the sprite. I
    // created this struct and typedef to implement that
    struct sObjectRecord {
        olc::Sprite *pTexture = nullptr;
        int x = 0;
        int y = 0;
    };
    typedef struct sObjectRecord Object;

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
        olc::vf2d result = { float( obj.x ), float( obj.y ) };
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

private:
    // put your class variables here
    olc::Sprite *pSprite1 = nullptr;
    olc::Sprite *pSprite2 = nullptr;

    Object object1;
    Object object2;

public:
    bool OnUserCreate() override {

        // load necessary sprites
        std::string sPath = "images/";
        pSprite1 = new olc::Sprite( sPath + "white.png" );
        pSprite2 = new olc::Sprite( sPath +   "red.png" );

        InitObject( object1, pSprite1, 0, 0 );
        InitObject( object2, pSprite2, 0, 0 );

        return true;
    }

    bool OnUserUpdate( float fElapsedTime ) override {

        // ==========   draw   ==========

        Clear( olc::BLACK );

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                // this is an SFML way of doing things :)
                SetPosition( object1, i * size, j * size );
                DrawSprite( object1.x, object1.y, object1.pTexture );
            }
        }

        return true;
    }
};

int main()
{
	Snake game;
	if (game.Construct( SCREEN_X / PIXEL_X, SCREEN_Y / PIXEL_Y, PIXEL_X, PIXEL_Y ))
		game.Start();

	return 0;
}

