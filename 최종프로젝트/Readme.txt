#20212021 원대호
README

프로젝트 개요
이 프로젝트의 주된 목표는 DFS, BFS, Dijkstra을 사용하여 maze를 탐색하고 그 경로를 추적합니다. 또한 게임 요소를 추가해 플레이어가 maze를 통해 이동하면서 score를 획득하고, 몬스터를 잡는 것이다.

프로젝트 구조
main.cpp: 프로그램의 진입점으로, 전체 게임 루프와 초기 설정을 처리.
ofApp.h 및 ofApp.cpp: OpenFrameworks를 사용한 주요 게임 로직과 알고리즘 구현이 포함된 파일.
ofxWinMenu.h 및 ofxWinMenu.cpp: Windows 환경에서 메뉴를 생성하고 관리하는 기능을 구현한 파일.

실행 방법
OpenFrameworks를 설치한다.
이 project의 file을 OpenFrameworks의 apps/myApps 디렉토리에 복사한다.
프로젝트를 visual studio에서 실행한다.
프로그램을 실행하면 DFS, BFS, Dijkstra에 대한 단축키를 눌러 경로를 탐색해 본다. 경로를 한 번 탐색해 본후 키보드 입력을 사용하여 플레이어를 이동시킨다. 플레이어는 미로를 탐색하며 몬스터를 잡고 탈출한다.
게임이 종료되면 점수와 경과 시간이 표시된다.

환경
운영체제: Windows
개발 도구: Visual Studio
필요한 library: OpenFrameworks