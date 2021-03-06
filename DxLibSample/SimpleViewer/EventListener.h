#pragma once

#include <vector>
#include <string>
#include "DxLib.h"
#include "ss/SS5EventListener.h"


class EventListener : public ss::SS5EventListener{
public:
	EventListener(){}
	~EventListener(){
		for(int textureId : m_textures){
			DeleteGraph(textureId);
		}
		m_textures.clear();
	}

	//テクスチャを事前に読み込みするためのコールバック
	void texturePreloadCallback(int cellMapIndex, const std::string& filename, ss::SsTexWrapMode wrapmode, ss::SsTexFilterMode filtermode){
		int textureId = LoadGraph(filename.c_str());
		m_textures.push_back(textureId);		//必ずcellMapIndex順(0～)呼ばれるので、push_backすればok
	}


	//テクスチャのロード・リリースのイベント。内部ではPlayer単位で管理されます
	ss::TextureID TextureLoad(int cellMapIndex, const std::string& texturePath, ss::SsTexWrapMode wrapmode, ss::SsTexFilterMode filtermode) override{
		SS_LOG("【EVENT】(SSTextureLoad) name:%s", texturePath.c_str());
		int textureId = m_textures[cellMapIndex];	//テクスチャは事前に読み込みしておくため、ここではreturnするだけ
		return textureId;
	}
	void TextureRelease(ss::TextureID textureId) override{
		SS_LOG("【EVENT】(SSTextureRelease) id%d", static_cast<int>(textureId));
	}


	//描画
	void DrawSprite(const ss::SSV3F_C4B_T2F_Quad& quad, ss::TextureID textureId, ss::BlendType blendType, ss::BlendType colorBlendVertexType) override{
		//DXライブラリ用の頂点バッファを作成する
		VERTEX_3D vertex[4] = {
			vertex3Dfrom(quad.tl),
			vertex3Dfrom(quad.bl),
			vertex3Dfrom(quad.tr),
			vertex3Dfrom(quad.br)
		};
		//3Dプリミティブの表示
		DrawPolygon3DBase(vertex, 4, DX_PRIMTYPE_TRIANGLESTRIP, textureId, true);
	}

private:
	//DXライブラリ用頂点バッファ作成関数
	static VERTEX_3D vertex3Dfrom(const ss::SSV3F_C4B_T2F &vct){
		VERTEX_3D v = {
			{ vct.vertices.x, vct.vertices.y, vct.vertices.z },
			vct.colors.b, vct.colors.g, vct.colors.r, vct.colors.a,
			vct.texCoords.u(), vct.texCoords.v()
		};
		return v;
	}

	
	std::vector<int> m_textures;
};

