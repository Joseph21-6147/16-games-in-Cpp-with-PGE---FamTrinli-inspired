/* "Let's make 16 games in C++ - part 9: XONIX" - by FamTrinli
    ==========================================

   Youtube: https://youtu.be/_5W5sYjDBnA?list=PLB_ibvUSN7mzUffhiay5g5GUHyJRO4DYr
   This implementation version is upto the end of the video - final version

   Implementation by Joseph21, october 5, 2022
   -------------------------------------------

   Fase 5 - game logic completed

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

#define GRID_Y      25   // grid height (in tiles)
#define GRID_X      40   //      width
#define TILE_SIZE   18   // (square) tile size

// the screen dimensions and pixel size
#define SCREEN_X     GRID_X * TILE_SIZE
#define SCREEN_Y     GRID_Y * TILE_SIZE
#define PIXEL_X      1
#define PIXEL_Y      1

// the different types a cell can have
#define CELL_WIP    -1     // used in drop()
#define CELL_EMPTY   0
#define CELL_BLUE    1
#define CELL_GREEN   2


// global variables
int grid[GRID_Y][GRID_X] = { CELL_EMPTY };


class XonixGame : public olc::PixelGameEngine {

public:
    XonixGame() {
        sAppName = "Xonix Game";
    }

private:
    struct Enemy {
        int ex, ey, edx, edy;
        float fAngle = 0.0f; // radians

        Enemy() {
            ex = ey = 300;
            edx = 3 - rand() % 7;   // gives random integer in [-3, +3]
            edy = 3 - rand() % 7;
        }

        // update coordinates of Enemy object. If a blue cell is hit, invert the direction,
        // and do another move to resolve the collision.
        void move() {
            ex += edx; if (grid[ey / TILE_SIZE][ex / TILE_SIZE] == CELL_BLUE) { edx = -edx; ex += edx; }
            ey += edy; if (grid[ey / TILE_SIZE][ex / TILE_SIZE] == CELL_BLUE) { edy = -edy; ey += edy; }
        }

        // add fDegrees to the internal stored fAngle of the enemy
        void rotate( float fDegrees ) {
            fAngle += fDegrees * 3.14159265f / 180.0f;
        }
    };

    // sets all empty cells in an enclosed space to CELL_WIP
    void drop( int cy, int cx ) {
        if (grid[cy][cx] == CELL_EMPTY) grid[cy][cx] = CELL_WIP;

        if (grid[cy - 1][cx    ] == CELL_EMPTY) drop( cy - 1, cx     );
        if (grid[cy + 1][cx    ] == CELL_EMPTY) drop( cy + 1, cx     );
        if (grid[cy    ][cx - 1] == CELL_EMPTY) drop( cy    , cx - 1 );
        if (grid[cy    ][cx + 1] == CELL_EMPTY) drop( cy    , cx + 1 );
    }

private:
    // put your class variables here
    olc::Sprite *sprPtrTile     = nullptr;
    olc::Sprite *sprPtrGameOver = nullptr;
    olc::Sprite *sprPtrEnemy    = nullptr;

    olc::Decal  *decPtrTile     = nullptr;
    olc::Decal  *decPtrGameover = nullptr;
    olc::Decal  *decPtrEnemy    = nullptr;

    olc::vf2d vPosGameover = { 100.0f, 100.0f };    // position on screen for gameover decal
    olc::vf2d vEnemyOrigin = {  20.0f,  20.0f };    // center point for enemy decal

    int enemyCount = 4;
    Enemy a[10];

    int px = 20, py = 0, pdx = 0, pdy = 0;    // player control variables

    float timer = 0.0f, delay = 0.07f;    // game control variables
    bool GameOn = true;

public:
    // Initialises the grid - make all empty except the border cells
    void InitGrid() {
        for (int i = 0; i < GRID_Y; i++) {
            for (int j = 0; j < GRID_X; j++) {
                if (i == 0 || j == 0 || i == GRID_Y - 1 || j == GRID_X - 1)
                    grid[i][j] = CELL_BLUE;
                else
                    grid[i][j] = CELL_EMPTY;
            }
        }
    }

    void InitPlayer() {
        px  = 20;
        py  =  0;
        pdx =  0;
        pdy =  0;
    }

    bool OnUserCreate() override {

        // seed randomizer
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
        decPtrTile     = load_decal_from_file( sPath +    "tiles.png", sprPtrTile     );
        decPtrGameover = load_decal_from_file( sPath + "gameover.png", sprPtrGameOver );
        decPtrEnemy    = load_decal_from_file( sPath +    "enemy.png", sprPtrEnemy    );

        // initialize the grid and player
        InitGrid();
        InitPlayer();

        // fail start up if decals are not created properly
        return (decPtrTile     != nullptr) &&
               (decPtrGameover != nullptr) &&
               (decPtrEnemy    != nullptr);
    }

    bool OnUserUpdate( float fElapsedTime ) override {

        // user controls
        if (GetKey( olc::Key::LEFT   ).bHeld) { pdx = -1; pdy =  0; }
        if (GetKey( olc::Key::RIGHT  ).bHeld) { pdx =  1; pdy =  0; }
        if (GetKey( olc::Key::UP     ).bHeld) { pdx =  0; pdy = -1; }
        if (GetKey( olc::Key::DOWN   ).bHeld) { pdx =  0; pdy =  1; }
        // escape key is reset button
        if (GetKey( olc::Key::ESCAPE ).bPressed) {
            InitGrid();
            InitPlayer();
            GameOn = true;
        }

        // timing
        timer += fElapsedTime;
        if (GameOn && timer > delay) {
            int cache_px = px;  // needed to check if player moved into another cell
            int cache_py = py;

            px += pdx;
            py += pdy;

            if (px <          0) px =          0;  // clamp player position at board boundaries
            if (px > GRID_X - 1) px = GRID_X - 1;
            if (py <          0) py =          0;
            if (py > GRID_Y - 1) py = GRID_Y - 1;

            // if player moved into another cell, check if it was green (if so : game over
            if (cache_px != px || cache_py != py) {
                if (grid[py][px] == CELL_GREEN) GameOn = false;
            }
            // set all cells that player has occupied to green
            if (grid[py][px] == CELL_EMPTY) grid[py][px] = CELL_GREEN;
            // reset timer for next tick
            timer -= delay;
        }

        // let all enemies animate themselves
        for (int i = 0; i < enemyCount; i++) {
            a[i].move();
            a[i].rotate( 10 );
        }

        // if player arrives at a blue cell, stop it's movement, and flush the enclosed space
        // that has no enemies inside
        if (grid[py][px] == CELL_BLUE) {
            pdx = pdy = 0;

            // flush all enclosed spaces that contain enemies with CELL_WIP
            for (int i = 0; i < enemyCount; i++) {
                drop( a[i].ey / TILE_SIZE, a[i].ex / TILE_SIZE );
            }
            // flush all enclosed spaces that contain no enemies with CELL_BLUE (and clear CELL_WIP flushing)
            for (int i = 0; i < GRID_Y; i++) {
                for (int j = 0; j < GRID_X; j++) {
                    if (grid[i][j] == CELL_WIP)
                        grid[i][j] = CELL_EMPTY;
                    else
                        grid[i][j] = CELL_BLUE;
                }
            }
        }

        // if an enemy hits a volatile (green) cell then the game is over...
        for (int i = 0; i < enemyCount; i++) {
            if (grid[a[i].ey / TILE_SIZE][a[i].ex / TILE_SIZE] == CELL_GREEN)
                GameOn = false;
        }

        // ==========   draw   ==========

        Clear( olc::BLACK );

        // draw the grid
        for (int i = 0; i < GRID_Y; i++) {
            for (int j = 0; j < GRID_X; j++) {
                if (grid[i][j] != CELL_EMPTY) {
                    olc::vf2d srcOrigin;
                    switch (grid[i][j]) {
                        case CELL_BLUE : srcOrigin = {  0, 0 };  break; // dark blue tile
                        case CELL_GREEN: srcOrigin = { 54, 0 };  break; // green tile
                    }
                    olc::vf2d dstPosition = { float( j * TILE_SIZE ), float( i * TILE_SIZE ) };
                    DrawPartialDecal( dstPosition, decPtrTile, srcOrigin, { TILE_SIZE, TILE_SIZE } );
                }
            }
        }

        // draw the player
        DrawPartialDecal(
            olc::vf2d( px * TILE_SIZE, py * TILE_SIZE ),    // positon on screen
            decPtrTile,                                     // ptr to decal
            olc::vf2d( 36, 0 ),                             // red tile
            olc::vf2d( TILE_SIZE, TILE_SIZE )               // size of part
        );

        // draw the enemies
        for (int i = 0; i < enemyCount; i++) {
            DrawRotatedDecal(
                olc::vf2d( a[i].ex, a[i].ey ),
                decPtrEnemy,
                a[i].fAngle,
                vEnemyOrigin
            );
        }

        // draw game over message if appropriate
        if (!GameOn) {
            DrawDecal( vPosGameover, decPtrGameover );
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
