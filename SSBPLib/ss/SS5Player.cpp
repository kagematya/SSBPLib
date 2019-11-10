#include "SS5Player.h"
#include <time.h>
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
#include "ResultState.h"
#include "SS5ResourceManager.h"
#include "SS5EventListener.h"
#include "effect/ssplayer_effect2.h"
#include "SS5Effect.h"
using namespace std;


namespace ss{


//乱数シードに利用するユニークIDを作成します。
//この値は全てのSS5プレイヤー共通で使用します
static int s_seedMakeID = 123456;
//エフェクトに与えるシードを取得する関数
static unsigned int getRandomSeed()
{
	s_seedMakeID++;	//ユニークIDを更新します。
	//時間＋ユニークIDにする事で毎回シードが変わるようにします。
	unsigned int rc = (unsigned int)time(0) + (s_seedMakeID);

	return rc;
}



SS5Player::SS5Player(SS5EventListener* eventListener, const ResourceSet* resource, const string& animeName)
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
#if 0
	, m_playerSetting()
#endif
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

SS5Player::~SS5Player()
{
	releaseParts();

	//テクスチャの解放イベントを投げる
	for(TextureID textureID : m_textures){
		m_eventListener->TextureRelease(textureID);
	}
	m_textures.clear();
}


void SS5Player::play(const string& animeName, float startFrame)
{
	const AnimeRef* animeRef = m_resource->m_animeCache->getReference(animeName);
	SS_ASSERT_LOG(animeRef, "Not found animation > anime=%s", animeName.c_str());
	
	play(animeRef, startFrame);
}

void SS5Player::play(const AnimeRef* animeRef, float startFrame)
{
	m_animationData = animeRef;
		
	allocParts(animeRef->m_numParts);	//割り当て
	setPartsParentage();				//親子関係構築
	
	setCurrentFrame(startFrame);
	setFrame(getCurrentFrameInt());

	//play実行時に最初のフレームのユーザーデータを確認する
	checkUserData(getCurrentFrameInt());
}


void SS5Player::allocParts(int numParts)
{
	releaseParts();	//すべてのパーツを消す

	//パーツ数だけ用意する
	m_parts.resize(numParts);
	m_drawOrderIndex.resize(numParts, 0);

	m_partVisible.resize(numParts, true);
	m_changeCellIndex.resize(numParts, -1);
}

void SS5Player::releaseParts()
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

void SS5Player::setPartsParentage()
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
			m_eventListener->EffectLoad(partIndex, refeffectName, getRandomSeed());
		}
	}
}


std::string SS5Player::getPlayAnimeName() const{
	return m_animationData->m_animePath;
}

int SS5Player::getMaxFrame() const{
	return m_animationData->m_animationData->numFrames;
}

float SS5Player::getCurrentFrame() const {
	return m_currentFrameTime;
}
int SS5Player::getCurrentFrameInt() const{
	return static_cast<int>(m_currentFrameTime);
}

void SS5Player::setCurrentFrame(float frame){
	m_currentFrameTime = frame;
}


void SS5Player::update(float dt)
{
	// フレームを進める.
	float nextFrameTime = m_currentFrameTime + (dt * getAnimeFPS());
	float nextFrameRemainder = nextFrameTime - static_cast<int>(nextFrameTime);
		
	int checkFrame = getCurrentFrameInt();			// ここから整数単位でフレームを進め、イベントをチェックする
	int seekCount = nextFrameTime - checkFrame;		// 進めるフレーム数
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

	setFrame(getCurrentFrameInt());
}





//再生しているアニメーションに含まれるパーツ数を取得
int SS5Player::getPartsNum() const
{
	return m_parts.size();		//return _animationData->m_numParts;
}

//indexからパーツ名を取得
string SS5Player::getPartName(int partIndex) const
{
	ToPointer ptr(m_resource->m_data);

	const PartData* partData = m_animationData->getPartData(partIndex);
	const char* name = ptr.toString(partData->name);
	return name;
}

//パーツ名からindexを取得
int SS5Player::indexOfPart(const string& partName) const
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
void SS5Player::getPartState(ResultState* result, int partIndex) const
{
	SS_ASSERT(result);
	SS_ASSERT(partIndex >= 0 && partIndex < m_parts.size());

	ToPointer ptr(m_resource->m_data);
	const PartData* partData = m_animationData->getPartData(partIndex);
	const CustomSprite* sprite = &m_parts[partIndex];

	result->m_worldMatrix = &(sprite->m_worldMatrix);
	result->m_alpha = sprite->m_alpha;
	result->m_boundsType = static_cast<BoundsType>(partData->boundsType);
	result->boundingRadius = sprite->m_state.getBoundingRadius();
	result->m_colorLabel = ptr.toString(partData->colorLabel);
}


//ラベル名からラベルの設定されているフレームを取得
//ラベルが存在しない場合は戻り値が-1となります。
//ラベル名が全角でついていると取得に失敗します。
int SS5Player::getLabelToFrame(const string& labelName) const
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
void SS5Player::setPartVisible(int partIndex, bool visible)
{
	SS_ASSERT(partIndex >= 0 && partIndex < m_parts.size());
	m_partVisible[partIndex] = visible;
}

//パーツに割り当たるセルを変更します
void SS5Player::setPartCell(int partIndex, const string& cellname)
{
	int changeCellIndex = -1;
	if (cellname != ""){
		changeCellIndex = m_resource->m_cellCache->indexOfCell(cellname);
	}
	m_changeCellIndex[partIndex] = changeCellIndex;	//セル番号を設定
}



void SS5Player::setFrame(int frameNo)
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
	for(CustomSprite& sprite : m_parts){
		sprite.updateToWorld();
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
void SS5Player::draw()
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

void SS5Player::checkUserData(int frameNo)
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


int SS5Player::getAnimeFPS() const{
	return m_animationData->m_animationData->fps;
}

#if 0
/** プレイヤーへの各種設定 ------------------------------*/
void SS5Player::setParentMatrix(const Matrix& matrix){
	m_playerSetting.m_parentMatrix = matrix;
}
const Matrix& SS5Player::getParentMatrix() const{
	return m_playerSetting.m_parentMatrix;
}
void SS5Player::setPosition(float x, float y, float z){
	m_playerSetting.m_position = Vector3(x, y, z);
}
const Vector3& SS5Player::getPosition() const{
	return m_playerSetting.m_position;
}
void SS5Player::setRotation(float x, float y, float z){
	m_playerSetting.m_rotation = Vector3(x, y, z);
}
const Vector3& SS5Player::getRotation() const{
	return m_playerSetting.m_rotation;
}
void SS5Player::setScale(float x, float y, float z){
	m_playerSetting.m_scale = Vector3(x, y, z);
}
const Vector3& SS5Player::getScale() const{
	return m_playerSetting.m_scale;
}
void SS5Player::setFlipLR(bool flip){
	m_playerSetting.m_flipLR = flip;
}
void SS5Player::setFlipTB(bool flip){
	m_playerSetting.m_flipTB = flip;
}
bool SS5Player::getFlipLR() const{
	return m_playerSetting.m_flipLR;
}
bool SS5Player::getFlipTB() const{
	return m_playerSetting.m_flipTB;
}

void SS5Player::setAlpha(float a){
	m_playerSetting.m_color.a = clamp(a, 0.0f, 1.0f);
}
float SS5Player::getAlpha() const{
	return m_playerSetting.m_color.a;
}

//アニメーションの色成分を変更します
void SS5Player::setColor(float r, float g, float b)
{
	m_playerSetting.m_color.r = clamp(r, 0.0f, 1.0f);
	m_playerSetting.m_color.g = clamp(g, 0.0f, 1.0f);
	m_playerSetting.m_color.b = clamp(b, 0.0f, 1.0f);
}
/*-------------------------------------------------------*/
#endif

vector<string> SS5Player::getAnimationList() const{
	return m_resource->m_animeCache->getAnimationList();
}


};
