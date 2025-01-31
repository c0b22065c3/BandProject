#include "DxLib.h"
#include "math.h"
#include "time.h"

#include <iostream>
#include <fstream>

// ��ʂ̉𑜓x
#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480

#define STANDARD_BPM	60		// ���BPM
#define MAX_BPM			400		// �ő��BPM

#define MAX_BEAT		64		// �ő�̃r�[�g

#define KILO			1000

#define FONT_SIZE		16		// �t�H���g�̃T�C�Y
#define FONT_THICK		2		// �t�H���g�̑���

// �{�^���̃T�C�Y
#define BUTTON_X		32
#define BUTTON_Y		24

#define CHECK_SIZE		16		// �`�F�b�N�{�b�N�X�̃T�C�Y

#define PATTERN_NUM		8		// �p�^�[���̃Z�[�u�f�[�^�̐�
#define BEAT_NUM		4		// 4���߂�1�܂Ƃ܂�

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

// �G�f�B�^�[�t���O
BOOL editor				= FALSE;

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

	// �h�����̃T�E���h�n���h��
	int drum_kick_1		= LoadSoundMem("DrumSound/maou_se_inst_drum1_kick.wav");	// �L�b�N
	int drum_kick_2		= LoadSoundMem("DrumSound/maou_se_inst_drum2_kick.wav");	// �L�b�N2
	int drum_snare_1	= LoadSoundMem("DrumSound/maou_se_inst_drum1_snare.wav");	// �X�l�A
	int drum_snare_2	= LoadSoundMem("DrumSound/maou_se_inst_drum2_snare.wav");	// �X�l�A2
	int drum_hat_1		= LoadSoundMem("DrumSound/maou_se_inst_drum1_hat.wav");		// �n�b�g
	int drum_hat_2		= LoadSoundMem("DrumSound/maou_se_inst_drum2_hat.wav");		// �n�b�g2
	int drum_symbal_1	= LoadSoundMem("DrumSound/maou_se_inst_drum1_cymbal.wav");	// �V���o��
	int drum_symbal_2	= LoadSoundMem("DrumSound/maou_se_inst_drum2_cymbal.wav");	// �V���o��2
	int drum_tom1_1		= LoadSoundMem("DrumSound/maou_se_inst_drum1_tom1.wav");	// �^��1
	int drum_tom1_2		= LoadSoundMem("DrumSound/maou_se_inst_drum2_tom1.wav");	// �^��1-2
	int drum_tom2_1		= LoadSoundMem("DrumSound/maou_se_inst_drum1_tom2.wav");	// �^��2
	int drum_tom2_2		= LoadSoundMem("DrumSound/maou_se_inst_drum2_tom2.wav");	// �^��2-2
	int drum_tom3_1		= LoadSoundMem("DrumSound/maou_se_inst_drum1_tom3.wav");	// �^��3
	int drum_tom3_2		= LoadSoundMem("DrumSound/maou_se_inst_drum2_tom3.wav");	// �^��3-2
	int drum_roll		= LoadSoundMem("DrumSound/maou_se_inst_drumroll.wav");		// �h�������[��

	// �n���h���z��
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

	// �摜�̃n���h��
	int image_nijika = LoadGraph("Image/nijika_doritos.png");
	int image_yamada = LoadGraph("Image/sekaino_yamada.png");

	int second = 0;				// �b��

	int beat = 16;				// �r�[�g
	int beatCount = 0;			// �J�E���g

	int night = 4;				// ���q
	int measure = 0;			// ���ߐ�
	int measureCount = 0;		// ���ߐ��̃J�E���g

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
	int lineCounter[PATTERN_NUM][BEAT_NUM] = { 0 };
	int beatsBuffer[PATTERN_NUM][BEAT_NUM][32][16] = { 0 };
	char stringBuffer[PATTERN_NUM][BEAT_NUM][32][16];

	int fileHandles[PATTERN_NUM][BEAT_NUM];
	
	// �e�L�X�g�t�@�C�����J��
	//int fileHandle = FileRead_open("PatternData/sample.txt");

	for (ice = 0; ice < PATTERN_NUM; ice++)
	{
		for (jam = 0; jam < BEAT_NUM; jam++)
		{
			sprintf_s(msg, "PatternData/Pattern%d/beat%d.txt", ice, jam);
			fileHandles[ice][jam] = FileRead_open(msg);

			// �t�@�C������s���ǂݍ���Ŋi�[
			while (FileRead_eof(fileHandles[ice][jam]) == 0)
			{
				FileRead_gets(stringBuffer[ice][jam][lineCounter[ice][jam]], sizeof(stringBuffer), fileHandles[ice][jam]);

				for (custard = 0; custard < sizeof(drum_set) / sizeof(int); custard++)
				{
					if (stringBuffer[ice][jam][lineCounter[ice][jam]][custard] == '0')
					{
						beatsBuffer[ice][jam][lineCounter[ice][jam]][custard] = 0;
					}
					else
					{
						beatsBuffer[ice][jam][lineCounter[ice][jam]][custard] = 1;
					}
				}

				lineCounter[ice][jam]++;
			}

			FileRead_close(fileHandles[ice][jam]);
		}
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
				if (beatCount > (beat >> 2) * night)
				{
					beatCount = 1;
				}

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

				// �����𒲐�
				night = lineCounter[pattern][measureCount - 1] / 4;

				//beatCount = beatCount % ((beat >> 2) * night);

				// �t�@�C���̓��e�ɉ���������炷
				for (ice = 0; ice < sizeof(drum_set) / sizeof(int); ice++)
				{
					if (beatsBuffer[pattern][measureCount - 1][beatCount - 1][ice])
					{
						PlaySoundMem(drum_set[ice], DX_PLAYTYPE_BACK);
					}
				}
			}
		}

		// �ȈՕ\��
		printfDx("%d�b\n", second);
		//printfDx("BPM%d\n", bpm);
		printfDx("%d\n", beatCount);
		//printfDx("%d�r�[�g\n", beat);
		printfDx("%d���q\n", night);
		//printfDx("%d����\n", measure);
		printfDx("%d����\n", measureCount);
		//printfDx("�p�^�[�� %d\n", pattern);

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

			night = lineCounter[pattern][measureEdit - 1] / 4;
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

			night = lineCounter[pattern][measureEdit - 1] / 4;
		}

		// �G�f�B�^�[�{�^��
		if (DrawButton(SCREEN_WIDTH - BUTTON_X * 4, editorY, BUTTON_X * 4, BUTTON_Y, "EDITOR", fontHandle24))
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

		// �G�f�B�^�[�̕\��
		if (editor)
		{
			// ���݂̏��ߐ���\��
			sprintf_s(msg, "%d", measureEdit);

			// ���ߐ���\��
			DrawButton(editorX, editorY, beat * 2, BUTTON_Y, msg, fontHandle24, TRUE);

			//DrawButton(editorX + beat * 2, editorY, beat * 28, BUTTON_Y, "", fontHandle24, TRUE);

			// ���݂̏��ߐ������o�I�ɕ\��
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

			// ����\��
			sprintf_s(msg, "%d", night);
			DrawButton(editorX + beat * 30, editorY, beat * 2, BUTTON_Y, msg, fontHandle24, TRUE);

			// �����v���X
			if (DrawButton(editorX + beat * 30, editorY + BUTTON_Y, beat * 2, beat * 2, "+", fontHandle32))
			{
				if (night < 7)
				{
					night++;
				}

				lineCounter[pattern][measureEdit - 1] = 4 * night;
			}

			// �����}�C�i�X
			if (DrawButton(editorX + beat * 30, editorY + BUTTON_Y + beat * 2, beat * 2, beat * 2, "-", fontHandle32))
			{
				if (night > 2)
				{
					night--;
				}

				lineCounter[pattern][measureEdit - 1] = 4 * night;
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
				if (measureEdit == 1)
				{
					measureEdit = BEAT_NUM;
				}
				else
				{
					measureEdit--;
				}

				night = lineCounter[pattern][measureEdit - 1] / 4;
			}

			// �J�[�\���̈ʒu���ړ�����{�^��
			for (ice = 0; ice < 28; ice++)
			{
				if (DrawButton(editorX + beat * (2 + ice), (SCREEN_HEIGHT >> 2) + beat * 15, beat * 1, BUTTON_Y, "", fontHandle24, TRUE))
				{
					if (!drum_start)
					{
						measureCount = measureEdit;
						beatCount = ice + 1;
					}
				}
			}

			// ���߂��E�Ɉړ�����{�^��
			if (DrawButton(editorX + beat * 30, (SCREEN_HEIGHT >> 2) + beat * 15, beat * 2, BUTTON_Y, "��", fontHandle24))
			{
				if (measureEdit == BEAT_NUM)
				{
					measureEdit = 1;
				}
				else
				{
					measureEdit++;
				}

				night = lineCounter[pattern][measureEdit - 1] / 4;
			}

			if (measureCount == measureEdit)
			{
				// �J�[�\���̕\��
				DrawBox(editorX + beat * (1 + beatCount), (SCREEN_HEIGHT >> 2) + beat * 15,
					editorX + beat * (2 + beatCount), (SCREEN_HEIGHT >> 2) + beat * 15 + BUTTON_Y, colourRed, FALSE);
			}

			// �N���A�{�^��
			if (DrawButton(editorX + beat * 4, BUTTON_Y * 17,
				beat * 8, BUTTON_Y, "CLEAR", fontHandle24))
			{
				integer = lineCounter[pattern][measureEdit - 1];

				lineCounter[pattern][measureEdit - 1] = 4 * 7;

				// �o�b�t�@��0����
				for (ice = 0; ice < lineCounter[pattern][measureEdit - 1]; ice++)
				{
					for (jam = 0; jam < sizeof(drum_set) / sizeof(int); jam++)
					{
						beatsBuffer[pattern][measureEdit - 1][ice][jam] = 0;
					}
				}

				lineCounter[pattern][measureEdit - 1] = integer;
			}

			// �Z�[�u�{�^��
			if (DrawButton(editorX + beat * 20, BUTTON_Y * 17,
				beat * 8, BUTTON_Y, "SAVE", fontHandle24))
			{
				sprintf_s(msg, "PatternData/Pattern%d/beat%d.txt", pattern, measureEdit - 1);
				std::ofstream file(msg);

				for (ice = 0; ice < lineCounter[pattern][measureEdit - 1]; ice++)
				{
					for (jam = 0; jam < sizeof(drum_set) / sizeof(int); jam++)
					{
						if (beatsBuffer[pattern][measureEdit - 1][ice][jam] == 0)
						{
							stringBuffer[pattern][measureEdit - 1][ice][jam] = '0';
						}
						else
						{
							stringBuffer[pattern][measureEdit - 1][ice][jam] = '1';
						}

						file << beatsBuffer[pattern][measureEdit - 1][ice][jam]; // �t�@�C���֏�������
					}

					file << std::endl; // ���s
				}
			}

			// �ҏW����\��
			for (ice = 0; ice < lineCounter[pattern][measureEdit - 1]; ice++)
			{
				for (jam = 0; jam < sizeof(drum_set) / sizeof(int); jam++)
				{
					lamp = beatsBuffer[pattern][measureEdit - 1][ice][jam];
					//printfDx("%d", lamp);

					// �����v��\��
					// �N���b�N���ꂽ��0��1�����ւ���
					if (DrawOnOffLamp(editorX + beat * (ice + 2), (SCREEN_HEIGHT >> 2) + beat * jam, beat, lamp))
					{
						if (lamp)
						{
							beatsBuffer[pattern][measureEdit - 1][ice][jam] = 0;
						}
						else
						{
							beatsBuffer[pattern][measureEdit - 1][ice][jam] = 1;
						}
					}
				}
			}

			// �����Ƃ̕~��
			for (ice = 0; ice < night; ice++)
			{
				// �J�[�\��������Ă�����Ԃ��Ȃ�
				if ((beatCount - 1) / 4 == ice && measureCount == measureEdit)
				{
					DrawBox(editorX + beat * (2 + 4 * ice), (SCREEN_HEIGHT >> 2),
						editorX + beat * (2 + 4 + 4 * ice), (SCREEN_HEIGHT >> 2) * 3, colourRed, FALSE);

				}
				else
				{
					DrawBox(editorX + beat * (2 + 4 * ice), (SCREEN_HEIGHT >> 2),
						editorX + beat * (2 + 4 + 4 * ice), (SCREEN_HEIGHT >> 2) * 3, colourYellow, FALSE);
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
			measure = 0;
			measureCount = 0;

			drum_start = FALSE;
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