/* "Let's make 16 games in C++ - part 7: CAR RACING (top down)" - by FamTrinli
    ==========================================================

   Youtube: https://youtu.be/YzhhVHb0WVY?list=PLB_ibvUSN7mzUffhiay5g5GUHyJRO4DYr
   This implementation version is upto: 0m22s in the video

   Implementation by Joseph21, october 3, 2022
   -------------------------------------------

   Fase 1 - setup and drawing

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
#define SCREEN_X    640
#define SCREEN_Y    480
#define PIXEL_X       1
#define PIXEL_Y       1


class TopDownCarRacing : public olc::PixelGameEngine {

public:
    TopDownCarRacing() {
        sAppName = "TopDownCarRacing";
    }

private:
    // ==========   (animated / sprite) Object   ==========

    // in SFML you can pass the position as an attribute to the sprite. I
    // created this struct and typedef to implement that
    struct sObjectRecord {
        olc::Sprite *pTexture = nullptr;
        olc::Decal  *pDecal   = nullptr;
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
    olc::Sprite *pBackgroundSprite = nullptr;
    olc::Sprite *pCarSprite = nullptr;
    olc::Decal  *pCarDecal  = nullptr;

    Object sCar;

public:
    bool OnUserCreate() override {

        // load sprites ...
        std::string sPath = "images/";
        pCarSprite        = new olc::Sprite( sPath +        "car.png" );
        pBackgroundSprite = new olc::Sprite( sPath + "background.png" );
        // ... and create object from it
        InitObject( sCar, pCarSprite, 320, 240 );
        pCarDecal = new olc::Decal( pCarSprite );
        // including decal and center point (for rotation)
        sCar.pDecal = pCarDecal;

        return true;
    }

    bool OnUserUpdate( float fElapsedTime ) override {

        // Draw
        Clear( olc::WHITE );
        DrawSprite( 0, 0, pBackgroundSprite );

        // using decal drawing, you can rotate the car easily and tint it any colour
        DrawDecal(
            olc::vf2d( sCar.x, sCar.y ),
            pCarDecal,
            olc::vf2d( 1.0f, 1.0f ),
            olc::RED
        );

        return true;
    }
};

int main()
{
	TopDownCarRacing game;
	if (game.Construct( SCREEN_X / PIXEL_X, SCREEN_Y / PIXEL_Y, PIXEL_X, PIXEL_Y, false, true ))
		game.Start();

	return 0;
}

