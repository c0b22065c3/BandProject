#include "DxLib.h"
#include "math.h"
#include "time.h"

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

	// �}�E�X���{�^���͈͓̔��ɂ���Ƃ�
	if (MouseInRange(beginX, beginY, beginX + sizeX, beginY + sizeY))
	{
		// ����\��
		DrawBox(beginX, beginY, beginX + sizeX, beginY + sizeY, colourBlack, FALSE);

		// �����̕\��
		DrawStringToHandle(str_x, beginY, str, colourBlack, fontHandle);

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
	int drum_kick_1 = LoadSoundMem("DrumSound/maou_se_inst_drum1_kick.wav");

	// �摜�̃n���h��
	int image_nijika = LoadGraph("Image/nijika_doritos.png");
	int image_yamada = LoadGraph("Image/sekaino_yamada.png");

	int second = 0;		// �b��

	int beat = 4;		// �r�[�g
	int beatCount = 0;	// �J�E���g

	int night = 4;		// ���q
	int measure = 0;	// ���ߐ�

	float bpmRatio = 1.0f;	// �BPM�Ƃ̔䗦
	float bpmScroll = 1.0f;	// BPM�̃X�N���[���o�[�̔䗦

	// �t�H���g�n���h��
	int fontHandle16 = CreateFontToHandle("PixelMplus12", FONT_SIZE, FONT_THICK);
	int fontHandle24 = CreateFontToHandle("PixelMplus12", FONT_SIZE + (FONT_SIZE >> 1), FONT_THICK);
	int fontHandle32 = CreateFontToHandle("PixelMplus12", FONT_SIZE * 2, FONT_THICK);

	// �ϐ�����ʂɕ\������ׂ̕ϐ�
	char msg[256] = "";

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

		printfDx("%d�b\n", second);
		printfDx("BPM%d\n", bpm);
		printfDx("%d\n", beatCount);
		printfDx("%d�r�[�g\n", beat);
		printfDx("%d���q\n", night);
		printfDx("%d����\n", measure);

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
		DrawStringToHandle(SCREEN_WIDTH - BUTTON_X * 6 + (FONT_SIZE >> 0), BUTTON_Y, "BPM", colourBlack, fontHandle24);

		// ���̃{�^��
		if (DrawButton(SCREEN_WIDTH - BUTTON_X * 4, BUTTON_Y, BUTTON_X, BUTTON_Y, "-", fontHandle24))
		{
			if (bpm > 0)
			{
				bpm--;
			}
		}

		sprintf_s(msg, "%d", bpm);
		DrawStringToHandle(SCREEN_WIDTH - BUTTON_X * 3 + (BUTTON_X >> 1), BUTTON_Y, msg, colourBlack, fontHandle24);

		// �E�̃{�^��
		if (DrawButton(SCREEN_WIDTH - BUTTON_X, BUTTON_Y, BUTTON_X, BUTTON_Y, "+", fontHandle24))
		{
			if (bpm < MAX_BPM)
			{
				bpm++;
			}
		}

		// �r�[�g�̑���
		// ����
		DrawStringToHandle(SCREEN_WIDTH - BUTTON_X * 6 + (FONT_SIZE >> 0), BUTTON_Y * 3, "beat", colourBlack, fontHandle24);

		// ���̃{�^��
		if (DrawButton(SCREEN_WIDTH - BUTTON_X * 4, BUTTON_Y * 3, BUTTON_X, BUTTON_Y, "-", fontHandle24))
		{
			if (beat > 1)
			{
				beat >>= 1;
			}
		}

		sprintf_s(msg, "%d", beat);
		DrawStringToHandle(SCREEN_WIDTH - BUTTON_X * 3 + (BUTTON_X >> 1), BUTTON_Y * 3, msg, colourBlack, fontHandle24);

		// �E�̃{�^��
		if (DrawButton(SCREEN_WIDTH - BUTTON_X, BUTTON_Y * 3, BUTTON_X, BUTTON_Y, "+", fontHandle24))
		{
			if (beat < 64)
			{
				beat <<= 1;
			}
		}

		// ���q�̑���
		// ����
		DrawStringToHandle(SCREEN_WIDTH - BUTTON_X * 6 + (FONT_SIZE >> 0), BUTTON_Y * 5, "��", colourBlack, fontHandle24);

		// ���̃{�^��
		if (DrawButton(SCREEN_WIDTH - BUTTON_X * 4, BUTTON_Y * 5, BUTTON_X, BUTTON_Y, "-", fontHandle24))
		{
			if (night > 2)
			{
				night--;
			}
		}

		sprintf_s(msg, "%d", night);
		DrawStringToHandle(SCREEN_WIDTH - BUTTON_X * 3 + (BUTTON_X >> 1), BUTTON_Y * 5, msg, colourBlack, fontHandle24);

		// �E�̃{�^��
		if (DrawButton(SCREEN_WIDTH - BUTTON_X, BUTTON_Y * 5, BUTTON_X, BUTTON_Y, "+", fontHandle24))
		{
			if (night < 7)
			{
				night++;
			}
		}

		// �X�^�[�g�{�^��
		if (DrawButton(SCREEN_WIDTH - BUTTON_X * 4, BUTTON_Y * 13, BUTTON_X * 4, BUTTON_Y, "PLAY", fontHandle24))
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

		// �A�v���I���{�^��
		if (DrawButton(SCREEN_WIDTH - BUTTON_X * 4, BUTTON_Y * 15, BUTTON_X * 4, BUTTON_Y, "OK", fontHandle24))
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

	DxLib_End(); // DX���C�u�����g�p�̏I������

	return 0;
}