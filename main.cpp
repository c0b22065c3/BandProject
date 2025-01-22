#include "DxLib.h"
#include "math.h"
#include "time.h"

// ��ʂ̉𑜓x
#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480

int MouseX, MouseY;		// �}�E�X��XY���W

// ����
int startTime;			// �Q�[���J�n����
int nowTime;			// ���݂̎���
int oldTime;			// �ЂƂO�̎���

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

		// ���݂̃L�[�{�[�h�����X�V
		GetHitKeyStateAll(keyState);

		// ���݂̃}�E�X�����X�V
		isMouseLeft		= ClickMouse(0);
		isMouseMiddle	= ClickMouse(1);
		isMouseRight	= ClickMouse(2);

		// �}�E�X�̈ʒu���擾
		GetMousePoint(&MouseX, &MouseY);

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