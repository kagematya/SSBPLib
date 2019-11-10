#include "SS5MatrixHolder.h"

namespace ss {


SS5MatrixHolder::SS5MatrixHolder()
	: m_matrix()
	, m_dirtyFlag(false)
	, m_position(0.0f, 0.0f, 0.0f)
	, m_rotation(0.0f, 0.0f, 0.0f)
	, m_scale(1.0f, 1.0f, 1.0f)
	, m_flipLR(false)
	, m_flipTB(false)
{
}

// SRT行列を取得
const Matrix& SS5MatrixHolder::getMatrix() const
{
	if (m_dirtyFlag) {
		m_matrix = makeMatrix();
		m_dirtyFlag = false;
	}
	return m_matrix;
}
	
// 位置
void SS5MatrixHolder::setPosition(float x, float y, float z) {
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;

	m_dirtyFlag = true;
}

const Vector3& SS5MatrixHolder::getPosition() const {
	return m_position;
}

// 回転(deg)
void SS5MatrixHolder::setRotation(float x, float y, float z) {
	m_rotation.x = x;
	m_rotation.y = y;
	m_rotation.z = z;

	m_dirtyFlag = true;
}

const Vector3& SS5MatrixHolder::getRotation() const {
	return m_rotation;
}

// スケール
void SS5MatrixHolder::setScale(float x, float y, float z) {
	m_scale.x = x;
	m_scale.y = y;
	m_scale.z = z;

	m_dirtyFlag = true;
}

const Vector3& SS5MatrixHolder::getScale() const {
	return m_scale;
}

// 左右反転
void SS5MatrixHolder::setFlipLR(bool flipLR) {
	m_flipLR = flipLR;
	m_dirtyFlag = true;
}

bool SS5MatrixHolder::getFlipLR() const {
	return m_flipLR;
}

// 上下反転
void SS5MatrixHolder::setFlipTB(bool flipTB) {
	m_flipTB = flipTB;
	m_dirtyFlag = true;
}

bool SS5MatrixHolder::getFlipTB() const {
	return m_flipTB;
}


// 行列を作る
Matrix SS5MatrixHolder::makeMatrix() const {
	Matrix mat;
	Vector3 scale = m_scale;
	if (m_flipLR) {
		scale.x = -scale.x;
	}
	if (m_flipTB) {
		scale.y = -scale.y;
	}
	Vector3 rotRadian = SSDegToRad(m_rotation);
	mat.setupSRzyxT(scale, rotRadian, m_position);
	return mat;
}


} // namespace ss
