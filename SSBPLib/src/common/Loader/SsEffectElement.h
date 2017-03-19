﻿#ifndef __SSEFFECTELEMENT__
#define __SSEFFECTELEMENT__


#include "sstypes.h"

namespace ss{
class SsEffectEmitter;



//命令種別
namespace SsEffectFunctionType
{
	enum enum_
	{
		Base,
		Basic	,
		RndSeedChange , 
		Delay,
		Gravity,
		Position,
		//TransPosition,
		Rotation,
		TransRotation,
		TransSpeed,
		TangentialAcceleration,
		InitColor,
		TransColor,
		AlphaFade,
		Size,
		TransSize,
		PointGravity,
		TurnToDirectionEnabled,
		InfiniteEmitEnabled,
	};
}



class SsEffectRenderEmitter;
class SsEffectRenderParticle;


class   SsEffectElementBase 
{
private:
	SsEffectFunctionType::enum_ myType;

public:
	SsEffectElementBase(SsEffectFunctionType::enum_ functionType)
		: myType (functionType) {}
	virtual ~SsEffectElementBase(){}

	SsEffectFunctionType::enum_ getFunctionType() const{ return myType; }

	////readerを介してデータを読み取る
	//void readData(DataArrayReader& reader) = 0;
	
	virtual void initalizeEffect(SsEffectEmitter* e) = 0;
};



//--------------------------------------------------------------------------------------
//パーティクルを構成する基本の値
class  ParticleElementBasic  : public SsEffectElementBase{
public:
	int			priority;			//表示優先度
	int			maximumParticle;	//最大パーティクル数
	int			attimeCreate;		//一度に作成するパーティクル数
	int			interval;			//生成間隔
	int			lifetime;			//エミッター生存時間
	float		speedMinValue;		//初速最小
	float		speedMaxValue;		//初速最大
	int 		lifespanMinValue;	//パーティクル生存時間最小
	int 		lifespanMaxValue;	//パーティクル生存時間最大
	float		angle;				//射出方向
	float		angleVariance;		//射出方向範囲

public:
	ParticleElementBasic();

	void initalizeEffect(SsEffectEmitter* e) override;
};


//--------------------------------------------------------------------------------------
//遅れ
class  ParticleElementRndSeedChange : public SsEffectElementBase{
public:
	int		Seed;					//上書きするシード値

public:
	ParticleElementRndSeedChange();

	void initalizeEffect(SsEffectEmitter* e) override;
};

//--------------------------------------------------------------------------------------
//遅れ
class  ParticleElementDelay : public SsEffectElementBase{
public:
	int		DelayTime;				//遅延時間

public:
	ParticleElementDelay();

	void initalizeEffect(SsEffectEmitter* e) override;
};


//--------------------------------------------------------------------------------------
//重力への影響
class  ParticleElementGravity : public SsEffectElementBase{
public:
	Vector2   Gravity;				//重力

public:
	ParticleElementGravity();

	void initalizeEffect(SsEffectEmitter* e) override;
};

//--------------------------------------------------------------------------------------
//発生位置への影響
class  ParticleElementPosition : public SsEffectElementBase{
public:
	float   OffsetXMinValue;		//X座標に加算最小
	float   OffsetXMaxValue;		//X座標に加算最大
	float   OffsetYMinValue;		//Y座標に加算最小
	float   OffsetYMaxValue;		//Y座標に加算最大
	
public:
	ParticleElementPosition();

	void initalizeEffect(SsEffectEmitter* e) override;
};


//--------------------------------------------------------------------------------------
//角度変化
class  ParticleElementRotation : public SsEffectElementBase{
public:
	float RotationMinValue;			//角度初期値最小
	float RotationMaxValue;			//角度初期値最大
	float RotationAddMinValue;		//角度初期加算値最小
	float RotationAddMaxValue;		//角度初期加算値最大

public:
	ParticleElementRotation();

	void initalizeEffect(SsEffectEmitter* e) override;
};

//--------------------------------------------------------------------------------------
//角度変化
class  ParticleElementRotationTrans : public SsEffectElementBase{
public:
	float   RotationFactor;			//角度目標加算値
	float	EndLifeTimePer;			//到達時間

public:
	ParticleElementRotationTrans();

	void initalizeEffect(SsEffectEmitter* e) override;
};

//--------------------------------------------------------------------------------------
class  ParticleElementTransSpeed : public SsEffectElementBase{
public:
	float SpeedMinValue;			//速度目標値最小
	float SpeedMaxValue;			//速度目標値最大

public:
	ParticleElementTransSpeed();

	void initalizeEffect(SsEffectEmitter* e) override;
};


//--------------------------------------------------------------------------------------
//接戦加速度を与える
class  ParticleElementTangentialAcceleration : public SsEffectElementBase{
public:
	float AccelerationMinValue;		//設定加速度最小
	float AccelerationMaxValue;		//設定加速度最大

public:
	ParticleElementTangentialAcceleration();

	void initalizeEffect(SsEffectEmitter* e) override;
};


//--------------------------------------------------------------------------------------
//頂点カラーを制御する
class  ParticleElementInitColor : public SsEffectElementBase{
public:
	SsU8Color ColorMinValue;		//設定カラー最小
	SsU8Color ColorMaxValue;		//設定カラー最大

public:
	ParticleElementInitColor();

	void initalizeEffect(SsEffectEmitter* e) override;
};

//--------------------------------------------------------------------------------------
//頂点カラーを制御する
class  ParticleElementTransColor : public SsEffectElementBase{
public:
	SsU8Color ColorMinValue;		//設定カラー最小
	SsU8Color ColorMaxValue;		//設定カラー最大

public:
	ParticleElementTransColor();

	void initalizeEffect(SsEffectEmitter* e) override;
};


//--------------------------------------------------------------------------------------
//著点カラーを制御する
class  ParticleElementAlphaFade : public SsEffectElementBase{
public:
	float disprangeMinValue;		//表示区間開始
	float disprangeMaxValue;		//表示区間終了

public:
	ParticleElementAlphaFade();

	void initalizeEffect(SsEffectEmitter* e) override;
};

//--------------------------------------------------------------------------------------
//サイズ初期
class  ParticleElementSize : public SsEffectElementBase{
public:
	float SizeXMinValue;			//幅倍率最小
	float SizeXMaxValue;			//幅倍率最大
	float SizeYMinValue;			//高さ倍率最小
	float SizeYMaxValue;			//高さ倍率最大
	float ScaleFactorMinValue;		//倍率最小
	float ScaleFactorMaxValue;		//倍率最大

public:
	ParticleElementSize();

	void initalizeEffect(SsEffectEmitter* e) override;
};


//--------------------------------------------------------------------------------------
//サイズ変更
class  ParticleElementTransSize : public SsEffectElementBase{
public:
	float SizeXMinValue;			//幅倍率最小
	float SizeXMaxValue;			//幅倍率最大
	float SizeYMinValue;			//高さ倍率最小
	float SizeYMaxValue;			//高さ倍率最大
	float ScaleFactorMinValue;		//倍率最小
	float ScaleFactorMaxValue;		//倍率最大

public:
	ParticleElementTransSize();

	void initalizeEffect(SsEffectEmitter* e) override;
};

//--------------------------------------------------------------------------------------
//重力点
class  ParticlePointGravity : public SsEffectElementBase{
public:
	Vector2   Position;				//重力点
	float		Power;				//パワー

public:
	ParticlePointGravity();

	void initalizeEffect(SsEffectEmitter* e) override;
};


//--------------------------------------------------------------------------------------
class  ParticleTurnToDirectionEnabled : public SsEffectElementBase{
public:
	float Rotation;					//方向オフセット

public:
	ParticleTurnToDirectionEnabled();

	void initalizeEffect(SsEffectEmitter* e) override;
};


//--------------------------------------------------------------------------------------
class  ParticleInfiniteEmitEnabled : public SsEffectElementBase{
//public:
//	//int flag;						//ダミーデータ

public:
	ParticleInfiniteEmitEnabled();

	void initalizeEffect(SsEffectEmitter* e) override;
};

};



#endif
