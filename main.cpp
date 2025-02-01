#include "DxLib.h"
#include "math.h"
#include "time.h"

#include <iostream>
#include <fstream>

#define SCREEN_WIDTH	640		// 画面の解像度X
#define SCREEN_HEIGHT	480		// 画面の解像度Y

#define STANDARD_BPM	60		// 基準のBPM
#define MAX_BPM			400		// 最大のBPM

#define MAX_BEAT		64		// 最大のビート

#define KILO			1000

#define FONT_SIZE		16		// フォントのサイズ
#define FONT_THICK		2		// フォントの太さ

#define BUTTON_X		32		// ボタンのサイズX
#define BUTTON_Y		24		// ボタンのサイズY

#define CHECK_SIZE		16		// チェックボックスのサイズ

#define PATTERN_NUM		16		// パターンのセーブデータの数
/*
#define BEAT_NUM		4		// 4小節で1まとまり
*/

int MouseX, MouseY;				// マウスのXY座標

// 時間
int startTime;					// ゲーム開始時間
int nowTime;					// 現在の時間
int oldTime;					// ひとつ前の時間

// BPM
int bpm = STANDARD_BPM * 2;		// 120BPM

// 色
unsigned int colourBlack	= GetColor(0, 0, 0);		// 黒
unsigned int colourWhite	= GetColor(255, 255, 255);	// 白 
unsigned int colourRed		= GetColor(255, 0, 0);		// 赤
unsigned int colourGreen	= GetColor(0, 255, 0);		// 緑
unsigned int colourBlue		= GetColor(0, 0, 255);		// 青
unsigned int colourYellow	= GetColor(255, 255, 0);	// 黄
unsigned int colourPurple	= GetColor(255, 0, 255);	// 紫
unsigned int colourWater	= GetColor(0, 255, 255);	// 水
unsigned int colourOrange	= GetColor(238, 120, 0);	// 橙

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

// セッションフラグ
BOOL session_start		= FALSE;

// 曲の構成
struct Composition
{
	const char* name; // 構成名
	int loop; // 繰り返す回数
	int patternNum; // パターン
};

// 画面モード
enum ScreenMode
{
	normal,
	editor,
	arrenge
};

// 進行
enum SessionProgress
{
	silence = 0,
	intro,
	verse,
	prechorus,
	chorus,
	bridge,
	outro
};

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
BOOL DrawButton(int beginX, int beginY, int sizeX, int sizeY, const char* str = "", int fontHandle = NULL, BOOL notTransparent = FALSE)
{
	int str_num = 0;
	int str_x;

	for (int i = 0; i < 8; i++)
	{
		if ((int)str[i] != 0)
		{
			str_num++;
		}
		else
		{
			break;
		}		
	}

	str_x = beginX + (sizeX >> 1) - ((FONT_SIZE + (FONT_SIZE >> 1)) >> 2) - (FONT_SIZE >> 1) * ((str_num >> 1));

	// マウスがボタンの範囲内にあるとき
	if (MouseInRange(beginX, beginY, beginX + sizeX, beginY + sizeY))
	{
		// 側を表示
		DrawBox(beginX, beginY, beginX + sizeX, beginY + sizeY, colourBlack, notTransparent);

		if (notTransparent)
		{
			// 文字の表示
			DrawStringToHandle(str_x, beginY, str, colourWhite, fontHandle);
		}
		else
		{
			// 文字の表示
			DrawStringToHandle(str_x, beginY, str, colourBlack, fontHandle);
		}

		// 指定のマウスボタンが押されたらTRUE
		if (isMouseLeft && !isOldMouseLeft)
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
		DrawStringToHandle(str_x, beginY, str, colourWhite, fontHandle);

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

// ON-OFFランプを表示する関数
BOOL DrawOnOffLamp(int beginX, int beginY, int size, BOOL lighting = FALSE)
{
	// ランプの表示
	DrawBox(beginX, beginY, beginX + size, beginY + size, colourBlue, lighting);

	if (MouseInRange(beginX, beginY, beginX + size, beginY + size) && ClickMouse(0))
	{
		if (isMouseLeft != isOldMouseLeft)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
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

	// ドラム音源のハンドラ配列
	int drum_set[15] = {
		LoadSoundMem("DrumSound/maou_se_inst_drum1_kick.wav"),		LoadSoundMem("DrumSound/maou_se_inst_drum2_kick.wav"),
		LoadSoundMem("DrumSound/maou_se_inst_drum1_snare.wav"),		LoadSoundMem("DrumSound/maou_se_inst_drum2_snare.wav"),
		LoadSoundMem("DrumSound/maou_se_inst_drum1_hat.wav"),		LoadSoundMem("DrumSound/maou_se_inst_drum2_hat.wav"),
		LoadSoundMem("DrumSound/maou_se_inst_drum1_cymbal.wav"),	LoadSoundMem("DrumSound/maou_se_inst_drum2_cymbal.wav"),
		LoadSoundMem("DrumSound/maou_se_inst_drum1_tom1.wav"),		LoadSoundMem("DrumSound/maou_se_inst_drum2_tom1.wav"),
		LoadSoundMem("DrumSound/maou_se_inst_drum1_tom2.wav"),		LoadSoundMem("DrumSound/maou_se_inst_drum2_tom2.wav"),
		LoadSoundMem("DrumSound/maou_se_inst_drum1_tom3.wav"),		LoadSoundMem("DrumSound/maou_se_inst_drum2_tom3.wav"),
		LoadSoundMem("DrumSound/maou_se_inst_drumroll.wav")
	};

	// 画像のハンドラ
	int image_nijika = LoadGraph("Image/nijika_doritos.png");
	int image_yamada = LoadGraph("Image/sekaino_yamada.png");

	int second = 0;				// 秒数

	int beat = 16;				// ビート
	int beatCount = 0;			// カウント
	int beatCountPart = 0;		// パートごとのビートカウント

	int meter = 4;				// 伯子

	int pattern = 0;			// パターンの番号

	int lamp = 0;				// ランプ点灯
	int measureEdit = 1;		// エディターでの現在の小節
	int editorX = 0;			// エディターの左端のX座標
	int editorY = BUTTON_Y * 4;	// エディターの左端のY座標

	float bpmRatio = 1.0f;		// 基準BPMとの比率
	float bpmScroll = 1.0f;		// BPMのスクロールバーの比率

	// フォントハンドル
	int fontHandle16 = CreateFontToHandle("PixelMplus12", FONT_SIZE, FONT_THICK);
	int fontHandle24 = CreateFontToHandle("PixelMplus12", FONT_SIZE + (FONT_SIZE >> 1), FONT_THICK);
	int fontHandle32 = CreateFontToHandle("PixelMplus12", FONT_SIZE * 2, FONT_THICK);

	// 変数を一時的に格納する為の変数
	int integer = 0;
	float floating = 0.0f;

	char msg[32] = "";

	// for文で繰り返す為の変数
	int ice = 0;
	int jam = 0;
	int custard = 0;
	int chocolate = 0;

	// テキストファイルの行数と格納先
	int lineCounter[PATTERN_NUM] = { 0 };

	int fileHandles[PATTERN_NUM];

	char stringBuffer[PATTERN_NUM][32][16];
	int beatsBuffer[PATTERN_NUM][32][16] = { 0 };

	int copyBuffer[32][16] = { 0 };
	int copyLine = 0;

	// 構造体
	Composition comp[] = { {"SESSION "}, {"In", 1, 0}, {"A", 1, 0}, {"B", 1, 0}, {"C", 1, 0}, {"D", 1, 0}, {"Out", 1, 0} };

	ScreenMode screen = normal;
	SessionProgress sessionProgress = silence;
	
	// テキストファイルを開く
	//int fileHandle = FileRead_open("PatternData/sample.txt");

	// ファイルの読み込み
	for (ice = 0; ice < PATTERN_NUM; ice++)
	{
		if (ice < 10)
		{
			sprintf_s(msg, "PatternData/beat0%d.txt", ice);
		}
		else
		{
			sprintf_s(msg, "PatternData/beat%d.txt", ice);
		}

		fileHandles[ice] = FileRead_open(msg);

		// ファイルを一行ずつ読み込んで格納
		while (FileRead_eof(fileHandles[ice]) == 0)
		{
			FileRead_gets(stringBuffer[ice][lineCounter[ice]], sizeof(stringBuffer), fileHandles[ice]);

			for (custard = 0; custard < sizeof(drum_set) / sizeof(int); custard++)
			{
				if (stringBuffer[ice][lineCounter[ice]][custard] == '0')
				{
					beatsBuffer[ice][lineCounter[ice]][custard] = 0;
				}
				else
				{
					beatsBuffer[ice][lineCounter[ice]][custard] = 1;
				}
			}

			lineCounter[ice]++;
		}

		FileRead_close(fileHandles[ice]);
	}

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

		// セッションモード
		/*
		if (session_start)
		{
			switch (sessionProgress)
			{
				// 入る前
			case silence:
				sessionProgress = intro;
				drum_start = TRUE;
				break;

				// イントロ
			case intro:
				if (measure >= comp[sessionProgress].loop * BEAT_NUM)
				{
					if (beatCountPart >= (beat >> 2) * night)
					{
						measure = 0;
						beatCountPart = 0;
						sessionProgress = verse;
					}
				}
				break;

				// Aメロ
			case verse:
				if (measure >= comp[sessionProgress].loop * BEAT_NUM)
				{
					if (beatCountPart >= (beat >> 2) * night)
					{
						measure = 0;
						beatCountPart = 0;
						sessionProgress = prechorus;
					}
				}
				break;

				// Bメロ
			case prechorus:
				if (measure >= comp[sessionProgress].loop * BEAT_NUM)
				{
					if (beatCountPart >= (beat >> 2) * night)
					{
						measure = 0;
						beatCountPart = 0;
						sessionProgress = chorus;
					}
				}
				break;

				// サビ
			case chorus:
				if (measure >= comp[sessionProgress].loop * BEAT_NUM)
				{
					if (beatCountPart >= (beat >> 2) * night)
					{
						measure = 0;
						beatCountPart = 0;
						sessionProgress = bridge;
					}
				}
				break;

				// Dメロ
			case bridge:
				if (measure >= comp[sessionProgress].loop * BEAT_NUM)
				{
					if (beatCountPart >= (beat >> 2) * night)
					{
						measure = 0;
						beatCountPart = 0;
						sessionProgress = outro;
					}
				}
				break;

				// アウトロ
			case outro:
				if (measure >= comp[sessionProgress].loop * BEAT_NUM)
				{
					if (beatCountPart >= (beat >> 2) * night)
					{
						measure = 0;
						measureCount = 0;
						beatCount = 0;
						beatCountPart = 0;
						sessionProgress = silence;
						drum_start = FALSE;
						session_start = FALSE;
					}
				}
				break;

			default:
				break;
			}

			pattern = comp[sessionProgress].patternNum;
		}
		*/

		// ドラムの音の処理
		if (drum_start)
		{
			// BPMの比率
			bpmRatio = (float)bpm * ((float)beat / 4.0f) / (float)STANDARD_BPM;

			// BPMに応じて音を鳴らす
			if (nowTime % (int)(KILO / bpmRatio) < oldTime % (int)(KILO / bpmRatio))
			{
				beatCount++;

				// カウントの調整
				if (beatCount > (beat >> 2) * meter)
				{
					beatCount = 1;
				}

				/*
				// 次の小節へ
				if (beatCount == 1)
				{
					measure++;
					measureCount++;

					if (measureCount > BEAT_NUM)
					{
						measureCount = 1;
					}

					measureEdit = measureCount;
				}
				*/

				// 伯数を調整
				meter = lineCounter[pattern] / 4;

				//beatCount = beatCount % ((beat >> 2) * night);

				// ファイルの内容に応じた音を鳴らす
				for (ice = 0; ice < sizeof(drum_set) / sizeof(int); ice++)
				{
					if (beatsBuffer[pattern][beatCount - 1][ice])
					{
						PlaySoundMem(drum_set[ice], DX_PLAYTYPE_BACK);
					}
				}

				/*
				// 現在のパートの最期の小節
				if (measure >= comp[sessionProgress].loop * BEAT_NUM)
				{
					beatCountPart++;
				}
				*/
			}
		}

		// 簡易表示
		printfDx("%d秒\n", second);
		printfDx("BPM%d\n", bpm);
		printfDx("%d\n", beatCount);
		printfDx("%dビート\n", beatCountPart);
		printfDx("%d伯子\n", meter);
		//printfDx("パターン %d\n", pattern);
		printfDx("進行 %d\n", sessionProgress);

		//// ファイルの中身を簡易表示
		//for (int i = 0; i < lineCounter[0][0]; i++)
		//{
		//	printfDx(stringBuffer[0][0][i]);
		//	printfDx("\n");
		//}

		// ------------------------------------
		// 描画処理
		// ------------------------------------
		ClearDrawScreen(); // 画面を焼き払う

		DrawGraph(0, 0, image_nijika, TRUE); // 虹夏ちゃんを表示

		// チェックが付いていたら山田を表示
		if (DrawCheckBox(100, BUTTON_Y, BUTTON_Y, "山田", fontHandle24, TRUE))
		{
			DrawGraph(0, 0, image_yamada, TRUE);
		}

		// BPMの操作
		// 文字
		DrawStringToHandle(SCREEN_WIDTH - BUTTON_X * 6 + (FONT_SIZE >> 0), BUTTON_Y * 0, "BPM", colourBlack, fontHandle24);

		// 左のボタン
		if (DrawButton(SCREEN_WIDTH - BUTTON_X * 4, BUTTON_Y * 0, BUTTON_X, BUTTON_Y, "-", fontHandle24))
		{
			if (bpm > 0)
			{
				bpm--;
			}
		}

		sprintf_s(msg, "%d", bpm);
		DrawStringToHandle(SCREEN_WIDTH - BUTTON_X * 3 + (BUTTON_X >> 1), BUTTON_Y * 0, msg, colourBlack, fontHandle24);

		// 右のボタン
		if (DrawButton(SCREEN_WIDTH - BUTTON_X, BUTTON_Y * 0, BUTTON_X, BUTTON_Y, "+", fontHandle24))
		{
			if (bpm < MAX_BPM)
			{
				bpm++;
			}
		}

		// パターンの操作
		// 文字
		DrawStringToHandle(SCREEN_WIDTH - BUTTON_X * 7 - (BUTTON_X >> 1) + (FONT_SIZE >> 0), BUTTON_Y * 2, "PATTERN", colourBlack, fontHandle24);

		// 左のボタン
		if (DrawButton(SCREEN_WIDTH - BUTTON_X * 4, BUTTON_Y * 2, BUTTON_X, BUTTON_Y, "-", fontHandle24))
		{
			if (pattern == 0)
			{
				pattern = PATTERN_NUM - 1;
			}
			else
			{
				pattern--;
			}

			meter = lineCounter[pattern] / 4;
		}

		sprintf_s(msg, "%d", pattern);
		DrawStringToHandle(SCREEN_WIDTH - BUTTON_X * 3 + BUTTON_X, BUTTON_Y * 2, msg, colourBlack, fontHandle24);

		// 右のボタン
		if (DrawButton(SCREEN_WIDTH - BUTTON_X, BUTTON_Y * 2, BUTTON_X, BUTTON_Y, "+", fontHandle24))
		{
			if (pattern == PATTERN_NUM - 1)
			{
				pattern = 0;
			}
			else
			{
				pattern++;
			}

			meter = lineCounter[pattern] / 4;
		}

		// エディタボタン
		if (DrawButton(SCREEN_WIDTH - BUTTON_X * 4, editorY, BUTTON_X * 4, BUTTON_Y, "EDITOR", fontHandle24))
		{
			if (screen == editor)
			{
				screen = normal;
			}
			else
			{
				screen = editor;
			}
		}

		// エディタの表示
		if (screen == editor)
		{
			// 現在のパターンを表示
			sprintf_s(msg, "%d", pattern);

			// パターンを表示
			DrawButton(editorX, editorY, beat * 2, BUTTON_Y, msg, fontHandle24, TRUE);

			//DrawButton(editorX + beat * 2, editorY, beat * 28, BUTTON_Y, "", fontHandle24, TRUE);

			// 現在の小節数を視覚的に表示
			/*
			for (ice = 0; ice < BEAT_NUM; ice++)
			{
				// 再生中の小節
				if (ice == measureCount - 1)
				{
					sprintf_s(msg, ">>>");
				}
				else
				{
					sprintf_s(msg, "%d", ice + 1);
				}

				// 編集中の小節
				if (ice == measureEdit - 1)
				{
					integer = colourRed;
				}
				else
				{
					integer = colourYellow;
				}

				// 小節数カーソル
				DrawButton(editorX + beat * 2 + beat * 7 * ice, editorY,
					beat * 7, BUTTON_Y, msg, fontHandle24, TRUE);

				// 枠を表示
				DrawBox(editorX + beat * 2 + beat * 7 * ice, editorY,
					editorX + beat * 2 + beat * 7 * ice + beat * 7, editorY + BUTTON_Y, integer, FALSE);
			}
			*/

			// 枠を表示
			DrawBox(editorX + beat * 2, editorY,
				editorX + beat * 2 + beat * 28, editorY + BUTTON_Y, colourBlack, TRUE);

			// 伯を表示
			sprintf_s(msg, "%d", meter);
			DrawButton(editorX + beat * 30, editorY, beat * 2, BUTTON_Y, msg, fontHandle24, TRUE);

			// 伯をプラス
			if (DrawButton(editorX + beat * 30, editorY + BUTTON_Y, beat * 2, BUTTON_Y, "+", fontHandle24))
			{
				if (meter < 7)
				{
					meter++;
				}

				lineCounter[pattern] = 4 * meter;
			}

			// 伯をマイナス
			if (DrawButton(editorX + beat * 30, editorY + BUTTON_Y * 2, beat * 2, BUTTON_Y, "-", fontHandle24))
			{
				if (meter > 2)
				{
					meter--;
				}

				lineCounter[pattern] = 4 * meter;
			}

			DrawBox(editorX, editorY + BUTTON_Y, editorX + beat * 2, editorY + BUTTON_Y + beat * 2, colourRed, TRUE);
			DrawBox(editorX, editorY + BUTTON_Y + beat * 2, editorX + beat * 2, editorY + BUTTON_Y + beat * 4, colourOrange, TRUE);
			DrawBox(editorX, editorY + BUTTON_Y + beat * 4, editorX + beat * 2, editorY + BUTTON_Y + beat * 6, colourYellow, TRUE);
			DrawBox(editorX, editorY + BUTTON_Y + beat * 6, editorX + beat * 2, editorY + BUTTON_Y + beat * 8, colourGreen, TRUE);
			DrawBox(editorX, editorY + BUTTON_Y + beat * 8, editorX + beat * 2, editorY + BUTTON_Y + beat * 10, colourWater, TRUE);
			DrawBox(editorX, editorY + BUTTON_Y + beat * 10, editorX + beat * 2, editorY + BUTTON_Y + beat * 12, colourBlue, TRUE);
			DrawBox(editorX, editorY + BUTTON_Y + beat * 12, editorX + beat * 2, editorY + BUTTON_Y + beat * 14, colourPurple, TRUE);
			DrawBox(editorX, editorY + BUTTON_Y + beat * 14, editorX + beat * 2, editorY + BUTTON_Y + beat * 15, colourRed, TRUE);

			// 小節を左に移動するボタン
			if (DrawButton(editorX, (SCREEN_HEIGHT >> 2) + beat * 15, beat * 2, BUTTON_Y, "←", fontHandle24))
			{
				if (pattern > 0)
				{
					pattern--;
				}
				else
				{
					pattern = PATTERN_NUM - 1;
				}

				meter = lineCounter[pattern] / 4;
			}

			// カーソルの位置を移動するボタン
			for (ice = 0; ice < 28; ice++)
			{
				if (DrawButton(editorX + beat * (2 + ice), (SCREEN_HEIGHT >> 2) + beat * 15, beat * 1, BUTTON_Y, "", fontHandle24, TRUE))
				{
					if (!drum_start)
					{
						beatCount = ice + 1;
					}
				}
			}

			// 小節を右に移動するボタン
			if (DrawButton(editorX + beat * 30, (SCREEN_HEIGHT >> 2) + beat * 15, beat * 2, BUTTON_Y, "→", fontHandle24))
			{
				if (pattern < PATTERN_NUM - 1)
				{
					pattern++;
				}
				else
				{
					pattern = 0;
				}

				meter = lineCounter[pattern] / 4;
			}

			// カーソルの表示
			if (beatCount != 0)
			{
				DrawBox(editorX + beat * (1 + beatCount), (SCREEN_HEIGHT >> 2) + beat * 15,
					editorX + beat * (2 + beatCount), (SCREEN_HEIGHT >> 2) + beat * 15 + BUTTON_Y, colourRed, FALSE);
			}

			// クリアボタン
			if (DrawButton(editorX + beat * 2, BUTTON_Y * 17,
				beat * 4, BUTTON_Y, "CLEAR ", fontHandle24))
			{
				integer = lineCounter[pattern];

				lineCounter[pattern] = 4 * 7;

				// バッファに0を代入
				for (ice = 0; ice < lineCounter[pattern]; ice++)
				{
					for (jam = 0; jam < sizeof(drum_set) / sizeof(int); jam++)
					{
						beatsBuffer[pattern][ice][jam] = 0;
					}
				}

				lineCounter[pattern] = integer;
			}

			// コピーボタン
			if (DrawButton(editorX + beat * 8, BUTTON_Y * 17,
				beat * 4, BUTTON_Y, "COPY", fontHandle24))
			{
				// コピー先の変数に代入
				for (ice = 0; ice < lineCounter[pattern]; ice++)
				{
					for (jam = 0; jam < sizeof(drum_set) / sizeof(int); jam++)
					{
						copyBuffer[ice][jam] = beatsBuffer[pattern][ice][jam];
					}
				}

				copyLine = lineCounter[pattern];
			}

			// ペーストボタン
			if (DrawButton(editorX + beat * 14, BUTTON_Y * 17,
				beat * 4, BUTTON_Y, "PASTE ", fontHandle24))
			{
				if (copyLine)
				{
					// コピー先の中身をバッファに保存
					for (ice = 0; ice < copyLine; ice++)
					{
						for (jam = 0; jam < sizeof(drum_set) / sizeof(int); jam++)
						{
							beatsBuffer[pattern][ice][jam] = copyBuffer[ice][jam];
						}
					}

					lineCounter[pattern] = copyLine;

					meter = lineCounter[pattern] / 4;
				}
			}

			// 
			if (DrawButton(editorX + beat * 20, BUTTON_Y * 17,
				beat * 4, BUTTON_Y, "", fontHandle24))
			{

			}

			// セーブボタン
			if (DrawButton(editorX + beat * 26, BUTTON_Y * 17,
				beat * 4, BUTTON_Y, "SAVE", fontHandle24))
			{
				if (pattern < 10)
				{
					sprintf_s(msg, "PatternData/beat0%d.txt", pattern);
				}
				else
				{
					sprintf_s(msg, "PatternData/beat%d.txt", pattern);
				}

				std::ofstream file(msg);

				for (ice = 0; ice < lineCounter[pattern]; ice++)
				{
					for (jam = 0; jam < sizeof(drum_set) / sizeof(int); jam++)
					{
						if (beatsBuffer[pattern][ice][jam] == 0)
						{
							stringBuffer[pattern][ice][jam] = '0';
						}
						else
						{
							stringBuffer[pattern][ice][jam] = '1';
						}

						file << beatsBuffer[pattern][ice][jam]; // ファイルへ書き込み
					}

					file << std::endl; // 改行
				}
			}

			// 編集部を表示
			for (ice = 0; ice < lineCounter[pattern]; ice++)
			{
				for (jam = 0; jam < sizeof(drum_set) / sizeof(int); jam++)
				{
					lamp = beatsBuffer[pattern][ice][jam];
					//printfDx("%d", lamp);

					// ランプを表示
					// クリックされたら0と1を入れ替える
					if (DrawOnOffLamp(editorX + beat * (ice + 2), (SCREEN_HEIGHT >> 2) + beat * jam, beat, lamp))
					{
						if (lamp)
						{
							beatsBuffer[pattern][ice][jam] = 0;
						}
						else
						{
							beatsBuffer[pattern][ice][jam] = 1;
						}

						// クリックされたら音が鳴る
						PlaySoundMem(drum_set[jam], DX_PLAYTYPE_BACK);
					}
				}
			}

			// 伯ごとの敷居
			for (ice = 0; ice < meter; ice++)
			{
				integer = colourYellow;
				
				// カーソルが被っていたら赤くなる
				/*
				if ((beatCount - 1) / 4 == ice && measureCount == measureEdit)
				{
					integer = colourRed;
				}
				*/

				DrawBox(editorX + beat * (2 + 4 * ice), (SCREEN_HEIGHT >> 2),
					editorX + beat * (2 + 4 + 4 * ice), (SCREEN_HEIGHT >> 2) * 3, integer, FALSE);
			}
		}

		// アレンジボタン
		if (DrawButton(SCREEN_WIDTH - BUTTON_X * 4, editorY + BUTTON_Y * 2, BUTTON_X * 4, BUTTON_Y, "ARRENGE ", fontHandle24))
		{
			if (screen == arrenge)
			{
				screen = normal;
			}
			else
			{
				screen = arrenge;
			}
		}

		// アレンジエディタの表示
		/*
		if (screen == arrenge)
		{
			for (ice = 1; ice < sizeof(comp) / sizeof(Composition); ice++)
			{
				DrawButton((SCREEN_WIDTH >> 3), editorY + BUTTON_Y * 2 * ice, BUTTON_X * 2, BUTTON_Y, comp[ice].name, fontHandle24);

				sprintf_s(msg, "%d", comp[ice].loop);

				// ループする回数を減らす
				if (DrawButton((SCREEN_WIDTH >> 3) + BUTTON_X * 3, editorY + BUTTON_Y * 2 * ice,
					BUTTON_X, BUTTON_Y, "-", fontHandle24) && comp[sessionProgress].name != comp[ice].name)
				{
					if (comp[ice].loop != 0)
					{
						comp[ice].loop--;
					}
				}

				// ループする回数を表示
				DrawButton((SCREEN_WIDTH >> 3) + BUTTON_X * 4, editorY + BUTTON_Y * 2 * ice, BUTTON_X, BUTTON_Y, msg, fontHandle24, TRUE);

				// ループする回数を増やす
				if (DrawButton((SCREEN_WIDTH >> 3) + BUTTON_X * 5, editorY + BUTTON_Y * 2 * ice,
					BUTTON_X, BUTTON_Y, "+", fontHandle24) && comp[sessionProgress].name != comp[ice].name)
				{
					if (comp[ice].loop < 16)
					{
						comp[ice].loop++;
					}
				}

				sprintf_s(msg, "%d", comp[ice].patternNum);

				// 前のパターンへ
				if (DrawButton((SCREEN_WIDTH >> 3) + BUTTON_X * 7, editorY + BUTTON_Y * 2 * ice,
					BUTTON_X, BUTTON_Y, "-", fontHandle24) && comp[sessionProgress].name != comp[ice].name)
				{
					if (comp[ice].patternNum != 0)
					{
						comp[ice].patternNum--;
					}
					else
					{
						comp[ice].patternNum = PATTERN_NUM - 1;
					}
				}

				// パターンナンバーを表示
				DrawButton((SCREEN_WIDTH >> 3) + BUTTON_X * 8, editorY + BUTTON_Y * 2 * ice, BUTTON_X, BUTTON_Y, msg, fontHandle24, TRUE);

				// 次のパターンへ
				if (DrawButton((SCREEN_WIDTH >> 3) + BUTTON_X * 9, editorY + BUTTON_Y * 2 * ice,
					BUTTON_X, BUTTON_Y, "+", fontHandle24) && comp[sessionProgress].name != comp[ice].name)
				{
					if (comp[ice].patternNum < PATTERN_NUM - 1)
					{
						comp[ice].patternNum++;
					}
					else
					{
						comp[ice].patternNum = 0;
					}
				}
			}
		}
		*/

		// セッションボタン
		if (DrawButton(SCREEN_WIDTH - BUTTON_X * 4, editorY + BUTTON_Y * 4, BUTTON_X * 4, BUTTON_Y, comp[sessionProgress].name, fontHandle24))
		{
			if (session_start)
			{
				session_start = FALSE;
				drum_start = FALSE;
			}
			else
			{
				session_start = TRUE;

				if (sessionProgress != silence)
				{
					drum_start = TRUE;
				}
			}
		}

		if (drum_start)
		{
			sprintf_s(msg, "STOP");
		}
		else
		{
			sprintf_s(msg, "PLAY");
		}

		// スタートボタン
		if (DrawButton(SCREEN_WIDTH - BUTTON_X * 4, BUTTON_Y * 15, BUTTON_X * 4, BUTTON_Y, msg, fontHandle24))
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

		// リセットボタン
		if (DrawButton(SCREEN_WIDTH - BUTTON_X * 4, BUTTON_Y * 17, BUTTON_X * 4, BUTTON_Y, "RESET", fontHandle24))
		{
			beatCount = 0;
			beatCountPart = 0;

			drum_start = FALSE;
			session_start = FALSE;

			sessionProgress = silence;
		}

		// アプリ終了ボタン
		if (DrawButton(SCREEN_WIDTH - BUTTON_X * 4, BUTTON_Y * 19, BUTTON_X * 4, BUTTON_Y, "OK", fontHandle24))
		{
			break;
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

	// 後始末

	// サウンドハンドルを削除
	for (ice = 0; ice < sizeof(drum_set) / sizeof(int); ice++)
	{
		DeleteSoundMem(drum_set[ice]);
	}

	// 画像のグラフィックハンドルを削除
	DeleteGraph(image_nijika);
	DeleteGraph(image_yamada);

	// フォントハンドルを削除
	DeleteFontToHandle(fontHandle16);
	DeleteFontToHandle(fontHandle24);
	DeleteFontToHandle(fontHandle32);

	//// ファイルを閉じる
	//for (int p = 0; p < PATTERN_NUM; p++)
	//{
	//	for (int b = 0; b < BEAT_NUM; b++)
	//	{
	//		FileRead_close(fileHandles[p][b]);
	//	}
	//}

	DxLib_End(); // DXライブラリ使用の終了処理

	return 0;
}