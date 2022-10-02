/* "Let's make 16 games in C++: SNAKE" - by FamTrinli
    =================================

   Youtube: (see: https://youtu.be/OBBrp43TX3A?list=PLB_ibvUSN7mzUffhiay5g5GUHyJRO4DYr)
   This implementation version is upto the end of the video - the final version

   Implementation by Joseph21, october 2, 2022
   -------------------------------------------

   Fase 4 - wrap boundaries & tail cutoff

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

    int dir, num = 4;

    struct sSnakeRec {
        int x = 0, y = 0;
    } s[100];

    struct Fruct {
        int x = 0, y = 0;
    } f;

    void Tick() {
        // advance all body segments of the snake
        for (int i = num; i > 0; --i) {
            s[i].x = s[i - 1].x;
            s[i].y = s[i - 1].y;
        }
        // advance the head of the snake
        if (dir == 0) s[0].y += 1;
        if (dir == 1) s[0].x -= 1;
        if (dir == 2) s[0].x += 1;
        if (dir == 3) s[0].y -= 1;
        // check if fruit is eaten
        if ((s[0].x == f.x) && (s[0].y == f.y)) {
            num += 1;
            f.x = rand() % N;
            f.y = rand() % M;
        }
        // wrap snake around playing field borders
        if (s[0].x > N) s[0].x = 0; if (s[0].x < 0) s[0].x = N;
        if (s[0].y > M) s[0].y = 0; if (s[0].y < 0) s[0].y = M;
        // cut of tail piece if snake hits itself
        for (int i = 1; i < num; i++) {
            if (s[0].x == s[i].x && s[0].y == s[i].y) num = i;
        }
    }

    float fTimer = 0.0f, fDelay = 0.2f;

public:
    bool OnUserCreate() override {

        srand( time( 0 ));

        // load necessary sprites
        std::string sPath = "images/";
        pSprite1 = new olc::Sprite( sPath + "white.png" );
        pSprite2 = new olc::Sprite( sPath +   "red.png" );

        // provide some initial values
        InitObject( object1, pSprite1, 0, 0 );
        InitObject( object2, pSprite2, 0, 0 );
        dir = 0;
        for (int i = 0; i <= num; i++) {
            s[num - i].x = 0;
            s[num - i].y = i;
        }
        f.x = 10;
        f.y = 10;

        return true;
    }

    bool OnUserUpdate( float fElapsedTime ) override {

        // ==========   user controls   ==========

        if (GetKey( olc::Key::LEFT  ).bPressed) dir = 1;
        if (GetKey( olc::Key::RIGHT ).bPressed) dir = 2;
        if (GetKey( olc::Key::UP    ).bPressed) dir = 3;
        if (GetKey( olc::Key::DOWN  ).bPressed) dir = 0;

        // ==========   timing   ==========

        fTimer += fElapsedTime;
        if (fTimer > fDelay) {
            fTimer -= fDelay;
            Tick();
        }

        // ==========   draw   ==========

        Clear( olc::BLACK );

        // draw playing field
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                // this is an SFML way of doing things :)
                SetPosition( object1, i * size, j * size );
                DrawSprite( object1.x, object1.y, object1.pTexture );
            }
        }

        // draw snake
        for (int i = 0; i < num; i++) {
            SetPosition( object2, s[i].x * size, s[i].y * size );
            DrawSprite( object2.x, object2.y, object2.pTexture );
        }

        // draw fruit
        DrawSprite( f.x * size, f.y * size, pSprite2 );   // this should be another colour ...

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
