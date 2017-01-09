#pragma once

#include "SS5PlayerTypes.h"
#include <memory>

namespace ss{

/**
 * State
 * �p�[�c�̏����i�[���܂��BState�̓��e�����Ƃɕ`�揈�����쐬���Ă��������B
 */
struct State
{
	int flags;						/// ���̃t���[���ōX�V���s����X�e�[�^�X�̃t���O
	int cellIndex;					/// �p�[�c�Ɋ��蓖�Ă�ꂽ�Z���̔ԍ�
	float x;						/// SS5�A�g���r���[�g�FX���W
	float y;						/// SS5�A�g���r���[�g�FY���W
	float z;						/// SS5�A�g���r���[�g�FZ���W
	float pivotX;					/// ���_X�I�t�Z�b�g�{�Z���ɐݒ肳�ꂽ���_�I�t�Z�b�gX
	float pivotY;					/// ���_Y�I�t�Z�b�g�{�Z���ɐݒ肳�ꂽ���_�I�t�Z�b�gY
	float rotationX;				/// X��]
	float rotationY;				/// Y��]
	float rotationZ;				/// Z��]
	float scaleX;					/// X�X�P�[��
	float scaleY;					/// Y�X�P�[��
	int opacity;					/// �s�����x�i0�`255�j
	float size_X;					/// SS5�A�g���r���[�g�FX�T�C�Y
	float size_Y;					/// SS5�A�g���r���[�g�FX�T�C�Y
	float uv_move_X;				/// SS5�A�g���r���[�g�FUV X�ړ�
	float uv_move_Y;				/// SS5�A�g���r���[�g�FUV Y�ړ�
	float uv_rotation;				/// SS5�A�g���r���[�g�FUV ��]
	float uv_scale_X;				/// SS5�A�g���r���[�g�FUV X�X�P�[��
	float uv_scale_Y;				/// SS5�A�g���r���[�g�FUV Y�X�P�[��
	float boundingRadius;			/// SS5�A�g���r���[�g�F�����蔼�a
	int colorBlendFunc;				/// SS5�A�g���r���[�g�F�J���[�u�����h�̃u�����h���@
	int colorBlendType;				/// SS5�A�g���r���[�g�F�J���[�u�����h�̒P�F�����_�J���[���B
	bool flipX;						/// �����]�i�e�q�֌W�v�Z�ρj
	bool flipY;						/// �c���]�i�e�q�֌W�v�Z�ρj
	bool isVisibled;				/// ��\���i�e�q�֌W�v�Z�ρj
	SSV3F_C4B_T2F_Quad quad;		/// ���_�f�[�^�A���W�A�J���[�l�AUV���܂܂��i���_�ό`�A�T�C�YXY�AUV�ړ�XY�AUV�X�P�[���AUV��]�A���]�����f�ρj
	TextuerData texture;			/// �Z���ɑΉ������e�N�X�`���ԍ��i�Q�[�����ŊǗ����Ă���ԍ���ݒ肷��j
	SSRect rect;					/// �Z���ɑΉ������e�N�X�`�����̕\���̈�i�J�n���W�A�������j
	int blendfunc;					/// �p�[�c�ɐݒ肳�ꂽ�u�����h���@
	float mat[16];					/// �p�[�c�̈ʒu���Z�o���邽�߂̃}�g���N�X�i�e�q�֌W�v�Z�ρj
									//�Đ��p�p�����[�^
	float Calc_rotationX;			/// X��]�i�e�q�֌W�v�Z�ρj
	float Calc_rotationY;			/// Y��]�i�e�q�֌W�v�Z�ρj
	float Calc_rotationZ;			/// Z��]�i�e�q�֌W�v�Z�ρj
	float Calc_scaleX;				/// X�X�P�[���i�e�q�֌W�v�Z�ρj
	float Calc_scaleY;				/// Y�X�P�[���i�e�q�֌W�v�Z�ρj
	int Calc_opacity;				/// �s�����x�i0�`255�j�i�e�q�֌W�v�Z�ρj
									//�C���X�^���X�A�g���r���[�g
	int			instanceValue_curKeyframe;
	int			instanceValue_startFrame;
	int			instanceValue_endFrame;
	int			instanceValue_loopNum;
	float		instanceValue_speed;
	int			instanceValue_loopflag;
	//�G�t�F�N�g�A�g���r���[�g
	int			effectValue_curKeyframe;
	int			effectValue_startTime;
	float		effectValue_speed;
	int			effectValue_loopflag;

	void init()
	{
		flags = 0;
		cellIndex = 0;
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		pivotX = 0.0f;
		pivotY = 0.0f;
		rotationX = 0.0f;
		rotationY = 0.0f;
		rotationZ = 0.0f;
		scaleX = 1.0f;
		scaleY = 1.0f;
		opacity = 255;
		size_X = 1.0f;
		size_Y = 1.0f;
		uv_move_X = 0.0f;
		uv_move_Y = 0.0f;
		uv_rotation = 0.0f;
		uv_scale_X = 1.0f;
		uv_scale_Y = 1.0f;
		boundingRadius = 0.0f;
		colorBlendFunc = 0;
		colorBlendType = 0;
		flipX = false;
		flipY = false;
		isVisibled = false;
		memset(&quad, 0, sizeof(quad));
		texture.handle = 0;
		texture.size_w = 0;
		texture.size_h = 0;
		rect.size.height = 0;
		rect.size.width = 0;
		rect.origin.x = 0;
		rect.origin.y = 0;
		blendfunc = 0;
		memset(&mat, 0, sizeof(mat));
		instanceValue_curKeyframe = 0;
		instanceValue_startFrame = 0;
		instanceValue_endFrame = 0;
		instanceValue_loopNum = 0;
		instanceValue_speed = 0;
		instanceValue_loopflag = 0;
		effectValue_curKeyframe = 0;
		effectValue_startTime = 0;
		effectValue_speed = 0;
		effectValue_loopflag = 0;

		Calc_rotationX = 0.0f;
		Calc_rotationY = 0.0f;
		Calc_rotationZ = 0.0f;
		Calc_scaleX = 1.0f;
		Calc_scaleY = 1.0f;
		Calc_opacity = 255;

	}

	State() { init(); }
};

} //namespace ss
