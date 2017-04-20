﻿#include "SS5Player.h"
#include "SS5PlayerData.h"
#include "SS5PlayerTypes.h"
#include "player/ToPointer.h"
#include "player/DataArrayReader.h"
#include "player/Util.h"
#include "player/AnimeCache.h"
#include "player/CellCache.h"
#include "player/CustomSprite.h"
#include "player/EffectCache.h"
#include "player/State.h"
#include "player/ResourceSet.h"
#include "player/PlayerDef.h"
#include "player/InstancePartStatus.h"
#include "ResluteState.h"
#include "ResourceManager.h"
#include "SS5EventListener.h"
#include "effect/ssplayer_effect2.h"
#include "SS5Effect.h"
using namespace std;


namespace ss{


//乱数シードに利用するユニークIDを作成します。
//この値は全てのSS5プレイヤー共通で使用します
int seedMakeID = 123456;
//エフェクトに与えるシードを取得する関数
unsigned int getRandomSeed()
{
	seedMakeID++;	//ユニークIDを更新します。
	//時間＋ユニークIDにする事で毎回シードが変わるようにします。
	unsigned int rc = (unsigned int)time(0) + (seedMakeID);

	return(rc);
}



Player::Player(SS5EventListener* eventListener, const ResourceSet* resource, const string& animeName)
	: m_eventListener(eventListener)
	, m_resource(resource)
	, m_animationData(nullptr)
	, m_textures()
	, m_parts()
	, m_drawOrderIndex()
	, m_partVisible()
	, m_changeCellIndex()
	, m_currentFrameTime(0.0f)
	, m_seedOffset(0)
	, m_playerSetting()
{
	SS_ASSERT_LOG(m_eventListener, "eventListener is null");
	SS_ASSERT_LOG(m_resource, "resource is null");

	//ロードイベントを投げてcellMapのテクスチャを取得する
	int cellMapNum = m_resource->m_cellCache->getCellMapNum();
	m_textures.resize(cellMapNum);
	for(int i = 0; i < cellMapNum; ++i){
		const CellMapTextureInfo& texInfo = m_resource->m_cellCache->getTextureInfo(i);

		m_textures[i] = m_eventListener->TextureLoad(
			i, texInfo.m_imagePaths, texInfo.m_wrapMode, texInfo.m_filterMode
		);
	}

	//最初にアニメーションを入れておく
	play(animeName, 0);
	SS_ASSERT_LOG(m_animationData, "animationData is null");
}

Player::~Player()
{
	releaseParts();

	//テクスチャの解放イベントを投げる
	for(TextureID textureID : m_textures){
		m_eventListener->TextureRelease(textureID);
	}
	m_textures.clear();
}


void Player::play(const string& animeName, int startFrameNo)
{
	const AnimeRef* animeRef = m_resource->m_animeCache->getReference(animeName);
	SS_ASSERT_LOG(animeRef, "Not found animation > anime=%s", animeName.c_str());
	
	play(animeRef, startFrameNo);
}

void Player::play(const AnimeRef* animeRef, int startFrameNo)
{
	m_animationData = animeRef;
		
	allocParts(animeRef->m_numParts);	//割り当て
	setPartsParentage();				//親子関係構築
	
	setCurrentFrame(startFrameNo);
	setFrame(getCurrentFrame());

	//play実行時に最初のフレームのユーザーデータを確認する
	checkUserData(getCurrentFrame());
}


void Player::allocParts(int numParts)
{
	releaseParts();	//すべてのパーツを消す

	//パーツ数だけ用意する
	m_parts.resize(numParts);
	m_drawOrderIndex.resize(numParts, 0);

	m_partVisible.resize(numParts, true);
	m_changeCellIndex.resize(numParts, -1);
}

void Player::releaseParts()
{
	// パーツの子CustomSpriteを全て削除
	for(int i = 0; i < m_parts.size(); ++i){
		CustomSprite* sprite = &m_parts[i];

		//ChildPlayerがあるなら、spriteを破棄する前にリリースイベントを飛ばす
		if(sprite->isInstancePart()){
			m_eventListener->ChildPlayerRelease(i);
		}
		if(sprite->isEffectPart()){
			m_eventListener->EffectRelease(i);
		}
	}
	m_parts.clear();
}

void Player::setPartsParentage()
{
	ToPointer ptr(m_resource->m_data);

	//親子関係を設定
	for(int partIndex = 0; partIndex < m_parts.size(); partIndex++){
		const PartData* partData = m_animationData->getPartData(partIndex);
		CustomSprite* sprite = &m_parts.at(partIndex);

		if(partData->parentIndex < 0){
			sprite->m_parent = nullptr;
		}
		else{
			sprite->m_parent = &m_parts.at(partData->parentIndex);
		}

		//変化しない値はここでセットします
		sprite->m_partType = static_cast<AnimationPartType>(partData->type);
		sprite->m_blendfunc = static_cast<BlendType>(partData->alphaBlendType);
		
		//インスタンスパーツならChildPlayerの生成イベントを飛ばす
		if(sprite->isInstancePart()){
			string refanimeName = ptr.toString(partData->refname);
			m_eventListener->ChildPlayerLoad(partIndex, refanimeName);
		}

		//エフェクトパーツならパラメータを設定する
		if(sprite->isEffectPart()){
			string refeffectName = ptr.toString(partData->effectfilename);
			m_eventListener->EffectLoad(partIndex, refeffectName);
		}
	}
}


int Player::getMaxFrame() const{
	return m_animationData->m_animationData->numFrames;
}

int Player::getCurrentFrame() const{
	return static_cast<int>(m_currentFrameTime);
}

void Player::setCurrentFrame(int frame){
	m_currentFrameTime = frame;
}


void Player::update(float dt)
{
	// フレームを進める.
	float nextFrameTime = m_currentFrameTime + (dt * getAnimeFPS());
	float nextFrameRemainder = nextFrameTime - static_cast<int>(nextFrameTime);
		
	int checkFrame = getCurrentFrame();
	int seekCount = nextFrameTime - getCurrentFrame();
	// 順再生時.
	for(int i = 0; i < seekCount; ++i){
		checkFrame = m_eventListener->LimitFrame(checkFrame + 1, getMaxFrame());	//範囲制限
		SS_ASSERT_LOG(0 <= checkFrame && checkFrame < getMaxFrame(), "checkFrame is out of range. checkFrame=%d", checkFrame);
			
		// このフレームのユーザーデータをチェック
		checkUserData(checkFrame);

		if(checkFrame == 0){	//一巡した
			m_seedOffset++;		//シードオフセットを加算
		}
	}
	// 逆再生時.
	for(int i = 0; i > seekCount; --i){
		checkFrame = m_eventListener->LimitFrame(checkFrame - 1, getMaxFrame());	//範囲制限
		SS_ASSERT_LOG(0 <= checkFrame && checkFrame < getMaxFrame(), "checkFrame is out of range. checkFrame=%d", checkFrame);

		// このフレームのユーザーデータをチェック
		checkUserData(checkFrame);

		if(checkFrame == getMaxFrame()-1){	//一巡した
			m_seedOffset++;		//シードオフセットを加算
		}
	}
		
	m_currentFrameTime = checkFrame + nextFrameRemainder;

	setFrame(getCurrentFrame());
}





//再生しているアニメーションに含まれるパーツ数を取得
int Player::getPartsNum() const
{
	return m_parts.size();		//return _animationData->m_numParts;
}

//indexからパーツ名を取得
string Player::getPartName(int partIndex) const
{
	ToPointer ptr(m_resource->m_data);

	const PartData* partData = m_animationData->getPartData(partIndex);
	const char* name = ptr.toString(partData->name);
	return name;
}

//パーツ名からindexを取得
int Player::indexOfPart(const string& partName) const
{
	ToPointer ptr(m_resource->m_data);

	for (int i = 0; i < m_animationData->m_numParts; i++){
		const PartData* partData = m_animationData->getPartData(i);
		const char* name = ptr.toString(partData->name);
	
		if(partName == name){	//if(partName == getPartName(i)) と同じ
			return i;
		}
	}
	return -1;
}

//パーツ情報取得
void Player::getPartState(ResluteState& result, int partIndex) const
{
	SS_ASSERT(partIndex >= 0 && partIndex < m_parts.size());

	ToPointer ptr(m_resource->m_data);
	const PartData* partData = m_animationData->getPartData(partIndex);

	//必要に応じて取得するパラメータを追加してください。
	//当たり判定などのパーツに付属するフラグを取得する場合は　partData　のメンバを参照してください。
	//親から継承したスケールを反映させる場合はxスケールは_mat.m[0]、yスケールは_mat.m[5]をかけて使用してください。
	const CustomSprite* sprite = &m_parts[partIndex];
#if 0
	//パーツアトリビュート
//					sprite->_state;												//SpriteStudio上のアトリビュートの値は_stateから取得してください
	result.flags = sprite->m_state.m_flags;						// このフレームで更新が行われるステータスのフラグ
	result.cellIndex = sprite->m_state.m_cellIndex;				// パーツに割り当てられたセルの番号
	result.x = sprite->m_state.m_position.x;
	result.y = sprite->m_state.m_position.y;
	result.z = sprite->m_state.m_position.z;
	result.pivotX = sprite->m_state.m_pivot.x;					// 原点Xオフセット＋セルに設定された原点オフセットX
	result.pivotY = sprite->m_state.m_pivot.y;					// 原点Yオフセット＋セルに設定された原点オフセットY
	result.rotationX = sprite->m_state.m_rotation.x;			// X回転（親子関係計算済）
	result.rotationY = sprite->m_state.m_rotation.y;			// Y回転（親子関係計算済）
	result.rotationZ = sprite->m_state.m_rotation.z;			// Z回転（親子関係計算済）
	result.scaleX = sprite->m_state.m_scale.x;					// Xスケール（親子関係計算済）
	result.scaleY = sprite->m_state.m_scale.y;					// Yスケール（親子関係計算済）
	result.opacity = sprite->m_state.m_opacity;					// 不透明度（0～255）（親子関係計算済）
	result.size_X = sprite->m_state.m_size.x;					// SS5アトリビュート：Xサイズ
	result.size_Y = sprite->m_state.m_size.y;					// SS5アトリビュート：Xサイズ
	result.uv_move_X = sprite->m_state.m_uvMove.x;				// SS5アトリビュート：UV X移動
	result.uv_move_Y = sprite->m_state.m_uvMove.y;				// SS5アトリビュート：UV Y移動
	result.uv_rotation = sprite->m_state.m_uvRotation;			// SS5アトリビュート：UV 回転
	result.uv_scale_X = sprite->m_state.m_uvScale.x;			// SS5アトリビュート：UV Xスケール
	result.uv_scale_Y = sprite->m_state.m_uvScale.y;			// SS5アトリビュート：UV Yスケール
	result.boundingRadius = sprite->m_state.m_boundingRadius;	// SS5アトリビュート：当たり半径
	result.colorBlendVertexFunc = sprite->m_state.m_colorBlendVertexFunc;	// SS5アトリビュート：カラーブレンドのブレンド方法
	result.flipX = sprite->m_state.m_flipX;						// 横反転（親子関係計算済）
	result.flipY = sprite->m_state.m_flipY;						// 縦反転（親子関係計算済）
	result.isVisibled = sprite->m_state.m_isVisibled;			// 非表示（親子関係計算済）
#endif
	//パーツ設定
	result.part_type = partData->type;							//パーツ種別
	result.part_boundsType = partData->boundsType;				//当たり判定種類
	result.part_alphaBlendType = partData->alphaBlendType;		// BlendType
	//ラベルカラー
	string colorName = ptr.toString(partData->colorLabel);
	if(colorName == COLORLABELSTR_NONE){
		result.part_labelcolor = COLORLABEL_NONE;
	}
	if(colorName == COLORLABELSTR_RED){
		result.part_labelcolor = COLORLABEL_RED;
	}
	if(colorName == COLORLABELSTR_ORANGE){
		result.part_labelcolor = COLORLABEL_ORANGE;
	}
	if(colorName == COLORLABELSTR_YELLOW){
		result.part_labelcolor = COLORLABEL_YELLOW;
	}
	if(colorName == COLORLABELSTR_GREEN){
		result.part_labelcolor = COLORLABEL_GREEN;
	}
	if(colorName == COLORLABELSTR_BLUE){
		result.part_labelcolor = COLORLABEL_BLUE;
	}
	if(colorName == COLORLABELSTR_VIOLET){
		result.part_labelcolor = COLORLABEL_VIOLET;
	}
	if(colorName == COLORLABELSTR_GRAY){
		result.part_labelcolor = COLORLABEL_GRAY;
	}
}


//ラベル名からラベルの設定されているフレームを取得
//ラベルが存在しない場合は戻り値が-1となります。
//ラベル名が全角でついていると取得に失敗します。
int Player::getLabelToFrame(const string& labelName) const
{
	ToPointer ptr(m_resource->m_data);
	const AnimationData* animeData = m_animationData->m_animationData;

	if (!animeData->labelData) return -1;
	const ss_offset* labelDataIndex = static_cast<const ss_offset*>(ptr(animeData->labelData));


	for (int i = 0; i < animeData->labelNum; i++ ){

		if (!labelDataIndex[i]) return -1;
		const ss_u16* labelDataArray = static_cast<const ss_u16*>(ptr(labelDataIndex[i]));

		DataArrayReader reader(labelDataArray);

		ss_offset offset = reader.readOffset();
		const char* str = ptr.toString(offset);
		int labelFrame = reader.readU16();

		if(labelName == str){
			return labelFrame;		//同じ名前のラベルが見つかった
		}
	}

	return -1;
}

//特定パーツの表示、非表示を設定します
//パーツ番号はスプライトスタジオのフレームコントロールに配置されたパーツが
//プライオリティでソートされた後、上に配置された順にソートされて決定されます。
void Player::setPartVisible(int partIndex, bool visible)
{
	SS_ASSERT(partIndex >= 0 && partIndex < m_parts.size());
	m_partVisible[partIndex] = visible;
}

//パーツに割り当たるセルを変更します
void Player::setPartCell(int partIndex, const string& cellname)
{
	int changeCellIndex = -1;
	if (cellname != ""){
		changeCellIndex = m_resource->m_cellCache->indexOfCell(cellname);
	}
	m_changeCellIndex[partIndex] = changeCellIndex;	//セル番号を設定
}



void Player::setFrame(int frameNo)
{
	ToPointer ptr(m_resource->m_data);
	const AnimationData* animeData = m_animationData->m_animationData;
	const ss_offset* frameDataIndex = static_cast<const ss_offset*>(ptr(animeData->frameData));
	
	const ss_u16* frameDataArray = static_cast<const ss_u16*>(ptr(frameDataIndex[frameNo]));
	DataArrayReader reader(frameDataArray);
	
	const AnimationInitialData* initialDataList = ptr.toAnimationInitialDatas(animeData);

	//各パーツの読み取り
	for (int index = 0; index < m_parts.size(); index++){

		int partIndex = reader.readS16();
		m_drawOrderIndex[index] = partIndex;

		const AnimationInitialData* init = &initialDataList[partIndex];
		CustomSprite* sprite = &m_parts.at(partIndex);

		sprite->m_state.readData(reader, init);

		//セルを取得する
		int cellIndex = sprite->m_state.getCellIndex();
		if (m_changeCellIndex[partIndex] != -1){	//ユーザーがセルを上書きした場合はそちらを使う
			cellIndex = m_changeCellIndex[partIndex];
		}
		const CellRef* cellRef = cellIndex >= 0 ? m_resource->m_cellCache->getReference(cellIndex) : nullptr;

		//各パーツのテクスチャ情報を設定
		sprite->m_textureID = TEXTURE_ID_INVALID;
		if (cellRef){
			sprite->m_textureID = m_textures[cellRef->m_cellMapIndex];
		}

		//quad更新
		sprite->constructQuad(cellRef);
	}

	// 行列更新してワールド変換する
	Matrix rootMatrix = m_playerSetting.getWorldMatrix();
	for(CustomSprite& sprite : m_parts){
		sprite.updateToWorld(rootMatrix, m_playerSetting.m_color);
	}

	// 特殊パーツのアップデート
	for (int partIndex = 0; partIndex < m_parts.size(); partIndex++){
		CustomSprite* sprite = &m_parts[partIndex];

		//インスタンスアニメーションがある場合は親パーツ情報を通知する
		if(sprite->isInstancePart()){
			m_eventListener->ChildPlayerUpdate(
				partIndex, sprite->m_worldMatrix, sprite->m_alpha,
				frameNo, sprite->m_state.getInstanceValue()		//InstancePartStatus::getFrame(frameNo), m_independent,,,
			);
		}

		//エフェクトのアップデート
		if (sprite->isEffectPart()){
			m_eventListener->EffectUpdate(
				partIndex, sprite->m_worldMatrix, sprite->m_alpha,
				frameNo, m_seedOffset, sprite->m_state.getEffectValue()
			);
		}
	}
}

//プレイヤーの描画
void Player::draw()
{
	for (int index = 0; index < m_parts.size(); index++){
		int partIndex = m_drawOrderIndex[index];
		//スプライトの表示
		const CustomSprite* sprite = &m_parts[partIndex];

		//非表示設定なら無視する
		if(sprite->isVisibled() == false || m_partVisible[index] == false){	//ユーザーが任意に非表示としたパーツも考慮する
			continue;
		}

		//パーツタイプに応じた描画イベントを投げる
		if (sprite->isInstancePart()){
			m_eventListener->ChildPlayerDraw(partIndex);	//インスタンスパーツ
		}
		else if (sprite->isEffectPart()){
			m_eventListener->EffectDraw(partIndex);		//エフェクトパーツ
		}
		else{
			const State& state = sprite->m_state;
			m_eventListener->DrawSprite(sprite->m_quad, sprite->m_textureID, sprite->m_blendfunc, state.getColorBlendVertexFunc());
		}
	}
}

void Player::checkUserData(int frameNo)
{
	ToPointer ptr(m_resource->m_data);

	const AnimationData* animeData = m_animationData->m_animationData;

	if (!animeData->userData) return;
	const ss_offset* userDataIndex = static_cast<const ss_offset*>(ptr(animeData->userData));

	if (!userDataIndex[frameNo]) return;
	const ss_u16* userDataArray = static_cast<const ss_u16*>(ptr(userDataIndex[frameNo]));
	
	DataArrayReader reader(userDataArray);
	int numUserData = reader.readU16();

	for (int i = 0; i < numUserData; i++){
		UserData userData;
		userData.readData(reader, ptr);
		m_eventListener->OnUserData(userData, frameNo);
	}

}


int Player::getAnimeFPS() const{
	return m_animationData->m_animationData->fps;
}

/** プレイヤーへの各種設定 ------------------------------*/
void Player::setRootMatrix(const Matrix& matrix){
	m_playerSetting.m_rootMatrix = matrix;
}
void Player::setPosition(float x, float y){
	m_playerSetting.m_position = Vector3(x, y, 0.0f);
}
void Player::getPosition(float* x, float* y) const{
	*x = m_playerSetting.m_position.x;
	*y = m_playerSetting.m_position.y;
}
void Player::setRotation(float x, float y, float z){
	m_playerSetting.m_rotation = Vector3(x, y, z);
}
void Player::getRotation(float* x, float* y, float* z) const{
	*x = m_playerSetting.m_rotation.x;
	*y = m_playerSetting.m_rotation.y;
	*z = m_playerSetting.m_rotation.z;
}
void Player::setScale(float x, float y){
	m_playerSetting.m_scale = Vector3(x, y, 1.0f);
}
void Player::getScale(float* x, float* y) const{
	*x = m_playerSetting.m_scale.x;
	*y = m_playerSetting.m_scale.y;
}
void Player::setAlpha(float a){
	m_playerSetting.m_color.a = clamp(a, 0.0f, 1.0f);
}
float Player::getAlpha() const{
	return m_playerSetting.m_color.a;
}

//アニメーションの色成分を変更します
void Player::setColor(float r, float g, float b)
{
	m_playerSetting.m_color.r = clamp(r, 0.0f, 1.0f);
	m_playerSetting.m_color.g = clamp(g, 0.0f, 1.0f);
	m_playerSetting.m_color.b = clamp(b, 0.0f, 1.0f);
}
/*-------------------------------------------------------*/

list<string> SS5Player::getAnimationList() const{
	return m_resource->m_animeCache->getAnimationList();
}


};
