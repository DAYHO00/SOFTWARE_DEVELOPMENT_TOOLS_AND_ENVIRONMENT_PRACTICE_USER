#include "ofApp.h"
#include <iostream>
using namespace std;

vector<string> maze_input;
vector<pair<int, int>> dfs_path;
vector<pair<int, int>> trial_path;

deque<pair<int, int>> bfs_path;
deque<tuple<int, int, int>> trial_bfs_path;
bool gameEnded = false; // 게임이 종료됐는지 여부를 추적하는 변수

// Dijkstra-specific variables
vector<pair<int, int>> dijkstra_path;
vector<pair<int, int>> trial_dijkstra_path;

//--------------------------------------------------------------
void ofApp::setup() {

    ofSetWindowTitle("Maze Example"); // Set the app name on the title bar
    ofSetFrameRate(15);
    ofBackground(255, 255, 255);
    isdfs = false;
    isbfs = false;
    isOpen = 0;
    score = 0;
    hasCaughtMonster = false;
    startTime = ofGetElapsedTimef(); // Start the timer
    gameEnded = false; // Initialize gameEnded
    isdijkstra = false;
    // Centre on the screen
    ofSetWindowPosition((ofGetScreenWidth() - windowWidth) / 2, (ofGetScreenHeight() - windowHeight) / 2);

    // Load a font rather than the default
    myFont.loadFont("verdana.ttf", 12, true, true);

    // Window handle used for topmost function
    hWnd = WindowFromDC(wglGetCurrentDC());

    // Disable escape key exit so we can exit fullscreen with Escape (see keyPressed)
    ofSetEscapeQuitsApp(false);

    //
    // Create a menu using ofxWinMenu
    //

    // A new menu object with a pointer to this class
    menu = new ofxWinMenu(this, hWnd);

    // Register an ofApp function that is called when a menu item is selected.
    // The function can be called anything but must exist. 
    // See the example "appMenuFunction".
    menu->CreateMenuFunction(&ofApp::appMenuFunction);

    // Create a window menu
    HMENU hMenu = menu->CreateWindowMenu();

    //
    // Create a "File" popup menu
    //
    HMENU hPopup = menu->AddPopupMenu(hMenu, "File");

    //
    // Add popup items to the File menu
    //

    // Open an maze file
    menu->AddPopupItem(hPopup, "Open", false, false); // Not checked and not auto-checked

    // Final File popup menu item is "Exit" - add a separator before it
    menu->AddPopupSeparator(hPopup);
    menu->AddPopupItem(hPopup, "Exit", false, false);

    //
    // View popup menu
    //
    hPopup = menu->AddPopupMenu(hMenu, "View");

    bShowInfo = true;  // screen info display on
    menu->AddPopupItem(hPopup, "Show DFS", false, false); // Checked
    bTopmost = false; // app is topmost
    menu->AddPopupItem(hPopup, "Show BFS"); // Not checked (default)
    menu->AddPopupItem(hPopup, "Show Dijkstra"); // Not checked (default)
    bFullscreen = false; // not fullscreen yet
    menu->AddPopupItem(hPopup, "Full screen", false, false); // Not checked and not auto-check

    //
    // Help popup menu
    //
    hPopup = menu->AddPopupMenu(hMenu, "Help");
    menu->AddPopupItem(hPopup, "About", false, false); // No auto check

    // Set the menu to the window
    menu->SetWindowMenu();
} // end Setup


//
// Menu function
//
// This function is called by ofxWinMenu when an item is selected.
// The title and state can be checked for required action.
// 
void ofApp::appMenuFunction(string title, bool bChecked) {

    ofFileDialogResult result;
    string filePath;
    size_t pos;

    //
    // File menu
    //
    if (title == "Open") {
        readFile();
        initializeGame();
    }
    if (title == "Exit") {
        ofExit(); // Quit the application
    }

    //
    // Window menu
    //
    if (title == "Show DFS") {
        if (isOpen)
        {
            dfs_path.clear();
            dfs_path.shrink_to_fit();
            trial_path.clear();
            trial_path.shrink_to_fit();
            isdfs = DFS();
            bShowInfo = bChecked;
        }
        else
            cout << "you must open file first" << endl;

    }
    if (title == "Show BFS") {
        if (isOpen) {
            bfs_path.clear();
            bfs_path.shrink_to_fit();
            trial_bfs_path.clear();
            trial_bfs_path.shrink_to_fit();
            isbfs = BFS();
            bShowInfo = bChecked;
        }
        else {
            cout << "you must open file first" << endl;
        }
    }

    if (title == "Show Dijkstra") {
        if (isOpen) {
            dijkstra_path.clear();
            dijkstra_path.shrink_to_fit();
            trial_dijkstra_path.clear();
            trial_dijkstra_path.shrink_to_fit();
            isdijkstra = Dijkstra();
            bShowInfo = bChecked;
        }
        else {
            cout << "you must open file first" << endl;
        }
    }

    if (title == "Full screen") {
        bFullscreen = !bFullscreen; // Not auto-checked and also used in the keyPressed function
        doFullScreen(bFullscreen); // But also take action immediately
    }

    //
    // Help menu
    //
    if (title == "About") {
        ofSystemAlertDialog("ofxWinMenu\nbasic example\n\nhttp://spout.zeal.co");
    }

} // end appMenuFunction


//--------------------------------------------------------------
void ofApp::update() {
    if (isOpen && !gameEnded) {
        if (!hasCaughtMonster) {
            moveMonster();
        }
    }
}


//--------------------------------------------------------------
void ofApp::draw() {

    char str[256];
    //ofBackground(0, 0, 0, 0);
    ofSetColor(100);
    ofSetLineWidth(8); // Set line width to be thicker

    // TO DO : DRAW MAZE; 
    // 저장된 자료구조를 이용해 미로를 그린다.
    // add code here
    //cout << maze_input.size();

    int i = 0;
    while (i < maze_input.size()) {
        int j = 0;
        while (j < maze_input[i].size()) {
            if (maze_input[i][j] == ' ') {
                ofSetColor(0, 255, 0); // Green color for the floor
                ofDrawRectangle(j * 10, i * 10, 10, 10);
            }
            else if (maze_input[i][j] == '-' || maze_input[i][j] == '|') {
                ofSetColor(255, 165, 0); // Orange color for the lines
                if (maze_input[i][j] == '-') {
                    ofDrawLine((j - 1) * 10, i * 10, (j + 1) * 10, i * 10);
                }
                else {
                    ofDrawLine(j * 10, (i - 1) * 10, j * 10, (i + 1) * 10);
                }
            }
            ++j;
        }
        ++i;
    }

    if (isdfs)
    {
        ofSetColor(200);
        ofSetLineWidth(8); // Set line width to be thicker for DFS path
        if (isOpen) {
            dfsdraw();
        }
        else
            cout << "You must open file first" << endl;
    }
    if (isbfs) {
        ofSetColor(200);
        ofSetLineWidth(8); // Set line width to be thicker for BFS path
        if (isOpen) {
            bfsdraw();
        }
        else
            cout << "You must open file first" << endl;
    }
    if (isdijkstra) {
        ofSetColor(200);
        ofSetLineWidth(8); // Set line width to be thicker for Dijkstra path
        if (isOpen) {
            dijkstradraw();
        }
        else
            cout << "You must open file first" << endl;
    }

    // Draw the player and monster last so they appear on top
    if (!gameEnded) {
        drawPlayer();
        if (!hasCaughtMonster) {
            drawMonster();
        }

        ofSetColor(255, 255, 0); // Yellow color for start point
        ofDrawCircle(start.second * 10, start.first * 10, 5); // Draw start point
        ofSetColor(255, 0, 0);
        ofDrawCircle(end.second * 10, end.first * 10, 5); // Draw end point
    }

    if (bShowInfo) {
        // Show keyboard duplicates of menu functions
        ofSetColor(0); // Black color for text
        float elapsedTime = gameEnded ? endTime - startTime : ofGetElapsedTimef() - startTime;
        sprintf(str, "Score: %d Time: %.2f seconds", score, elapsedTime);
        myFont.drawString(str, 15, ofGetHeight() - 20);
    }

    // Display "The End" when the game ends
    if (gameEnded) {
        ofSetColor(0, 0, 0);
        myFont.drawString("The End", ofGetWidth() / 2 - 30, ofGetHeight() / 2);
    }
}



void ofApp::doFullScreen(bool bFull)
{
    // Enter full screen
    if (bFull) {
        // Remove the menu but don't destroy it
        menu->RemoveWindowMenu();
        // hide the cursor
        ofHideCursor();
        // Set full screen
        ofSetFullscreen(true);
    }
    else {
        // return from full screen
        ofSetFullscreen(false);
        // Restore the menu
        menu->SetWindowMenu();
        // Restore the window size allowing for the menu
        ofSetWindowShape(windowWidth, windowHeight + GetSystemMetrics(SM_CYMENU));
        // Centre on the screen
        ofSetWindowPosition((ofGetScreenWidth() - ofGetWidth()) / 2, (ofGetScreenHeight() - ofGetHeight()) / 2);
        // Show the cursor again
        ofShowCursor();
        // Restore topmost state
        if (bTopmost) doTopmost(true);
    }

} // end doFullScreen


void ofApp::doTopmost(bool bTop)
{
    if (bTop) {
        // get the current top window for return
        hWndForeground = GetForegroundWindow();
        // Set this window topmost
        SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        ShowWindow(hWnd, SW_SHOW);
    }
    else {
        SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        ShowWindow(hWnd, SW_SHOW);
        // Reset the window that was topmost before
        if (GetWindowLong(hWndForeground, GWL_EXSTYLE) & WS_EX_TOPMOST)
            SetWindowPos(hWndForeground, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        else
            SetWindowPos(hWndForeground, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }
} // end doTopmost


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    if (!gameEnded) {
        // Escape key exit has been disabled but it can be checked here
        if (key == VK_ESCAPE) {
            // Disable fullscreen set, otherwise quit the application as usual
            if (bFullscreen) {
                bFullscreen = false;
                doFullScreen(false);
            }
            else {
                ofExit();
            }
        }

        // Remove or show screen info
        if (key == ' ') {
            bShowInfo = !bShowInfo;
            // Update the menu check mark because the item state has been changed here
            menu->SetPopupItem("Show DFS", bShowInfo);
        }

        if (key == 'f') {
            bFullscreen = !bFullscreen;
            doFullScreen(bFullscreen);
            // Do not check this menu item
            // If there is no menu when you call the SetPopupItem function it will crash
        }

        if (key == 'd') {
            isbfs = false;
            isdijkstra = false;
            if (isOpen)
            {
                dfs_path.clear();
                dfs_path.shrink_to_fit();
                trial_path.clear();
                trial_path.shrink_to_fit();
                isdfs = DFS();
            }
            else
                cout << "Error: FILE OPEN FIRST" << endl;
        }

        if (key == 'b') {
            isdfs = false;
            isdijkstra = false;
            if (isOpen)
            {
                bfs_path.clear();
                bfs_path.shrink_to_fit();
                trial_bfs_path.clear();
                trial_bfs_path.shrink_to_fit();
                isbfs = BFS();
            }
            else
                cout << "Error: FILE OPEN FIRST" << endl;
        }

        if (key == 'j') {
            isdfs = false;
            isbfs = false;
            if (isOpen)
            {
                dijkstra_path.clear();
                dijkstra_path.shrink_to_fit();
                trial_dijkstra_path.clear();
                trial_dijkstra_path.shrink_to_fit();
                isdijkstra = Dijkstra();
            }
            else
                cout << "Error: FILE OPEN FIRST" << endl;
        }

        if (key == OF_KEY_LEFT) {
            // 왼쪽 화살표 키가 눌렸을 때
            if (start.second > 0 && maze_input[start.first][start.second - 1] != '|') {
                // 현재 위치가 미로의 왼쪽 끝이 아니고, 왼쪽에 벽이 없을 때
                start.second--; // 플레이어를 왼쪽으로 이동
                score++; // 점수 증가
                if (isMonsterCaught()) {
                    // 몬스터를 잡았을 때 추가 점수
                    score += 5;
                    hasCaughtMonster = true; // 몬스터 잡힘 상태 설정
                }
            }
        }
        if (key == OF_KEY_RIGHT) {
            // 오른쪽 화살표 키가 눌렸을 때
            if (start.second < maze_input[0].size() - 1 && maze_input[start.first][start.second + 1] != '|') {
                // 현재 위치가 미로의 오른쪽 끝이 아니고, 오른쪽에 벽이 없을 때
                start.second++; // 플레이어를 오른쪽으로 이동
                score++; // 점수 증가
                if (isMonsterCaught()) {
                    // 몬스터를 잡았을 때 추가 점수
                    score += 5;
                    hasCaughtMonster = true; // 몬스터 잡힘 상태 설정
                }
            }
        }
        if (key == OF_KEY_UP) {
            // 위쪽 화살표 키가 눌렸을 때
            if (start.first > 0 && maze_input[start.first - 1][start.second] != '-') {
                // 현재 위치가 미로의 위쪽 끝이 아니고, 위쪽에 벽이 없을 때
                start.first--; // 플레이어를 위로 이동
                score++; // 점수 증가
                if (isMonsterCaught()) {
                    // 몬스터를 잡았을 때 추가 점수
                    score += 5;
                    hasCaughtMonster = true; // 몬스터 잡힘 상태 설정
                }
            }
        }
        if (key == OF_KEY_DOWN) {
            // 아래쪽 화살표 키가 눌렸을 때
            if (start.first < maze_input.size() - 1 && maze_input[start.first + 1][start.second] != '-') {
                // 현재 위치가 미로의 아래쪽 끝이 아니고, 아래쪽에 벽이 없을 때
                start.first++; // 플레이어를 아래로 이동
                score++; // 점수 증가
                if (isMonsterCaught()) {
                    // 몬스터를 잡았을 때 추가 점수
                    score += 5;
                    hasCaughtMonster = true; // 몬스터 잡힘 상태 설정
                }
            }
        }

        // 플레이어가 종료 지점에 도달했을 때
        if (start == end) {
            gameEnded = true; // 게임 종료 설정
            endTime = ofGetElapsedTimef(); // 종료 시간 기록
        }
    }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
bool ofApp::readFile()
{
    ofFileDialogResult openFileResult = ofSystemLoadDialog("Select .maz file");
    string filePath;
    size_t pos;
    // Check whether the user opened a file
    if (openFileResult.bSuccess) {
        ofLogVerbose("User selected a file");

        //We have a file, check it and process it
        string fileName = openFileResult.getName();
        //string fileName = "maze0.maz";
        cout << "file name is " << fileName << "\n";
        filePath = openFileResult.getPath();
        printf("Open\n");
        pos = filePath.find_last_of(".");
        if (pos != string::npos && pos != 0 && filePath.substr(pos + 1) == "maz") {

            ofFile file(fileName);

            if (!file.exists()) {
                cout << "Target file does not exists." << endl;
                return false;
            }
            else {
                cout << "We found the target file." << endl;
                isOpen = 1;
            }

            ofBuffer buffer(file);

            // Input_flag is a variable for indication the type of input.
            // If input_flag is zero, then work of line input is progress.
            // If input_flag is one, then work of dot input is progress.
            int input_flag = 0;

            // Idx is a variable for index of array.
            int idx = 0;

            // Read file line by line
            int cnt = 0;


            // TO DO
            // .maz 파일을 input으로 받아서 적절히 자료구조에 넣는다
            freeMemory(); // maze_input 벡터의 메모리를 해제 

            auto lines = buffer.getLines();
            //buffer 객체에서 라인들을 가져온다. 
            // auto 키워드를 이용해 lines의 타입을 compiler가 자동으로 추론하게 한다. 
            maze_input.assign(lines.begin(), lines.end());
            // maze_input 벡터에 lines 벡터의 내용을 할당한다.
            //assign 함수는 lines 벡터의 시작부터 끝까지의 모든 요소를 maze_input에 복사한다.
            // 결론: maze_input 벡터는 buffer의 모든 라인들로 채워지게 된다.

            return true;
        }
        else {
            printf("  Needs a '.maz' extension\n");
            return false;
        }
    }
}
void ofApp::freeMemory() {

    //TO DO
    // malloc한 memory를 free해주는 함수

    maze_input.clear();
    maze_input.shrink_to_fit();
    dfs_path.clear();
    dfs_path.shrink_to_fit();
    trial_path.clear();
    trial_path.shrink_to_fit();
    bfs_path.clear();
    bfs_path.shrink_to_fit();
    trial_bfs_path.clear();
    trial_bfs_path.shrink_to_fit();
    dijkstra_path.clear();
    dijkstra_path.shrink_to_fit();
    trial_dijkstra_path.clear();
    trial_dijkstra_path.shrink_to_fit();

    isdfs = false;
    isbfs = false;
    isdijkstra = false;
}

bool ofApp::Dijkstra() {
    int rows = maze_input.size();
    int cols = maze_input[0].size();

    // 거리와 이전 위치를 저장할 벡터 초기화
    vector<vector<int>> distances(rows, vector<int>(cols, INT_MAX));
    vector<vector<pair<int, int>>> predecessors(rows, vector<pair<int, int>>(cols, { -1, -1 }));

    // 우선순위 큐 초기화
    priority_queue<pair<int, pair<int, int>>, vector<pair<int, pair<int, int>>>, greater<>> pq;

    int startX = 1, startY = 1;
    distances[startY][startX] = 0;
    pq.emplace(0, make_pair(startY, startX));

    // 이동 방향 설정 (오른쪽, 왼쪽, 아래, 위)
    vector<int> dx = { 1, -1, 0, 0 };
    vector<int> dy = { 0, 0, 1, -1 };

    while (!pq.empty()) {
        auto [dist, pos] = pq.top();
        auto [y, x] = pos;
        pq.pop();

        if (dist > distances[y][x]) {
            continue;
        }

        // 4방향으로 이동 시도
        for (int i = 0; i < 4; ++i) {
            int newX = x + dx[i];
            int newY = y + dy[i];

            // 유효한 위치인지와 벽이 아닌지 확인
            if (newX >= 0 && newX < cols && newY >= 0 && newY < rows && maze_input[newY][newX] != '|' && maze_input[newY][newX] != '-') {
                int newDist = dist + 1;
                if (newDist < distances[newY][newX]) {
                    distances[newY][newX] = newDist;
                    predecessors[newY][newX] = { y, x };
                    pq.emplace(newDist, make_pair(newY, newX));
                }
            }
        }
    }

    // 최단 경로 추적
    int end_X = cols - 2, end_Y = rows - 2;
    pair<int, int> current = { end_Y, end_X };
    while (current != make_pair(startY, startX)) {
        dijkstra_path.push_back(current);
        current = predecessors[current.first][current.second];
        if (current == make_pair(-1, -1)) {
            dijkstra_path = {};
            return false; // 경로가 없음
        }
    }
    dijkstra_path.emplace_back(startY, startX);
    reverse(dijkstra_path.begin(), dijkstra_path.end());
    return true;
}

void ofApp::dijkstradraw() {
    ofSetLineWidth(8);

    ofSetColor(255, 0, 255);
    for (size_t i = 0; i < dijkstra_path.size() - 1; ++i) {
        ofDrawLine(dijkstra_path[i].second * 10, dijkstra_path[i].first * 10,
            dijkstra_path[i + 1].second * 10, dijkstra_path[i + 1].first * 10);
    }
}

bool ofApp::DFS() {

    // 미로 크기 및 초기화
    int rows = maze_input.size();
    int cols = maze_input[0].size();
    vector<vector<int>> visited(rows, vector<int>(cols, 0));
    // visited는 방문한 셀을 추적하기 위한 2차원 벡터이다.
   // 초기값은 모두 0이다. (방문하지 않음을 의미).


    int dx[4] = { 2, 0, -2, 0 };
    int dy[4] = { 0, 2, 0, -2 };
    //미로가 벽으로 구분돼어 있기 때문에
    //벽을 건너 띄어 통로만 따라 이동하기 위해서
    //2칸씩 이동한다.
    //오른쪽 아래 왼쪽 위

    int startX = 1, startY = 1;
    dfs_path.push_back({ startY, startX });
    trial_path.push_back({ startY, startX });
    visited[startY][startX] = 1;

    //시작을 1,1 로 setting하고 현재 겨로와 시동중인 경로를 추적한다.
    // 시작 지점 1로 설정하며 방문 처리

    //DFS loop
    while (!dfs_path.empty()) {
        int x = dfs_path.back().second;
        int y = dfs_path.back().first;

        // dfs_path.back()을 통해 스택의 마지막 요소 반환.
        // 현재 탐색 중인 위치 저장
        trial_path.push_back({ y, x });
        // 현재까지 탐색한 경로를 기록


        if (y == rows - 2 && x == cols - 2) {
            return true;
        }
        // 현재 위치가 미로의 마지막 경로를 찾으면 true 반환

        bool moved = false;
        for (int i = 0; i < 4; ++i) {
            int newX = x + dx[i];
            int newY = y + dy[i];
            // 4가지 방향으로 이동을 시도
            // 새로운 위치를 계산
            if (newX >= 0 && newX < cols && newY >= 0 && newY < rows && visited[newY][newX] == 0 && maze_input[y + dy[i] / 2][x + dx[i] / 2] != '-' && maze_input[y + dy[i] / 2][x + dx[i] / 2] != '|') {
                // 새로운 위치가 미로의 범위 내에 있는지.
                // 새로운 위치가 아직 방문되지 않았는지.
                 // 현재 위치와 새로운 위치 사이에 벽이 없는지.
                // 판단
                visited[newY][newX] = 1;
                // 새로운 위치 방문처리
                dfs_path.push_back({ newY, newX });
                //새로운 위치를 추가하여 경로를 업데이트
                moved = true;
                break;
            }
        }


        if (!moved) {
            dfs_path.pop_back();
        }
        // 이동하지 못 했기 때문에 stack의 맨 마지막 요소 제거
    }
    return false;
}

void ofApp::dfsdraw() {
    ofSetLineWidth(8); // Set line width to be thicker for DFS path
    //그릴 선의 두께를 5픽셀로 설정

    ofSetColor(255, 0, 0);
    for (size_t i = 0; i < trial_path.size() - 1; ++i) {
        ofDrawLine(trial_path[i].second * 10, trial_path[i].first * 10,
            trial_path[i + 1].second * 10, trial_path[i + 1].first * 10);
    }
    // 두 점의 자표 계산해서 drawline


    ofSetColor(255, 0, 255);
    for (size_t i = 0; i < dfs_path.size() - 1; ++i) {
        ofDrawLine(dfs_path[i].second * 10, dfs_path[i].first * 10,
            dfs_path[i + 1].second * 10, dfs_path[i + 1].first * 10);
    }
}


bool ofApp::BFS() {
    vector<vector<int>> visited(maze_input.size(), vector<int>(maze_input[0].size(), 0));

    int dx[4] = { 2, 0, -2, 0 };
    int dy[4] = { 0, 2, 0, -2 };
    // 오른쪽 아래 왼쪽 위 순서로 탐색
    int bf_x = 1, bf_y = 1;

    int bfs_index = 0;

    bfs_path.push_back({ bf_y, bf_x });
    trial_bfs_path.push_back({ bf_y, bf_x, -1 });
    visited[1][1] = 1;

    while (!bfs_path.empty()) {
        auto [bf_y, bf_x] = bfs_path.front();
        bfs_path.pop_front();

        if (bf_y == maze_input.size() - 2 && bf_x == maze_input[0].size() - 2) {
            return true;
        }

        for (int i = 0; i < 4; ++i) {
            int new_Y = bf_y + dy[i];
            int new_X = bf_x + dx[i];


            if (new_Y >= 0 && new_Y < maze_input.size() && new_X >= 0 && new_X < maze_input[0].size()) {
                if (visited[new_Y][new_X] == 0 && maze_input[bf_y + dy[i] / 2][bf_x + dx[i] / 2] != '-' && maze_input[bf_y + dy[i] / 2][bf_x + dx[i] / 2] != '|') {
                    visited[new_Y][new_X] = 1;
                    bfs_path.emplace_back(new_Y, new_X);
                    trial_bfs_path.emplace_back(new_Y, new_X, bfs_index);
                }
            }
        }

        ++bfs_index;
    }


    return false;

}


void ofApp::bfsdraw() {
    ofSetLineWidth(8);
    ofSetColor(255, 0, 0);
    for (size_t i = 1; i < trial_bfs_path.size(); ++i) {
        auto [current_Y, current_X, parent_num] = trial_bfs_path[i];
        auto [parentY, parentX, _] = trial_bfs_path[parent_num];
        ofDrawLine(current_X * 10, current_Y * 10, parentX * 10, parentY * 10);

    }

    ofSetColor(255, 0, 255);
    int end_path = trial_bfs_path.size() - 1;

    while (end_path >= 0 &&
        (get<0>(trial_bfs_path[end_path]) != maze_input.size() - 2 || get<1>(trial_bfs_path[end_path]) != maze_input[0].size() - 2)) {
        --end_path;
    }

    while (end_path >= 0 && get<2>(trial_bfs_path[end_path]) != -1) {
        auto [current_Y, current_X, parent_num] = trial_bfs_path[end_path];
        auto [parentY, parentX, _] = trial_bfs_path[parent_num];
        ofDrawLine(current_X * 10, current_Y * 10, parentX * 10, parentY * 10);
        end_path = parent_num;

    }
}

void ofApp::initializeGame() {
    // 게임 초기 설정
    start = { 1, 1 }; // 시작 위치 설정
    end = { maze_input.size() - 2, maze_input[0].size() - 2 }; // 종료 위치 설정
    monster = { maze_input.size() / 2, maze_input[0].size() / 2 }; // 몬스터 초기 위치 설정
    score = 0; // 점수 초기화
    hasCaughtMonster = false; // 몬스터 잡힘 상태 초기화
    startTime = ofGetElapsedTimef(); // 게임 시작 시간 기록
    gameEnded = false; // 게임 종료 상태 초기화
}

void ofApp::moveMonster() {
    // 몬스터를 랜덤하게 움직임
    int direction = ofRandom(4); // 4가지 방향 중 하나를 랜덤 선택
    int dx[4] = { 1, -1, 0, 0 }; // x축 이동 방향 배열
    int dy[4] = { 0, 0, 1, -1 }; // y축 이동 방향 배열
    int new_x = monster.second + dx[direction]; // 새로운 x 위치
    int new_y = monster.first + dy[direction]; // 새로운 y 위치
    if (new_x > 0 && new_x < maze_input[0].size() && new_y > 0 && new_y < maze_input.size() &&
        maze_input[new_y][new_x] != '-' && maze_input[new_y][new_x] != '|') {
        // 새로운 위치가 미로 내에 있고 벽이 아닐 경우에만 몬스터 이동
        monster = { new_y, new_x }; // 몬스터 위치 업데이트
    }
}

bool ofApp::isMonsterCaught() {
    // 플레이어가 몬스터를 잡았는지 확인
    return start == monster;
}

void ofApp::drawMonster() {
    // 몬스터를 그리기
    ofSetColor(0, 0, 255); // 파란색 설정
    ofDrawCircle(monster.second * 10, monster.first * 10, 5); // 몬스터 위치에 원 그리기
}

void ofApp::drawPlayer() {
    // 플레이어를 그리기
    ofSetColor(255, 255, 0); // 노란색 설정
    ofDrawCircle(start.second * 10, start.first * 10, 5); // 플레이어 위치에 원 그리기
}
