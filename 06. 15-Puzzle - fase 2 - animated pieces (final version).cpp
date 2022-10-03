/* "Let's make 16 games in C++: 15-PUZZLE" - by FamTrinli
    =====================================

   Youtube: (see: https://youtu.be/NJZ2K6HPpl4?list=PLB_ibvUSN7mzUffhiay5g5GUHyJRO4DYr)
   This implementation version is upto: 1m04s in the video - the final version

   Implementation by Joseph21, october 3, 2022
   -------------------------------------------

   Fase 2 - piece animation

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
#define SCREEN_X    256
#define SCREEN_Y    256
#define PIXEL_X       1
#define PIXEL_Y       1


class ShiftingPuzzle : public olc::PixelGameEngine {

public:
    ShiftingPuzzle() {
        sAppName = "15-Puzzle!";
    }

private:
    // ==========   (animated / sprite) Object   ==========

    // in SFML you can pass the position as an attribute to the sprite. I
    // created this struct and typedef to implement that
    struct sObjectRecord {
        olc::Sprite *pTexture = nullptr;
        int x = 0;
        int y = 0;
        bool bMoving = false;
        int newX = 0;
        int newY = 0;
        int nSpeed = 0;
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

    void AnimateObject( Object &obj ) {
        if (obj.bMoving) {
            int dx = 0, dy = 0;
            int diffX = obj.newX - obj.x;
            int diffY = obj.newY - obj.y;
            if (obj.newX > obj.x) dx = +1;
            if (obj.newX < obj.x) dx = -1;
            if (obj.newY > obj.y) dy = +1;
            if (obj.newY < obj.y) dy = -1;

            obj.x += dx * obj.nSpeed;
            obj.y += dy * obj.nSpeed;
            if (diffX == 0 && diffY == 0) {
                // animation is done
                obj.bMoving = false;
            }
        }
    }

    olc::vf2d GetPosition( Object &obj ) {
        olc::vf2d result = { float( obj.x ), float( obj.y ) };
        return result;
    }

    void SetPosition( Object &obj, int nx, int ny ) {
        obj.x = nx;
        obj.y = ny;
    }

    void SetAnimation( Object &obj, int targetX, int targetY, int nSpeed ) {
        obj.bMoving = true;
        obj.newX = targetX;
        obj.newY = targetY;
        obj.nSpeed = nSpeed;
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
    // the sprite sheet with all the 15 elements in it (and the empty cell)
    olc::Sprite *t = nullptr;

    int w = 64; // tile size?
    int grid[6][6] = { 0 };
//    olc::Sprite *sprite[20] = { nullptr };  // contains pointers to the partial sprites
    Object sprite[20];  // contains pointers to the partial sprites

public:
    bool OnUserCreate() override {

        // load the sprite sheet
        t = new olc::Sprite( "images/15.png" );

        // initialize the grid and the sprite array
        int n = 0;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                n++;
                sprite[n].pTexture = t->Duplicate( olc::vi2d( i * w, j * w ), olc::vi2d( w, w ));
                sprite[n].x = i * w;
                sprite[n].y = j * w;

                sprite[n].newX = sprite[n].x;
                sprite[n].newY = sprite[n].y;
                grid[i + 1][j + 1] = n;
            }
        }

        return true;
    }

    bool OnUserUpdate( float fElapsedTime ) override {

        // be sensitive to left mouse click
        if (GetMouse( 0 ).bPressed) {
            olc::vi2d pos = GetMousePos();
            int x = pos.x / w + 1;
            int y = pos.y / w + 1;

            int dx = 0;
            int dy = 0;

            // determine which way to move the cell that's clicked
            if (grid[x + 1][y    ] == 16) { dx =  1; dy =  0; }
            if (grid[x    ][y + 1] == 16) { dx =  0; dy =  1; }
            if (grid[x    ][y - 1] == 16) { dx =  0; dy = -1; }
            if (grid[x - 1][y    ] == 16) { dx = -1; dy =  0; }

            // swap the empty cell and the clicked cell
            int nSwap = grid[x][y];
            grid[x     ][y     ] = 16;
            grid[x + dx][y + dy] = nSwap;

            // animation
            SetAnimation( sprite[nSwap], (x - 1 + dx) * w, (y - 1 + dy) * w, 2 );
        }

        // draw
        Clear( olc::WHITE );
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                int n = grid[i + 1][j + 1];
                AnimateObject( sprite[n] );
//                DrawSprite( i * w, j * w, sprite[n].pTexture );
                if (grid[i + 1][j + 1] != 16)
                    DrawSprite( sprite[n].x, sprite[n].y, sprite[n].pTexture );
            }
        }

        return true;
    }
};

int main()
{
	ShiftingPuzzle game;
	if (game.Construct( SCREEN_X / PIXEL_X, SCREEN_Y / PIXEL_Y, PIXEL_X, PIXEL_Y, false, true ))
		game.Start();

	return 0;
}

