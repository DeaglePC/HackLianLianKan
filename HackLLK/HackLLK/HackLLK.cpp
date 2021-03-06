// HackLLK.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include <vector>
#include <map>
#include "CProcessBitmap.h"

//#define MY_TEST
//#define MY_DEBUG

const char* DEAGLEPC[8] = {
	" _____                   _      _____   _____ ",
	"|  __ \\                 | |    |  __ \\ / ____|",
	"| |  | | ___  __ _  __ _| | ___| |__) | |     ",
	"| |  | |/ _ \\/ _` |/ _` | |/ _ \\  ___/| |     ",
	"| |__| |  __/ (_| | (_| | |  __/ |    | |____ ",
	"|_____/ \\___|\\__,_|\\__, |_|\\___|_|     \\_____|",
	"                    __/ |                     ",
	"                   |___/                      "};

// QQ游戏 - 连连看角色版
#define GAME_TITLE "QQ游戏 - 连连看角色版"

int g_nSleepTime = 0;

// 游戏绘图信息
// 游戏块相对于游戏窗口的位置
const POINT GAME_PLAY_POS = { 15,180 };
const unsigned int BLOCK_WIDTH = 30;
const unsigned int BLOCK_HEIGHT = 34;
const unsigned int BLOCK_ROW = 11;
const unsigned int BLOCK_COLUMN = 19;
const unsigned int GAME_LINE = 1;
const unsigned int GAME_PLAY_WINDOW_WIDTH = BLOCK_WIDTH * BLOCK_COLUMN + BLOCK_COLUMN - GAME_LINE;	// 588
const unsigned int GAME_PLAY_WINDOW_HEIGHT = BLOCK_HEIGHT * BLOCK_ROW + BLOCK_ROW - GAME_LINE;		// 384


// 连连看游戏窗口句柄
HWND g_hWnd = 0;
int g_nGameWindowWidth;
int g_nGameWindowHeight;

HBITMAP g_h2BlockBitmap[BLOCK_ROW][BLOCK_COLUMN];
int g_n2GameMatrix[BLOCK_ROW][BLOCK_COLUMN] = { 0 };

// 只保留类型不同的
std::vector<HBITMAP> g_hBlockBitmaps;
std::map<HBITMAP, int> g_mapBlockType;

HWND GetGameWindow()
{
	return FindWindow(NULL, GAME_TITLE);
}

// 截取游戏图像
HBITMAP GetGameBitmap()
{
	RECT rect;
	::GetWindowRect(g_hWnd, &rect);
	g_nGameWindowWidth = rect.right - rect.left;
	g_nGameWindowHeight = rect.bottom - rect.top;

	return CProcessBitmap::GetWindowBitmap(g_hWnd);
}

// 得到每个小方块
void GenerateBlockBitmap()
{
	HBITMAP hBitmapGame = GetGameBitmap();
	HBITMAP hPlayBitmap = CProcessBitmap::GetCutBitmap(g_hWnd, hBitmapGame,
		GAME_PLAY_POS.x, GAME_PLAY_POS.y, GAME_PLAY_WINDOW_WIDTH, GAME_PLAY_WINDOW_HEIGHT);

	for (int i = 0; i < BLOCK_COLUMN; ++i)
	{
		for (int j = 0; j < BLOCK_ROW; ++j)
		{
			int x = i == 0 ? 0 : i * BLOCK_WIDTH + i;
			int y = j == 0 ? 0 : j * BLOCK_HEIGHT + j;
			// 微调，去掉边缘5px
			x += 5;
			y += 5;
			int width = BLOCK_WIDTH - 10;
			int height = BLOCK_HEIGHT - 10;
			HBITMAP hBlockBitmap = CProcessBitmap::GetCutBitmap(g_hWnd, hPlayBitmap, x, y, width, height);
			g_h2BlockBitmap[j][i] = hBlockBitmap;
		}
	}
	DeleteObject(hBitmapGame);
	DeleteObject(hPlayBitmap);
}

// 检查是否存在于全局变量里面
bool IsExistBlockBitmap(HBITMAP hBitmap)
{
	for (size_t i = 0; i < g_hBlockBitmaps.size(); ++i)
	{
		if (CProcessBitmap::IsBitmapEqual(g_hWnd, g_hBlockBitmaps[i], hBitmap))
		{
			return true;
		}
	}
	return false;
}

// 为每种方块生成一个值
int GenerateBlockType()
{
	//HBITMAP hEmptyBlockBitmap = (HBITMAP)LoadImage(NULL, "empty.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	//HBITMAP hEmptyBlockBitmap = (HBITMAP)LoadBitmap((HINSTANCE)GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP_EMPTY));
	//if (hEmptyBlockBitmap == NULL)
	//{
	//	printf("Load empty image error!\n");
	//	return -1;
	//}

	//g_hBlockBitmaps.push_back(hEmptyBlockBitmap);
	int nType = 1;
	//g_mapBlockType[hEmptyBlockBitmap] = nType++;
	for (int i = 0; i < BLOCK_ROW; ++i)
	{
		for (int j = 0; j < BLOCK_COLUMN; ++j)
		{
			HBITMAP hBitmapCurrent = g_h2BlockBitmap[i][j];
			if (CProcessBitmap::IsAllSameRGB(g_hWnd,hBitmapCurrent))
			{
				g_hBlockBitmaps.push_back(hBitmapCurrent);
				g_mapBlockType[hBitmapCurrent] = 0;
			}
			else if (!IsExistBlockBitmap(hBitmapCurrent))
			{
				g_hBlockBitmaps.push_back(hBitmapCurrent);
				g_mapBlockType[hBitmapCurrent] = nType++;
			}
#ifdef MY_DEBUG
			if (nType - 1 == 1)
			{
				// size
				BITMAP bmpTmp;
				GetObject(g_h2BlockBitmap[i][j], sizeof(BITMAP), &bmpTmp);
				printf("empty bitmap size: %d %d\n", bmpTmp.bmWidth, bmpTmp.bmHeight);
				GetObject(g_h2BlockBitmap[i][j], sizeof(BITMAP), &bmpTmp);
				printf("now bitmap size: %d %d\n", bmpTmp.bmWidth, bmpTmp.bmHeight);

				// draw
				HWND hWndTmp = (HWND)0x00010168;
				HDC hDcTmp = GetDC(hWndTmp);
				HDC hMemDc = CreateCompatibleDC(hDcTmp);
				SelectObject(hMemDc, g_h2BlockBitmap[i][j]);
				BitBlt(hDcTmp, 600, 10, 30, 30, hMemDc, 0, 0, SRCCOPY);
				DeleteObject(hMemDc);
			}
#endif // MY_DEBUG
		}
	}
	return nType;
}

void DestroyBitmap()
{
	for (int i = 0; i < BLOCK_ROW; ++i)
	{
		for (int j = 0; j < BLOCK_COLUMN; ++j)
		{
			DeleteObject(g_h2BlockBitmap[i][j]);
		}
	}
	g_hBlockBitmaps.clear();
	g_mapBlockType.clear();
}

int GetBlockTypeFromHbitmap(HBITMAP hBitmap)
{
	for (size_t i = 0; i < g_hBlockBitmaps.size(); i++)
	{
		if (CProcessBitmap::IsBitmapEqual(g_hWnd,hBitmap,g_hBlockBitmaps[i]))
		{
			return g_mapBlockType[g_hBlockBitmaps[i]];
		}
	}
	printf("error: unknow block!\n");
	return 0;
}

void GenerateMatrix()
{
	GenerateBlockBitmap();
	int ret = GenerateBlockType();
	printf("%d\n", ret);

	for (int i = 0; i < BLOCK_ROW; ++i)
	{
		for (int j = 0; j < BLOCK_COLUMN; ++j)
		{
			g_n2GameMatrix[i][j] = GetBlockTypeFromHbitmap(g_h2BlockBitmap[i][j]);
			printf("%2d ", g_n2GameMatrix[i][j]);
		}
		printf("\n");
	}
}

bool IsHorizontalGood(int x1, int y1, int x2, int y2)
{
	if (x1 != x2)
	{
		return false;
	}
	int left = min(y1, y2);
	int right = max(y1, y2);
	if (right - left == 1)
	{
		return true;
	}
	for (int i = left + 1; i < right; ++i)
	{
		if (g_n2GameMatrix[x1][i] != 0)
		{
			return false;
		}
	}
	return true;
}

bool IsVerticalGood(int x1, int y1, int x2, int y2)
{
	if (y1 != y2)
	{
		return false;
	}
	int top = min(x1, x2);
	int bottom = max(x1, x2);
	if (bottom - top == 1)
	{
		return true;
	}
	for (int i = top + 1; i < bottom; ++i)
	{
		if (g_n2GameMatrix[i][y1] != 0)
		{
			return false;
		}
	}
	return true;
}

bool IsOneTurnGood(int x1, int y1, int x2, int y2)
{
	if (x1 == x2 || y1 == y2)
	{
		return false;
	}

	if (g_n2GameMatrix[x1][y2] == 0 && IsHorizontalGood(x1, y1, x1, y2) && IsVerticalGood(x1, y2, x2, y2))
	{
		return true;
	}
	if (g_n2GameMatrix[x2][y1] == 0 && IsHorizontalGood(x2, y1, x2, y2) && IsVerticalGood(x2, y1, x1, y1))
	{
		return true;
	}
	return false;
}

bool IsTwoTurnGood(int x1, int y1, int x2, int y2)
{
	for (int i = 0; i < BLOCK_ROW; ++i)
	{
		for (int j = 0; j < BLOCK_COLUMN; ++j)
		{
			if (g_n2GameMatrix[i][j] != 0)
			{
				continue;
			}
			if (i != x1 && i != x2 && j != y1 && j != y2)
			{
				continue;
			}
			if ((i == x1 && j == y2) || (i == x2 && j == y1))
			{
				continue;
			}
			if (IsOneTurnGood(x1, y1, i, j) && (IsHorizontalGood(i, j, x2, y2) || IsVerticalGood(i, j, x2, y2)))
			{
				return true;
			}
			if (IsOneTurnGood(i, j, x2, y2) && (IsHorizontalGood(i, j, x1, y1) || IsVerticalGood(i, j, x1, y1)))
			{
				return true;
			}
		}
	}
	return false;

	/*
	int x, y;	// cross 1
	int xx, yy;	// cross 2
	// 俩拐点，外侧拐
	int topx = min(x1, x2);
	int bottomx = max(x1, x2);
	for (int i = 0; i < BLOCK_ROW; ++i)
	{
		if (i < topx || i > bottomx)
		{
			x = i, y = y1;
			xx = i, yy = y2;
			if (IsHorizontalGood(x, y, xx, yy) && IsVerticalGood(x, y, x1, y1) && IsVerticalGood(xx, yy, x2, y2))
			{
				return true;
			}
		}
	}
	int lefty = min(y1, y2);
	int righty = max(y1, y2);
	for (int i = 0; i < BLOCK_COLUMN; ++i)
	{
		if (i < lefty || i > righty)
		{
			x = x1, y = i;
			xx = y2, yy = i;
			if (IsVerticalGood(x, y, xx, yy) && IsHorizontalGood(x, y, x1, y1) && IsHorizontalGood(xx, yy, x2, y2))
			{
				return true;
			}
		}
	}

	// 俩拐点，内侧拐
	if ((x1 < x2 && y1 < y2) || (x1 > x2 && y1 > y2))
	{
		int tx1 = min(x1, x2), tx2 = max(x1, x2);
		int ty1 = min(y1, y2), ty2 = max(y1, y2);
		for (int i = tx1 + 1; i < tx2; ++i)
		{
			x = i;
			y = ty1;
			xx = i;
			yy = ty2;

			if (IsHorizontalGood(x, y, xx, yy) && IsVerticalGood(x, y, tx1, ty1) && IsVerticalGood(xx, yy, tx2, ty2))
			{
				return true;
			}
		}
		for (int i = ty1 + 1; i < ty2; ++i)
		{
			x = tx1;
			y = i;
			xx = tx2;
			yy = i;
			if (IsVerticalGood(x, y, xx, yy) && IsHorizontalGood(x, y, tx1, ty1) && IsVerticalGood(xx, yy, tx2, ty2))
			{
				return true;
			}
		}
	}
	else if ((x1 < x2 && y1 > y2) || (x1 > x2 && y1 < y2))
	{
		int tx1 = min(x1, x2), tx2 = max(x1, x2);
		int ty1 = max(y1, y2), ty2 = min(y1, y2);
		for (int i = tx1 + 1; i < tx2; ++i)
		{
			x = i;
			y = ty1;
			xx = i;
			yy = ty2;
			if (IsHorizontalGood(x, y, xx, yy) && IsVerticalGood(x, y, tx1, ty1) && IsHorizontalGood(xx, yy, tx2, ty2))
			{
				return true;
			}
		}
		for (int i = ty1 + 1; i < ty2; ++i)
		{
			x = tx1;
			y = i;
			xx = tx2;
			yy = i;
			if (IsVerticalGood(x, y, xx, yy) && IsHorizontalGood(x, y, tx1, ty1) && IsHorizontalGood(xx, yy, tx2, ty2))
			{
				return true;
			}
		}
	}
	
	return false;
	*/
}

bool IsGood(int x1, int y1, int x2, int y2)
{
	if (x1 == x2 && y1 == y2)
	{
		return false;
	}
	if (g_n2GameMatrix[x1][y1] == 0 || g_n2GameMatrix[x2][y2] == 0)
	{
		return false;
	}
	if (g_n2GameMatrix[x1][y1] != g_n2GameMatrix[x2][y2])
	{
		return false;
	}
	bool bIsGood = IsHorizontalGood(x1, y1, x2, y2) || IsVerticalGood(x1, y1, x2, y2) || 
		IsOneTurnGood(x1, y1, x2, y2) || IsTwoTurnGood(x1, y1, x2, y2);
	if (bIsGood)
	{
		return true;
	}
	return false;
}

void Click(int x, int y)
{
	SendMessage(g_hWnd, WM_LBUTTONDOWN, 0, MAKELPARAM(x, y));
	Sleep(1);
	SendMessage(g_hWnd, WM_LBUTTONUP, 0, MAKELPARAM(x, y));
}

void CleanBlock(int x, int y)
{
	g_n2GameMatrix[x][y] = 0;
	int nPosX = GAME_PLAY_POS.x + y * (BLOCK_WIDTH + GAME_LINE) + 1;
	int nPosY = GAME_PLAY_POS.y + x * (BLOCK_HEIGHT + GAME_LINE) + 1;
	printf("%d %d -- %d %d\n", x, y, nPosX, nPosY);
	Click(nPosX, nPosY);
}

void FindClean()
{
	for (int i = 0; i < BLOCK_ROW; ++i)
	{
		for (int j = 0; j < BLOCK_COLUMN; ++j)
		{
			if (g_n2GameMatrix[i][j] != 0)
			{
				for (int k = 0; k < BLOCK_ROW; ++k)
				{
					for (int l = 0; l < BLOCK_COLUMN; l++)
					{
						if (IsGood(i, j, k, l))
						{
							CleanBlock(i, j);
							CleanBlock(k, l);
							Sleep(g_nSleepTime);
						}
					}
				}
			}
		}
	}
}

bool IsClear()
{
	for (int i = 0; i < BLOCK_ROW; ++i)
	{
		for (int j = 0; j < BLOCK_COLUMN; ++j)
		{
			if (g_n2GameMatrix[i][j] != 0)
			{
				return false;
			}
		}
	}
	return true;
}

void CleanAll()
{
	int n = 0;
	for (int i = 0; i < 105; ++i)
	{
		FindClean();
		if (IsClear())
		{
			break;
		}
	}

	printf("\n");
	for (int i = 0; i < BLOCK_ROW; ++i)
	{
		for (int j = 0; j < BLOCK_COLUMN; ++j)
		{
			printf("%2d ",g_n2GameMatrix[i][j]);
		}
		printf("\n");
	}
}

void Init()
{
	memset(g_n2GameMatrix, 0, sizeof(g_n2GameMatrix));
	memset(g_h2BlockBitmap, 0, sizeof(g_h2BlockBitmap));
}

void RunGame()
{
	Init();
	g_hWnd = GetGameWindow();
	if (g_hWnd == 0)
	{
		printf("Game not found\n");
		return;
	}
	printf("HWND : %x\n", (int)g_hWnd);

	GenerateMatrix();
	CleanAll();
	DestroyBitmap();
}

void DisplayMsg()
{
	for (int i = 0; i < 8; ++i)
	{
		printf("%s\n", DEAGLEPC[i]);
	}
	printf("\n");
}

int main()
{
#ifdef MY_TEST
	// test
	HBITMAP hEmptyBlockBitmap = (HBITMAP)LoadImage(NULL, "empty.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	HBITMAP hBmp = (HBITMAP)LoadImage(NULL, "./tmp/0-0.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	HBITMAP hBitmap = CProcessBitmap::IsBitmapEqual((HWND)666, hBmp, hEmptyBlockBitmap);
	CxImage xImage;
	xImage.CreateFromHBITMAP(hBitmap);
	xImage.Save("666.bmp", CXIMAGE_FORMAT_BMP);
	DeleteObject(hBitmap);
	return 1;
#endif MY_TEST

	DisplayMsg();
	while (true)
	{
		printf("输入每步执行间隔(ms)，-1表示结束: ");
		int time;
		scanf_s("%d", &time);
		if (time < 0)
		{
			break;
		}
		g_nSleepTime = time;
		RunGame();
		// 无解,使用俩重置道具
		for (int i = 0; i < 2; ++i)
		{
			if (!IsClear())
			{
				Click(655, 200);
				Sleep(10000);
				RunGame();
			}
			else
			{
				break;
			}
		}
	}
    return 0;
}

