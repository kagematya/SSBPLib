#include "ResourceManager.h"
#include "SS5PlayerData.h"
#include "SS5PlayerPlatform.h"
#include "player/Util.h"
#include "player/ToPointer.h"
#include "player/CellCache.h"
#include "player/AnimeCache.h"
#include "player/EffectCache.h"
#include "player/ResourceSet.h"


namespace ss{

/**
 * definition
 */
static const ss_u32 DATA_ID = 0x42505353;
static const ss_u32 DATA_VERSION = 4;



/**
 * utilites
 */
static void splitPath(std::string& directoty, std::string& filename, const std::string& path)
{
	std::string f = path;
	std::string d = "";

	size_t pos = path.find_last_of("/");
	if(pos == std::string::npos) pos = path.find_last_of("\\");	// for win

	if(pos != std::string::npos)
	{
		d = path.substr(0, pos + 1);
		f = path.substr(pos + 1);
	}

	directoty = d;
	filename = f;
}


/**
 * ResourceManager
 */

static ResourceManager* defaultInstance = NULL;
const std::string ResourceManager::s_null;

ResourceManager* ResourceManager::getInstance()
{
	if(!defaultInstance)
	{
		defaultInstance = ResourceManager::create();
	}
	return defaultInstance;
}

ResourceManager::ResourceManager(void)
{
}

ResourceManager::~ResourceManager()
{
	removeAllData();
}

ResourceManager* ResourceManager::create()
{
	ResourceManager* obj = new ResourceManager();
	return obj;
}

ResourceSet* ResourceManager::getData(const std::string& dataKey)
{
	ResourceSet* rs = _dataDic.at(dataKey);
	return rs;
}

std::string ResourceManager::addData(const std::string& dataKey, const ProjectData* data, const std::string& imageBaseDir)
{
	SS_ASSERT_LOG(data != NULL, "Invalid data");
	SS_ASSERT_LOG(data->dataId == DATA_ID, "Not data id matched");
	SS_ASSERT_LOG(data->version == DATA_VERSION, "Version number of data does not match");

	// imageBaseDir�̎w�肪�Ȃ��Ƃ��R���o�[�g���Ɏw�肳�ꂽ�p�X���g�p����
	std::string baseDir = imageBaseDir;
	if(imageBaseDir == s_null && data->imageBaseDir)
	{
		ToPointer ptr(data);
		const char* dir = ptr.toString(data->imageBaseDir);
		baseDir = dir;
	}

	//�A�j���̓G�t�F�N�g���Q�Ƃ��A�G�t�F�N�g�̓Z�����Q�Ƃ���̂ł��̏��ԂŐ�������K�v������
	CellCache* cellCache = CellCache::create(data, baseDir);

	EffectCache* effectCache = EffectCache::create(data, baseDir, cellCache);	//

	AnimeCache* animeCache = AnimeCache::create(data);

	ResourceSet* rs = new ResourceSet();
	rs->data = data;
	rs->isDataAutoRelease = false;
	rs->cellCache = cellCache;
	rs->animeCache = animeCache;
	rs->effectCache = effectCache;
	_dataDic.insert(std::map<std::string, ResourceSet*>::value_type(dataKey, rs));

	return dataKey;
}

std::string ResourceManager::addDataWithKey(const std::string& dataKey, const std::string& ssbpFilepath, const std::string& imageBaseDir)
{

	std::string fullpath = ssbpFilepath;

	unsigned long nSize = 0;
	void* loadData = SSFileOpen(fullpath.c_str(), "rb", &nSize);
	if(loadData == NULL)
	{
		std::string msg = "Can't load project data > " + fullpath;
		SS_ASSERT_LOG(loadData != NULL, msg.c_str());
	}

	const ProjectData* data = static_cast<const ProjectData*>(loadData);
	SS_ASSERT_LOG(data->dataId == DATA_ID, "Not data id matched");
	SS_ASSERT_LOG(data->version == DATA_VERSION, "Version number of data does not match");

	std::string baseDir = imageBaseDir;
	if(imageBaseDir == s_null)
	{
		// imageBaseDir�̎w�肪�Ȃ��Ƃ�
		if(data->imageBaseDir)
		{
			// �R���o�[�g���Ɏw�肳�ꂽ�p�X���g�p����
			ToPointer ptr(data);
			const char* dir = ptr.toString(data->imageBaseDir);
			baseDir = dir;
		}
		else
		{
			// �v���W�F�N�g�t�@�C���Ɠ����f�B���N�g�����w�肷��
			std::string directory;
			std::string filename;
			splitPath(directory, filename, ssbpFilepath);
			baseDir = directory;
		}
		//SS_LOG("imageBaseDir: %s", baseDir.c_str());
	}

	addData(dataKey, data, baseDir);

	// ���\�[�X���j�������Ƃ��ꏏ�Ƀ��[�h�����f�[�^���j������
	ResourceSet* rs = getData(dataKey);
	SS_ASSERT_LOG(rs != NULL, "");
	rs->isDataAutoRelease = true;

	return dataKey;
}

std::string ResourceManager::addData(const std::string& ssbpFilepath, const std::string& imageBaseDir)
{
	// �t�@�C���������o��
	std::string directory;
	std::string filename;
	splitPath(directory, filename, ssbpFilepath);

	// �g���q�����
	std::string dataKey = filename;
	size_t pos = filename.find_last_of(".");
	if(pos != std::string::npos)
	{
		dataKey = filename.substr(0, pos);
	}

	//�o�^����Ă��閼�O�����肷��
	std::map<std::string, ResourceSet*>::iterator it = _dataDic.find(dataKey);
	if(it != _dataDic.end())
	{
		//�o�^����Ă���ꍇ�͏������s��Ȃ�
		std::string str = "";
		return str;
	}

	return addDataWithKey(dataKey, ssbpFilepath, imageBaseDir);
}

void ResourceManager::removeData(const std::string& dataKey)
{
	ResourceSet* rs = getData(dataKey);

	//�o�C�i���f�[�^�̍폜
	delete rs;
	_dataDic.erase(dataKey);
}

void ResourceManager::removeAllData()
{
	//�S���\�[�X�̉��
	while(!_dataDic.empty())
	{
		std::map<std::string, ResourceSet*>::iterator it = _dataDic.begin();
		std::string ssbpName = it->first;
		removeData(ssbpName);
	}
	_dataDic.clear();
}

//�f�[�^���A�Z�������w�肵�āA�Z���Ŏg�p���Ă���e�N�X�`����ύX����
bool ResourceManager::changeTexture(char* ssbpName, char* ssceName, long texture)
{
	bool rc = false;

	ResourceSet* rs = getData(ssbpName);
	rc = rs->cellCache->setCellRefTexture(rs->data, ssceName, texture);

	return(rc);
}

//�w�肵���f�[�^�̃e�N�X�`����j�����܂�
bool ResourceManager::releseTexture(char* ssbpName)
{

	ResourceSet* rs = getData(ssbpName);
	bool rc = rs->cellCache->releseTexture(rs->data);

	return(rc);
}

//�A�j���[�V�����̑��t���[�������擾����
int ResourceManager::getMaxFrame(std::string ssbpName, std::string animeName)
{
	int rc = -1;

	ResourceSet* rs = getData(ssbpName);
	AnimeRef* animeRef = rs->animeCache->getReference(animeName);
	if(animeRef == NULL)
	{
		//std::string msg = Format("Not found animation > anime=%s", animeName.c_str());
		std::string msg = std::string("Not found animation > anime=") + animeName;
		SS_ASSERT_LOG(animeRef != NULL, msg.c_str());
	}
	rc = animeRef->animationData->numFrames;

	return(rc);
}

} //namespace ss
