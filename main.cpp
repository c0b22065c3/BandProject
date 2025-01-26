#include "DxLib.h"
#include "math.h"
#include "time.h"

// ��ʂ̉𑜓x
#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480

#define STANDARD_BPM	60		// ���BPM
#define MAX_BPM			400		// �ő��BPM

#define KILO			1000

#define FONT_SIZE		64		// �t�H���g�̃T�C�Y
#define FONT_THICK		3		// �t�H���g�̑���

int MouseX, MouseY;				// �}�E�X��XY���W

// ����
int startTime;					// �Q�[���J�n����
int nowTime;					// ���݂̎���
int oldTime;					// �ЂƂO�̎���

// BPM
int bpm = STANDARD_BPM;

// �F
unsigned int colorBlack = GetColor(0, 0, 0);
unsigned int colorWhite = GetColor(255, 255, 255);
unsigned int colorRed	= GetColor(255, 0, 0);

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
BOOL DrawButton(int beginX, int beginY, int endX, int endY, unsigned int color, int mouseButton, const char* str = "", unsigned int strColor = 0, int fontHandle = NULL)
{
	int buttonX = endX - beginX;
	int buttonY = endY - beginY;

	// �}�E�X���{�^���͈͓̔��ɂ���Ƃ�
	if (MouseInRange(beginX, beginY, endX, endY))
	{
		// ����\��
		DrawBox(beginX, beginY, endX, endY, color, FALSE);

		// �����̕\��
		DrawStringToHandle(beginX + (buttonX >> 1) - (FONT_SIZE >> 2), beginY + (buttonY >> 1) - (FONT_SIZE >> 1), str, color, fontHandle);

		// �w��̃}�E�X�{�^���������ꂽ��TRUE
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
		// ����\���i���߁j
		DrawBox(beginX, beginY, endX, endY, color, TRUE);

		// �����̕\��
		DrawStringToHandle(beginX + (buttonX >> 1) - (FONT_SIZE >> 2), beginY + (buttonY >> 1) - (FONT_SIZE >> 1), str, strColor, fontHandle);

		return FALSE;
	}
}

// �`�F�b�N�{�b�N�X��\������֐�
BOOL DrawCheckBox(int beginX, int beginY, int size, const char* str = "", int fontHandle = NULL)
{
	static BOOL check = FALSE;

	DrawBox(beginX, beginY, beginX + size, beginY + size, colorWhite, FALSE); // �{�b�N�X��\��
	DrawStringToHandle(beginX + size + (size >> 2), beginY, str, colorWhite, fontHandle); // ������\��

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
		DrawLine(beginX, beginY + ((beginY + size - beginY) >> 1), beginX + ((beginX + size - beginX) >> 1), beginY + size, colorRed, size >> 3);
		DrawLine(beginX + ((beginX + size - beginX) >> 1), beginY + size, beginX + size, beginY, colorRed, size >> 3);
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

	if (DxLib_Init() == -1) // DX���C�u��������������
	{
		return -1; // �G���[���N�����璼���ɏI��
	}

	// ------------------------------------
	// �ϐ���`�]�[��
	// ------------------------------------

	// �h�����̃T�E���h�n���h��
	int drum_kick_1 = LoadSoundMem("DrumSound/maou_se_inst_drum1_kick.wav");

	int second = 0;		// �b��

	int beat = 4;		// �r�[�g
	int beatCount = 0;	// �J�E���g

	int night = 4;		// ���q
	int measure = 0;	// ���ߐ�

	float bpmRatio = 1.0f;	// �BPM�Ƃ̔䗦
	float bpmScroll = 1.0f;	// BPM�̃X�N���[���o�[�̔䗦

	// �t�H���g�n���h��
	int buttonFontHandle = CreateFontToHandle("PixelMplus12", FONT_SIZE, FONT_THICK);
	int checkBoxFontHandle = CreateFontToHandle("PixelMplus12", FONT_SIZE >> 1, FONT_THICK);

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

		// ���̃{�^��
		if (DrawButton(SCREEN_WIDTH * 4 >> 4, SCREEN_HEIGHT * 10 >> 4,
			SCREEN_WIDTH * 6 >> 4, SCREEN_HEIGHT * 12 >> 4,
			colorWhite, 0, "-", colorBlack, buttonFontHandle))
		{
			if (!isOldMouseLeft)
			{
				if (bpm > 0)
				{
					bpm--;
				}
			}
		}

		// �E�̃{�^��
		if (DrawButton(SCREEN_WIDTH * 10 >> 4, SCREEN_HEIGHT * 10 >> 4,
			SCREEN_WIDTH * 12 >> 4, SCREEN_HEIGHT * 12 >> 4,
			colorWhite, 0, "+", colorBlack, buttonFontHandle))
		{
			if (!isOldMouseLeft)
			{
				if (bpm < MAX_BPM)
				{
					bpm++;
				}
			}
		}

		DrawCheckBox(200, 200, 32, "�R�c", checkBoxFontHandle);

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