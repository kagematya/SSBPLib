﻿#include "DxLib.h"
#include <fstream>
#include "ss/SS5Player.h"
#include "ss/SS5Effect.h"
#include "ss/SS5ResourceManager.h"
#include "ss/ResluteState.h"
#include "ss/SS5EventListener.h"

//メモリリークチェック用---------------------------------------------------------
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
//-----------------------------------------------------------------------------

static int previousTime;
static int waitTime;
int mGameExec;

#define WAIT_FRAME (16)

void init(void);
void update(float dt);
void draw(void);
void relese(void);


//DXライブラリ用頂点バッファ作成関数
static VERTEX_3D vertex3Dfrom(const ss::SSV3F_C4B_T2F &vct)
{
	VERTEX_3D v = {
		{ vct.vertices.x, vct.vertices.y, vct.vertices.z },
		vct.colors.b, vct.colors.g, vct.colors.r, vct.colors.a,
		vct.texCoords.u(), vct.texCoords.v()
	};
	return v;
}


/// SS5プレイヤー
ss::SS5Player *ssplayer;
ss::SS5ResourceManager *resman;
std::map<std::string, int> g_textures;	//<ファイル名, テクスチャID>

//テクスチャの事前読み
void texturePreloadCallback(int cellMapIndex, const std::string& filename, ss::SsTexWrapMode wrapmode, ss::SsTexFilterMode filtermode){
	int textureId = LoadGraph(filename.c_str());
	g_textures.insert(std::make_pair(filename, textureId));
}
void textureRelease(){
	for(auto& name_id : g_textures){
		DeleteGraph(name_id.second);
	}
	g_textures.clear();
}


/*イベントリスナーの実装*/
class SampleSS5EventListener: public ss::SS5EventListener{
public:
	int m_drawSpriteCount;	//【デバッグ用】スプライト表示数


	//テクスチャのロード・リリースのイベント。内部ではPlayer単位で管理されます
	ss::TextureID TextureLoad(int cellMapIndex, const std::string& texturePath, ss::SsTexWrapMode wrapmode, ss::SsTexFilterMode filtermode) override{
		int textureId = g_textures.at(texturePath);	//テクスチャは事前に読み込みしておくため、ここではreturnするだけ
		SS_LOG("【EVENT】(SSTextureLoad) name:%s, id:%d", texturePath.c_str(), textureId);
		return textureId;
		//return LoadGraph(pszFileName);
	}
	void TextureRelease(ss::TextureID handle) override{
		auto it = std::find_if(
			g_textures.begin(), g_textures.end(),
			[&](const std::pair<std::string, int>& v){ return v.second == static_cast<int>(handle); }
		);
		SS_LOG("【EVENT】(SSTextureRelease) name:%s, id%d", it->first.c_str(), static_cast<int>(handle));
		//DeleteGraph(handle);						//テクスチャの破棄は事前読みと対になる位置でやるため、ここではなにもしない
	}


	//描画 //ひとまずSS5PlayerPlatform.cppの中身をそのまま持ってきた
	void DrawSprite(const ss::SSV3F_C4B_T2F_Quad& quad, ss::TextureID textureId, ss::BlendType blendType, ss::BlendType colorBlendVertexType) override{
		m_drawSpriteCount++;	//デバッグ用

		//描画ファンクション
		switch(blendType){
		case ss::BLEND_MIX:		///< 0 ブレンド（ミックス）
			if(quad.tl.colors.a == 255 && quad.tr.colors.a == 255 && quad.bl.colors.a == 255 && quad.br.colors.a == 255){
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
			else{
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
			}
			break;
		case ss::BLEND_MUL:		///< 1 乗算
			SetDrawBlendMode(DX_BLENDMODE_MULA, 255);
			break;
		case ss::BLEND_ADD:		///< 2 加算
			SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
			break;
		case ss::BLEND_SUB:		///< 3 減算
			SetDrawBlendMode(DX_BLENDMODE_SUB, 255);
			break;
		}

	
		//頂点カラーブレンド
		//厳密に再現するには専用のシェーダーを使い、テクスチャにカラー値を合成する必要がある
		//作成する場合はssShader_frag.h、CustomSpriteのコメントとなってるシェーダー処理を参考にしてください。
		switch(colorBlendVertexType){
		case ss::BLEND_MIX:		///< 0 これは、頂点カラーの色にするのが正しいらしい
			break;
		case ss::BLEND_MUL:		///< 1 乗算
			// ブレンド方法は乗算以外未対応
			// とりあえず左上の色を反映させる
			SetDrawBright(quad.tl.colors.r, quad.tl.colors.g, quad.tl.colors.b);
			break;
		case ss::BLEND_ADD:		///< 2 加算
			break;
		case ss::BLEND_SUB:		///< 3 減算
			break;
		}

		//DXライブラリ用の頂点バッファを作成する
		VERTEX_3D vertex[4] = {
			vertex3Dfrom(quad.tl),
			vertex3Dfrom(quad.bl),
			vertex3Dfrom(quad.tr),
			vertex3Dfrom(quad.br)
		};
		//3Dプリミティブの表示
		DrawPolygon3DBase(vertex, 4, DX_PRIMTYPE_TRIANGLESTRIP, textureId, true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);	//ブレンドステートを通常へ戻す
		SetDrawBright(255, 255, 255);
	}


	int LimitFrame(int frame, int maxFrame) override{
		return ss::SS5EventListener::LimitFrame(frame, maxFrame);	//フレーム制限はデフォルト実装に任せる
	}
	void OnUserData(const ss::UserData& userData, int frameNo) override{}	//ユーザーデータは特に何も処理しない


	//インスタンスアニメーションのイベント
	void ChildPlayerLoad(int parentPartIndex, const std::string& animName) override{}
	void ChildPlayerRelease(int parentPartIndex) override{}
	void ChildPlayerUpdate(
		int parentPartIndex, const ss::Matrix& parentWorldMatrix, float parentAlpha,
		int parentFrame, const ss::InstancePartStatus& instanceAttribute
	) override{}
	void ChildPlayerDraw(int parentPartIndex) override{}


	//エフェクトのイベント ----------------------------------------------------
	void EffectLoad(int parentPartIndex, const std::string& effectName, int seed) override{
		assert(m_effects.find(parentPartIndex) == m_effects.end());
		//エフェクトとイベントリスナーを生成する
		ss::SS5EventListener* listener = new SampleSS5EventListener();
		ss::SS5Effect* effect = resman->createEffect(listener, "effectsample", effectName, seed);
		m_effects.insert(std::make_pair(parentPartIndex, effect));
		m_effectEventListeners.insert(std::make_pair(parentPartIndex, listener));
	}
	void EffectRelease(int parentPartIndex) override{
		auto it = m_effects.find(parentPartIndex);
		assert(it != m_effects.end());
		
		//エフェクトとイベントリスナーを破棄する
		resman->destroyEffect(it->second);
		m_effects.erase(it);

		ss::SS5EventListener* listener = m_effectEventListeners.at(parentPartIndex);
		delete listener;
		m_effectEventListeners.erase(parentPartIndex);
	}

	void EffectUpdate(
		int parentPartIndex, const ss::Matrix& parentWorldMatrix, float parentAlpha,
		int parentFrame, int parentSeedOffset, const ss::EffectPartStatus& effectAttribute
	) override{
		auto it = m_effects.find(parentPartIndex);
		assert(it != m_effects.end());

		ss::SS5Effect* effect = it->second;
		effect->effectUpdate(parentWorldMatrix, parentAlpha, parentFrame, parentSeedOffset, effectAttribute);
	}
	
	void EffectDraw(int parentPartIndex) override{
		auto it = m_effects.find(parentPartIndex);
		assert(it != m_effects.end());
		
		ss::SS5Effect* effect = it->second;
		effect->draw();
	}

private:
	std::map<int, ss::SS5Effect*> m_effects;
	std::map<int, ss::SS5EventListener*> m_effectEventListeners;
};
SampleSS5EventListener g_eventListener;


//ファイル読み込みして中身を返します
std::vector<char> readfile(const std::string& filename)
{
	std::ifstream ifs(filename, std::ios::in | std::ios::binary);
	assert(ifs);

	ifs.seekg(0, std::fstream::end);
	size_t filesize = ifs.tellg();
	ifs.seekg(0, std::fstream::beg);

	std::vector<char> buf(filesize, 0);
	ifs.read(buf.data(), filesize);
	return buf;
}


/**
* メイン関数
*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//メモリリークチェック用
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
#endif

	//DXライブラリの初期化
	ChangeWindowMode(true);	//ウインドウモード
	SetGraphMode(800, 600, GetColorBitDepth() );
	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;			// エラーが起きたら直ちに終了
	}
	SetDrawScreen(DX_SCREEN_BACK);

	//メインループ
	mGameExec = 1;
	previousTime = GetNowCount();
	
	// プレイヤー初期化
	init( );
	
	while(mGameExec == 1){
		ClearDrawScreen();
		update((float)waitTime / 1000.0f );		//ゲームの更新
		draw();									//ゲームの描画
		ScreenFlip();							//描画結果を画面に反映

		//次のフレームまでの時間待ち
		waitTime = GetNowCount() - previousTime;
		previousTime = GetNowCount();

		if (waitTime < WAIT_FRAME){
			WaitTimer((WAIT_FRAME - waitTime));
		}else{
			if(ProcessMessage() == -1) mGameExec = 0;
		}
	}

	/// プレイヤー終了処理
	relese( );


	DxLib_End();			// ＤＸライブラリ使用の終了処理

	return 0;				// ソフトの終了 
}

void init( void )
{
	/**********************************************************************************

	SSアニメ表示のサンプルコード
	Visual Studio Community、DXライブラリで動作を確認しています。
	ssbpとpngがあれば再生する事ができますが、Resourcesフォルダにsspjも含まれています。

	**********************************************************************************/
	//リソースマネージャの作成
	resman =  new ss::SS5ResourceManager();
	

	//ssbpファイルの読み込み
	std::vector<char> buf = readfile("Resources/character_template_comipo/character_template1.ssbp");

	//アニメデータをリソースに追加
	resman->regist(
		buf.data(), buf.size(),
		"character_template1",					//登録名
		"Resources/character_template_comipo/",	//画像ファイルの読み込み元ルートパス
		texturePreloadCallback
	);

	//プレイヤーにリソースを割り当てる
	//addDataで指定した登録名とアニメーション名を指定する
	ssplayer = resman->createPlayer(&g_eventListener, "character_template1", "character_template_3head/stance");       //
	

	//表示位置を設定
	ssplayer->setPosition(800/2, 150);
	//スケール設定	//反転させたいときは-の値を指定してください
	ssplayer->setScale(0.5f, 0.5f);
	//回転を設定
	ssplayer->setRotation(0.0f, 0.0f, 0.0f);
	//透明度を設定
	ssplayer->setAlpha(1.0f);


	//エフェクトのテストのためついでに追加しておく
	buf = readfile("Resources/ParticleEffectSample/ParticleEffectSample.ssbp");
	resman->regist(
		buf.data(), buf.size(),
		"effectsample",
		"Resources/ParticleEffectSample/",
		texturePreloadCallback
	);
}

//メインループ
//Zボタンでアニメをポーズ、再開を切り替えできます。
//ポーズ中は左右キーで再生するフレームを変更できます。
static bool sstest_push = false;
static int sstest_count = 0;
static bool sstest_pause = false;
void update(float dt)
{
	char str[128];

	//パーツ名から座標等のステートの取得を行う場合はgetPartStateを使用します。
	ss::ResluteState result;
	int partIndex = ssplayer->indexOfPart("body");
	if(partIndex < 0){
		partIndex = 0;
	}
	ssplayer->getPartState(result, partIndex);

	//取得座用の表示
	sprintf_s(str, "body = x:%f y:%f", result.x, result.y);
	DrawString(100, 120, str, GetColor(255, 255, 255));


	//キー入力操作
	int animax = ssplayer->getMaxFrame();
	if (CheckHitKey(KEY_INPUT_ESCAPE)){
		mGameExec = 0;
	}

	if (CheckHitKey(KEY_INPUT_Z)){
		if (sstest_push == false ){
			if (sstest_pause == false ){
				sstest_pause = true;
				sstest_count = ssplayer->getCurrentFrame();
			}
			else{
				sstest_pause = false;
			}
		}
		sstest_push = true;
	}
	else if (CheckHitKey(KEY_INPUT_UP)){
		if (sstest_push == false){
			sstest_count += 5;
			if (sstest_count >= animax){
				sstest_count = 0;
			}
		}
		sstest_push = true;
	}
	else if (CheckHitKey(KEY_INPUT_DOWN)){
		if (sstest_push == false){
			sstest_count -= 5;
			if (sstest_count < 0){
				sstest_count = animax - 1;
			}
		}
		sstest_push = true;
	}
	else if (CheckHitKey(KEY_INPUT_LEFT)){
		if (sstest_push == false){
			sstest_count--;
			if (sstest_count < 0){
				sstest_count = animax-1;
			}
		}
		sstest_push = true;
	}
	else if (CheckHitKey(KEY_INPUT_RIGHT)){
		if (sstest_push == false){
			sstest_count++;
			if (sstest_count >= animax){
				sstest_count = 0;
			}
		}
		sstest_push = true;
	}
	else if(CheckHitKey(KEY_INPUT_X)){
		if(sstest_push == false){
			resman->destroyPlayer(ssplayer);
			ssplayer = resman->createPlayer(&g_eventListener, "character_template1", "character_template_3head/stance");
			//ssplayer->play("character_template_3head/stance");

			ssplayer->setPosition(800 / 2, 150);
			ssplayer->setScale(0.5f, 0.5f);
			sstest_count = 0;
		}
		sstest_push = true;
	}
	else if(CheckHitKey(KEY_INPUT_C)){
		if(sstest_push == false){
			resman->destroyPlayer(ssplayer);
			ssplayer = resman->createPlayer(&g_eventListener, "effectsample", "e001/emission");
			//ssplayer->play("e001/emission");

			ssplayer->setPosition(800 / 2, 300);
			ssplayer->setScale(0.5f, 0.5f);
			sstest_count = 0;
		}
		sstest_push = true;
	}
	else{
		sstest_push = false;
	}

	if (sstest_pause == true){
		ssplayer->setCurrentFrame(sstest_count % animax);
		dt = 0;
	}
	else{
		sstest_count = ssplayer->getCurrentFrame();
	}

	//アニメーションのフレームを表示
	sprintf_s(str, "play:%d frame:%d drawCount:%d", (int)sstest_pause, sstest_count, g_eventListener.m_drawSpriteCount);
	DrawString(100, 100, str, GetColor(255, 255, 255));

	//プレイヤーの更新、引数は前回の更新処理から経過した時間
	ssplayer->update(dt);

}

//描画
void draw(void)
{
	g_eventListener.m_drawSpriteCount = 0;//デバッグ用変数のクリア
	
	//プレイヤーの描画
	ssplayer->draw();
}

/**
* プレイヤー終了処理
*/
void relese( void )
{
	//SS5Playerの削除
	delete (ssplayer);	
	delete (resman);
	
	textureRelease();
}









