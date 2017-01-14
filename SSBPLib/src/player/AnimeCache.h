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
	* packNameとanimeNameを指定してAnimeRefを得る
	*/
	AnimeRef* getReference(const std::string& packName, const std::string& animeName)
	{
		std::string key = toPackAnimeKey(packName, animeName);
		AnimeRef* ref = _dic.at(key);
		return ref;
	}

	/**
	* animeNameのみ指定してAnimeRefを得る
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
			SSLOG("%s", (*it).second);
			++it;
		}
	}

protected:
	void init(const ProjectData* data)
	{
		SS_ASSERT2(data != NULL, "Invalid data");

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

				// packName + animeNameでの登録
				std::string key = toPackAnimeKey(packName, animeName);
				SSLOG("anime key: %s", key.c_str());
				_dic.insert(std::map<std::string, AnimeRef*>::value_type(key, ref));

				// animeNameのみでの登録
				//				_dic.insert(std::map<std::string, AnimeRef*>::value_type(animeName, ref));

			}
		}
	}

	static std::string toPackAnimeKey(const std::string& packName, const std::string& animeName)
	{
		//return Format("%s/%s", packName.c_str(), animeName.c_str());
		return packName + "/" + animeName;
	}

	//キャッシュの削除
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
