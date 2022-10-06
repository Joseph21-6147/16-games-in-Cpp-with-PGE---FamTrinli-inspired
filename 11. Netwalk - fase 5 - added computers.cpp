/* "Let's make 16 games in C++ - part 11: NETWALK (Pipe puzzle)" - by FamTrinli
    ===========================================================

   Youtube: https://youtu.be/XajeSjilUDs?list=PLB_ibvUSN7mzUffhiay5g5GUHyJRO4DYr
   This implementation version is upto 2m54s into the video

   Implementation by Joseph21, october 6, 2022
   -------------------------------------------

   Fase 5 - added computers

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

// definition of the DIR array
olc::vi2d Up     = {  0, -1 };
olc::vi2d Down   = {  0, +1 };
olc::vi2d Right  = {  1,  0 };
olc::vi2d Left   = { -1,  0 };
olc::vi2d DIR[4] = { Up, Right, Down, Left };

class NetwalkGame : public olc::PixelGameEngine {

public:
    NetwalkGame() {
        sAppName = "The Pipe Puzzle";
    }

private:
    olc::vf2d offset = { 65.0f, 55.0f };   // offset in the background sprite



    struct pipe {
        std::vector<olc::vi2d> dirs;

        int orientation = 0;     // angle in degrees
        float angle;             // also in degrees, used for smooth rotation animation
        bool on;

        pipe() { angle = 0; }

        void rotate() {
            for (int i = 0; i < (int)dirs.size(); i++) {
                if (dirs[i] == Up   ) dirs[i] = Right; else
                if (dirs[i] == Right) dirs[i] = Down ; else
                if (dirs[i] == Down ) dirs[i] = Left ; else
                if (dirs[i] == Left ) dirs[i] = Up   ;
            }
        }

        bool isConnect( olc::vi2d dir ) {

            for (auto d : dirs) {
                if (d == dir)
                    return true;
            }
            return false;
        }

    } grid[N][N];

    olc::Sprite *sBackground = nullptr;   // pointers for the sprites & decals needed
    olc::Sprite *sPipe       = nullptr;
    olc::Sprite *sComputer   = nullptr;
    olc::Decal  *dBackground = nullptr;
    olc::Decal  *dPipe       = nullptr;
    olc::Decal  *dComputer   = nullptr;

    olc::vf2d vPipeCenter = { 27, 27 };   // pipes are rotated, center point is needed

public:

    pipe &cell( olc::vi2d v ) { return grid[v.x][v.y]; }

    bool isOut( olc::vi2d v ) {
        return (v.x < 0 || v.x >= N ||
                v.y < 0 || v.y >= N);
    }

    void generatePuzzle() {

        std::vector<olc::vi2d> nodes;
        nodes.push_back( olc::vi2d( rand() % N, rand() % N ));

        while (!nodes.empty()) {
            int n = rand() % nodes.size();  // select a random node from the WIP list
            olc::vi2d v = nodes[n];
            olc::vi2d d = DIR[rand() % 4];  // select a random direction

            if (cell( v ).dirs.size() == 3) {
                nodes.erase( nodes.begin() + n );
                continue;
            }

            bool complete = true;
            for (auto D : DIR) {
                if (!isOut( v + D ) && cell( v + D ).dirs.empty())
                    complete = false;
            }
            if (complete) {
                nodes.erase( nodes.begin() + n );
                continue;
            }

            if (isOut( v + d )) continue;
            if (!cell( v + d ).dirs.empty()) continue;
            cell( v ).dirs.push_back( d );
            cell( v + d ).dirs.push_back( - d );
            nodes.push_back( v + d );
        }
    }

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

        generatePuzzle();

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                pipe &p = grid[j][i];

                for (int n = 4; n > 0; n--) {     // find orientation
                    std::string s = "";
                    for (auto d : DIR) {
                        s += p.isConnect( d ) ? '1': '0';
                    }
                    if (s == "0011" || s == "0111" || s == "0101" || s == "0010") p.orientation = n;
                    p.rotate();
                }
            }
        }
        return true;
    }

    bool OnUserUpdate( float fElapsedTime ) override {

        // user controls
        if (GetMouse( olc::Mouse::LEFT ).bPressed) {
            // if left mouse button clicked - rotate the tile under the mouse cursor 1 quarter (90 degrees)
            olc::vi2d pos = GetMousePos() + olc::vi2d( ts / 2, ts / 2 ) - offset;
            pos /= ts;
            grid[pos.x][pos.y].orientation++;

        }

        // draw

        Clear( olc::BLACK );
        DrawDecal( { 0, 0 }, dBackground );

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                pipe &p = grid[j][i];

                int kind = p.dirs.size();
                if (kind == 2 && p.dirs[0] == -p.dirs[1]) kind = 0;

                p.angle += 100.0f * fElapsedTime;

                if (p.angle > p.orientation * 90) p.angle = p.orientation * 90;

                DrawPartialRotatedDecal(
                    olc::vf2d( j * ts, i * ts ) + offset,         // dst position
                    dPipe,
                    p.angle * 3.14159265f / 180.0f,               // angle - translate from deg to rad
                    vPipeCenter,                                  // center point
                    { ts * float(kind), 0 },                      // src position
                    { ts, ts }                                    // src size
                );

                if (kind == 1) {
                    olc::vf2d vCompPos;
                    if (p.on)
                        vCompPos = { 53, 0 };
                    else
                        vCompPos = { 0, 0 };
                    olc::vf2d vCompSize = { 36, 36 };
                    DrawPartialDecal(
                        olc::vf2d( j * ts, i * ts ) + offset - olc::vf2d( 18, 18 ),
                        dComputer,
                        vCompPos,
                        vCompSize
                    );
                }
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

