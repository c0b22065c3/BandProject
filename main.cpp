#include "DxLib.h"
#include "math.h"
#include "time.h"

// 画面の解像度
#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480

#define STANDARD_BPM	60		// 基準のBPM
#define MAX_BPM			400		// 最大のBPM

#define KILO			1000

#define FONT_SIZE		64		// フォントのサイズ
#define FONT_THICK		3		// フォントの太さ

int MouseX, MouseY;				// マウスのXY座標

// 時間
int startTime;					// ゲーム開始時間
int nowTime;					// 現在の時間
int oldTime;					// ひとつ前の時間

// BPM
int bpm = STANDARD_BPM;

// 色
unsigned int colourBlack	= GetColor(0, 0, 0);		// 黒
unsigned int colourWhite	= GetColor(255, 255, 255);	// 白 
unsigned int colourRed		= GetColor(255, 0, 0);		// 赤
unsigned int colourGreen	= GetColor(0, 255, 0);		// 緑
unsigned int colourBlue		= GetColor(0, 0, 255);		// 青

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

// スタートフラグ
BOOL drum_start			= FALSE;

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

// マウスが範囲内に入っているかを判定する関数
BOOL MouseInRange(int x1, int y1, int x2, int y2)
{
	if (MouseX >= x1 && MouseX <= x2 && MouseY >= y1 && MouseY <= y2)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

// ボタンを表示する関数
BOOL DrawButton(int beginX, int beginY, int sizeX, int sizeY, int mouseButton, const char* str = "", int fontHandle = NULL)
{
	// マウスがボタンの範囲内にあるとき
	if (MouseInRange(beginX, beginY, beginX + sizeX, beginY + sizeY))
	{
		// 側を表示
		DrawBox(beginX, beginY, beginX + sizeX, beginY + sizeY, colourBlack, FALSE);

		// 文字の表示
		DrawStringToHandle(beginX + (sizeX >> 1) - (FONT_SIZE >> 2), beginY + (sizeY >> 1) - (FONT_SIZE >> 1), str, colourBlack, fontHandle);

		// 指定のマウスボタンが押されたらTRUE
		if (ClickMouse(mouseButton))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		// 側を表示（透過）
		DrawBox(beginX, beginY, beginX + sizeX, beginY + sizeY, colourBlack, TRUE);

		// 文字の表示
		DrawStringToHandle(beginX + (sizeX >> 1) - (FONT_SIZE >> 2), beginY + (sizeY >> 1) - (FONT_SIZE >> 1), str, colourWhite, fontHandle);

		return FALSE;
	}
}

// チェックボックスを表示する関数
BOOL DrawCheckBox(int beginX, int beginY, int size, const char* str = "", int fontHandle = NULL, BOOL defaultCheck = FALSE)
{
	static BOOL check = defaultCheck;

	DrawBox(beginX, beginY, beginX + size, beginY + size, colourBlack, FALSE); // ボックスを表示
	DrawStringToHandle(beginX + size + (size >> 2), beginY, str, colourBlack, fontHandle); // 文字を表示

	if (MouseInRange(beginX, beginY, beginX + size, beginY + size) && ClickMouse(0) && isMouseLeft != isOldMouseLeft)
	{
		if (check)
		{
			check = FALSE;
		}
		else
		{
			check = TRUE;
		}
	}

	if (check)
	{
		// チェックを表示
		DrawLine(beginX, beginY + ((beginY + size - beginY) >> 1), beginX + ((beginX + size - beginX) >> 1), beginY + size, colourRed, size >> 3);
		DrawLine(beginX + ((beginX + size - beginX) >> 1), beginY + size, beginX + size, beginY, colourRed, size >> 3);
	}

	return check;
}

// スクロールバーを表示する関数
float DrawScrollBarWidth(int begin, int end, int place, int cursorSize, unsigned int color, float external)
{
	int length = end - begin;
	int cursorLength;

	static int cursorX = end - (length >> 1);
	static int cursorY = place;

	static BOOL isMove = FALSE;

	DrawBox(begin - 4, place - 4, end + 4, place + 4, color, TRUE);

	// マウスが押されている状態か
	if (ClickMouse(0))
	{
		// ボタンの範囲内にあるか
		if (MouseInRange(cursorX - cursorSize, cursorY - cursorSize, cursorX + cursorSize, cursorY + cursorSize))
		{
			// 左クリックした
			if (ClickMouse(0))
			{
				isMove = TRUE;
			}
		}
	}
	else
	{
		isMove = FALSE;
	}

	// カーソルをマウスのX座標に合わせる
	if (isMove)
	{
		cursorX = MouseX;

		// 範囲外に収める
		if (cursorX <= begin)
		{
			cursorX = begin;
		}

		if (cursorX >= end)
		{
			cursorX = end;
		}
	}
	else
	{
		cursorX = (int)((float)length * external) + begin;
	}

	// カーソルを表示
	DrawCircle(cursorX, cursorY, cursorSize, color, TRUE);

	// バーの左はじからカーソルまでの長さ
	cursorLength = cursorX - begin;

	return (float)cursorLength / (float)length;
}

// メイン関数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ChangeWindowMode(TRUE); // ウィンドウモードに変更
	SetGraphMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32); // 解像度の設定
	SetMainWindowText("ギター練習アプリケーション（仮）"); // ウィンドウのタイトルを変更

	SetMouseDispFlag(TRUE); // マウスを画面上でも表示する

	SetBackgroundColor(255, 255, 255); // 背景を白色に設定

	if (DxLib_Init() == -1) // DXライブラリ初期化処理
	{
		return -1; // エラーが起きたら直ちに終了
	}

	// ------------------------------------
	// 変数定義ゾーン
	// ------------------------------------

	// ドラムのサウンドハンドラ
	int drum_kick_1 = LoadSoundMem("DrumSound/maou_se_inst_drum1_kick.wav");

	// 画像のハンドラ
	int image_nijika = LoadGraph("Image/nijika_doritos.png");
	int image_yamada = LoadGraph("Image/sekaino_yamada.png");

	int second = 0;		// 秒数

	int beat = 4;		// ビート
	int beatCount = 0;	// カウント

	int night = 4;		// 伯子
	int measure = 0;	// 小節数

	float bpmRatio = 1.0f;	// 基準BPMとの比率
	float bpmScroll = 1.0f;	// BPMのスクロールバーの比率

	// フォントハンドル
	int buttonFontHandle = CreateFontToHandle("PixelMplus12", FONT_SIZE, FONT_THICK);
	int checkBoxFontHandle = CreateFontToHandle("PixelMplus12", FONT_SIZE >> 1, FONT_THICK);

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

		second = nowTime / KILO;

		// 現在のキーボード情報を更新
		GetHitKeyStateAll(keyState);

		// 現在のマウス情報を更新
		isMouseLeft		= ClickMouse(0);
		isMouseMiddle	= ClickMouse(1);
		isMouseRight	= ClickMouse(2);

		// マウスの位置を取得
		GetMousePoint(&MouseX, &MouseY);

		if (drum_start)
		{
			bpmRatio = (float)bpm * ((float)beat / 4.0f) / (float)STANDARD_BPM;

			if (nowTime % (int)(KILO / bpmRatio) < oldTime % (int)(KILO / bpmRatio))
			{
				PlaySoundMem(drum_kick_1, DX_PLAYTYPE_BACK);

				beatCount = beatCount++;

				if (beatCount % (beat * night / 4) == 1)
				{
					measure++;
				}
			}
		}

		printfDx("%d秒\n", second);
		printfDx("BPM%d\n", bpm);
		printfDx("%d\n", beatCount);
		printfDx("%dビート\n", beat);
		printfDx("%d伯子\n", night);
		printfDx("%d小節\n", measure);

		// ------------------------------------
		// 描画処理
		// ------------------------------------
		ClearDrawScreen(); // 画面を焼き払う

		DrawGraph(0, 0, image_nijika, TRUE); // 虹夏ちゃんを表示

		// チェックが付いていたら山田を表示
		if (DrawCheckBox(100, 0, 32, "山田", checkBoxFontHandle, TRUE))
		{
			DrawGraph(0, 0, image_yamada, TRUE);
		}

		// 左のボタン
		if (DrawButton(320 - 64, 0, 64, 48,
			0, "-", buttonFontHandle))
		{
			if (!isOldMouseLeft)
			{
				if (bpm > 0)
				{
					bpm--;
				}
			}
		}

		// 右のボタン
		if (DrawButton(320 + 64, 0, 64, 48,
			0, "+", buttonFontHandle))
		{
			if (!isOldMouseLeft)
			{
				if (bpm < MAX_BPM)
				{
					bpm++;
				}
			}
		}

		// スタートボタン
		if (DrawButton(320 + 64 * 3, 0, 64, 48,
			0, "P", buttonFontHandle))
		{
			if (!isOldMouseLeft)
			{
				if (drum_start)
				{
					drum_start = FALSE;
				}
				else
				{
					drum_start = TRUE;
				}
			}
		}

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