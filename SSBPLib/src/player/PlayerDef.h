#pragma once

namespace ss{

//�v���W�F�N�g�t���O
enum {
	HEAD_FLAG_rootPartFunctionAsVer4 = 1 << 0,		//�s�����x�E���]�E��\���A�g���r���[�g�̌p�����@��Ver.4�Ɠ��l�ɂ���
	HEAD_FLAG_dontUseMatrixForTransform = 1 << 1,	//�e�q�̍��W�ϊ��Ƀ}�g���b�N�X���g�p���Ȃ��iVer4�݊��j
};

/**
 * �Đ�����t���[���Ɋ܂܂��p�[�c�f�[�^�̃t���O
 */
enum {
	PART_FLAG_INVISIBLE = 1 << 0,		/// ��\��
	PART_FLAG_FLIP_H = 1 << 1,		/// �����]
	PART_FLAG_FLIP_V = 1 << 2,		/// �c���]

	// optional parameter flags
	PART_FLAG_CELL_INDEX = 1 << 3,		/// �Z���ԍ�
	PART_FLAG_POSITION_X = 1 << 4,		/// X���W
	PART_FLAG_POSITION_Y = 1 << 5,		/// Y���W
	PART_FLAG_POSITION_Z = 1 << 6,		/// Z���W
	PART_FLAG_PIVOT_X = 1 << 7,		/// ���_�I�t�Z�b�gX
	PART_FLAG_PIVOT_Y = 1 << 8,		/// ���_�I�t�Z�b�gY
	PART_FLAG_ROTATIONX = 1 << 9,		/// X��]
	PART_FLAG_ROTATIONY = 1 << 10,		/// Y��]
	PART_FLAG_ROTATIONZ = 1 << 11,		/// Z��]
	PART_FLAG_SCALE_X = 1 << 12,		/// �X�P�[��X
	PART_FLAG_SCALE_Y = 1 << 13,		/// �X�P�[��Y
	PART_FLAG_OPACITY = 1 << 14,		/// �s�����x
	PART_FLAG_COLOR_BLEND = 1 << 15,		/// �J���[�u�����h
	PART_FLAG_VERTEX_TRANSFORM = 1 << 16,		/// ���_�ό`

	PART_FLAG_SIZE_X = 1 << 17,		/// �T�C�YX
	PART_FLAG_SIZE_Y = 1 << 18,		/// �T�C�YY

	PART_FLAG_U_MOVE = 1 << 19,		/// UV�ړ�X
	PART_FLAG_V_MOVE = 1 << 20,		/// UV�ړ�Y
	PART_FLAG_UV_ROTATION = 1 << 21,		/// UV��]
	PART_FLAG_U_SCALE = 1 << 22,		/// UV�X�P�[��X
	PART_FLAG_V_SCALE = 1 << 23,		/// UV�X�P�[��Y
	PART_FLAG_BOUNDINGRADIUS = 1 << 24,		/// �����蔼�a

	PART_FLAG_INSTANCE_KEYFRAME = 1 << 25,		/// �C���X�^���X
	PART_FLAG_EFFECT_KEYFRAME = 1 << 26,		/// �G�t�F�N�g

	NUM_PART_FLAGS
};

/**
 * ���_�ό`�t���O
 */
enum {
	VERTEX_FLAG_LT = 1 << 0,
	VERTEX_FLAG_RT = 1 << 1,
	VERTEX_FLAG_LB = 1 << 2,
	VERTEX_FLAG_RB = 1 << 3,
	VERTEX_FLAG_ONE = 1 << 4	// color blend only
};

/**
 * �C���X�^���X���[�v�ݒ�t���O
 */
enum {
	INSTANCE_LOOP_FLAG_INFINITY = 1 << 0,		//
	INSTANCE_LOOP_FLAG_REVERSE = 1 << 1,
	INSTANCE_LOOP_FLAG_PINGPONG = 1 << 2,
	INSTANCE_LOOP_FLAG_INDEPENDENT = 1 << 3,
};

//�G�t�F�N�g�A�g���r���[�g�̃��[�v�t���O
enum {
	EFFECT_LOOP_FLAG_INDEPENDENT = 1 << 0,
};

/**
 * Animation Part Type
 */
enum
{
	PARTTYPE_INVALID = -1,
	PARTTYPE_NULL,			/// null�B�̈��������SRT���̂݁B�������~�`�̓����蔻��͐ݒ�\�B
	PARTTYPE_NORMAL,		/// �ʏ�p�[�c�B�̈�����B�摜�͖����Ă������B
	PARTTYPE_TEXT,			/// �e�L�X�g(�\��@�������j
	PARTTYPE_INSTANCE,		/// �C���X�^���X�B���A�j���A�p�[�c�ւ̎Q�ƁB�V�[���ҏW���[�h�̑�ւɂȂ����
	PARTTYPE_EFFECT,		// ss5.5�Ή��G�t�F�N�g�p�[�c
	PARTTYPE_NUM
};

/*
 * �����蔻��̎��
 */
enum
{
	INVALID = -1,
	NONE,			///< �����蔻��Ƃ��Ďg��Ȃ��B
	QUAD,			///< ���݂ɕό`����l�ӌ`�B���_�ό`�ȂǓK�p��̂S�p�����񂾗̈�B�ł��d���B
	AABB,			///< ��]���Ȃ��S�̂��͂ދ�`�Ō�������
	CIRCLE,			///< �^�~�̔��a�ŋ����ɂ�蔻�肷��
	CIRCLE_SMIN,	///< �^�~�̔��a�ŋ����ɂ�蔻�肷�� (�X�P�[����x,y�̍ŏ��l���Ƃ�j
	CIRCLE_SMAX,	///< �^�~�̔��a�ŋ����ɂ�蔻�肷�� (�X�P�[����x,y�̍ő�l���Ƃ�j
	num
};

/**
 * ���u�����h���@
 */
enum BlendType
{
	BLEND_MIX,		///< 0 �u�����h�i�~�b�N�X�j
	BLEND_MUL,		///< 1 ��Z
	BLEND_ADD,		///< 2 ���Z
	BLEND_SUB		///< 3 ���Z
};

/*
Common\Loader\sstypes.h�Ɏ��ۂ̒�`������܂��B
/// �e�N�X�`�����b�v���[�h
namespace SsTexWrapMode
{
	enum _enum
	{
		invalid = -1,	/// �Ȃ�
		clamp,			/// �N�����v����
		repeat,			/// ���s�[�g
		mirror,			/// �~���[
		num
	};
};

/// �e�N�X�`���t�B���^�[���[�h ��f��ԕ��@
namespace SsTexFilterMode
{
	enum _enum
	{
		invalid = -1,
		nearlest,	///< �j�A���X�g�l�C�o�[
		linear,		///< ���j�A�A�o�C���j�A
		num
	};
};
*/

//�J���[���x���萔
#define COLORLABELSTR_NONE		""
#define COLORLABELSTR_RED		"Red"
#define COLORLABELSTR_ORANGE	"Orange"
#define COLORLABELSTR_YELLOW	"Yellow"
#define COLORLABELSTR_GREEN		"Green"
#define COLORLABELSTR_BLUE		"Blue"
#define COLORLABELSTR_VIOLET	"Violet"
#define COLORLABELSTR_GRAY		"Gray"
enum
{
	COLORLABEL_NONE,		///< 0 �Ȃ�
	COLORLABEL_RED,			///< 1 ��
	COLORLABEL_ORANGE,		///< 2 �I�����W
	COLORLABEL_YELLOW,		///< 3 ���F
	COLORLABEL_GREEN,		///< 4 ��
	COLORLABEL_BLUE,		///< 5 ��
	COLORLABEL_VIOLET,		///< 6 ��
	COLORLABEL_GRAY,		///< 7 �D�F
};

} //namespace ss
