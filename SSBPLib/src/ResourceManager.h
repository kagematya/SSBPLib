#pragma once

#include <string>
#include <map>

namespace ss{
class ResourceSet;
class ProjectData;

/**
 * ResourceManager
 */
 //class ResourceManager : public Ref
class ResourceManager
{
public:
	static const std::string s_null;

	/**
	 * �f�t�H���g�C���X�^���X���擾���܂�.
	 *
	 * @return �f�t�H���g��ResourceManager�C���X�^���X
	 */
	static ResourceManager* getInstance();

	/**
	 * ssbp�t�@�C����ǂݍ��݊Ǘ��ΏۂƂ��܂�.
	 * dataKey��ssbp�̃t�@�C�����i�g���q�Ȃ��j�ɂȂ�܂�.
	 *
	 * @param  ssbpFilepath  ssbp�t�@�C���̃p�X
	 * @param  imageBaseDir  �摜�t�@�C���̓ǂݍ��݌����[�g�p�X. �ȗ�����ssbp�̂���ꏊ�����[�g�Ƃ��܂�.
	 * @return dataKey
	 */
	std::string addData(const std::string& ssbpFilepath, const std::string& imageBaseDir = s_null);

	/**
	 * ssbp�t�@�C����ǂݍ��݊Ǘ��ΏۂƂ��܂�.
	 *
	 * @param  dataKey       dataKey�̎w��
	 * @param  ssbpFilepath  ssbp�t�@�C���̃p�X
	 * @param  imageBaseDir  �摜�t�@�C���̓ǂݍ��݌����[�g�p�X. �ȗ�����ssbp�̂���ꏊ�����[�g�Ƃ��܂�.
	 * @return dataKey
	 */
	std::string addDataWithKey(const std::string& dataKey, const std::string& ssbpFilepath, const std::string& imageBaseDir = s_null);

	/**
	 * �w�肳�ꂽssbp�f�[�^���Ǘ��ΏۂƂ��܂�.
	 *
	 * @param  dataKey       dataKey�̎w��
	 * @param  data          ssbp�f�[�^
	 * @param  imageBaseDir  �摜�t�@�C���̓ǂݍ��݌����[�g�p�X. �ȗ�����ssbp�̂���ꏊ�����[�g�Ƃ��܂�.
	 * @return dataKey
	 */
	std::string addData(const std::string& dataKey, const ProjectData* data, const std::string& imageBaseDir = s_null);

	/**
	 * �w��f�[�^��������܂�.
	 * �p�X�A�g���q��������ssbp�����w�肵�Ă��������B
	 *
	 * @param  dataKey
	 */
	void removeData(const std::string& dataKey);

	/**
	 * �S�Ẵf�[�^��������܂�.
	 */
	void removeAllData();

	/**
	* ���O�ɑΉ�����f�[�^�擾���܂�.
	*/
	ResourceSet* getData(const std::string& dataKey);

	/**
	* �w�肵���Z���̃e�N�X�`����ύX���܂�.
	* @param  ssbpName       ssbp���i�g���q�������t�@�C�����j
	* @param  ssceName       ssce���i�g���q�������t�@�C�����j
	* @param  texture        �ύX��̃e�N�X�`���n���h��
	* @return �ύX���s������
	*/
	bool changeTexture(char* ssbpName, char* ssceName, long texture);

	/**
	* �w�肵���f�[�^�̃e�N�X�`����j�����܂��B
	* @param  dataName       ssbp���i�g���q�������t�@�C�����j
	* @return �������s
	*/
	bool releseTexture(char* ssbpName);

	/**
	* �ǂݍ���ł���ssbp����A�j���[�V�����̑��t���[�������擾���܂��B
	* @param  ssbpName       ssbp���i�g���q�������t�@�C�����j
	* @param  animeName      ssae/���[�V������
	* @return �A�j���[�V�����̑��t���[���i���݂��Ȃ��ꍇ�̓A�T�[�g�j
	*/
	int getMaxFrame(std::string ssbpName, std::string animeName);

	/**
	 * �V����ResourceManager�C���X�^���X���\�z���܂�.
	 *
	 * @return ResourceManager�C���X�^���X
	 */
	static ResourceManager* create();

public:
	ResourceManager(void);
	virtual ~ResourceManager();

protected:
	std::map<std::string, ResourceSet*>	_dataDic;
};

} //namespace ss
