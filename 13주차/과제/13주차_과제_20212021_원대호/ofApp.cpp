/*

	ofxWinMenu basic example - ofApp.cpp

	Example of using ofxWinMenu addon to create a menu for a Microsoft Windows application.

	Copyright (C) 2016-2017 Lynn Jarvis.

	https://github.com/leadedge

	http://www.spout.zeal.co

	=========================================================================
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
	=========================================================================

	03.11.16 - minor comment cleanup
	21.02.17 - rebuild for OF 0.9.8

*/
#include "ofApp.h"
#include <iostream>
using namespace std;

vector<string> maze_input;
vector<pair<int, int>> dfs_path;
vector<pair<int, int>> trial_path;

deque<pair<int, int>> bfs_path;
deque<tuple<int, int, int>> trial_bfs_path;

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetWindowTitle("Maze Example"); // Set the app name on the title bar
	ofSetFrameRate(15);
	ofBackground(255, 255, 255);
	// Get the window size for image loading
	windowWidth = ofGetWidth();
	windowHeight = ofGetHeight();
	isdfs = false;
	isbfs = false;
	isOpen = 0;
	// Centre on the screen
	ofSetWindowPosition((ofGetScreenWidth() - windowWidth) / 2, (ofGetScreenHeight() - windowHeight) / 2);

	// Load a font rather than the default
	myFont.loadFont("verdana.ttf", 12, true, true);

	// Load an image for the example
	//myImage.loadImage("lighthouse.jpg");

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
// The the title and state can be checked for required action.
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

	if (title == "Full screen") {
		bFullscreen = !bFullscreen; // Not auto-checked and also used in the keyPressed function
		doFullScreen(bFullscreen); // But als take action immediately
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

}


//--------------------------------------------------------------
void ofApp::draw() {

	char str[256];
	//ofBackground(0, 0, 0, 0);
	ofSetColor(100);
	ofSetLineWidth(5);

	// TO DO : DRAW MAZE; 
	// 저장된 자료구조를 이용해 미로를 그린다.
	// add code here
	//cout << maze_input.size();

	int i = 0;
	while (i < maze_input.size()) {
		int j = 0;
		while (j < maze_input[i].size()) {
			switch (maze_input[i][j]) {
			case '-':
				ofDrawLine((j - 1) * 10, i * 10, (j + 1) * 10, i * 10);
				break;
			case '|':
				ofDrawLine(j * 10, (i - 1) * 10, j * 10, (i + 1) * 10);
				break;
			default:
				break;
			}
			++j;
		}
		++i;
	}

	// 2차원 배열(maze_input)의 각 요소를 순회하면서 특정 문자('-'와 '|')를 찾고, 해당 문자에 따라 line을 그린다.


	if (isdfs)
	{
		ofSetColor(200);
		ofSetLineWidth(5);
		if (isOpen) {
			//cout << "we can draw!\n";
			dfsdraw();
		}
		else
			cout << "You must open file first" << endl;
	}
	if (isbfs) {
		ofSetColor(200);
		ofSetLineWidth(5);
		if (isOpen) {
			//cout << "we can draw!\n";
			bfsdraw();
		}
		else
			cout << "You must open file first" << endl;
	}
	if (bShowInfo) {
		// Show keyboard duplicates of menu functions
		ofSetColor(200);
		sprintf(str, " comsil project");
		myFont.drawString(str, 15, ofGetHeight() - 20);
	}

} // end Draw


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
		if (isOpen)
		{
			dfs_path.clear();
			dfs_path.shrink_to_fit();
			trial_path.clear();
			trial_path.shrink_to_fit();
			isdfs = DFS();
		}
		else
			cout <<  "Error: FILE OPEN FIRST" << endl;
	}

	if (key == 'b') {
		isdfs = false;
		if (isOpen)
		{
			bfs_path.clear();
			bfs_path.shrink_to_fit();
			trial_bfs_path.clear();
			trial_bfs_path.shrink_to_fit();
			isbfs = BFS();
		}
		else
			cout <<  "Error: FILE OPEN FIRST" << endl;
	}
} // end keyPressed

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

	isdfs = false;
	isbfs = false;
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
	ofSetLineWidth(5);
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
	ofSetLineWidth(5);
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
