// Cyber Snake - Smooth Rendering (C++11 / MinGW 6.3.0)
// Compile:  g++ part52_smooth.cpp -o part52 -std=c++11
// Run:      chcp 65001 && .\part52.exe

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

// ===== Smooth sleep (no thread dep) =====
inline void sleep_ms(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

// ===== Windows VT support =====
#ifdef _WIN32
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#ifndef DISABLE_NEWLINE_AUTO_RETURN
#define DISABLE_NEWLINE_AUTO_RETURN 0x0008
#endif
#endif

template<typename T>
T clamp11(T v,T lo,T hi){return std::max(lo,std::min(v,hi));}

enum Dir{NONE=0,UP,DOWN,LEFT,RIGHT};
struct Pt{int r,c;};

namespace T {
#ifdef _WIN32
bool enableVT(){
    HANDLE h=GetStdHandle(STD_OUTPUT_HANDLE);
    if(h==INVALID_HANDLE_VALUE)return false;
    DWORD m=0;if(!GetConsoleMode(h,&m))return false;
    m|=ENABLE_VIRTUAL_TERMINAL_PROCESSING|DISABLE_NEWLINE_AUTO_RETURN;
    return SetConsoleMode(h,m);
}
#endif
inline void moveTo(int r,int c){cout<<"\x1b["<<r<<";"<<c<<"H";}
inline void reset(){cout<<"\x1b[0m";}
inline void hideCursor(){cout<<"\x1b[?25l";}
inline void showCursor(){cout<<"\x1b[?25h";}
inline void flush(){cout.flush();}
struct TermSize{int rows,cols;};
#ifdef _WIN32
TermSize size(){CONSOLE_SCREEN_BUFFER_INFO i;GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&i);
return{i.srWindow.Bottom-i.srWindow.Top+1,i.srWindow.Right-i.srWindow.Left+1};}
#else
TermSize size(){winsize ws{};ioctl(STDOUT_FILENO,TIOCGWINSZ,&ws);
return{(int)ws.ws_row,(int)ws.ws_col};}
#endif
}

// ===== Keyboard =====
#ifdef _WIN32
static constexpr int KEY_UP=1001,KEY_DOWN=1002,KEY_LEFT=1003,KEY_RIGHT=1004;
int getKeyNonBlocking(){
    if(!_kbhit())return 0;
    int ch=_getch();
    if(ch==0||ch==224){
        int s=_getch();
        switch(s){case 72:return KEY_UP;case 80:return KEY_DOWN;
                  case 75:return KEY_LEFT;case 77:return KEY_RIGHT;}
        return 0;}
    return ch;
}
#else
struct Raw{
    termios orig{};bool active=false;
    Raw(){enable();}
    void enable(){
        if(active)return;tcgetattr(STDIN_FILENO,&orig);
        termios raw=orig;raw.c_lflag&=~(ICANON|ECHO);
        raw.c_cc[VMIN]=0;raw.c_cc[VTIME]=0;
        tcsetattr(STDIN_FILENO,TCSANOW,&raw);
        int f=fcntl(STDIN_FILENO,F_GETFL,0);
        fcntl(STDIN_FILENO,F_SETFL,f|O_NONBLOCK);
        active=true;
    }
    ~Raw(){if(active)tcsetattr(STDIN_FILENO,TCSANOW,&orig);}
} rawguard;

static constexpr int KEY_UP=1001,KEY_DOWN=1002,KEY_LEFT=1003,KEY_RIGHT=1004;
int getKeyNonBlocking(){
    unsigned char b[4];int n=(int)read(STDIN_FILENO,b,sizeof(b));
    if(n<=0)return 0;
    if(b[0]==0x1B&&n>=3&&b[1]=='['){
        if(b[2]=='A')return KEY_UP;
        if(b[2]=='B')return KEY_DOWN;
        if(b[2]=='C')return KEY_RIGHT;
        if(b[2]=='D')return KEY_LEFT;}
    return b[0];
}
#endif

// ===== Game =====
struct Fruit{Pt p;int type;};

class CyberSnake{
public:
    CyberSnake():rng((uint64_t)steady_clock::now().time_since_epoch().count()){init();}
    void init(){
        auto ts=T::size();
        int mr=ts.rows-6,mc=ts.cols-10;
        rows=clamp11(max(12,mr),12,16);
        cols=clamp11(max(20,mc),20,36);
        if(rows%2==0)rows--;if(cols%2==0)cols--;
        board.assign(rows,string(cols,' '));
        snake.clear();snake2.clear();
        int sr=rows/2,sc=cols/2;
        int sr1=max(0,sr-2),sr2=min(rows-1,sr+2);
        for(int i=0;i<5;i++)snake.push_back({sr1,sc-i});
        for(int i=0;i<5;i++)snake2.push_back({sr2,sc-i});
        dir=RIGHT;pending_dir=RIGHT;dir_changed=false;
        dir2=RIGHT;pending_dir2=RIGHT;dir_changed2=false;
        alive=true;p1Lost=false;p2Lost=false;paused=false;score=0;score2=0;level=1;fruitsEaten=0;
        baseDelay=100;curDelay=baseDelay;obstacles.clear();
        placeFruit();
        T::hideCursor();
        #ifdef _WIN32
        T::enableVT();
        #endif
    }
    void run(){
        auto next=steady_clock::now();
        drawSplashThenWaitKey();
        while(true){
            int k=getKeyNonBlocking();
            if(!handleInput(k))break;
            auto now=steady_clock::now();
            if(now>=next){
                if(alive&&!paused)step();
                drawSmooth();
                dir_changed=false;dir_changed2=false;
                next=now+milliseconds(max(25,curDelay));
            }else sleep_ms(2);
        }
        cleanup();
    }
private:
    int rows=16,cols=36;
    vector<string>board;deque<Pt>snake;deque<Pt>snake2;vector<Pt>obstacles;
    Fruit fruit;Dir dir=RIGHT,pending_dir=RIGHT;
    Dir dir2=RIGHT,pending_dir2=RIGHT;
    bool dir_changed=false,dir_changed2=false,alive=true,paused=false;
    bool p1Lost=false,p2Lost=false;
    int score=0,score2=0,level=1,fruitsEaten=0,baseDelay=100,curDelay=100;
    mt19937_64 rng;
    string buffer;

    string neon(int i){int r=(int)(80+120*(sin(i*0.5)*0.5+0.5));
        int g=(int)(160+80*(cos(i*0.3)*0.5+0.5));
        int b=(int)(200+40*(sin(i*0.7)*0.5+0.5));
        char buf[64];sprintf(buf,"\x1b[38;2;%d;%d;%dm",r,g,b);return string(buf);}
    void placeFruit(){
        vector<Pt>e;vector<vector<char>>u(rows,vector<char>(cols,0));
        for(auto&s:snake)u[s.r][s.c]=1;for(auto&s:snake2)u[s.r][s.c]=1;for(auto&o:obstacles)u[o.r][o.c]=1;
        for(int r=0;r<rows;r++)for(int c=0;c<cols;c++)if(!u[r][c])e.push_back({r,c});
        if(e.empty())return;uniform_int_distribution<int>d(0,(int)e.size()-1);
        Pt p=e[d(rng)];uniform_int_distribution<int>t(0,99);int tt=t(rng);
        fruit={p,(tt<75?0:(tt<90?1:2))};
    }
    void maybeAddObstacle(){
        if(fruitsEaten>0&&fruitsEaten%6==0&&obstacles.size()<(size_t)(rows/5)){
            vector<Pt>e;vector<vector<char>>u(rows,vector<char>(cols,0));
            for(auto&s:snake)u[s.r][s.c]=1;for(auto&s:snake2)u[s.r][s.c]=1;for(auto&o:obstacles)u[o.r][o.c]=1;
            for(int r=0;r<rows;r++)for(int c=0;c<cols;c++)if(!u[r][c])e.push_back({r,c});
            if(e.empty())return;uniform_int_distribution<int>d(0,(int)e.size()-1);
            obstacles.push_back(e[d(rng)]);}
    }
    void step(){
        dir=pending_dir;dir2=pending_dir2;
        Pt h=snake.front(),n=h;
        if(dir==UP)n.r--;else if(dir==DOWN)n.r++;else if(dir==LEFT)n.c--;else n.c++;
        Pt h2=snake2.front(),n2=h2;
        if(dir2==UP)n2.r--;else if(dir2==DOWN)n2.r++;else if(dir2==LEFT)n2.c--;else n2.c++;

        bool grow=(n.r==fruit.p.r&&n.c==fruit.p.c);
        bool grow2=(n2.r==fruit.p.r&&n2.c==fruit.p.c);
        if(!grow)snake.pop_back();
        if(!grow2)snake2.pop_back();

        bool p1_hit=false;
        if(n.r<0||n.r>=rows||n.c<0||n.c>=cols)p1_hit=true;
        if(!p1_hit){for(auto&o:obstacles)if(o.r==n.r&&o.c==n.c){p1_hit=true;break;}}
        if(!p1_hit){for(auto&s:snake)if(s.r==n.r&&s.c==n.c){p1_hit=true;break;}}
        if(!p1_hit){for(auto&s:snake2)if(s.r==n.r&&s.c==n.c){p1_hit=true;break;}}

        bool p2_hit=false;
        if(n2.r<0||n2.r>=rows||n2.c<0||n2.c>=cols)p2_hit=true;
        if(!p2_hit){for(auto&o:obstacles)if(o.r==n2.r&&o.c==n2.c){p2_hit=true;break;}}
        if(!p2_hit){for(auto&s:snake2)if(s.r==n2.r&&s.c==n2.c){p2_hit=true;break;}}
        if(!p2_hit){for(auto&s:snake)if(s.r==n2.r&&s.c==n2.c){p2_hit=true;break;}}

        if(n.r==n2.r&&n.c==n2.c){p1_hit=true;p2_hit=true;}

        if(p1_hit||p2_hit){
            alive=false;p1Lost=p1_hit;p2Lost=p2_hit;return;
        }

        snake.push_front(n);
        snake2.push_front(n2);

        if(grow||grow2){
            fruitsEaten++;
            int ptVal=(fruit.type==0?10:(fruit.type==1?25:8));
            if(grow)score+=ptVal;
            if(grow2)score2+=ptVal;
            if(fruit.type==2)curDelay=max(30,curDelay-15);
            if(fruitsEaten%4==0){level++;curDelay=max(30,curDelay-6);}
            placeFruit();maybeAddObstacle();
        }
    }
    bool handleInput(int k){
        if(k==0)return true;if(k=='q'||k=='Q')return false;
        if(k=='r'||k=='R'){init();return true;}
        if(k=='p'||k=='P'){paused=!paused;return true;}
        if(!alive||paused)return true;
        if(!dir_changed){
            Dir c=dir;
            if(k==KEY_UP)c=UP;
            else if(k==KEY_DOWN)c=DOWN;
            else if(k==KEY_LEFT)c=LEFT;
            else if(k==KEY_RIGHT)c=RIGHT;
            if(!isOpposite(dir,c)&&c!=NONE){pending_dir=c;dir_changed=true;}
        }
        if(!dir_changed2){
            Dir c2=dir2;
            if(k=='w'||k=='W')c2=UP;
            else if(k=='s'||k=='S')c2=DOWN;
            else if(k=='a'||k=='A')c2=LEFT;
            else if(k=='d'||k=='D')c2=RIGHT;
            if(!isOpposite(dir2,c2)&&c2!=NONE){pending_dir2=c2;dir_changed2=true;}
        }
        return true;}
    static bool isOpposite(Dir a,Dir b){
        return (a==UP&&b==DOWN)||(a==DOWN&&b==UP)||(a==LEFT&&b==RIGHT)||(a==RIGHT&&b==LEFT);}
    void drawSmooth(){
        buffer.clear();
        int top_offset=5;
        // auto ts=T::size();
        // int top=max(1,(ts.rows-(rows+6))/2);
        // int left=max(2,(ts.cols-(cols+36))/2);
        buffer+="\x1b["+to_string(top_offset)+";1H"; // move cursor top
        // border top
        for(int c=0;c<cols+4;c++)buffer+=neon(c)+"═";
        buffer+="\n";
        for(int r=0;r<rows;r++){
            buffer+=neon(r)+"║";
            for(int c=0;c<cols;c++){
                bool drawn=false;
                for(auto&o:obstacles)
                    if(o.r==r&&o.c==c){
                        buffer+="\x1b[38;2;200;80;80m#";
                        drawn=true;
                        break;
                    }
                if(!drawn){
                    bool snakeCell=false;
                    for(size_t i=0;i<snake.size();++i){
                        Pt s=snake[i];
                        if(s.r==r&&s.c==c){
                            if(i==0)buffer+="\x1b[38;2;120;255;230m■";
                            else buffer+="\x1b[38;2;60;200;150m■";
                            snakeCell=true;
                            break;
                        }
                    }
                    if(!snakeCell){
                        for(size_t i=0;i<snake2.size();++i){
                            Pt s=snake2[i];
                            if(s.r==r&&s.c==c){
                                if(i==0)buffer+="\x1b[38;2;255;180;120m■";
                                else buffer+="\x1b[38;2;220;100;50m■";
                                snakeCell=true;
                                break;
                            }
                        }
                    }
                    if(!snakeCell){
                        if(r==fruit.p.r&&c==fruit.p.c){
                            if(fruit.type==0)buffer+="\x1b[38;2;255;120;120m●";
                            else if(fruit.type==1)buffer+="\x1b[38;2;255;215;90m★";
                            else buffer+="\x1b[38;2;160;240;160m✦";
                        }else buffer+=" ";
                    }
                }
            }
            buffer+="\x1b[0m║\n";
        }
        for(int c=0;c<cols+4;c++)buffer+=neon(c)+"═";
        buffer+="\x1b[0m\n";
        buffer+="P1 Score:"+to_string(score)+"  P2 Score:"+to_string(score2)+"  Level:"+to_string(level);
        if(!alive){
            if(p1Lost&&p2Lost) buffer+="  \x1b[38;2;255;100;100m[GAME OVER - Both Players Lost]";
            else if(p1Lost) buffer+="  \x1b[38;2;255;100;100m[GAME OVER - Player 1 Lost]";
            else if(p2Lost) buffer+="  \x1b[38;2;255;100;100m[GAME OVER - Player 2 Lost]";
        }
        if(paused)buffer+="  \x1b[38;2;255;255;120m[PAUSED]";
        cout<<buffer;
        T::flush();
    }
    void drawSplashThenWaitKey(){
        T::moveTo(1,1);cout<<"\x1b[2J";
        cout<<"\n\x1b[1;38;2;120;200;255mCyber - Neon Snake\x1b[0m\n";
        cout<<"Controls: Arrows / WASD   P:Pause  R:Restart  Q:Quit\n";
        cout << "Eat fruits (● ★ ✦) and avoid walls!\n";
        cout << "---------------------------------------------\n";
        cout<<"Press any key to start...";
        T::flush();
        while(true){int k=getKeyNonBlocking();if(k!=0)break;sleep_ms(10);}
    }
    void cleanup(){T::reset();T::showCursor();T::moveTo(999,1);cout<<"\n";}
};

int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);SetConsoleCP(CP_UTF8);T::enableVT();
#endif
    CyberSnake g;g.run();
    return 0;
}
