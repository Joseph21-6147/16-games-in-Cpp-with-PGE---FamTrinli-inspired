/* "Let's make 16 games in C++ - part 7: CAR RACING (top down)" - by FamTrinli
    ==========================================================

   Youtube: https://youtu.be/YzhhVHb0WVY?list=PLB_ibvUSN7mzUffhiay5g5GUHyJRO4DYr
   This implementation version is upto: 2m04s in the video

   Implementation by Joseph21, october 3, 2022
   -------------------------------------------

   Fase 4 - multiple cars

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
        int cntrX = 0;    // center point
        int cntrY = 0;
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

#define NR_CARS   5   // nr of cars

    struct Car {
        float x, y, speed, angle;
        Car() { speed = 2; angle = 0; }
        void move() {
            x += sin( angle ) * speed;
            y -= cos( angle ) * speed;
        }
    } car[NR_CARS];

    // put your class variables here
    olc::Sprite *pBackgroundSprite = nullptr;
    olc::Sprite *pCarSprite = nullptr;
    olc::Decal  *pCarDecal  = nullptr;

    Object sCar, sBackground;

    float x = 300.0f, y = 300.0f;
    float speed = 0.0f, angle = 0.0f;
    float maxSpeed = 12.0f;
    float acc = 0.2f, dec = 0.3f;
    float turnSpeed = 0.08f;

    int offsetX = 0, offsetY = 0;

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
        sCar.cntrX = 22;    // equivalent of setOrigin in SFML
        sCar.cntrY = 22;

        InitObject( sBackground, pBackgroundSprite, 0, 0 );

        for (int i = 0; i < NR_CARS; i++) {
            car[i].x = 300 + i * 50;
            car[i].y = 1700 + i * 80;
            car[i].speed = 7 + i;
        }

        return true;
    }

    bool OnUserUpdate( float fElapsedTime ) override {

        // user controls - be sensitive to arrow keys
        bool Up    = GetKey( olc::Key::UP    ).bHeld;
        bool Right = GetKey( olc::Key::RIGHT ).bHeld;
        bool Down  = GetKey( olc::Key::DOWN  ).bHeld;
        bool Left  = GetKey( olc::Key::LEFT  ).bHeld;

        // car movement
        if (Up && speed < maxSpeed) {
            if (speed < 0)
                speed += dec;
            else
                speed += acc;
        }

        if (Down && speed > -maxSpeed) {
            if (speed > 0)
                speed -= dec;
            else
                speed -= acc;
        }

        if (!Up && !Down) {
            if (speed - dec > 0)
                speed -= dec;
            else if (speed + dec < 0)
                speed += dec;
            else
                speed = 0;
        }

        if (Right && speed != 0) angle += turnSpeed * speed / maxSpeed;
        if (Left  && speed != 0) angle -= turnSpeed * speed / maxSpeed;

        car[0].speed = speed;
        car[0].angle = angle;

        for (int i = 0; i < NR_CARS; i++) car[i].move();

        if (car[0].x > 320) offsetX = car[0].x - 320;
        if (car[0].y > 240) offsetY = car[0].y - 240;

        // Draw
        Clear( olc::WHITE );
        SetPosition( sBackground, -offsetX, -offsetY );
        DrawSprite( sBackground.x, sBackground.y, sBackground.pTexture );

        olc::Pixel colors[10] = { olc::RED, olc::GREEN, olc::MAGENTA, olc::BLUE, olc::WHITE };

        for (int i = 0; i < NR_CARS; i++) {
            SetPosition( sCar, car[i].x - offsetX, car[i].y - offsetY );
            // using decal drawing, you can rotate the car easily and tint it any colour
            DrawRotatedDecal(
                olc::vf2d( sCar.x, sCar.y ),
                pCarDecal,
                car[i].angle,
                olc::vf2d( sCar.cntrX, sCar.cntrY ),
                olc::vf2d( 1.0f, 1.0f ),
                colors[i]
            );
        }

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

