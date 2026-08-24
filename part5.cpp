//Part 5 (final)
//execution commands:
//g++ part5.cpp -o part5
//chcp 65001
//.\part5.exe

#include <bits/stdc++.h>
#ifdef _WIN32
  #include <conio.h>
  #include <windows.h>
#else
  #include <termios.h>
  #include <unistd.h>
  #include <fcntl.h>
  #include <sys/ioctl.h>
#endif

using namespace std;
using namespace std::chrono;

enum Dir { NONE=0, UP, DOWN, LEFT, RIGHT };
struct Pt { int r, c; };

namespace T {
#ifdef _WIN32
bool enableVT() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return false;
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return false;
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;
    return SetConsoleMode(hOut, dwMode);
}
#endif

inline void esc(const string &s){ cout << "\x1b[" << s; }
inline void reset(){ cout << "\x1b[0m"; }
inline void hideCursor(){ cout << "\x1b[?25l"; }
inline void showCursor(){ cout << "\x1b[?25h"; }
inline void moveTo(int r,int c){ cout << "\x1b[" << r << ";" << c << "H"; }
inline void clear(){ cout << "\x1b[2J"; }
inline void flush(){ cout.flush(); }

inline void fgRGB(int r,int g,int b){ cout << "\x1b[38;2;"<<r<<";"<<g<<";"<<b<<"m"; }
inline void bgRGB(int r,int g,int b){ cout << "\x1b[48;2;"<<r<<";"<<g<<";"<<b<<"m"; }

struct TermSize { int rows, cols; };
#ifdef _WIN32
TermSize size() {
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
    int rows = info.srWindow.Bottom - info.srWindow.Top + 1;
    int cols = info.srWindow.Right - info.srWindow.Left + 1;
    return { rows, cols };
}
#else
TermSize size() {
    winsize ws{};
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    return { (int)ws.ws_row, (int)ws.ws_col };
}
#endif
} 

#ifdef _WIN32
static constexpr int KEY_UP = 1001, KEY_DOWN = 1002, KEY_LEFT = 1003, KEY_RIGHT = 1004;
int getKeyNonBlocking() {
    if (!_kbhit()) return 0;
    int ch = _getch();
    if (ch == 0 || ch == 224) { 
        int s = _getch();
        switch (s) {
            case 72: return KEY_UP;
            case 80: return KEY_DOWN;
            case 75: return KEY_LEFT;
            case 77: return KEY_RIGHT;
            default: return 0;
        }
    }
    return ch;
}
#else
struct Raw {
    termios orig{};
    bool active=false;
    Raw(){ enable(); }
    void enable(){
        if (active) return;
        tcgetattr(STDIN_FILENO,&orig);
        termios raw = orig;
        raw.c_lflag &= ~(ICANON|ECHO);
        raw.c_cc[VMIN]=0; raw.c_cc[VTIME]=0;
        tcsetattr(STDIN_FILENO,TCSANOW,&raw);
        int flags = fcntl(STDIN_FILENO,F_GETFL,0);
        fcntl(STDIN_FILENO,F_SETFL,flags | O_NONBLOCK);
        active = true;
    }
    ~Raw(){ if(active) tcsetattr(STDIN_FILENO,TCSANOW,&orig); }
} rawguard;

static constexpr int KEY_UP = 1001, KEY_DOWN = 1002, KEY_LEFT = 1003, KEY_RIGHT = 1004;
int getKeyNonBlocking() {
    unsigned char buf[4];
    int n = (int)read(STDIN_FILENO, buf, sizeof(buf));
    if (n <= 0) return 0;
    if (buf[0] == 0x1B && n >= 3 && buf[1] == '[') {
        if (buf[2] == 'A') return KEY_UP;
        if (buf[2] == 'B') return KEY_DOWN;
        if (buf[2] == 'C') return KEY_RIGHT;
        if (buf[2] == 'D') return KEY_LEFT;
    }
    return buf[0];
}
#endif

//Game

struct Fruit {
    Pt p;
    int type; 
};

struct Snake {
    deque<Pt> body;
    Dir dir = RIGHT;
    Dir pending_dir = RIGHT;
    bool dir_changed = false;
    bool lost = false;
    int score = 0;
};

class CyberSnake {
public:
    CyberSnake() : rng((uint64_t)steady_clock::now().time_since_epoch().count()) {
        init();
    }

    void init() {
        auto ts = T::size();
        int max_rows = ts.rows - 6;
        int max_cols = ts.cols - 10;

        rows = clamp(16, 12, max(12, max_rows));    
        cols = clamp(36, 20, max(20, max_cols));
        if (rows % 2 == 0) rows--;
        if (cols % 2 == 0) cols--;

        board.assign(rows, string(cols, ' '));
        snakes.assign(NUM_SNAKES, Snake());
        int sr = rows/2, sc = cols/2;
        
        for (size_t s_idx = 0; s_idx < NUM_SNAKES; ++s_idx) {
            snakes[s_idx].body.clear();
            // initial snake length 5
            for (int i = 0; i < 5; i++) snakes[s_idx].body.push_back({sr, sc - i});
            snakes[s_idx].dir = RIGHT;
            snakes[s_idx].pending_dir = RIGHT;
            snakes[s_idx].dir_changed = false;
            snakes[s_idx].lost = false;
            snakes[s_idx].score = 0;
        }

        alive = true;
        paused = false;
        level = 1; fruitsEaten = 0;
        baseDelay = 100; curDelay = baseDelay;
        obstacles.clear();
        placeFruit();
        T::clear();
        T::hideCursor();
        #ifdef _WIN32
        T::enableVT();
        #endif
    }

    void run() {
        auto nextTime = steady_clock::now();
        drawSplashThenWaitKey();
        while (true) {
            int k = getKeyNonBlocking();
            if (!handleInput(k)) break; // false > quit

            auto now = steady_clock::now();
            if (now >= nextTime) {
                if (alive && !paused) {
                    step();
                }
                draw();
                for (auto &s : snakes) s.dir_changed = false;
                nextTime = now + milliseconds(max(25, curDelay));
            } else {
                this_thread::sleep_for(milliseconds(2));
            }
        }
        cleanup();
    }

private:
    static constexpr size_t NUM_SNAKES = 1; // "how many snakes there are" is written in exactly one place
    int rows=16, cols=36;
    vector<string> board;
    vector<Snake> snakes;
    vector<Pt> obstacles;
    Fruit fruit;
    bool alive = true, paused = false;
    int level = 1, fruitsEaten = 0;
    int baseDelay = 100, curDelay = 100;
    std::mt19937_64 rng;

    string neon(int i) {
        int r = (int)(80 + 120.0 * (sin(i*0.5) * 0.5 + 0.5));
        int g = (int)(160 + 80.0 * (cos(i*0.3) * 0.5 + 0.5));
        int b = (int)(200 + 40.0 * (sin(i*0.7) * 0.5 + 0.5));
        char buf[64];
        sprintf(buf, "\x1b[38;2;%d;%d;%dm", r,g,b);
        return string(buf);
    }

    void drawFrame(int top, int left, int h, int w) {
        // gradient top/bottom bars using neon colors
        for (int c = 0; c < w; ++c) {
            string col = neon(c);
            T::moveTo(top, left + c); cout << col << "═";
            T::moveTo(top + h - 1, left + c); cout << col << "═";
        }
        for (int r = 1; r < h-1; ++r) {
            string col = neon(r);
            T::moveTo(top + r, left); cout << col << "║";
            T::moveTo(top + r, left + w - 1); cout << col << "║";
        }
        T::reset();
    }

    void draw() {
        T::clear();
        auto ts = T::size();
        int top = max(1, (ts.rows - (rows + 6)) / 2);
        int left = max(2, (ts.cols - (cols + 36)) / 2);

        int frameW = cols + 4;
        int frameH = rows + 4;
        drawFrame(top, left, frameH, frameW);

        for (int r=0;r<rows;r++){
            for (int c=0;c<cols;c++){
                T::moveTo(top + 2 + r, left + 2 + c);
                if (((r+c)&1)==0) { cout << "\x1b[48;2;10;12;16m \x1b[0m"; }
                else { cout << "\x1b[48;2;8;10;14m \x1b[0m"; }
            }
        }

        // obstacles
        for (auto &o: obstacles) {
            T::moveTo(top+2+o.r, left+2+o.c);
            cout << "\x1b[48;2;50;10;10m" << "\x1b[38;2;200;80;80m" << "#" << "\x1b[0m";
        }

        // fruit
        T::moveTo(top+2+fruit.p.r, left+2+fruit.p.c);
        if (fruit.type == 0) cout << "\x1b[38;2;255;120;120m" << "●" << "\x1b[0m";
        else if (fruit.type == 1) cout << "\x1b[38;2;255;215;90m" << "★" << "\x1b[0m";
        else cout << "\x1b[38;2;160;240;160m" << "✦" << "\x1b[0m";

        for (size_t s_idx = 0; s_idx < snakes.size(); ++s_idx) {
            for (size_t i = 0; i < snakes[s_idx].body.size(); ++i) {
                Pt s = snakes[s_idx].body[i];
                T::moveTo(top + 2 + s.r, left + 2 + s.c);
                // head
                if (i == 0) cout << "\x1b[38;2;120;255;230m" << "■" << "\x1b[0m";
                else {
                    int t = (int)( (double)i / max(1,(int)snakes[s_idx].body.size()) * 120 );
                    int r = max(20, 30 + t), g = max(80, 200 - t), b = max(30, 80 + t/2);
                    char buf[64]; sprintf(buf, "\x1b[38;2;%d;%d;%dm", r,g,b);
                    cout << buf << "■" << "\x1b[0m";
                }
            }
        }

        // Side HUD
        int hudL = left + frameW + 2;
        T::moveTo(top + 1, hudL); cout << "\x1b[38;2;200;120;255mCyber - Neon Snake\x1b[0m";
        T::moveTo(top + 3, hudL); cout << "Score: " << (snakes.empty() ? 0 : snakes[0].score);
        T::moveTo(top + 4, hudL); cout << "Level: " << level;
        T::moveTo(top + 5, hudL); cout << "Speed: " << (1000 / max(1, curDelay));
        T::moveTo(top + 7, hudL); cout << "Fruit: ● normal  ★ bonus  ✦ speed";
        T::moveTo(top + 9, hudL); cout << "Controls: Arrows / WASD";
        T::moveTo(top + 10, hudL); cout << "P - Pause | R - Restart | Q - Quit";

        // footer / game over
        if (!alive) {
            T::moveTo(top + frameH + 1, left + 2);
            cout << "\x1b[1;38;2;255;120;140m GAME OVER — Press R to restart or Q to quit \x1b[0m";
        } else if (paused) {
            T::moveTo(top + frameH + 1, left + 2);
            cout << "\x1b[1;38;2;240;220;120m PAUSED — Press P to resume \x1b[0m";
        }

        T::flush();
    }

    //Gameplay
    void placeFruit() {
        vector<Pt> empties;
        vector<vector<char>> used(rows, vector<char>(cols, 0));
        for (const auto &s : snakes) {
            for (const auto &pt : s.body) used[pt.r][pt.c] = 1;
        }
        for (auto &o : obstacles) used[o.r][o.c] = 1;
        for (int r=0;r<rows;r++) for (int c=0;c<cols;c++) if (!used[r][c]) empties.push_back({r,c});
        if (empties.empty()) return;
        uniform_int_distribution<int> d(0, (int)empties.size()-1);
        Pt p = empties[d(rng)];
        uniform_int_distribution<int> t(0,99);
        int tt = t(rng);
        int type = (tt < 75 ? 0 : (tt < 90 ? 1 : 2)); 
        fruit = {p, type};
    }

    void maybeAddObstacle() {
        if (fruitsEaten > 0 && fruitsEaten % 6 == 0 && obstacles.size() < (size_t)(rows/5)) {
            // place one obstacle
            vector<Pt> empties;
            vector<vector<char>> used(rows, vector<char>(cols, 0));
            for (const auto &s : snakes) {
                for (const auto &pt : s.body) used[pt.r][pt.c] = 1;
            }
            for (auto &o : obstacles) used[o.r][o.c] = 1;
            for (int r=0;r<rows;r++) for (int c=0;c<cols;c++) if (!used[r][c]) empties.push_back({r,c});
            if (empties.empty()) return;
            uniform_int_distribution<int> d(0,(int)empties.size()-1);
            obstacles.push_back(empties[d(rng)]);
        }
    }

    void step() {
        for (auto &s : snakes) s.dir = s.pending_dir;

        vector<Pt> nxt(snakes.size());
        vector<bool> grow(snakes.size(), false);

        for (size_t i = 0; i < snakes.size(); ++i) {
            Pt head = snakes[i].body.front();
            nxt[i] = head;
            switch (snakes[i].dir) {
                case UP:    nxt[i].r--; break;
                case DOWN:  nxt[i].r++; break;
                case LEFT:  nxt[i].c--; break;
                case RIGHT: nxt[i].c++; break;
                default: break;
            }
            
            // will the snake grow this tick?
            grow[i] = (nxt[i].r == fruit.p.r && nxt[i].c == fruit.p.c);

            // if not growing, the tail cell will be vacated BEFORE checking self-collision
            if (!grow[i]) {
                snakes[i].body.pop_back();               
            }
        }

        // wall, obstacle, and snake collisions
        for (size_t i = 0; i < snakes.size(); ++i) {
            if (nxt[i].r < 0 || nxt[i].r >= rows || nxt[i].c < 0 || nxt[i].c >= cols) { alive = false; return; }
            for (auto &o : obstacles) if (o.r == nxt[i].r && o.c == nxt[i].c) { alive = false; return; }
            for (size_t j = 0; j < snakes.size(); ++j) {
                for (const auto &s : snakes[j].body) {
                    if (s.r == nxt[i].r && s.c == nxt[i].c) { alive = false; return; }
                }
            }
        }

        // place new head and handle fruit
        for (size_t i = 0; i < snakes.size(); ++i) {
            snakes[i].body.push_front(nxt[i]);

            if (grow[i]) {
                fruitsEaten++;
                if (fruit.type == 0) snakes[i].score += 10;
                else if (fruit.type == 1) snakes[i].score += 25;
                else { // speed fruit
                    snakes[i].score += 8;
                    curDelay = max(30, curDelay - 15);
                }
                if (fruitsEaten % 4 == 0) {
                    level++;
                    curDelay = max(30, curDelay - 6);
                }
                placeFruit();
                maybeAddObstacle();
            }
        }
    }

    //Input handling
    bool handleInput(int key) {
        if (key == 0) return true;
        // allow quit anytime
        if (key == 'q' || key == 'Q') return false;
        if (key == 'r' || key == 'R') { init(); return true; }
        if (key == 'p' || key == 'P') { paused = !paused; return true; }
        if (!alive) return true;
        if (paused) return true;

        if (snakes.empty() || snakes[0].dir_changed) return true;

        Dir candidate = snakes[0].dir;
        if (key == KEY_UP || key == 'w' || key == 'W') candidate = UP;
        else if (key == KEY_DOWN || key == 's' || key == 'S') candidate = DOWN;
        else if (key == KEY_LEFT || key == 'a' || key == 'A') candidate = LEFT;
        else if (key == KEY_RIGHT || key == 'd' || key == 'D') candidate = RIGHT;

        // prevent 180-degree turn
        if (!isOpposite(snakes[0].dir, candidate) && candidate != NONE) {
            snakes[0].pending_dir = candidate;
            snakes[0].dir_changed = true;
        }
        return true;
    }

    static bool isOpposite(Dir a, Dir b) {
        return (a==UP && b==DOWN) || (a==DOWN && b==UP) || (a==LEFT && b==RIGHT) || (a==RIGHT && b==LEFT);
    }

    //splash and cleanup
    void drawSplashThenWaitKey() {
        T::clear();
        auto ts = T::size();
        int top = max(2, ts.rows/6);
        int left = max(4, ts.cols/6);
        T::moveTo(top, left);
        cout << "\n";
        cout << "  \x1b[1;38;2;120;200;255mCyber - Neon Snake\x1b[0m\n\n";
        cout << "  Controls: Arrows / WASD   P: Pause   R: Restart   Q: Quit\n";
        cout << "  Unique features: neon border, 3 fruit types, obstacles, tail gradient.\n\n";
        cout << "  Press any key to start...";
        T::flush();
        // wait for any key
        while (true) {
            int k = getKeyNonBlocking();
            if (k != 0) break;
            this_thread::sleep_for(milliseconds(10));
        }
    }

    void cleanup() {
        T::reset();
        T::showCursor();
        T::moveTo(999,1);
        cout << "\n";
    }
};

//main
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    T::enableVT(); 
#endif

    CyberSnake game;
    game.run();
    return 0;
}
