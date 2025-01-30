#include "DxLib.h"
#include "math.h"
#include "time.h"

// 画面の解像度
#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480

#define STANDARD_BPM	60		// 基準のBPM
#define MAX_BPM			400		// 最大のBPM

#define MAX_BEAT		64		// 最大のビート

#define KILO			1000

#define FONT_SIZE		16		// フォントのサイズ
#define FONT_THICK		2		// フォントの太さ

// ボタンのサイズ
#define BUTTON_X		32
#define BUTTON_Y		24

#define CHECK_SIZE		16		// チェックボックスのサイズ

#define PATTERN_NUM		8		// パターンのセーブデータの数
#define BEAT_NUM		4		// 4小節で1まとまり

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

// エディターフラグ
BOOL editor				= FALSE;

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
BOOL DrawButton(int beginX, int beginY, int sizeX, int sizeY, const char* str = "", int fontHandle = NULL)
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
		DrawBox(beginX, beginY, beginX + sizeX, beginY + sizeY, colourBlack, FALSE);

		// 文字の表示
		DrawStringToHandle(str_x, beginY, str, colourBlack, fontHandle);

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

	// ドラムのサウンドハンドラ
	int drum_kick_1		= LoadSoundMem("DrumSound/maou_se_inst_drum1_kick.wav");	// キック
	int drum_kick_2		= LoadSoundMem("DrumSound/maou_se_inst_drum2_kick.wav");	// キック2
	int drum_snare_1	= LoadSoundMem("DrumSound/maou_se_inst_drum1_snare.wav");	// スネア
	int drum_snare_2	= LoadSoundMem("DrumSound/maou_se_inst_drum2_snare.wav");	// スネア2
	int drum_hat_1		= LoadSoundMem("DrumSound/maou_se_inst_drum1_hat.wav");		// ハット
	int drum_hat_2		= LoadSoundMem("DrumSound/maou_se_inst_drum2_hat.wav");		// ハット2
	int drum_symbal_1	= LoadSoundMem("DrumSound/maou_se_inst_drum1_cymbal.wav");	// シンバル
	int drum_symbal_2	= LoadSoundMem("DrumSound/maou_se_inst_drum2_cymbal.wav");	// シンバル2
	int drum_tom1_1		= LoadSoundMem("DrumSound/maou_se_inst_drum1_tom1.wav");	// タム1
	int drum_tom1_2		= LoadSoundMem("DrumSound/maou_se_inst_drum2_tom1.wav");	// タム1-2
	int drum_tom2_1		= LoadSoundMem("DrumSound/maou_se_inst_drum1_tom2.wav");	// タム2
	int drum_tom2_2		= LoadSoundMem("DrumSound/maou_se_inst_drum2_tom2.wav");	// タム2-2
	int drum_tom3_1		= LoadSoundMem("DrumSound/maou_se_inst_drum1_tom3.wav");	// タム3
	int drum_tom3_2		= LoadSoundMem("DrumSound/maou_se_inst_drum2_tom3.wav");	// タム3-2
	int drum_roll		= LoadSoundMem("DrumSound/maou_se_inst_drumroll.wav");		// ドラムロール

	// ハンドラ配列
	int drum_set[15] = {
		drum_kick_1,	drum_kick_2, 
		drum_snare_1,	drum_snare_2,
		drum_hat_1,		drum_hat_2,
		drum_symbal_1,	drum_symbal_2,
		drum_tom1_1,	drum_tom1_2,
		drum_tom2_1,	drum_tom2_2,
		drum_tom3_1,	drum_tom3_2,
		drum_roll
	};

	// 画像のハンドラ
	int image_nijika = LoadGraph("Image/nijika_doritos.png");
	int image_yamada = LoadGraph("Image/sekaino_yamada.png");

	int second = 0;				// 秒数

	int beat = 16;				// ビート
	int beatCount = 0;			// カウント

	int night = 4;				// 伯子
	int measure = 0;			// 小節数
	int measureCount = 0;		// 小節数のカウント

	int pattern = 0;			// パターンの番号

	int lamp = 0;				// ランプ点灯
	int measureEdit = 1;		// エディターでの現在の小節
	int editorX = 0;			// エディターの左端のX座標
	int editorY = BUTTON_Y * 4;	// エディターの左端のY座標

	float bpmRatio = 1.0f;	// 基準BPMとの比率
	float bpmScroll = 1.0f;	// BPMのスクロールバーの比率

	// フォントハンドル
	int fontHandle16 = CreateFontToHandle("PixelMplus12", FONT_SIZE, FONT_THICK);
	int fontHandle24 = CreateFontToHandle("PixelMplus12", FONT_SIZE + (FONT_SIZE >> 1), FONT_THICK);
	int fontHandle32 = CreateFontToHandle("PixelMplus12", FONT_SIZE * 2, FONT_THICK);

	// 変数を画面に表示する為の変数
	char msg[32] = "";

	// テキストファイルの行数と格納先
	int lineCounter[PATTERN_NUM][BEAT_NUM] = { 0 };
	int beatsBuffer[PATTERN_NUM][BEAT_NUM][32][16] = { 0 };
	char stringBuffer[PATTERN_NUM][BEAT_NUM][32][16];

	int fileHandles[PATTERN_NUM][BEAT_NUM];
	
	// テキストファイルを開く
	//int fileHandle = FileRead_open("PatternData/sample.txt");

	for (int p = 0; p < PATTERN_NUM; p++)
	{
		for (int b = 0; b < BEAT_NUM; b++)
		{
			sprintf_s(msg, "PatternData/Pattern%d/beat%d.txt", p, b);
			fileHandles[p][b] = FileRead_open(msg);

			// ファイルを一行ずつ読み込んで格納
			while (FileRead_eof(fileHandles[p][b]) == 0)
			{
				FileRead_gets(stringBuffer[p][b][lineCounter[p][b]], sizeof(stringBuffer), fileHandles[p][b]);

				for (int i = 0; i < sizeof(drum_set) / sizeof(int); i++)
				{
					if (stringBuffer[p][b][lineCounter[p][b]][i] == '0')
					{
						beatsBuffer[p][b][lineCounter[p][b]][i] = 0;
					}
					else if (stringBuffer[p][b][lineCounter[p][b]][i] == '1')
					{
						beatsBuffer[p][b][lineCounter[p][b]][i] = 1;
					}
				}

				lineCounter[p][b]++;
			}
		}
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
				if (beatCount > (beat >> 2) * night)
				{
					beatCount = 1;
				}

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

				// 伯数を調整
				night = lineCounter[pattern][measureCount - 1] / 4;

				//beatCount = beatCount % ((beat >> 2) * night);

				// ファイルの内容に応じた音を鳴らす
				for (int i = 0; i < sizeof(drum_set) / sizeof(int); i++)
				{
					if (beatsBuffer[pattern][measureCount - 1][beatCount - 1][i])
					{
						PlaySoundMem(drum_set[i], DX_PLAYTYPE_BACK);
					}
				}
			}
		}

		// 簡易表示
		printfDx("%d秒\n", second);
		//printfDx("BPM%d\n", bpm);
		printfDx("%d\n", beatCount);
		//printfDx("%dビート\n", beat);
		printfDx("%d伯子\n", night);
		//printfDx("%d小節\n", measure);
		printfDx("%d小節\n", measureCount);
		//printfDx("パターン %d\n", pattern);

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
		DrawStringToHandle(SCREEN_WIDTH - BUTTON_X * 6 + (FONT_SIZE >> 0), BUTTON_Y, "BPM", colourBlack, fontHandle24);

		// 左のボタン
		if (DrawButton(SCREEN_WIDTH - BUTTON_X * 4, BUTTON_Y, BUTTON_X, BUTTON_Y, "-", fontHandle24))
		{
			if (bpm > 0)
			{
				bpm--;
			}
		}

		sprintf_s(msg, "%d", bpm);
		DrawStringToHandle(SCREEN_WIDTH - BUTTON_X * 3 + (BUTTON_X >> 1), BUTTON_Y, msg, colourBlack, fontHandle24);

		// 右のボタン
		if (DrawButton(SCREEN_WIDTH - BUTTON_X, BUTTON_Y, BUTTON_X, BUTTON_Y, "+", fontHandle24))
		{
			if (bpm < MAX_BPM)
			{
				bpm++;
			}
		}

		// パターンの操作
		// 文字
		DrawStringToHandle(SCREEN_WIDTH - BUTTON_X * 6 + (FONT_SIZE >> 0), BUTTON_Y * 3, "beat", colourBlack, fontHandle24);

		// 左のボタン
		if (DrawButton(SCREEN_WIDTH - BUTTON_X * 4, BUTTON_Y * 3, BUTTON_X, BUTTON_Y, "-", fontHandle24))
		{
			if (pattern == 0)
			{
				pattern = PATTERN_NUM - 1;
			}
			else
			{
				pattern--;
			}
		}

		sprintf_s(msg, "%d", pattern);
		DrawStringToHandle(SCREEN_WIDTH - BUTTON_X * 3 + (BUTTON_X >> 1), BUTTON_Y * 3, msg, colourBlack, fontHandle24);

		// 右のボタン
		if (DrawButton(SCREEN_WIDTH - BUTTON_X, BUTTON_Y * 3, BUTTON_X, BUTTON_Y, "+", fontHandle24))
		{
			if (pattern == PATTERN_NUM - 1)
			{
				pattern = 0;
			}
			else
			{
				pattern++;
			}
		}

		// エディットボタン
		if (DrawButton(SCREEN_WIDTH - BUTTON_X * 4, BUTTON_Y * 9, BUTTON_X * 4, BUTTON_Y, "EDITOR", fontHandle24))
		{
			if (editor)
			{
				editor = FALSE;
			}
			else
			{
				editor = TRUE;
			}
		}

		// エディターの表示
		if (editor)
		{
			// 枠を表示
			DrawBox(editorX, editorY,
				editorX + beat * 32, editorY + BUTTON_Y, colourBlack, TRUE); // 上
			DrawBox(editorX + beat * 2, (SCREEN_HEIGHT >> 2) * 3,
				editorX + beat * 32 - beat * 2, (SCREEN_HEIGHT >> 2) * 3 + BUTTON_Y, colourBlack, TRUE); // 下

			if (measureCount == measureEdit)
			{
				// カーソルの表示
				DrawBox(editorX + beat * (1 + beatCount), (SCREEN_HEIGHT >> 2) * 3,
					editorX + beat * (2 + beatCount), (SCREEN_HEIGHT >> 2) * 3 + BUTTON_Y, colourRed, FALSE);
			}
			
			// 現在の小節数を表示
			sprintf_s(msg, "%d", measureEdit);
			DrawStringToHandle(editorX + (FONT_SIZE >> 1), (SCREEN_HEIGHT >> 2) - beat * 2 + (FONT_SIZE >> 2), msg, colourWhite, fontHandle32);

			// 小節を左に移動
			if (DrawButton(editorX, (SCREEN_HEIGHT >> 2) + beat * 15, beat * 2, BUTTON_Y, "←", fontHandle24))
			{
				if (measureEdit == 1)
				{
					measureEdit = BEAT_NUM;
				}
				else
				{
					measureEdit--;
				}
			}

			// 小節を右に移動
			if (DrawButton(editorX + beat * 32 - beat * 2, (SCREEN_HEIGHT >> 2) + beat * 15, beat * 2, BUTTON_Y, "→", fontHandle24))
			{
				if (measureEdit == BEAT_NUM)
				{
					measureEdit = 1;
				}
				else
				{
					measureEdit++;
				}
			}

			printfDx("伯数 %d\n", lineCounter[pattern][measureEdit - 1]);

			for (int i = 0; i < lineCounter[pattern][measureEdit - 1]; i++)
			{
				for (int j = 0; j < sizeof(drum_set) / sizeof(int); j++)
				{
					lamp = beatsBuffer[pattern][measureEdit - 1][i][j];
					//printfDx("%d", lamp);

					// ランプを表示
					// クリックされたら0と1を入れ替える
					if (DrawOnOffLamp(editorX + beat * (i + 2), (SCREEN_HEIGHT >> 2) + beat * j, beat, lamp))
					{
						if (lamp)
						{
							beatsBuffer[pattern][measureEdit - 1][i][j] = 0;
						}
						else
						{
							beatsBuffer[pattern][measureEdit - 1][i][j] = 1;
						}
					}
				}
			}

			// 伯ごとの敷居
			for (int i = 0; i < night; i++)
			{
				// カーソルが被っていたら赤くなる
				if ((beatCount - 1) / 4 == i)
				{
					DrawBox(editorX + beat * (2 + 4 * i), (SCREEN_HEIGHT >> 2),
						editorX + beat * (2 + 4 + 4 * i), (SCREEN_HEIGHT >> 2) * 3, colourRed, FALSE);

				}
				else
				{
					DrawBox(editorX + beat * (2 + 4 * i), (SCREEN_HEIGHT >> 2),
						editorX + beat * (2 + 4 + 4 * i), (SCREEN_HEIGHT >> 2) * 3, colourYellow, FALSE);
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
		if (DrawButton(SCREEN_WIDTH - BUTTON_X * 4, BUTTON_Y * 13, BUTTON_X * 4, BUTTON_Y, msg, fontHandle24))
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
		if (DrawButton(SCREEN_WIDTH - BUTTON_X * 4, BUTTON_Y * 15, BUTTON_X * 4, BUTTON_Y, "RESET", fontHandle24))
		{
			beatCount = 0;
			measure = 0;
			measureCount = 0;

			drum_start = FALSE;
		}

		// アプリ終了ボタン
		if (DrawButton(SCREEN_WIDTH - BUTTON_X * 4, BUTTON_Y * 17, BUTTON_X * 4, BUTTON_Y, "OK", fontHandle24))
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
	for (int i = 0; i < sizeof(drum_set) / sizeof(int); i++)
	{
		DeleteSoundMem(drum_set[i]);
	}

	// 画像のグラフィックハンドルを削除
	DeleteGraph(image_nijika);
	DeleteGraph(image_yamada);

	// フォントハンドルを削除
	DeleteFontToHandle(fontHandle16);
	DeleteFontToHandle(fontHandle24);
	DeleteFontToHandle(fontHandle32);

	// ファイルを閉じる
	for (int p = 0; p < PATTERN_NUM; p++)
	{
		for (int b = 0; b < BEAT_NUM; b++)
		{
			FileRead_close(fileHandles[p][b]);
		}
	}

	DxLib_End(); // DXライブラリ使用の終了処理

	return 0;
}