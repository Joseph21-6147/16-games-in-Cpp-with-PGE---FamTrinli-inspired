/* "Let's make 16 games in C++ - part 10: BEJEWELED" - by FamTrinli
    ===============================================

   Youtube: https://youtu.be/YNXrFOynalE?list=PLB_ibvUSN7mzUffhiay5g5GUHyJRO4DYr
   This implementation version is upto 1m21s into the video

   Implementation by Joseph21, october 6, 2022
   -------------------------------------------

   Fase 2 - jewel swap and animation

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

#define ts 54     // tile size in background

// the screen dimensions and pixel size
#define SCREEN_X    740
#define SCREEN_Y    480
#define PIXEL_X       1
#define PIXEL_Y       1


class BejeweledGame : public olc::PixelGameEngine {

public:
    BejeweledGame() {
        sAppName = "Match-3 Game";
    }

private:
    // defines the playing board as a 10x10 grid of pieces
    struct piece {
        int x, y;       // screen (pixel) coordinates for this grid piece
        int col, row;   // tile (cell) coordinates
        int kind;       // what kind of piece is it?
        int match;      // ?

        // constructor
        piece() { match = 0; }

    } grid[10][10];

    void swap( piece p1, piece p2 ) {
        std::swap( p1.col, p2.col );
        std::swap( p1.row, p2.row );

        grid[p1.row][p1.col] = p1;   // if you'd use reference parameters, this is not neccessary ig...
        grid[p2.row][p2.col] = p2;
    }

private:
    // pointers to the necessary sprites and decals
    olc::Sprite *sBackground = nullptr;
    olc::Sprite *sGems       = nullptr;
    olc::Decal  *dBackground = nullptr;
    olc::Decal  *dGems       = nullptr;

    // offset of upper left cell in background sprite (in pixels)
    olc::vi2d offset = { 48, 24 };

    int x0, y0, x, y;
    int click = 0;
    olc::vi2d pos;

public:
    bool OnUserCreate() override {

        // seed reandomizer
        srand( time( 0 ));

        // This lambda function loads the sprite from sFileName, and creates a decal from it.
        // The decal pointer is returned, the sprite pointer is passed as parameter.
        // Upon failure, both spritePtr and returned pointer will be nullptr, and an error is given to std::cout
        auto load_decal_from_file = [=]( std::string sFileName, olc::Sprite *spritePtr ) {
            spritePtr = new olc::Sprite( sFileName );
            olc::Decal *auxDecalPtr = new olc::Decal( spritePtr );
            if (spritePtr->width == 0 || spritePtr->height == 0) {
                std::cout << "ERROR: OnUserCreate() --> couldn't load: " << sFileName << std::endl;
                delete spritePtr  ; spritePtr   = nullptr;
                delete auxDecalPtr; auxDecalPtr = nullptr;
            }
            return auxDecalPtr;
        };
        // load all the sprites/decals you need
        std::string sPath = "images/";
        dBackground = load_decal_from_file( sPath + "background.png", sBackground );
        dGems       = load_decal_from_file( sPath +       "gems.png", sGems       );

        // fill the grid cells with random jewels
        for (int i = 1; i <= 8; i++) {
            for (int j = 1; j <= 8; j++) {
                grid[i][j].kind = rand() % 7;    // set kind of jewel at random
                grid[i][j].col = j;              // set column and row in grid
                grid[i][j].row = i;
                grid[i][j].x = j * ts;           // set screen (pixel) coords for this cell
                grid[i][j].y = i * ts;
            }
        }

        return true;
    }

    bool OnUserUpdate( float fElapsedTime ) override {

        // ==========   user controls   ==========

        if (GetMouse( olc::Mouse::LEFT ).bPressed) {
            click += 1;
            pos = GetMousePos() - offset;
        }

        // mouse click
        if (click == 1) {
            x0 = pos.x / ts + 1;
            y0 = pos.y / ts + 1;
        }
        if (click == 2) {
            x = pos.x / ts + 1;
            y = pos.y / ts + 1;
            // check if previously and currently clicked cell are neighbours
            if (abs( x - x0 ) + abs( y - y0 ) == 1) {
                // if they are, swap them
                swap( grid[y0][x0], grid[y][x] );
                click = 0;
            } else {
                // if they're not neighbours, reset click variable
                click = 1;
            }
        }

        // moving animation
        for (int i = 1; i <= 8; i++) {
            for (int j = 1; j <= 8; j++) {
                piece &p = grid[i][j];   // check previous notes for this ref. variable
                // check if the screen coordinate differs from the grid coordinate
                int dx = p.x - p.col * ts;
                int dy = p.y - p.row * ts;
                // if it does, move the swapped pieces 1 pixel per frame until they match again
                if (dx) p.x -= dx / abs( dx );
                if (dy) p.y -= dy / abs( dy );
            }
        }
        // ==========   draw   ==========

        // Clear() is not needed - draw background decal
        DrawDecal( { 0.0f, 0.0f }, dBackground );

        // draw the grid by drawing each of its cells
        for (int i = 1; i <= 8; i++) {
            for (int j = 1; j <= 8; j++) {
                piece p = grid[i][j];
                DrawPartialDecal(
                    olc::vf2d( p.x + offset.x - ts, p.y + offset.y - ts ),
                    dGems,
                    olc::vf2d( p.kind * 49,  0 ),     // source origin
                    olc::vf2d(          49, 49 )      // source size
                );
            }
        }

        return true;
    }
};

int main()
{
	BejeweledGame game;
	if (game.Construct( SCREEN_X / PIXEL_X, SCREEN_Y / PIXEL_Y, PIXEL_X, PIXEL_Y, false, true ))
		game.Start();

	return 0;
}

