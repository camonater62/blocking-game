#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "olcSoundWaveEngine.h"

using namespace std;

class Game : public olc::PixelGameEngine {
public:
    Game() {
        sAppName = "Undertale thingy";
    }

private:

public:
    bool OnUserCreate() override {

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
        if (GetKey(olc::ESCAPE).bPressed) {
            return false;
        }

        return true;
    }

    bool OnUserDestroy() override {

        return true;
    }
};

int main() {
    Game game;
    if (game.Construct(640, 480, 2, 2))
        game.Start();
}