#include "DxLib.h"
#include "math.h"
#include "time.h"

// ��ʂ̉𑜓x
#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480

#define STANDARD_BPM	60	// ���BPM

#define KILO			1000

int MouseX, MouseY;		// �}�E�X��XY���W

// ����
int startTime;			// �Q�[���J�n����
int nowTime;			// ���݂̎���
int oldTime;			// �ЂƂO�̎���

// BPM
int bpm = STANDARD_BPM;

// �F
unsigned int colorBlack = GetColor(0, 0, 0);
unsigned int colorWhite = GetColor(255, 255, 255);

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
		printfDx("%d\n", beatCount);
		printfDx("%d�r�[�g\n", beat);
		printfDx("%d���q\n", night);
		printfDx("%d����\n", measure);

		// ------------------------------------
		// �`�揈��
		// ------------------------------------
		ClearDrawScreen(); // ��ʂ��Ă�����

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