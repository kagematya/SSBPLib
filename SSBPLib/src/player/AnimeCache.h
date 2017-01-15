#pragma once

namespace ss{

/**
 * AnimeRef
 */
struct AnimeRef
{
	std::string				packName;
	std::string				animeName;
	const AnimationData*	animationData;
	const AnimePackData*	animePackData;
};


/**
 * AnimeCache
 */
class AnimeCache
{
public:
	AnimeCache()
	{
	}
	~AnimeCache()
	{
		releseReference();
	}
	static AnimeCache* create(const ProjectData* data)
	{
		AnimeCache* obj = new AnimeCache();
		if(obj)
		{
			obj->init(data);
		}
		return obj;
	}

	/**
	* packName��animeName���w�肵��AnimeRef�𓾂�
	*/
	AnimeRef* getReference(const std::string& packName, const std::string& animeName)
	{
		std::string key = toPackAnimeKey(packName, animeName);
		AnimeRef* ref = _dic.at(key);
		return ref;
	}

	/**
	* animeName�̂ݎw�肵��AnimeRef�𓾂�
	*/
	AnimeRef* getReference(const std::string& animeName)
	{
		AnimeRef* ref = _dic.at(animeName);
		return ref;
	}

	void dump()
	{
		std::map<std::string, AnimeRef*>::iterator it = _dic.begin();
		while(it != _dic.end())
		{
			SS_LOG("%s", (*it).second);
			++it;
		}
	}

protected:
	void init(const ProjectData* data)
	{
		SS_ASSERT_LOG(data != NULL, "Invalid data");

		ToPointer ptr(data);
		const AnimePackData* animePacks = ptr.toAnimePackDatas(data);

		for(int packIndex = 0; packIndex < data->numAnimePacks; packIndex++)
		{
			const AnimePackData* pack = &animePacks[packIndex];
			const AnimationData* animations = ptr.toAnimationDatas(pack);
			const char* packName = ptr.toString(pack->name);

			for(int animeIndex = 0; animeIndex < pack->numAnimations; animeIndex++)
			{
				const AnimationData* anime = &animations[animeIndex];
				const char* animeName = ptr.toString(anime->name);

				AnimeRef* ref = new AnimeRef();
				ref->packName = packName;
				ref->animeName = animeName;
				ref->animationData = anime;
				ref->animePackData = pack;

				// packName + animeName�ł̓o�^
				std::string key = toPackAnimeKey(packName, animeName);
				SS_LOG("anime key: %s", key.c_str());
				_dic.insert(std::map<std::string, AnimeRef*>::value_type(key, ref));

				// animeName�݂̂ł̓o�^
				//				_dic.insert(std::map<std::string, AnimeRef*>::value_type(animeName, ref));

			}
		}
	}

	static std::string toPackAnimeKey(const std::string& packName, const std::string& animeName)
	{
		//return Format("%s/%s", packName.c_str(), animeName.c_str());
		return packName + "/" + animeName;
	}

	//�L���b�V���̍폜
	void releseReference(void)
	{
		std::map<std::string, AnimeRef*>::iterator it = _dic.begin();
		while(it != _dic.end())
		{
			AnimeRef* ref = it->second;
			if(ref)
			{
				delete ref;
				it->second = 0;
			}
			it++;
		}
		_dic.clear();
	}

protected:
	std::map<std::string, AnimeRef*>	_dic;
};

} //namespace ss
