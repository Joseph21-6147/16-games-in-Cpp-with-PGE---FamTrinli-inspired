/* "Let's make 16 games in C++ - part 9: XONIX" - by FamTrinli
    ==========================================

   Youtube: https://youtu.be/_5W5sYjDBnA?list=PLB_ibvUSN7mzUffhiay5g5GUHyJRO4DYr
   This implementation version is upto 2m02s into the video

   Implementation by Joseph21, october 5, 2022
   -------------------------------------------

   Fase 4 - added enemies

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

#define M    25   // grid size
#define N    40
#define ts   18   // (square) tile size

// keep the screen dimensions constant and vary the resolution by adapting the pixel size
#define SCREEN_X    N * ts
#define SCREEN_Y    M * ts
#define PIXEL_X       1
#define PIXEL_Y       1


// global variables
int grid[M][N] = { 0 };


class XonixGame : public olc::PixelGameEngine {

public:
    XonixGame() {
        sAppName = "Xonix Game";
    }

private:
    struct Enemy {
        int x, y, dx, dy;
        float fAngle = 0.0f; // radians

        Enemy() {
            x = y = 300;
            dx = 4 - rand() % 8;
            dy = 4 - rand() % 8;
        }

        void move() {
            // update coordinates of Enemy object. If a border is hit, invert the direction.
            x += dx; if (grid[y / ts][x / ts] == 1) { dx = -dx; x += dx; }
            y += dy; if (grid[y / ts][x / ts] == 1) { dy = -dy; y += dy; }
        }

        void rotate( float fDegrees ) {
            fAngle += fDegrees * 3.14159265f / 180.0f;
        }
    };

private:
    // put your class variables here
    olc::Sprite *tTile     = nullptr;
    olc::Sprite *tGameOver = nullptr;
    olc::Sprite *tEnemy    = nullptr;

    olc::Decal  *sTile     = nullptr;
    olc::Decal  *sGameover = nullptr;
    olc::Decal  *sEnemy    = nullptr;

    olc::vf2d vPosGameover = { 100.0f, 100.0f };
    olc::vf2d vEnemyOrigin = { 20.0f, 20.0f };

    int enemyCount = 4;
    Enemy a[10];

    int x = 0, y = 0, dx = 0, dy = 0;
    float timer = 0.0f, delay = 0.07f;
    bool GameOver = false;

public:

    void BoardReset() {
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                if (i == 0 || j == 0 || i == M - 1 || j == N - 1)
                    grid[i][j] = 1;
                else
                    grid[i][j] = 0;
            }
        }
    }

    bool OnUserCreate() override {

        // initialize your assets here
        srand( time( 0 ));

        auto load_decal_from_file = [=]( std::string sFileName, olc::Sprite *spritePtr, int &w, int &h ) {
            spritePtr = new olc::Sprite( sFileName );
            olc::Decal *auxDecalPtr = new olc::Decal( spritePtr );
            w = spritePtr->width;
            h = spritePtr->height;
//            delete spritePtr;    // don't delete!! The sprite is needed for the decal to function properly...

            if (w == 0 || h == 0) {
                std::cout << "ERROR: OnUserCreate() --> couldn't load: " << sFileName << std::endl;
                delete spritePtr  ; spritePtr   = nullptr;
                delete auxDecalPtr; auxDecalPtr = nullptr;
            }
            return auxDecalPtr;
        };
        int nNotUsed;
        std::string sPath = "images/";
        sTile     = load_decal_from_file( sPath +    "tiles.png", tTile    , nNotUsed, nNotUsed );
        sGameover = load_decal_from_file( sPath + "gameover.png", tGameOver, nNotUsed, nNotUsed );
        sEnemy    = load_decal_from_file( sPath +    "enemy.png", tEnemy   , nNotUsed, nNotUsed );

        BoardReset();

        // fail start up if decals are not created properly
        return sTile != nullptr && sGameover != nullptr;
    }

    bool OnUserUpdate( float fElapsedTime ) override {

        // your update code per frame here
        timer += fElapsedTime;

        // user controls
        if (GetKey( olc::Key::LEFT  ).bPressed) { dx = -1; dy =  0; }
        if (GetKey( olc::Key::RIGHT ).bPressed) { dx =  1; dy =  0; }
        if (GetKey( olc::Key::UP    ).bPressed) { dx =  0; dy = -1; }
        if (GetKey( olc::Key::DOWN  ).bPressed) { dx =  0; dy =  1; }

        if (GetKey( olc::Key::R    ).bPressed) {   // reset - additional functionality
            BoardReset();
            x = y = dx = dy = 0;
            timer = 0.0f;
            GameOver = false;
        }

        // timing
        if (!GameOver && timer > delay) {
            x += dx;
            y += dy;

            if (x <     0) x =     0;
            if (x > N - 1) x = N - 1;
            if (y <     0) y =     0;
            if (y > M - 1) y = M - 1;

            if (grid[y][x] == 2) GameOver = true;
            if (grid[y][x] == 0) grid[y][x] = 2;

            timer -= delay;
        }

        for (int i = 0; i < enemyCount; i++) a[i].move();

        // ==========   draw   ==========

        Clear( olc::BLACK );

        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                olc::vf2d srcOrigin;
                if (grid[i][j] == 0) continue;
                if (grid[i][j] == 1) srcOrigin = {  0, 0 };
                if (grid[i][j] == 2) srcOrigin = { 54, 0 };
                olc::vf2d dstPosition = { float( j * ts ), float( i * ts ) };
                DrawPartialDecal( dstPosition, sTile, srcOrigin, { ts, ts } );
            }
        }
        DrawPartialDecal(
            olc::vf2d( x * ts, y * ts ),    // positon on screen
            sTile,                          // ptr to decal
            olc::vf2d( 36, 0 ),             // origin of part
            olc::vf2d( ts, ts )             // size of part
        );

        for (int i = 0; i < enemyCount; i++) {
            a[i].rotate( 10 );
            DrawRotatedDecal(
                olc::vf2d( a[i].x, a[i].y ),
                sEnemy,
                a[i].fAngle,
                vEnemyOrigin
            );
        }

        if (GameOver) {
            DrawDecal( vPosGameover, sGameover );
        }

        return true;
    }
};

int main()
{
	XonixGame game;
	if (game.Construct( SCREEN_X / PIXEL_X, SCREEN_Y / PIXEL_Y, PIXEL_X, PIXEL_Y, false, true ))
		game.Start();

	return 0;
}
