#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#define OLC_SOUNDWAVE
#include "olcSoundWaveEngine.h"

using namespace std;

enum Direction {
    UP = 0,
    RIGHT,
    DOWN,
    LEFT
};

class HitObject {
public:
    float time;
    bool hit;
    Direction dir;
};

class Song {
public:
    Song(string osufile, string wav, olc::sound::WaveEngine* engine) {
        this->engine = engine;
        audio.LoadAudioWaveform(wav);
        processOsuFile(osufile);
        playing = false;
        time = 0.0f;
    }

private:
    olc::sound::WaveEngine* engine;
    olc::sound::Wave audio;
    vector<HitObject> hitObjects;
    int leadin;
    bool playing;
    float time;

    vector<string> split(string s, string delim) {
        vector<string> res;
        size_t pos = 0;
        string token;
        while ((pos = s.find(delim)) != string::npos) {
            token = s.substr(0, pos);
            res.push_back(token);
            s.erase(0, pos + delim.length());
        }
        res.push_back(s);
        return res;
    }

    void processOsuFile(string osufile) {
        ifstream osu(osufile);

        hash<string> hasher;
        srand(hasher(osufile));

        auto GeneralFunc = [&](string line) {
            if (line.rfind("AudioLeadIn") == 0) {
                leadin = stoi(line.substr(line.find(':') + 1));
            }
        };

        auto HitObjFunc = [&](string line) {
            vector<string> args = split(line, ",");
            int time = stoi(args[2]) + leadin;
            HitObject obj;
            obj.time = time;
            obj.hit = false;
            obj.dir = Direction(rand() % 4);
            hitObjects.push_back(obj);
        };

        auto TimingFunc = [&](string line) {
            // no need?
        };

        auto DifficultyFunc = [&](string line) {
            // no need?
        };

        string line = "";
        string section = "";
        while (getline(osu, line)) {
            if (line.length() < 1) continue;

            if (line[0] == '[') {
                section = line.substr(1, line.find_last_of(']') - 1);
            }
            else if (section == "General") {
                GeneralFunc(line);
            }
            else if (section == "HitObjects") {
                HitObjFunc(line);
            }
            else if (section == "TimingPoints") {
                TimingFunc(line);
            }
            else if (section == "Difficulty") {
                DifficultyFunc(line);
            }
        }
    }

public:
    bool Playing() {
        return playing;
    }
    
    void Start() {
        playing = true;
        engine->PlayWaveform(&audio);
    }

    void Update(float fElapsedTime) {
        time += fElapsedTime;
    }

    int TimeMillis() {
        return int(time * 1000.0f);
    }

    vector<HitObject>& GetHitObjects() {
        return hitObjects;
    }
};

class Game : public olc::PixelGameEngine {
public:
    Game() {
        sAppName = "Undertale thingy";
    }

private:
    
    Direction playerDirection;

    olc::sound::WaveEngine engine;
    unique_ptr<Song> song;

    void Rotate(olc::vf2d& pos, Direction d) {
        float temp = 0;
        switch (d) {
            case UP:
                break;
            case RIGHT:
                temp = pos.y;
                pos.y = pos.x;
                pos.x = -temp;
                break;
            case DOWN:
                pos.y = -pos.y;
                break;
            case LEFT:
                temp = pos.y;
                pos.y = pos.x;
                pos.x = temp;
                break;
            default:
                break;
        }
    }

public:
    bool OnUserCreate() override {
        playerDirection = UP;
        engine.InitialiseAudio();

        song = make_unique<Song>("Toby Fox - Spear of Justice (Cut Ver.) (Silence1020) [Insane].osu", "audio.wav", &engine);

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
        if (!song->Playing()) {
            song->Start();
        }
        else {
            song->Update(fElapsedTime);
        }

        if (GetKey(olc::ESCAPE).bPressed) {
            return false;
        }

        if (GetKey(olc::UP).bHeld || GetKey(olc::W).bHeld) {
            playerDirection = UP;
        }
        if (GetKey(olc::DOWN).bHeld || GetKey(olc::S).bHeld) {
            playerDirection = DOWN;
        }
        if (GetKey(olc::LEFT).bHeld || GetKey(olc::A).bHeld) {
            playerDirection = LEFT;
        }
        if (GetKey(olc::RIGHT).bHeld || GetKey(olc::D).bHeld) {
            playerDirection = RIGHT;
        }

        Clear(olc::BLACK);

        FillCircle(ScreenWidth() / 2, ScreenHeight() / 2, 5, olc::RED);

        olc::vf2d center = { float(ScreenWidth() / 2), float(ScreenHeight() / 2) };
        olc::vf2d blockPos = { -25, -30 };
        olc::vf2d blockSize = { 50, 5 };
        Rotate(blockPos, playerDirection);
        if (playerDirection == RIGHT || playerDirection == LEFT) {
            swap(blockSize.x, blockSize.y);
        }
        blockPos += center;
        FillRect(blockPos, blockSize, olc::BLUE);

        vector<HitObject>& hitObjs = song->GetHitObjects();
        const int millis = song->TimeMillis();
        for (auto& ho : hitObjs) {
            if (ho.time - millis >= 5000) {
                break;
            }

            if (!ho.hit && ho.time - millis > -1000) {
                if (ho.dir == playerDirection && ho.time - millis <= 30 && ho.time - millis >= -50) {
                    ho.hit = true;
                }

                olc::vf2d pos = { 0, -30 - (ho.time - millis) / 2.0f };
                Rotate(pos, ho.dir);
                pos += center;

                FillCircle(pos, 7, olc::YELLOW);
            }
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