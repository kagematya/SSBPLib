﻿/**
*  SS5PlayerPlatform.h
*/

#ifndef SS5PlayerPlatform_h
#define SS5PlayerPlatform_h
#include "SS5Player.h"
#include "common/loader/sstypes.h"
#include <stdio.h>
#include <string>

namespace ss
{
	struct State;
	struct UserData;
	class Player;

#if 0
	extern TextureID SSTextureLoad(const char* pszFileName, SsTexWrapMode::_enum  wrapmode, SsTexFilterMode::_enum filtermode);
	extern bool SSTextureRelese(TextureID handle);
	extern bool isAbsolutePath(const std::string& strPath);
#endif
	extern void SSDrawSprite(State state);
#if 0
	extern bool SSGetTextureSize(TextureID handle, int &w, int &h);
#endif
	
#if 0
	//userDataは一時オブジェクトなのでコピーして使ってください
	extern void SSonUserData(Player *player, const UserData &userData, int frame);
	extern int  SSLimitFrame(int frame, int maxFrame);
#endif


};	// namespace ss

#endif
