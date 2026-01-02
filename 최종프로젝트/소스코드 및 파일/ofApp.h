#pragma once

#include "ofMain.h"
#include "ofxWinMenu.h" // Menu addon
#include <vector>
#include <string>
#include <cstring>
#include <queue>
#include <tuple>
#include <limits.h>

class ofApp : public ofBaseApp {

public:

    void setup();
    void update();
    void draw();

    void keyPressed(int key); // Traps escape key if exit disabled
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    bool readFile();
    void freeMemory();
    bool DFS();
    void dfsdraw();
    bool BFS();
    void bfsdraw();
    bool Dijkstra();
    void dijkstradraw();

    int HEIGHT;//미로의 높이
    int WIDTH;//미로의 너비
    char** input;//텍스트 파일의 모든 정보를 담는 이차원 배열이다.
    vector<string> lines;
    int** visited;//방문여부를 저장할 포인
    int maze_col;//미로칸의 열의 인덱스를 가리킨다.
    int maze_row;//미로칸의 행의 인덱스를 가리킨다.
    int k;
    int isOpen; //파일이 열렸는지를 판단하는 변수. 0이면 안열렸고 1이면 열렸다.
    int isDFS;//DFS함수를 실행시켰는지 판단하는 변수. 0이면 실행안했고 1이면 실행했다.
    int isBFS;//BFS함수를 실행시켰는지 판단하는 변수. 0이면 실행안했고 1이면 실행했다.
    int isDijkstra;//Dijkstra 함수를 실행시켰는지 판단하는 변수. 0이면 실행안했고 1이면 실행했다.

    typedef struct vertex {
        struct vertex* upside;
        struct vertex* leftside;
        struct vertex* rightside;
        struct vertex* downside;
        bool visit;
        bool revisit;
    }vertex;
    vertex*** mst; // maze represented as spanning tree
    vertex* root;

    // Menu
    ofxWinMenu* menu; // Menu object
    void appMenuFunction(string title, bool bChecked); // Menu return function

    // Used by example app
    ofTrueTypeFont myFont;
    ofImage myImage;
    float windowWidth, windowHeight;
    HWND hWnd; // Application window
    HWND hWndForeground; // current foreground window

    // Example menu variables
    bool bShowInfo;
    bool bFullscreen;
    bool bTopmost;
    bool isdfs;
    bool isbfs;
    bool isdijkstra;
    // Example functions
    void doFullScreen(bool bFull);
    void doTopmost(bool bTop);

    // New variables for the game
    pair<int, int> start;
    pair<int, int> end;
    pair<int, int> monster;
    int score;
    bool hasCaughtMonster;
    float startTime; // To track game start time
    float endTime; // To track game end time

    void moveMonster();
    bool isMonsterCaught();
    void initializeGame();
    void drawMonster();
    void drawPlayer();

    // Dijkstra-specific variables
    vector<vector<int>> distances;
    vector<vector<pair<int, int>>> predecessors;
};
