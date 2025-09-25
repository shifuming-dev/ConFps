// ConFps.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
#include <chrono>
using namespace std;

int nScreenWidth = 120; // Console screen width
int nScreenHeight = 40; // Console screen height

float fPlayerX = 8.0f; // Player start position X
float fPlayerY = 8.0f; // Player start position Y
float fPlayerA = 0.0f; // PlayerA 

int nMapWidth = 16; // Map width
int nMapHeight = 16; // Map height

float fFOV = 3.14159f / 4.0f; // Field of view
float fDepth = 16.0f; // Maximum rendering distance

int main()
{
	// ===== 统一用hConsole句柄（活动缓冲区）=====
	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole); // 激活当前缓冲区
	DWORD dwBytesWritten = 0;

	// 用hConsole设置窗口尺寸
	// 1. 设置缓冲区大小（必须 ≥ 窗口大小）
	SetConsoleScreenBufferSize(hConsole, { (SHORT)nScreenWidth, (SHORT)nScreenHeight });
	// 2. 设置窗口可视区域（0,0 到..，对应列×行）
	SMALL_RECT windowRect = { 0, 0, (SHORT)(nScreenWidth - 1), (SHORT)(nScreenHeight - 1) };
	SetConsoleWindowInfo(hConsole, TRUE, &windowRect);
	// 3. 设置窗口标题
	SetConsoleTitle(L"FPS");

	wstring map;

	map += L"################";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"################";

	auto tp1 = chrono::system_clock::now();
	auto tp2 = chrono::system_clock::now();

	while (1)
	{
		tp2 = chrono::system_clock::now();
		chrono::duration<float> elapsedTime = tp2 - tp1;
		tp1 = tp2;
		float fElapsedTime = elapsedTime.count();
		if(GetAsyncKeyState((unsigned short)'A') & 0x8000 || GetAsyncKeyState((unsigned short)VK_LEFT) & 0x8000)
			fPlayerA -= (0.1f) *  fElapsedTime; // Rotate left

		if (GetAsyncKeyState((unsigned short)'D') & 0x8000 || GetAsyncKeyState((unsigned short)VK_RIGHT) & 0x8000)
			fPlayerA += (0.1f) * fElapsedTime; // Rotate right

		if (GetAsyncKeyState((unsigned short)'W') & 0x8000);


		for(int x=0;x<nScreenWidth;x++)
		{
			float fRayAngle = (fPlayerA - fFOV / 2.0f) + (float(x) / float(nScreenWidth))* fFOV;
			float fDistanceToWall = 0;
			bool bHitWall = false;

			float fEyeX = sinf(fRayAngle); // Unit vector for ray in player space
			float fEyeY = cosf(fRayAngle);
			// 1. 逐步延伸射线，直到碰到墙壁

			while (!bHitWall && fDistanceToWall <fDepth)
			{
				fDistanceToWall += 0.1f;
				int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
				int nTestY = (int)(fPlayerY + fEyeY * fDistanceToWall);
				// Test if ray is out of bounds
				if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight)
				{
					bHitWall = true; // Just set distance to maximum depth
					fDistanceToWall = 16.0f;
				}
				else
				{
					// Ray is inbounds so test to see if the ray cell is a wall block
					if (map[nTestY * nMapWidth + nTestX] == '#')
					{
						bHitWall = true;
					}
				}
			}
			// 2. 计算墙壁高度
			int nCeiling = (float(nScreenHeight / 2.0) - nScreenHeight / ((float)fDistanceToWall));
			int nFloor = nScreenHeight - nCeiling;

			short nShade = ' ';

			if(fDistanceToWall<=fDepth/4.0f)       nShade=0x2588; // Very close
			else if(fDistanceToWall<fDepth/3.0f)   nShade=0x2593;
			else if(fDistanceToWall<fDepth/2.0f)   nShade=0x2592;
			else if(fDistanceToWall<fDepth)        nShade=0x2591;
			else                                   nShade = ' ';    // Too far away

			for(int y=0;y<nScreenHeight;y++)
			{
				if(y<=nCeiling)
					screen[y*nScreenWidth+x]=' ';
				else if(y>nCeiling && y<=nFloor)
					screen[y*nScreenWidth+x]= nShade;
				else
				{
					float b = 1.0f - (((float)y - nScreenHeight / 2.0f) / ((float)nScreenHeight / 2.0f));
					if (b < 0.25)       screen[y * nScreenWidth + x] = '#';
					else if (b < 0.5)  screen[y * nScreenWidth + x] = 'x';
					else if (b < 0.75) screen[y * nScreenWidth + x] = '.';
					else if (b < 0.9)  screen[y * nScreenWidth + x] = '-';
					else               screen[y * nScreenWidth + x] = ' ';
				}
			}
		}


		screen[nScreenWidth * nScreenHeight - 1] = '\0';
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);

	}
	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
