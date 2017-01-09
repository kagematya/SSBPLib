#pragma once

namespace ss{

/**
 * UserData
 */
struct UserData
{
	enum {
		FLAG_INTEGER = 1 << 0,
		FLAG_RECT = 1 << 1,
		FLAG_POINT = 1 << 2,
		FLAG_STRING = 1 << 3
	};

	const char*	partName;		/// Part name
	int			frameNo;		/// Frame no

	int			flags;			/// Flags of valid data
	int			integer;		/// Integer
	int			rect[4];		/// Rectangle Left, Top, Right, Bottom
	int			point[2];		/// Position X, Y
	const char*	str;			/// String (zero terminated)
	int			strSize;		/// String size (byte count)
};


/**
 * LabelData
 */
struct LabelData
{
	std::string	str;			/// String (zero terminated)
	int			strSize;		/// String size (byte count)
	int			frameNo;		/// Frame no
};

//�C���X�^���X�f�[�^
struct Instance
{
	int			refStartframe;		//�J�n�t���[��
	int			refEndframe;		//�I���t���[��
	float		refSpeed;			//�Đ����x
	int			refloopNum;			//���[�v��
	bool		infinity;			//�������[�v
	bool		reverse;			//�t�đI
	bool		pingpong;			//����
	bool		independent;		//�Ɨ�����
	void clear(void)
	{
		refStartframe = 0;			//�J�n�t���[��
		refEndframe = 1;			//�I���t���[��
		refSpeed = 1;				//�Đ����x
		refloopNum = 1;				//���[�v��
		infinity = false;			//�������[�v
		reverse = false;			//�t�đI
		pingpong = false;			//����
		independent = false;		//�Ɨ�����
	}
};

} //namespace ss
