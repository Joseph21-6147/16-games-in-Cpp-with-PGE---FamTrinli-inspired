/* "Let's make 16 games in C++ - part 11: NETWALK (Pipe puzzle)" - by FamTrinli
    ===========================================================

   Youtube: https://youtu.be/XajeSjilUDs?list=PLB_ibvUSN7mzUffhiay5g5GUHyJRO4DYr
   This implementation version is upto 0m45s into the video

   Implementation by Joseph21, october 6, 2022
   -------------------------------------------

   Fase 1 - setup and simple rendering

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


#define N             6  // (square) grid dimensions
#define ts           54  // (square) tile size

// screen dimensions and pixel size
#define SCREEN_X    390
#define SCREEN_Y    390
#define PIXEL_X       1
#define PIXEL_Y       1


class NetwalkGame : public olc::PixelGameEngine {

public:
    NetwalkGame() {
        sAppName = "The Pipe Puzzle";
    }

private:
    olc::vf2d offset = { 65.0f, 55.0f };   // offset in the background sprite

    struct pipe {
        int orientation = 0;     // angle in degrees
        bool on;

    } grid[N][N];

    olc::Sprite *sBackground = nullptr;   // pointers for the sprites & decals needed
    olc::Sprite *sPipe       = nullptr;
    olc::Sprite *sComputer   = nullptr;
    olc::Decal  *dBackground = nullptr;
    olc::Decal  *dPipe       = nullptr;
    olc::Decal  *dComputer   = nullptr;

    olc::vf2d vPipeCenter = { 27, 27 };   // pipes are rotated, center point is needed

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
        std::string sPath = "images/";
        dBackground = load_decal_from_file( sPath + "background.png", sBackground );
        dPipe       = load_decal_from_file( sPath +      "pipes.png", sPipe       );
        // for future use
        dComputer   = load_decal_from_file( sPath +       "comp.png", sComputer   );

        return true;
    }

    bool OnUserUpdate( float fElapsedTime ) override {

        // draw

        Clear( olc::BLACK );
        DrawDecal( { 0, 0 }, dBackground );

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                pipe &p = grid[j][i];
                DrawPartialRotatedDecal(
                    olc::vf2d( j * ts, i * ts ) + offset,         // dst position
                    dPipe,
                    p.orientation * 3.14159265f / 180.0f,         // angle - translate from deg to rad
                    vPipeCenter,                                  // center point
                    { ts * 2, 0 },                                // src position
                    { ts, ts }                                    // src size
                );
            }
        }

        return true;
    }

    bool OnUserDestroy() override {

        delete dBackground; dBackground = nullptr;
        delete sBackground; sBackground = nullptr;
        delete dPipe;       dPipe       = nullptr;
        delete sPipe;       sPipe       = nullptr;
        delete dComputer;   dComputer   = nullptr;
        delete sComputer;   sComputer   = nullptr;

        return true;
    }
};

int main()
{
	NetwalkGame game;
	if (game.Construct( SCREEN_X / PIXEL_X, SCREEN_Y / PIXEL_Y, PIXEL_X, PIXEL_Y ))
		game.Start();

	return 0;
}

