/* "Let's make 16 games in C++ - part 12: MAHJONG SOLITAIRE" - by FamTrinli
    =======================================================

   Youtube: https://youtu.be/-CvqYl_w8Vk?list=PLB_ibvUSN7mzUffhiay5g5GUHyJRO4DYr
   This implementation version is upto the end of the video (final version)

   Implementation by Joseph21, october 6, 2022
   -------------------------------------------

   Fase 4 - special rules for seasons and flowers

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

#include "vector_types.h"   // for the vi3d vector type (it's not included in the PGE...)

// screen dimensions and pixel size
#define SCREEN_X    740
#define SCREEN_Y    570
#define PIXEL_X       1
#define PIXEL_Y       1


int field[50][50][50] = { 0 };    // playing field (but why so big...?)

// This function returns a reference to a specific cell in the field addressed by its
// parameters. But whence the y + 2 and the x + 2? And why are x and y swapped in order?
int &f( int x, int y, int z ) { return field[y + 2][x + 2][z]; }
// This is an overload of the previous function
int &f( vi3d v ) { return f( v.x, v.y, v.z ); }

// returns true if the mahjong piece at (x, y, z) is available for selection (i.e. not blocked)
bool isOpen( int x, int y, int z ) {
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            // if it is surrounded at both horizontal sides, it's not open
            if (f( x + 2, y + i, z ) > 0 && f( x - 2, y + j, z ) > 0) return false;
        }
    }
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            // if there's a piece on top of it, it's not open
            if (f( x + i, y + j, z + 1 ) > 0) return false;
        }
    }
    return true;
}


class MahjongGame : public olc::PixelGameEngine {

public:
    MahjongGame() {
        sAppName = "Mahjong Solitaire";
    }

private:
    // pointers to sprites and decals
    olc::Sprite *sTiles      = nullptr;
    olc::Sprite *sBackground = nullptr;
    olc::Decal  *dTiles      = nullptr;
    olc::Decal  *dBackground = nullptr;

    const int w = 48;   // tile size of mahjong pieces (in pixels)
    const int h = 66;

    int stepX = w / 2 - 2;
    int stepY = h / 2 - 2;

    float offX = 4.6f, offY = 7.1f;   // z offset
    vi3d v1, v2;

public:
    bool OnUserCreate() override {

        // initialize your assets here

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
        std::string sPath = "files/";
        dBackground = load_decal_from_file( sPath + "background.png", sBackground );
        dTiles      = load_decal_from_file( sPath +      "tiles.png", sTiles      );

        // load from file
        std::fstream myfile( "files/map.txt" );
        for (int y = 0; y < 18; y++) {
            for (int x = 0; x < 30; x++) {
                char a;
                myfile >> a;
                int n = a - '0';  // convert ascii digit to numeric with same value
                for (int z = 0; z < n; z++) {
                    if (f( x - 1, y - 1, z ))
                        f( x - 1, y, z ) = f( x, y - 1, z ) = 0;
                    else
                        f( x, y, z ) = 1;
                }
            }
        }

        // shuffle
        for (int k = 1;; k++) {
            std::vector<vi3d> opens;
            for (int z = 0; z < 10; z++) {
                for (int y = 0; y < 18; y++) {
                    for (int x = 0; x < 30; x++) {
                        if (f( x, y, z ) > 0 && isOpen( x, y, z )) opens.push_back( vi3d( x, y, z ));
                    }
                }
            }
            int n = opens.size();
            if (n < 2) break;
            int a = 0, b = 0;
            while (a == b) { a = rand() % n; b = rand() % n; }
            f( opens[a] ) = -k; if (k > 34) k++;
            f( opens[b] ) = -k;
            k %= 42;
        }
        for (int z = 0; z < 10; z++) {
            for (int y = 0; y < 18; y++) {
                for (int x = 0; x < 30; x++) {
                    f( x, y, z ) *= -1;
                }
            }
        }
        return true;
    }

    bool OnUserUpdate( float fElapsedTime ) override {

        // user controls
        if (GetMouse( olc::Mouse::LEFT ).bPressed) {
            for (int z = 0; z < 10; z++) {
                // grab mouse cursor position
                olc::vi2d pos = GetMousePos();
                int x = (pos.x - z * offX) / stepX;   // translate from screen to board coordinates
                int y = (pos.y + z * offY) / stepY;

                for (int i = 0; i < 2; i++) {
                    for (int j = 0; j < 2; j++) {
                        if (f( x - i, y - j, z ) > 0 && isOpen( x - i, y - j, z ))
                            v1 = vi3d( x - i, y - j, z );
                    }
                }
                if (v1 == v2) continue;

                int a = f( v1 ), b = f( v2 );
                                // range 35-38 seasons, range 39-42 flowers
                if (a == b || (a > 34 && a < 39 && b > 34 && b < 39) || (a >= 39 && b >= 39)) {
                    f( v1 ) *= -1;
                    f( v2 ) *= -1;
                }
                v2 = v1;
            }
        }

        // clear screen and draw background decal
        Clear( olc::BLACK );
        DrawDecal( { 0.0f, 0.0f }, dBackground );

        // draw the board with all the pieces
        for (int z = 0; z < 10; z++) {
            for (int x = 30; x >= 0; x--) {
                for (int y = 0; y < 18; y++) {
                    int k = f( x, y, z ) - 1;
                    if (k < 0) continue;

                    DrawPartialDecal(
                        olc::vf2d( x * stepX + z * offX, y * stepY - z * offY ),
                        dTiles,
                        olc::vf2d( k * w, (isOpen( x, y, z ) ? h : 0 )),  // draw bright version if open
                        olc::vf2d( w, h )
                    );
                }
            }
        }

        return true;
    }

    bool OnUserDestroy() override {

        // your cleanup code here
        delete dBackground; dBackground = nullptr;
        delete sBackground; sBackground = nullptr;
        delete dTiles     ; dTiles      = nullptr;
        delete sTiles     ; sTiles      = nullptr;

        return true;
    }
};

int main()
{
	MahjongGame game;
	if (game.Construct( SCREEN_X / PIXEL_X, SCREEN_Y / PIXEL_Y, PIXEL_X, PIXEL_Y ))
		game.Start();

	return 0;
}

