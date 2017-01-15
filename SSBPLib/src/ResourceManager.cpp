﻿#include "ResourceManager.h"
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

#if 0
std::string ResourceManager::addData(const std::string& dataKey, const ProjectData* data, const std::string& imageBaseDir)
{
	SS_ASSERT_LOG(data != NULL, "Invalid data");
	SS_ASSERT_LOG(data->dataId == DATA_ID, "Not data id matched");
	SS_ASSERT_LOG(data->version == DATA_VERSION, "Version number of data does not match");

	// imageBaseDirの指定がないときコンバート時に指定されたパスを使用する
	std::string baseDir = imageBaseDir;
	if(imageBaseDir == s_null && data->imageBaseDir)
	{
		ToPointer ptr(data);
		const char* dir = ptr.toString(data->imageBaseDir);
		baseDir = dir;
	}

	ResourceSet* rs = new ResourceSet(data, baseDir);
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
		// imageBaseDirの指定がないとき
		if(data->imageBaseDir)
		{
			// コンバート時に指定されたパスを使用する
			ToPointer ptr(data);
			const char* dir = ptr.toString(data->imageBaseDir);
			baseDir = dir;
		}
		else
		{
			// プロジェクトファイルと同じディレクトリを指定する
			std::string directory;
			std::string filename;
			splitPath(directory, filename, ssbpFilepath);
			baseDir = directory;
		}
		//SS_LOG("imageBaseDir: %s", baseDir.c_str());
	}

	addData(dataKey, data, baseDir);

	// リソースが破棄されるとき一緒にロードしたデータも破棄する
	ResourceSet* rs = getData(dataKey);
	SS_ASSERT_LOG(rs != NULL, "");

	return dataKey;
}

std::string ResourceManager::addData(const std::string& ssbpFilepath, const std::string& imageBaseDir)
{
	// ファイル名を取り出す
	std::string directory;
	std::string filename;
	splitPath(directory, filename, ssbpFilepath);

	// 拡張子を取る
	std::string dataKey = filename;
	size_t pos = filename.find_last_of(".");
	if(pos != std::string::npos)
	{
		dataKey = filename.substr(0, pos);
	}

	//登録されている名前か判定する
	std::map<std::string, ResourceSet*>::iterator it = _dataDic.find(dataKey);
	if(it != _dataDic.end())
	{
		//登録されている場合は処理を行わない
		std::string str = "";
		return str;
	}

	return addDataWithKey(dataKey, ssbpFilepath, imageBaseDir);
}
#endif

bool ResourceManager::addData(const void *data, size_t dataSize, const std::string &dataKey, const std::string &imageBaseDir)
{
	SS_ASSERT_LOG(data, "Invalid data");
	SS_ASSERT_LOG(dataSize > 0, "dataSize is zero");
	//登録済みかどうかの判定
	if(_dataDic.find(dataKey) != _dataDic.end()){
		return false;
	}

	/***** 新規登録 *****/

	//画像ファイルのディレクトリパスを作る
	std::string baseDir = getImageBaseDir(imageBaseDir, static_cast<const ProjectData*>(data));

	//データを作って登録
	ResourceSet* rs = new ResourceSet(static_cast<const char*>(data), dataSize, baseDir);

	_dataDic.insert(std::make_pair(dataKey, rs));
	return true;
}


std::string ResourceManager::getImageBaseDir(const std::string &imageBaseDir, const ProjectData *data) const
{
	if(imageBaseDir == s_null){	// imageBaseDirの指定がないときはパスを作る

		if(data->imageBaseDir){
			// コンバート時に指定されたパスを使用する
			ToPointer ptr(data);
			return ptr.toString(data->imageBaseDir);
		}

		//// プロジェクトファイルと同じディレクトリを指定する
		//std::string directory;
		//std::string filename;
		//splitPath(directory, filename, ssbpFilepath);
		//return directory;
	}
	return imageBaseDir;
}


void ResourceManager::removeData(const std::string& dataKey)
{
	ResourceSet* rs = getData(dataKey);

	//バイナリデータの削除
	delete rs;
	_dataDic.erase(dataKey);
}

void ResourceManager::removeAllData()
{
	//全リソースの解放
	while(!_dataDic.empty())
	{
		std::map<std::string, ResourceSet*>::iterator it = _dataDic.begin();
		std::string ssbpName = it->first;
		removeData(ssbpName);
	}
	_dataDic.clear();
}

//データ名、セル名を指定して、セルで使用しているテクスチャを変更する
bool ResourceManager::changeTexture(char* ssbpName, char* ssceName, long texture)
{
	bool rc = false;

	ResourceSet* rs = getData(ssbpName);
	rc = rs->m_cellCache->setCellRefTexture(rs->m_data, ssceName, texture);

	return(rc);
}

//指定したデータのテクスチャを破棄します
bool ResourceManager::releseTexture(char* ssbpName)
{

	ResourceSet* rs = getData(ssbpName);
	bool rc = rs->m_cellCache->releseTexture(rs->m_data);

	return(rc);
}

//アニメーションの総フレーム数を取得する
int ResourceManager::getMaxFrame(std::string ssbpName, std::string animeName)
{
	int rc = -1;

	ResourceSet* rs = getData(ssbpName);
	AnimeRef* animeRef = rs->m_animeCache->getReference(animeName);
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
