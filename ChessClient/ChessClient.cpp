#pragma comment(lib, "Ws2_32.lib")
#include <iostream>
#include <WS2tcpip.h>

#include<GL/glew.h>
#include<GL/freeglut.h>
#include<gl/freeglut_ext.h>

//////////////////////////////////////////////////////////////////////
//// Define 
//////////////////////////////////////////////////////////////////////

#define MAX_BUFFER 1024
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 3500

GLuint window_width = 720;
GLuint window_height = 720;

//---------------------
GLvoid InitChessPoints(GLvoid);
GLvoid DrawScene(GLvoid);
GLvoid DrawChessBoard(GLvoid);
GLvoid DrawChessPin(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(int key, int x, int y);

POINTFLOAT chessBoardPoint[8][8];
POINT chessPinPoint;

SOCKET serverSocket;

void RefreshChessPinPoint();

int main(int argc, char** argv) // 윈도우 출력하고 콜백함수 설정 
{
	std::string si;
	std::cout << "서버 아이피 입력: ";
	std::cin >> si;
	PCSTR serverIp = si.c_str();
	
	// 클라
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	serverSocket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, 0);
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, serverIp, &serverAddr.sin_addr);
	if (connect(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		std::cout << "Connect Error" << std::endl;
	}

	//--- 윈도우 생성하기
	glutInit(&argc, argv);							// glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);	// 디스플레이 모드 설정
	glutInitWindowPosition(0, 0);					// 윈도우의 위치 지정
	glutInitWindowSize(window_width, window_height);	// 윈도우의 크기 지정
	glutCreateWindow("Chess");				// 윈도우 생성 (윈도우 이름)

	//--- GLEW 초기화하기
	InitChessPoints();
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)						// glew 초기화
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	//Shader ourShader("Vertex_Shader.glvs", "Fragment_Shader.glfs");
	//float vertices[] = {
	//	0.1f, -0.1f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-right
	//	0.1f, -0.1f, 0.0f, 0.0f, 0.0f, 0.0f, // botton-left
	//	0.1f, -0.1f, 0.0f, 0.0f, 0.0f, 0.0f, // top-left
	//	0.1f, -0.1f, 0.0f, 0.0f, 0.0f, 0.0f // top-right
	//};

	glutDisplayFunc(DrawScene);		// 출력 함수의 지정
	glutReshapeFunc(Reshape);		// 다시 그리기 함수 지정

	//--------------------------------
	// 추가 코드
	glutSpecialFunc(Keyboard);

	//--------------------------------

	glutMainLoop();					// 이벤트 처리 시작

	closesocket(serverSocket);
	WSACleanup();
}

GLvoid InitChessPoints() {
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			POINTFLOAT p = { (float)(-0.7 + j * 0.2), (float)(0.7 - i * 0.2) };
			chessBoardPoint[i][j] = p;
		}
	}

	chessPinPoint = { 4, 4 };
}

GLvoid DrawScene() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);	// (r, g, b, 1.0f(알파값))
	glClear(GL_COLOR_BUFFER_BIT);			// 설정된 색으로 전체를 칠하기
	glLoadIdentity();

	// 그리기 부분 구현
	// --- 그리기 관련 부분이 여기에 포함된다
	DrawChessBoard();
	DrawChessPin();

	// 그리기 끝

	glutSwapBuffers();								// 화면에 출력하기
}

GLvoid DrawChessBoard() {
	glLineWidth(1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	bool isBlack = false;
	for (int i = 0; i < 8; ++i) {
		isBlack = !isBlack;
		for (int j = 0; j < 8; ++j) {
			isBlack = !isBlack;
			(isBlack) ? (glColor3f(0.0f, 0.0f, 0.0f)) : (glColor3f(1.0f, 1.0f, 1.0f));
			glRectf(chessBoardPoint[i][j].x - 0.1f, chessBoardPoint[i][j].y - 0.1f, chessBoardPoint[i][j].x + 0.1f, chessBoardPoint[i][j].y + 0.1f);
		}
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f((1.0f) / 255 * 150, (1.0f) / 255 * 75, 0.0);
	glRectf(-0.8f, -0.8f, 0.8f, 0.8f);
	//glColor3f((1.0f)/255 * 150, (1.0f)/255 * 75, 0.0 );
}

void DrawChessPin() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(0.0f, 0.0f, 0.0f);

	POINTFLOAT p = chessBoardPoint[chessPinPoint.x][chessPinPoint.y];
	glRectf(p.x - 0.05f, p.y - 0.05f, p.x + 0.05f, p.y + 0.05f);

	glEnable(GL_POINT_SMOOTH);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(2.0f);
	glColor3f((1.0f) / 255 * 0, (1.0f) / 255 * 255, 1.0f);
	glRectf(p.x - 0.05f, p.y - 0.05f, p.x + 0.05f, p.y + 0.05f);
	glDisable(GL_POINT_SMOOTH);
}

GLvoid Reshape(int w, int h) // 콜백 함수: 다시 그리기 
{
	glViewport(0, 0, window_width, window_height);
}

GLvoid Keyboard(int key, int x, int y) {

	char messageBuffer[MAX_BUFFER + 1];
	std::string data;

	switch (key)
	{
	case GLUT_KEY_LEFT:
		data = "LEFT";
		//(chessPinPoint.y > 0) ? (chessPinPoint.y -= 1) : (chessPinPoint.y = 0);
		break;

	case GLUT_KEY_RIGHT:
		data = "RIGHT";
		//(chessPinPoint.y < 7) ? (chessPinPoint.y += 1) : (chessPinPoint.y = 7);
		break;

	case GLUT_KEY_UP:
		data = "UP";
		//(chessPinPoint.x > 0) ? (chessPinPoint.x -= 1) : (chessPinPoint.x = 0);
		break;

	case GLUT_KEY_DOWN:
		data = "DOWN";
		//(chessPinPoint.x < 7) ? (chessPinPoint.x += 1) : (chessPinPoint.x = 7);
		break;
	}

	strcpy_s(messageBuffer, MAX_BUFFER, data.c_str());

	int bufferLen = static_cast<int>(strlen(messageBuffer));
	if (bufferLen == 0) return;

	int sendBytes = send(serverSocket, messageBuffer, bufferLen + 1, 0);
	std::cout << "Sent: " << messageBuffer << "(" << sendBytes << " bytes)" << std::endl;

	int len;
	int receiveBytes = recv(serverSocket, (char*)&len, sizeof(int), 0);
	receiveBytes = recv(serverSocket, messageBuffer, len, 0);
	POINT* temp;
	temp = (POINT*)messageBuffer;

	chessPinPoint.x = temp->x;
	chessPinPoint.y = temp->y;

	glutPostRedisplay();
}

void RefreshChessPinPoint()
{
	char messageBuffer[MAX_BUFFER + 1];
	std::string data = "POINT";
	strcpy_s(messageBuffer, MAX_BUFFER, data.c_str());

	int bufferLen = static_cast<int>(strlen(messageBuffer));
	if (bufferLen == 0) return;

	int sendBytes = send(serverSocket, messageBuffer, bufferLen + 1, 0);
	std::cout << "Sent: " << messageBuffer << "(" << sendBytes << " bytes)" << std::endl;
}
