#pragma once

namespace ss{

/**
 * ResluteState
 * �Q�[�����ɕԂ��p�[�c�X�e�[�^�X�B
 * �K�v�ɉ����ăJ�X�^�}�C�Y���Ă��������B
 */
struct ResluteState
{
	int flags;						/// ���̃t���[���ōX�V���s����X�e�[�^�X�̃t���O
	int cellIndex;					/// �p�[�c�Ɋ��蓖�Ă�ꂽ�Z���̔ԍ�
	float x;						/// SS5�A�g���r���[�g�FX���W
	float y;						/// SS5�A�g���r���[�g�FY���W
	float z;						/// SS5�A�g���r���[�g�FZ���W
	float pivotX;					/// ���_X�I�t�Z�b�g�{�Z���ɐݒ肳�ꂽ���_�I�t�Z�b�gX
	float pivotY;					/// ���_Y�I�t�Z�b�g�{�Z���ɐݒ肳�ꂽ���_�I�t�Z�b�gY
	float rotationX;				/// X��]�i�e�q�֌W�v�Z�ρj
	float rotationY;				/// Y��]�i�e�q�֌W�v�Z�ρj
	float rotationZ;				/// Z��]�i�e�q�֌W�v�Z�ρj
	float scaleX;					/// X�X�P�[���i�e�q�֌W�v�Z�ρj
	float scaleY;					/// Y�X�P�[���i�e�q�֌W�v�Z�ρj
	int opacity;					/// �s�����x�i0�`255�j�i�e�q�֌W�v�Z�ρj
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

	int	part_type;					/// �p�[�c���
	int	part_boundsType;			/// �����蔻����
	int	part_alphaBlendType;		/// BlendType
	int	part_labelcolor;			/// ���x���J���[
};

} //namespace ss
