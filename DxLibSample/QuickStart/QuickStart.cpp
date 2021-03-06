#include "QuickStart.h"
#include <fstream>
#include <vector>
#include "ss/SS5ResourceManager.h"
#include "ss/SS5Player.h"
#include "ss/SS5EventListener.h"
#include "ss/SS5MatrixHolder.h"
using namespace std;
using namespace ss;


QuickStart::QuickStart()
	: m_ss5ResourceManager()
	, m_ss5Player(nullptr)
	, m_eventListener()
{
	
}

QuickStart::~QuickStart(){

}


/*
	再生手順まとめ

	ssbpファイルを読み込み
	SS5ResourceManagerにssbpファイルを登録
	SS5ResourceManager::createPlayer()でプレーヤーを生成
	SS5Player::play()で再生
		あとは、update(), draw() を呼んでください
*/
void QuickStart::initialize()
{
	/*
	 * ssbpファイルを読み込んで登録する
	 */
	
	//ファイル読み込み
	ifstream ifs("character_template_comipo/character_template1.ssbp", ios::in | ios::binary);
	assert(ifs);
	ifs.seekg(0, ios::end);
	int fileSize = ifs.tellg();			//ファイルサイズ取得
	ifs.seekg(0, ios::beg);

	vector<char> file(fileSize, 0);
	ifs.read(file.data(), file.size());	//ファイル読み込み

	//登録
	m_ss5ResourceManager.regist(
		file.data(),					//ssbpデータ
		file.size(),					//ssbpデータサイズ
		"ssbpRegistName",				//登録名
		"character_template_comipo/"	//テクスチャのあるフォルダを指定
	);
	

	
	/*
	 * 登録名からプレーヤーを生成する
	 */

	//生成
	m_ss5Player = m_ss5ResourceManager.createPlayer(
		&m_eventListener,					//イベント処理の指定
		"ssbpRegistName"					//登録名
	);



	/*
	 * プレーヤーの設定
	 */
	
	//再生させるにはアニメーション名を指定する
	m_ss5Player->play("character_template_3head/stance");

	//値を変えたい場合は次のようにして設定できます
	SS5MatrixHolder transform;
	transform.setPosition(400, 200);			//表示位置
	transform.setScale(0.5f, 0.5f);				//スケール
	transform.setRotation(0.0f, 0.0f, 0.0f);	//回転

	m_ss5Player->setRootMatrix(transform.getMatrix());
	m_ss5Player->setAlpha(1.0);					//透明度
}



void QuickStart::finalize()
{
	//プレーヤーの破棄
	m_ss5ResourceManager.destroyPlayer(m_ss5Player);

	//登録解除
	m_ss5ResourceManager.unregist("ssbpRegistName");
}



void QuickStart::update()
{
	static const int FPS = 60;
	
	m_ss5Player->update(1.0f / FPS);	//毎フレームのアップデート
}

void QuickStart::draw()
{
	m_ss5Player->draw();				//描画
}

