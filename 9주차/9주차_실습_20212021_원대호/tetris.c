#include "tetris.h"

static struct sigaction act, oact;

Node* head = NULL;
Node* tail = NULL;

int main(){
	int exit=0;

	createRankList();

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));

	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}

void InitTetris(){

	for(int j=0;j<HEIGHT;j++)
		for(int i=0;i<WIDTH;i++)
			field[j][i]=0;

	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
	nextBlock[2]=rand()%7;
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();
	DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);
	DrawBox(9,WIDTH+10,4,8);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(15,WIDTH+10);
	printw("SCORE");
	DrawBox(16,WIDTH+10,1,8);

}

int GetCommand(){
	int command;
	command = wgetch(stdscr);

	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){

	for(int j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(int i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(17,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
		move(10+i, WIDTH+13);
		for( j=0; j<4; j++) {
			if(block[nextBlock[2]][0][i][j] == 1) {
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}
	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

/////////////////////////첫주차 실습에서 구현해야 할 함수/////////////////////////

int CheckToMove(char f[HEIGHT][WIDTH], int currentBlock, int blockRotate, int blockY, int blockX) {
	// user code
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			int y = blockY + i;
			int x = blockX + j;
			if (block[currentBlock][blockRotate][i][j] == 1) {
				if (y < 0 || y >= HEIGHT || x < 0 || x >= WIDTH || f[y][x] == 1) {
					return 0;
				}
			}
		}
	}
	return 1;

}

void DrawChange(char f[HEIGHT][WIDTH], int command, int currentBlock, int blockRotate, int blockY, int blockX) {
	// user code
	int p_blockX = blockX, p_blockY = blockY, p_blockRotate = blockRotate;
	//1. 이전 블록 정보를 찾는다. ProcessCommand의 switch문을 참조할 것
	switch (command) {
	case KEY_UP:
		p_blockRotate = (p_blockRotate + 3) % 4;
		break;
	case KEY_DOWN:
		p_blockY = blockY - 1;
		break;
	case KEY_RIGHT:
		p_blockX = blockX - 1;
		break;
	case KEY_LEFT:
		p_blockX = blockX + 1;
		break;
	default:
		break;
	}


	//2. 이전 블록 정보를 지운다. DrawBlock함수 참조할 것.
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (block[currentBlock][p_blockRotate][i][j] == 1 && i + blockY >= 0) {
				move(i + p_blockY + 1, j + p_blockX + 1);
				printw(".");
			}
		}
	}

	//그림자 위치 계산
	int index;
	for (index = p_blockY; index <= HEIGHT; index++) {
		if (CheckToMove(field, currentBlock, p_blockRotate, index + 1, p_blockX) == 0)
			break;
	}
	//그림자 출력
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (block[currentBlock][p_blockRotate][i][j] == 1 && index + i >= 0) {
				move(index + i + 1, p_blockX + j + 1);
				printw(".");
			}
		}
	}

	//3. 새로운 블록 정보를 그린다. 
	DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
	move(HEIGHT, WIDTH + 10);
}



void BlockDown(int sig){
	// user code
	if (CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX)) {
		blockY++;
		DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
	}
	else {
		if(blockY==-1) {
			gameOver = TRUE;
		}
		else {
			score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
			score += DeleteLine(field);
			nextBlock[0] = nextBlock[1];
			nextBlock[1] = rand() % 7;
			DrawNextBlock(nextBlock);

			blockRotate = 0;
			blockY = -1;
			blockX = WIDTH / 2 - 2;
			DrawNextBlock(nextBlock);
			PrintScore(score);
			DrawField();
			DrawBlock(blockY, blockX, nextBlock[0], blockRotate, ' ');
		}
	}
	timed_out=0;

}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int result=0;
	 for(int i=0; i<4; i++){
		 for(int j=0; j<4; j++) {
			if(block[currentBlock][blockRotate][i][j]==1) {
				if (blockY + i == HEIGHT - 1 || f[blockY + i + 1][blockX + j] == 1) {
					result++;
				}
				f[blockY+i][blockX+j]=1;
			}
		}
	}

	return result* 10;

	//Block이 추가된 영역의 필드값을 바꾼다.
}

int DeleteLine(char f[HEIGHT][WIDTH]) {
	// user code
	int d_flag = 1, line = 0;
	//1. 필드를 탐색하여, 꽉 찬 구간이 있는지 탐색한다.
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			if (f[i][j] == 0) {
				d_flag = 0;
				break;
			}
		}
		if (d_flag == 1) {
			line++;
			for (int k = i; k > 0; k--) {
				for (int l = 0; l < WIDTH; l++) {
					f[k][l] = f[k - 1][l];
				}
			}
			for (int l = 0; l < WIDTH; l++) {
				f[0][l] = 0;
			}
		}
		d_flag = 1;
	}
	//2. 꽉 찬 구간이 있으면 해당 구간을 지운다. 즉, 해당 구간으로 필드값을 한칸씩 내린다.
	return (line * line) * 100;
}

///////////////////////////////////////////////////////////////////////////
void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate) {
	DrawShadow(y,x,blockID,blockRotate);
	DrawBlock(y,x,blockID,blockRotate,' ');
}


void DrawShadow(int y, int x, int blockID, int blockRotate) {
	// user code
	int tmp= 0;
	while (CheckToMove(field, nextBlock[0], blockRotate, tmp + y + 1, x)) {
		tmp++;
	}
	DrawBlock(tmp + y, x, blockID, blockRotate, '/');
}

void createRankList() {
	// 목적: Input파일인 "rank.txt"에서 랭킹 정보를 읽어들임, 읽어들인 정보로 랭킹 목록 생성
	// 1. "rank.txt"열기
	// 2. 파일에서 랭킹정보 읽어오기
	// 3. LinkedList로 저장
	// 4. 파일 닫기
	FILE* fp;
	int i, j;

	//1. 파일 열기
	fp = fopen("rank.txt", "r");
	if (fp == NULL) {
		return;
	}

	// 2. 정보읽어오기
	/* int fscanf(FILE* stream, const char* format, ...);
	stream:데이터를 읽어올 스트림의 FILE 객체를 가리키는 파일포인터
	format: 형식지정자 등등
	변수의 주소: 포인터
	return: 성공할 경우, fscanf 함수는 읽어들인 데이터의 수를 리턴, 실패하면 EOF리턴 */
	// EOF(End Of File): 실제로 이 값은 -1을 나타냄, EOF가 나타날때까지 입력받아오는 if문
	if (fscanf(fp, "%d", &score_number) == EOF) {
		score_number = 0;
	}

	for (i = 0; i < score_number; i++) {
		char* name = malloc(sizeof(char) * 17);
		int score;
		if (fscanf(fp, "%16s %d", name, &score) == 2) {
			Node* new_node = malloc(sizeof(Node));
			if (new_node == NULL) {
				perror("Failure: Memory allocation");
				exit(EXIT_FAILURE);
			}
			strcpy(new_node->name, name);
			new_node->score = score;
			new_node->next = NULL;
			if (head == NULL) {
				head = new_node;
			}
			else {
				tail->next = new_node;
			}
			tail = new_node;
		}
		else {
			fprintf(stderr, "Error: Reading Data from File\n");
			exit(EXIT_FAILURE);
		}
		free(name);
	}
	// 4. 파일닫기
	fclose(fp);
}

void rank() {
	//목적: rank 메뉴를 출력하고 점수 순으로 X부터~Y까지 출력함
	//1. 문자열 초기화
	int X = 1, Y = score_number, ch, i, j, count = 0;
	clear();

	//2. printw()로 3개의 메뉴출력
	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");

	//3. wgetch()를 사용하여 변수 ch에 입력받은 메뉴번호 저장
	ch = wgetch(stdscr);

	//4. 각 메뉴에 따라 입력받을 값을 변수에 저장
	//4-1. 메뉴1: X, Y를 입력받고 적절한 input인지 확인 후(X<=Y), X와 Y사이의 rank 출력
	if (ch == '1') {
		echo();
		printw("X: ");
		scanw("%d", &X);
		noecho();
		echo();
		printw("Y: ");
		scanw("%d", &Y);
		noecho();
		if (Y <= X || Y > score_number || X < 1) {
			printw("search failure: no rank in the list");
		
		}
		else {
			count = 0;
			Node* ptr = head;
			printw("      name     |   score\n");
			printw("----------------------------\n");

			for (i = 1; i < X && ptr != NULL; i++) {
				ptr = ptr->next;
			}

			for (i = X; i <= Y && ptr != NULL; i++) {
				count++;
				printw("%-16s| %d\n", ptr->name, ptr->score);
				ptr = ptr->next;
			}
		}
	}

	//4-2. 메뉴2: 문자열을 받아 저장된 이름과 비교하고 이름에 해당하는 리스트를 출력
	else if (ch == '2') {

	}

	//4-3. 메뉴3: rank번호를 입력받아 리스트에서 삭제
	else if (ch == '3') {
	}
	getch();

}

void writeRankFile() {
	// 목적: 추가된 랭킹 정보가 있으면 새로운 정보를 "rank.txt"에 쓰고 없으면 종료
	int sn, i;
	//1. "rank.txt" 연다
	FILE* fp = fopen("rank.txt", "w");

	//2. 랭킹 정보들의 수를 "rank.txt"에 기록
	fprintf(fp, "%d\n", score_number);
	//fwrite("\n", sizeof(char), 1, fp);
	//3. 탐색할 노드가 더 있는지 체크하고 있으면 다음 노드로 이동, 없으면 종료
	Node* ptr = head;
	while (ptr != NULL) {
		fprintf(fp, "%s %d\n", ptr->name, ptr->score);
		ptr = ptr->next;
	}

	fclose(fp);
}

void newRank(int score) {
	char str[NAMELEN + 1];
	clear();
	echo();
	printw("your name: ");
	scanw("%s", str);
	noecho();

	Node* New_node = malloc(sizeof(Node));
	if (New_node == NULL) {
		perror("Memory allocation failed");
		exit(EXIT_FAILURE);
	}
	strcpy(New_node->name, str);
	New_node->score = score;
	New_node->next = NULL;


	if (head== NULL) {
		head = New_node;
		tail = New_node;
	}
	else {
		Node* curr = head;
		Node* prev = NULL;

		while (curr != NULL && curr->score >= score) {
			prev = curr;
			curr = curr->next;
		}

		if (prev == NULL) {
			New_node->next = head;
			head = New_node;
		}
		else {
			prev->next = New_node;
			New_node->next = curr;
		}
	}

	score_number++;
	writeRankFile();
}


void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
}

int recommend(RecNode *root){
	//user code
}

void recommendedPlay(){
	// user code
}
