#include "DxLib.h"
#include "math.h"
#include "time.h"

// 画面の解像度
#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480

int MouseX, MouseY;		// マウスのXY座標

// 時間
int startTime;			// ゲーム開始時間
int nowTime;			// 現在の時間
int oldTime;			// ひとつ前の時間

// 色
unsigned int colorBlack = GetColor(0, 0, 0);
unsigned int colorWhite = GetColor(255, 255, 255);

// キーボード情報
char keyState[256];		// キーボード情報
char oldKeyState[256];	// ひとつ前のキーボード情報

// マウス情報
BOOL isMouseLeft		= FALSE;
BOOL isMouseMiddle		= FALSE;
BOOL isMouseRight		= FALSE;

// ひとつ前のマウス情報
BOOL isOldMouseLeft		= FALSE;
BOOL isOldMouseMiddle	= FALSE;
BOOL isOldMouseRight	= FALSE;

// 乱数を取得する関数
int GetRandom(int min, int max)
{
	return min + (int)(rand() * (max - min + 1.0) / (1.0 + RAND_MAX));
}

// マウスクリックされたかを判定する関数
BOOL ClickMouse(int button)
{
	switch (button)
	{
		// 左クリックのとき
	case 0:
		if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	case 1:
		if ((GetMouseInput() & MOUSE_INPUT_MIDDLE) != 0)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	case 2:
		if ((GetMouseInput() & MOUSE_INPUT_RIGHT) != 0)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}

	default:
		return FALSE;
	}
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ChangeWindowMode(TRUE); // ウィンドウモードに変更
	SetGraphMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32); // 解像度の設定
	SetMainWindowText("ギター練習アプリケーション（仮）"); // ウィンドウのタイトルを変更

	SetMouseDispFlag(TRUE); // マウスを画面上でも表示する

	if (DxLib_Init() == -1) // DXライブラリ初期化処理
	{
		return -1; // エラーが起きたら直ちに終了
	}

	// ------------------------------------
	// 変数定義ゾーン
	// ------------------------------------

	// ひとつ前のキーボード情報を初期化
	for (int key = 0; key < 256; key++)
	{
		oldKeyState[key] = 0;
	}

	// 毎回違う乱数を生成
	srand((unsigned int)time(NULL));

	// ゲーム開始時間を得る
	startTime = GetNowCount();

	// ゲームループ
	while (ProcessMessage() == 0)
	{
		// ------------------------------------
		// 前処理
		// ------------------------------------
		
		// ゲーム開始からの経過時間を更新
		nowTime = GetNowCount() - startTime;

		// 現在のキーボード情報を更新
		GetHitKeyStateAll(keyState);

		// 現在のマウス情報を更新
		isMouseLeft		= ClickMouse(0);
		isMouseMiddle	= ClickMouse(1);
		isMouseRight	= ClickMouse(2);

		// マウスの位置を取得
		GetMousePoint(&MouseX, &MouseY);

		// ------------------------------------
		// 描画処理
		// ------------------------------------
		ClearDrawScreen(); // 画面を焼き払う

		// ------------------------------------
		// 後処理
		// ------------------------------------

		// キーボード情報を保存
		for (int key = 0; key < 256; key++)
		{
			oldKeyState[key] = keyState[key];
		}

		// マウス情報を保存
		isOldMouseLeft	 = isMouseLeft;
		isOldMouseMiddle = isMouseMiddle;
		isOldMouseRight	 = isMouseRight;

		// 時間を保存
		oldTime = nowTime;

		ScreenFlip(); // 過去を忘却

		clsDx(); // 簡易文字を抹殺
	}

	DxLib_End(); // DXライブラリ使用の終了処理

	return 0;
}