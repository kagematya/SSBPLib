#pragma once
#include "math/Vector3.h"
#include "math/Matrix.h"

namespace ss {

/** SRT行列を持ってる人。dirtyflagで必要な時だけ計算します */
class SS5MatrixHolder {
public:
	SS5MatrixHolder();

	// SRT行列を取得
	const Matrix& getMatrix() const;
	
	// 位置
	void setPosition(float x, float y, float z = 0.0f);
	const Vector3& getPosition() const;

	// 回転(deg)
	void setRotation(float x, float y, float z);
	const Vector3& getRotation() const;

	// スケール
	void setScale(float x, float y, float z = 1.0f);
	const Vector3& getScale() const;

	// 左右反転
	void setFlipLR(bool flipLR);
	bool getFlipLR() const;

	// 上下反転
	void setFlipTB(bool flipTB);
	bool getFlipTB() const;

private:
	mutable Matrix  m_matrix;		// SRT行列
	mutable bool    m_dirtyFlag;	// 変更があったか？

	Vector3 m_position;		// 位置
	Vector3 m_rotation;		// 回転(deg)
	Vector3 m_scale;		// スケール
	bool	m_flipLR;		// 左右反転
	bool	m_flipTB;		// 上下反転

	// 行列を作る
	Matrix makeMatrix() const;
};

} // namespace ss
