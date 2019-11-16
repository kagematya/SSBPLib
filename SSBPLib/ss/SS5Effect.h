#pragma once
#include <vector>
#include "SS5PlayerTypes.h"
#include "effect/sstypes.h"
#include "math/Matrix.h"
#include "player/EffectPartStatus.h"

namespace ss{
class SS5EventListener;
class SsEffectModel;
class SsEffectNode;
class SsEffectEmitter;
struct particleDrawData;
struct CellRef;
class ResourceSet;


class SS5Effect{
private:
	SS5EventListener* m_eventListener;
	const ResourceSet* m_resource;		//ssbp
	const SsEffectModel* m_effectData;	//エフェクトのパラメータデータ
	std::vector<TextureID> m_textures;	//テクスチャ

	//Modelに記載されているエミッタのリスト
	std::vector<SsEffectEmitter*> m_emmiterList;
	std::vector<SsEffectEmitter*> m_updateList;

	//ランダムシード
	int	m_mySeed;

	float m_nowFrame;		//フレーム(小数点を考慮)	//memo:nowというよりもtotalな気がする
	float m_targetFrame;	//こちらのフレームの値でdrawされる

	size_t m_effectTimeLength;

	bool m_infinite;	//無限に発生出来るかどうか

	bool m_isPlay;
	bool m_isLoop;

	int  m_seedOffset;
	bool m_isWarningData;

private:
	void particleDraw(SsEffectEmitter* e, double time, SsEffectEmitter* parent, const particleDrawData* plp);
	void initEmitter(SsEffectEmitter* e, const SsEffectNode* node);


public:
	/** Effectインスタンスを構築します。利用するときはResourceManger::create, destroyを使ってください */
	SS5Effect(SS5EventListener* eventListener, const ResourceSet* resource, const std::string& effectName, int seed);
	~SS5Effect();	//memo:なるべくResourceManger.create, destroyを使ってほしい
	

	void play(){ m_isPlay = true; }
	void stop(){ m_isPlay = false; }
	void setLoop(bool flag){ m_isLoop = flag; }
	bool isPlay() const{ return m_isPlay; }
	bool isLoop() const{ return m_isLoop; }

	void setFrame(float frame){
		m_nowFrame = frame;
	}
	float getFrame() const{ return m_nowFrame; }

	void update(float dt);
	void draw();

private:
	void initialize();

public:
	size_t getEffectTimeLength() const;
	int	getFPS() const;


	void drawSprite(
		const CellRef* refCell,
		SsRenderBlendType blendType,
		const Matrix& localMatrix,
		const SSColor4B& color,
		TextureID textureId
	);

	void setSeedOffset(int offset);
	bool isInfinity() const{ return m_infinite; }
	bool isWarning() const{ return m_isWarningData; }

	//
	void setRootMatrix(const Matrix& matrix);
	const Matrix& getRootMatrix() const;

	void setAlpha(float a);							/*[0:1]*/
	float getAlpha() const;							/*[0:1]*/

private:
	Matrix m_rootMatrix;
	float m_alpha;


public:
	//todo:これは後で削除する
	void effectUpdate(
		const Matrix& parentWorldMatrix, float parentAlpha,
		int parentFrame, int parentSeedOffset, const EffectPartStatus& effectAttribute
	){
		bool isValid = effectAttribute.isValidFrame(parentFrame);
		//有効フレーム&&未再生 --> 再生開始のタイミング
		if(isValid && !isPlay()){
			setFrame(effectAttribute.m_startTime);	//再生開始時間
			play();
		}

		setAlpha(parentAlpha);
		setRootMatrix(parentWorldMatrix);
	
		if(isValid){
			if(effectAttribute.m_independent){
				//独立動作
				setLoop(true);
				update(1.0f/60.0f * effectAttribute.m_speed);	//・・・というより独立動作なのだからこの外側でupdateするべき。dtは渡さないので今は適当に値入れとく
			}
			else{
				float nextFrame = effectAttribute.getFrame(parentFrame);

				setSeedOffset(parentSeedOffset);
				setFrame(nextFrame);
				update(0);
			}
		}
	}
};

} //namespace ss
