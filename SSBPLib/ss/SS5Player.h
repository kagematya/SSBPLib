#pragma once
#include "SS5PlayerTypes.h"
#include <map>
#include <vector>
#include <deque>
#include <string>
#include <list>
#include "player/CustomSprite.h"
#include "player/State.h"
#include "player/PlayerSetting.h"

namespace ss{
struct AnimeRef;
class ResourceSet;
struct ResultState;
class SS5EventListener;


/**
 * SS5Player
 */
class SS5Player{
public:
	/** SS5Playerインスタンスを構築します。利用するときはResourceManger::create, destroyを使ってください */
	SS5Player(SS5EventListener* eventListener, const ResourceSet* resource, const std::string& animeName);
	~SS5Player();	//memo:なるべくResourceManger.create, destroyを使ってほしい

	/**
	 * アニメーションの再生を開始します.
	 * アニメーション名から再生するデータを選択します.
	 * "ssae名/モーション名"で指定してください.
	 * sample.ssaeのanime_1を指定する場合、sample/anime_1となります.
	 * @param  animeName	再生するアニメーション名
	 * @param  startFrame	再生を開始するフレームの指定. 省略時は0
	 */
	void play(const std::string& animeName, float startFrame = 0);


	/* 毎フレーム呼ぶもの */
	void update(float dt);		//更新
	void draw();				//描画

	
	/** 再生しているアニメーション名を返します. */
	std::string getPlayAnimeName() const;

	/* 再生フレーム */
	int getMaxFrame() const;			//アニメーションの総フレームを取得
	float getCurrentFrame() const;		//再生フレームを取得
	int getCurrentFrameInt() const;		//整数化した再生フレームを取得
	void setCurrentFrame(float frame);	//再生フレームの設定。update呼ぶまで反映されません

	/* パーツへのアクセサ */
	int getPartsNum() const;										//再生しているアニメーションに含まれるパーツ数を取得
	std::string getPartName(int partIndex) const;					//indexからパーツ名を取得
	int indexOfPart(const std::string& partName) const;				//パーツ名からindexを取得
	void getPartState(ResultState* result, int partIndex) const;	//パーツ情報を取得
	int getLabelToFrame(const std::string& labelName) const;		//ラベル名からフレーム位置を取得

	
	/** 指定したパーツの表示、非表示を設定します */
	void setPartVisible(int partIndex, bool visible);

	/**
	 * パーツ名からパーツに割り当たるセルを変更します.
	 * この関数で設定したパーツは参照セルアトリビュートの影響をうけません。
	 * アニメに設定されたセルに戻す場合は、セル名に""を指定してください。
	 * @param partIndex	パーツ名
	 * @param cellname	表示させたいセル名("セルマップ名/セル名"の形式)
	 */
	void setPartCell(int partIndex, const std::string& cellname);


	/** プレイヤー本体に最初に掛ける行列を設定します */
	void setRootMatrix(const Matrix& matrix);
	const Matrix& getRootMatrix() const;

	/** プレイヤー本体の透明度を設定します[0:1]*/
	void setAlpha(float a);
	float getAlpha() const;


	/** 再生可能なアニメーションのリストを返す */
	std::vector<std::string> getAnimationList() const;

private:
	void allocParts(int numParts);
	void releaseParts();
	void setPartsParentage();

	void play(const AnimeRef* animeRef, float startFrame);
	void setFrame(int frameNo);
	void checkUserData(int frameNo);

	int getAnimeFPS() const;
	
private:
	SS5EventListener*  m_eventListener;
	const ResourceSet* m_resource;			//ssbp
	const AnimeRef*	   m_animationData;		//アニメーションデータ
	std::vector<TextureID> m_textures;		//テクスチャ

	std::vector<CustomSprite> m_parts;
	std::vector<int> m_drawOrderIndex;	//描画優先順でpartIndexを保存しておく  priority: [0] > [max]

	//書き換え用。アニメーションが切り替わっても設定は維持されます
	std::deque<bool> m_partVisible;		//visibleの上書き。デフォルトはtrue。vector<bool>は特殊化されるのでdequeで代用する
	std::vector<int> m_changeCellIndex;	//cellの差し替わり番号 未使用時は-1
	
	
	float m_currentFrameTime;		//現在のフレーム。小数点を考慮するが、フレームに直すには単にintにすれば良い
	int m_seedOffset;				//エフェクトシードオフセット

	Matrix m_rootMatrix;			// 最初にかかる行列
	float m_alpha;					// alpha[0:1]


private: //non copyable
	SS5Player(const SS5Player &o) = delete;
	SS5Player& operator=(const SS5Player &o) = delete;
};


} // namespace ss
