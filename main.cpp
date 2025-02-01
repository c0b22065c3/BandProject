#include "DxLib.h"
#include "math.h"
#include "time.h"

#include <iostream>
#include <fstream>

#define SCREEN_WIDTH	640		// ��ʂ̉𑜓xX
#define SCREEN_HEIGHT	480		// ��ʂ̉𑜓xY

#define STANDARD_BPM	60		// ���BPM
#define MAX_BPM			400		// �ő��BPM

#define MAX_BEAT		64		// �ő�̃r�[�g

#define KILO			1000

#define FONT_SIZE		16		// �t�H���g�̃T�C�Y
#define FONT_THICK		2		// �t�H���g�̑���

#define BUTTON_X		32		// �{�^���̃T�C�YX
#define BUTTON_Y		24		// �{�^���̃T�C�YY

#define CHECK_SIZE		16		// �`�F�b�N�{�b�N�X�̃T�C�Y

#define PATTERN_NUM		16		// �p�^�[���̃Z�[�u�f�[�^�̐�
/*
#define BEAT_NUM		4		// 4���߂�1�܂Ƃ܂�
*/

int MouseX, MouseY;				// �}�E�X��XY���W

// ����
int startTime;					// �Q�[���J�n����
int nowTime;					// ���݂̎���
int oldTime;					// �ЂƂO�̎���

// BPM
int bpm = STANDARD_BPM * 2;		// 120BPM

// �F
unsigned int colourBlack	= GetColor(0, 0, 0);		// ��
unsigned int colourWhite	= GetColor(255, 255, 255);	// �� 
unsigned int colourRed		= GetColor(255, 0, 0);		// ��
unsigned int colourGreen	= GetColor(0, 255, 0);		// ��
unsigned int colourBlue		= GetColor(0, 0, 255);		// ��
unsigned int colourYellow	= GetColor(255, 255, 0);	// ��
unsigned int colourPurple	= GetColor(255, 0, 255);	// ��
unsigned int colourWater	= GetColor(0, 255, 255);	// ��
unsigned int colourOrange	= GetColor(238, 120, 0);	// ��

// �L�[�{�[�h���
char keyState[256];		// �L�[�{�[�h���
char oldKeyState[256];	// �ЂƂO�̃L�[�{�[�h���

// �}�E�X���
BOOL isMouseLeft		= FALSE;
BOOL isMouseMiddle		= FALSE;
BOOL isMouseRight		= FALSE;

// �ЂƂO�̃}�E�X���
BOOL isOldMouseLeft		= FALSE;
BOOL isOldMouseMiddle	= FALSE;
BOOL isOldMouseRight	= FALSE;

// �X�^�[�g�t���O
BOOL drum_start			= FALSE;

// �Z�b�V�����t���O
BOOL session_start		= FALSE;

// �Ȃ̍\��
struct Composition
{
	const char* name; // �\����
	int loop; // �J��Ԃ���
	int patternNum; // �p�^�[��
};

// ��ʃ��[�h
enum ScreenMode
{
	normal,
	editor,
	arrenge
};

// �i�s
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

// �������擾����֐�
int GetRandom(int min, int max)
{
	return min + (int)(rand() * (max - min + 1.0) / (1.0 + RAND_MAX));
}

// �}�E�X�N���b�N���ꂽ���𔻒肷��֐�
BOOL ClickMouse(int button)
{
	switch (button)
	{
		// ���N���b�N�̂Ƃ�
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

// �}�E�X���͈͓��ɓ����Ă��邩�𔻒肷��֐�
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

// �{�^����\������֐�
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

	// �}�E�X���{�^���͈͓̔��ɂ���Ƃ�
	if (MouseInRange(beginX, beginY, beginX + sizeX, beginY + sizeY))
	{
		// ����\��
		DrawBox(beginX, beginY, beginX + sizeX, beginY + sizeY, colourBlack, notTransparent);

		if (notTransparent)
		{
			// �����̕\��
			DrawStringToHandle(str_x, beginY, str, colourWhite, fontHandle);
		}
		else
		{
			// �����̕\��
			DrawStringToHandle(str_x, beginY, str, colourBlack, fontHandle);
		}

		// �w��̃}�E�X�{�^���������ꂽ��TRUE
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
		// ����\���i���߁j
		DrawBox(beginX, beginY, beginX + sizeX, beginY + sizeY, colourBlack, TRUE);

		// �����̕\��
		DrawStringToHandle(str_x, beginY, str, colourWhite, fontHandle);

		return FALSE;
	}
}

// �`�F�b�N�{�b�N�X��\������֐�
BOOL DrawCheckBox(int beginX, int beginY, int size, const char* str = "", int fontHandle = NULL, BOOL defaultCheck = FALSE)
{
	static BOOL check = defaultCheck;

	DrawBox(beginX, beginY, beginX + size, beginY + size, colourBlack, FALSE); // �{�b�N�X��\��
	DrawStringToHandle(beginX + size + (size >> 2), beginY, str, colourBlack, fontHandle); // ������\��

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
		// �`�F�b�N��\��
		DrawLine(beginX, beginY + ((beginY + size - beginY) >> 1), beginX + ((beginX + size - beginX) >> 1), beginY + size, colourRed, size >> 3);
		DrawLine(beginX + ((beginX + size - beginX) >> 1), beginY + size, beginX + size, beginY, colourRed, size >> 3);
	}

	return check;
}

// ON-OFF�����v��\������֐�
BOOL DrawOnOffLamp(int beginX, int beginY, int size, BOOL lighting = FALSE)
{
	// �����v�̕\��
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

// �X�N���[���o�[��\������֐�
float DrawScrollBarWidth(int begin, int end, int place, int cursorSize, unsigned int color, float external)
{
	int length = end - begin;
	int cursorLength;

	static int cursorX = end - (length >> 1);
	static int cursorY = place;

	static BOOL isMove = FALSE;

	DrawBox(begin - 4, place - 4, end + 4, place + 4, color, TRUE);

	// �}�E�X��������Ă����Ԃ�
	if (ClickMouse(0))
	{
		// �{�^���͈͓̔��ɂ��邩
		if (MouseInRange(cursorX - cursorSize, cursorY - cursorSize, cursorX + cursorSize, cursorY + cursorSize))
		{
			// ���N���b�N����
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

	// �J�[�\�����}�E�X��X���W�ɍ��킹��
	if (isMove)
	{
		cursorX = MouseX;

		// �͈͊O�Ɏ��߂�
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

	// �J�[�\����\��
	DrawCircle(cursorX, cursorY, cursorSize, color, TRUE);

	// �o�[�̍��͂�����J�[�\���܂ł̒���
	cursorLength = cursorX - begin;

	return (float)cursorLength / (float)length;
}

// ���C���֐�
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ChangeWindowMode(TRUE); // �E�B���h�E���[�h�ɕύX
	SetGraphMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32); // �𑜓x�̐ݒ�
	SetMainWindowText("�M�^�[���K�A�v���P�[�V�����i���j"); // �E�B���h�E�̃^�C�g����ύX

	SetMouseDispFlag(TRUE); // �}�E�X����ʏ�ł��\������

	SetBackgroundColor(255, 255, 255); // �w�i�𔒐F�ɐݒ�

	if (DxLib_Init() == -1) // DX���C�u��������������
	{
		return -1; // �G���[���N�����璼���ɏI��
	}

	// ------------------------------------
	// �ϐ���`�]�[��
	// ------------------------------------

	// �h���������̃n���h���z��
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

	// �摜�̃n���h��
	int image_nijika = LoadGraph("Image/nijika_doritos.png");
	int image_yamada = LoadGraph("Image/sekaino_yamada.png");

	int second = 0;				// �b��

	int beat = 16;				// �r�[�g
	int beatCount = 0;			// �J�E���g
	int beatCountPart = 0;		// �p�[�g���Ƃ̃r�[�g�J�E���g

	int meter = 4;				// ���q

	int pattern = 0;			// �p�^�[���̔ԍ�

	int lamp = 0;				// �����v�_��
	int measureEdit = 1;		// �G�f�B�^�[�ł̌��݂̏���
	int editorX = 0;			// �G�f�B�^�[�̍��[��X���W
	int editorY = BUTTON_Y * 4;	// �G�f�B�^�[�̍��[��Y���W

	float bpmRatio = 1.0f;		// �BPM�Ƃ̔䗦
	float bpmScroll = 1.0f;		// BPM�̃X�N���[���o�[�̔䗦

	// �t�H���g�n���h��
	int fontHandle16 = CreateFontToHandle("PixelMplus12", FONT_SIZE, FONT_THICK);
	int fontHandle24 = CreateFontToHandle("PixelMplus12", FONT_SIZE + (FONT_SIZE >> 1), FONT_THICK);
	int fontHandle32 = CreateFontToHandle("PixelMplus12", FONT_SIZE * 2, FONT_THICK);

	// �ϐ����ꎞ�I�Ɋi�[����ׂ̕ϐ�
	int integer = 0;
	float floating = 0.0f;

	char msg[32] = "";

	// for���ŌJ��Ԃ��ׂ̕ϐ�
	int ice = 0;
	int jam = 0;
	int custard = 0;
	int chocolate = 0;

	// �e�L�X�g�t�@�C���̍s���Ɗi�[��
	int lineCounter[PATTERN_NUM] = { 0 };

	int fileHandles[PATTERN_NUM];

	char stringBuffer[PATTERN_NUM][32][16];
	int beatsBuffer[PATTERN_NUM][32][16] = { 0 };

	int copyBuffer[32][16] = { 0 };
	int copyLine = 0;

	// �\����
	Composition comp[] = { {"SESSION "}, {"In", 1, 0}, {"A", 1, 0}, {"B", 1, 0}, {"C", 1, 0}, {"D", 1, 0}, {"Out", 1, 0} };

	ScreenMode screen = normal;
	SessionProgress sessionProgress = silence;
	
	// �e�L�X�g�t�@�C�����J��
	//int fileHandle = FileRead_open("PatternData/sample.txt");

	// �t�@�C���̓ǂݍ���
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

		// �t�@�C������s���ǂݍ���Ŋi�[
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

	// �ЂƂO�̃L�[�{�[�h����������
	for (int key = 0; key < 256; key++)
	{
		oldKeyState[key] = 0;
	}

	// ����Ⴄ�����𐶐�
	srand((unsigned int)time(NULL));

	// �Q�[���J�n���Ԃ𓾂�
	startTime = GetNowCount();

	// �Q�[�����[�v
	while (ProcessMessage() == 0)
	{
		// ------------------------------------
		// �O����
		// ------------------------------------
		
		// �Q�[���J�n����̌o�ߎ��Ԃ��X�V
		nowTime = GetNowCount() - startTime;

		second = nowTime / KILO;

		// ���݂̃L�[�{�[�h�����X�V
		GetHitKeyStateAll(keyState);

		// ���݂̃}�E�X�����X�V
		isMouseLeft		= ClickMouse(0);
		isMouseMiddle	= ClickMouse(1);
		isMouseRight	= ClickMouse(2);

		// �}�E�X�̈ʒu���擾
		GetMousePoint(&MouseX, &MouseY);

		// �Z�b�V�������[�h
		/*
		if (session_start)
		{
			switch (sessionProgress)
			{
				// ����O
			case silence:
				sessionProgress = intro;
				drum_start = TRUE;
				break;

				// �C���g��
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

				// A����
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

				// B����
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

				// �T�r
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

				// D����
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

				// �A�E�g��
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

		// �h�����̉��̏���
		if (drum_start)
		{
			// BPM�̔䗦
			bpmRatio = (float)bpm * ((float)beat / 4.0f) / (float)STANDARD_BPM;

			// BPM�ɉ����ĉ���炷
			if (nowTime % (int)(KILO / bpmRatio) < oldTime % (int)(KILO / bpmRatio))
			{
				beatCount++;

				// �J�E���g�̒���
				if (beatCount > (beat >> 2) * meter)
				{
					beatCount = 1;
				}

				/*
				// ���̏��߂�
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

				// �����𒲐�
				meter = lineCounter[pattern] / 4;

				//beatCount = beatCount % ((beat >> 2) * night);

				// �t�@�C���̓��e�ɉ���������炷
				for (ice = 0; ice < sizeof(drum_set) / sizeof(int); ice++)
				{
					if (beatsBuffer[pattern][beatCount - 1][ice])
					{
						PlaySoundMem(drum_set[ice], DX_PLAYTYPE_BACK);
					}
				}

				/*
				// ���݂̃p�[�g�̍Ŋ��̏���
				if (measure >= comp[sessionProgress].loop * BEAT_NUM)
				{
					beatCountPart++;
				}
				*/
			}
		}

		// �ȈՕ\��
		printfDx("%d�b\n", second);
		printfDx("BPM%d\n", bpm);
		printfDx("%d\n", beatCount);
		printfDx("%d�r�[�g\n", beatCountPart);
		printfDx("%d���q\n", meter);
		//printfDx("�p�^�[�� %d\n", pattern);
		printfDx("�i�s %d\n", sessionProgress);

		//// �t�@�C���̒��g���ȈՕ\��
		//for (int i = 0; i < lineCounter[0][0]; i++)
		//{
		//	printfDx(stringBuffer[0][0][i]);
		//	printfDx("\n");
		//}

		// ------------------------------------
		// �`�揈��
		// ------------------------------------
		ClearDrawScreen(); // ��ʂ��Ă�����

		DrawGraph(0, 0, image_nijika, TRUE); // ���Ă�����\��

		// �`�F�b�N���t���Ă�����R�c��\��
		if (DrawCheckBox(100, BUTTON_Y, BUTTON_Y, "�R�c", fontHandle24, TRUE))
		{
			DrawGraph(0, 0, image_yamada, TRUE);
		}

		// BPM�̑���
		// ����
		DrawStringToHandle(SCREEN_WIDTH - BUTTON_X * 6 + (FONT_SIZE >> 0), BUTTON_Y * 0, "BPM", colourBlack, fontHandle24);

		// ���̃{�^��
		if (DrawButton(SCREEN_WIDTH - BUTTON_X * 4, BUTTON_Y * 0, BUTTON_X, BUTTON_Y, "-", fontHandle24))
		{
			if (bpm > 0)
			{
				bpm--;
			}
		}

		sprintf_s(msg, "%d", bpm);
		DrawStringToHandle(SCREEN_WIDTH - BUTTON_X * 3 + (BUTTON_X >> 1), BUTTON_Y * 0, msg, colourBlack, fontHandle24);

		// �E�̃{�^��
		if (DrawButton(SCREEN_WIDTH - BUTTON_X, BUTTON_Y * 0, BUTTON_X, BUTTON_Y, "+", fontHandle24))
		{
			if (bpm < MAX_BPM)
			{
				bpm++;
			}
		}

		// �p�^�[���̑���
		// ����
		DrawStringToHandle(SCREEN_WIDTH - BUTTON_X * 7 - (BUTTON_X >> 1) + (FONT_SIZE >> 0), BUTTON_Y * 2, "PATTERN", colourBlack, fontHandle24);

		// ���̃{�^��
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

		// �E�̃{�^��
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

		// �G�f�B�^�{�^��
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

		// �G�f�B�^�̕\��
		if (screen == editor)
		{
			// ���݂̃p�^�[����\��
			sprintf_s(msg, "%d", pattern);

			// �p�^�[����\��
			DrawButton(editorX, editorY, beat * 2, BUTTON_Y, msg, fontHandle24, TRUE);

			//DrawButton(editorX + beat * 2, editorY, beat * 28, BUTTON_Y, "", fontHandle24, TRUE);

			// ���݂̏��ߐ������o�I�ɕ\��
			/*
			for (ice = 0; ice < BEAT_NUM; ice++)
			{
				// �Đ����̏���
				if (ice == measureCount - 1)
				{
					sprintf_s(msg, ">>>");
				}
				else
				{
					sprintf_s(msg, "%d", ice + 1);
				}

				// �ҏW���̏���
				if (ice == measureEdit - 1)
				{
					integer = colourRed;
				}
				else
				{
					integer = colourYellow;
				}

				// ���ߐ��J�[�\��
				DrawButton(editorX + beat * 2 + beat * 7 * ice, editorY,
					beat * 7, BUTTON_Y, msg, fontHandle24, TRUE);

				// �g��\��
				DrawBox(editorX + beat * 2 + beat * 7 * ice, editorY,
					editorX + beat * 2 + beat * 7 * ice + beat * 7, editorY + BUTTON_Y, integer, FALSE);
			}
			*/

			// �g��\��
			DrawBox(editorX + beat * 2, editorY,
				editorX + beat * 2 + beat * 28, editorY + BUTTON_Y, colourBlack, TRUE);

			// ����\��
			sprintf_s(msg, "%d", meter);
			DrawButton(editorX + beat * 30, editorY, beat * 2, BUTTON_Y, msg, fontHandle24, TRUE);

			// �����v���X
			if (DrawButton(editorX + beat * 30, editorY + BUTTON_Y, beat * 2, BUTTON_Y, "+", fontHandle24))
			{
				if (meter < 7)
				{
					meter++;
				}

				lineCounter[pattern] = 4 * meter;
			}

			// �����}�C�i�X
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

			// ���߂����Ɉړ�����{�^��
			if (DrawButton(editorX, (SCREEN_HEIGHT >> 2) + beat * 15, beat * 2, BUTTON_Y, "��", fontHandle24))
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

			// �J�[�\���̈ʒu���ړ�����{�^��
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

			// ���߂��E�Ɉړ�����{�^��
			if (DrawButton(editorX + beat * 30, (SCREEN_HEIGHT >> 2) + beat * 15, beat * 2, BUTTON_Y, "��", fontHandle24))
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

			// �J�[�\���̕\��
			if (beatCount != 0)
			{
				DrawBox(editorX + beat * (1 + beatCount), (SCREEN_HEIGHT >> 2) + beat * 15,
					editorX + beat * (2 + beatCount), (SCREEN_HEIGHT >> 2) + beat * 15 + BUTTON_Y, colourRed, FALSE);
			}

			// �N���A�{�^��
			if (DrawButton(editorX + beat * 2, BUTTON_Y * 17,
				beat * 4, BUTTON_Y, "CLEAR ", fontHandle24))
			{
				integer = lineCounter[pattern];

				lineCounter[pattern] = 4 * 7;

				// �o�b�t�@��0����
				for (ice = 0; ice < lineCounter[pattern]; ice++)
				{
					for (jam = 0; jam < sizeof(drum_set) / sizeof(int); jam++)
					{
						beatsBuffer[pattern][ice][jam] = 0;
					}
				}

				lineCounter[pattern] = integer;
			}

			// �R�s�[�{�^��
			if (DrawButton(editorX + beat * 8, BUTTON_Y * 17,
				beat * 4, BUTTON_Y, "COPY", fontHandle24))
			{
				// �R�s�[��̕ϐ��ɑ��
				for (ice = 0; ice < lineCounter[pattern]; ice++)
				{
					for (jam = 0; jam < sizeof(drum_set) / sizeof(int); jam++)
					{
						copyBuffer[ice][jam] = beatsBuffer[pattern][ice][jam];
					}
				}

				copyLine = lineCounter[pattern];
			}

			// �y�[�X�g�{�^��
			if (DrawButton(editorX + beat * 14, BUTTON_Y * 17,
				beat * 4, BUTTON_Y, "PASTE ", fontHandle24))
			{
				if (copyLine)
				{
					// �R�s�[��̒��g���o�b�t�@�ɕۑ�
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

			// �Z�[�u�{�^��
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

						file << beatsBuffer[pattern][ice][jam]; // �t�@�C���֏�������
					}

					file << std::endl; // ���s
				}
			}

			// �ҏW����\��
			for (ice = 0; ice < lineCounter[pattern]; ice++)
			{
				for (jam = 0; jam < sizeof(drum_set) / sizeof(int); jam++)
				{
					lamp = beatsBuffer[pattern][ice][jam];
					//printfDx("%d", lamp);

					// �����v��\��
					// �N���b�N���ꂽ��0��1�����ւ���
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

						// �N���b�N���ꂽ�特����
						PlaySoundMem(drum_set[jam], DX_PLAYTYPE_BACK);
					}
				}
			}

			// �����Ƃ̕~��
			for (ice = 0; ice < meter; ice++)
			{
				integer = colourYellow;
				
				// �J�[�\��������Ă�����Ԃ��Ȃ�
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

		// �A�����W�{�^��
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

		// �A�����W�G�f�B�^�̕\��
		/*
		if (screen == arrenge)
		{
			for (ice = 1; ice < sizeof(comp) / sizeof(Composition); ice++)
			{
				DrawButton((SCREEN_WIDTH >> 3), editorY + BUTTON_Y * 2 * ice, BUTTON_X * 2, BUTTON_Y, comp[ice].name, fontHandle24);

				sprintf_s(msg, "%d", comp[ice].loop);

				// ���[�v����񐔂����炷
				if (DrawButton((SCREEN_WIDTH >> 3) + BUTTON_X * 3, editorY + BUTTON_Y * 2 * ice,
					BUTTON_X, BUTTON_Y, "-", fontHandle24) && comp[sessionProgress].name != comp[ice].name)
				{
					if (comp[ice].loop != 0)
					{
						comp[ice].loop--;
					}
				}

				// ���[�v����񐔂�\��
				DrawButton((SCREEN_WIDTH >> 3) + BUTTON_X * 4, editorY + BUTTON_Y * 2 * ice, BUTTON_X, BUTTON_Y, msg, fontHandle24, TRUE);

				// ���[�v����񐔂𑝂₷
				if (DrawButton((SCREEN_WIDTH >> 3) + BUTTON_X * 5, editorY + BUTTON_Y * 2 * ice,
					BUTTON_X, BUTTON_Y, "+", fontHandle24) && comp[sessionProgress].name != comp[ice].name)
				{
					if (comp[ice].loop < 16)
					{
						comp[ice].loop++;
					}
				}

				sprintf_s(msg, "%d", comp[ice].patternNum);

				// �O�̃p�^�[����
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

				// �p�^�[���i���o�[��\��
				DrawButton((SCREEN_WIDTH >> 3) + BUTTON_X * 8, editorY + BUTTON_Y * 2 * ice, BUTTON_X, BUTTON_Y, msg, fontHandle24, TRUE);

				// ���̃p�^�[����
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

		// �Z�b�V�����{�^��
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

		// �X�^�[�g�{�^��
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

		// ���Z�b�g�{�^��
		if (DrawButton(SCREEN_WIDTH - BUTTON_X * 4, BUTTON_Y * 17, BUTTON_X * 4, BUTTON_Y, "RESET", fontHandle24))
		{
			beatCount = 0;
			beatCountPart = 0;

			drum_start = FALSE;
			session_start = FALSE;

			sessionProgress = silence;
		}

		// �A�v���I���{�^��
		if (DrawButton(SCREEN_WIDTH - BUTTON_X * 4, BUTTON_Y * 19, BUTTON_X * 4, BUTTON_Y, "OK", fontHandle24))
		{
			break;
		}

		// ------------------------------------
		// �㏈��
		// ------------------------------------

		// �L�[�{�[�h����ۑ�
		for (int key = 0; key < 256; key++)
		{
			oldKeyState[key] = keyState[key];
		}

		// �}�E�X����ۑ�
		isOldMouseLeft	 = isMouseLeft;
		isOldMouseMiddle = isMouseMiddle;
		isOldMouseRight	 = isMouseRight;

		// ���Ԃ�ۑ�
		oldTime = nowTime;

		ScreenFlip(); // �ߋ���Y�p

		clsDx(); // �ȈՕ����𖕎E
	}

	// ��n��

	// �T�E���h�n���h�����폜
	for (ice = 0; ice < sizeof(drum_set) / sizeof(int); ice++)
	{
		DeleteSoundMem(drum_set[ice]);
	}

	// �摜�̃O���t�B�b�N�n���h�����폜
	DeleteGraph(image_nijika);
	DeleteGraph(image_yamada);

	// �t�H���g�n���h�����폜
	DeleteFontToHandle(fontHandle16);
	DeleteFontToHandle(fontHandle24);
	DeleteFontToHandle(fontHandle32);

	//// �t�@�C�������
	//for (int p = 0; p < PATTERN_NUM; p++)
	//{
	//	for (int b = 0; b < BEAT_NUM; b++)
	//	{
	//		FileRead_close(fileHandles[p][b]);
	//	}
	//}

	DxLib_End(); // DX���C�u�����g�p�̏I������

	return 0;
}