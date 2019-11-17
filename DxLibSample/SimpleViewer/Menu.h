#pragma once

#include <memory>
#include <iostream>
#include <vector>
#include <list>
#include <string>

namespace ss{
	class SS5Player;
	class SS5MatrixHolder;
}
class EventListener;


class MenuInterface{
public:
	virtual ~MenuInterface(){}
	virtual void action(int up, int down, int left, int right, int enter, int cancel) = 0;
	virtual void draw(std::ostream &oss) = 0;
};


template<class T>
class MenuBase : public MenuInterface{
public:
	MenuBase(T* t) :m_context(t){}
	virtual ~MenuBase(){}

	T* get(){ return m_context; }
	const T* get() const{ return m_context; }
private:
	T* m_context;
};

using SS5Base = MenuBase<ss::SS5Player>;
using TransformBase = MenuBase<ss::SS5MatrixHolder>;


//メニューのセレクタ
class MenuRoot : public MenuInterface {
public:
	MenuRoot(ss::SS5Player* p, ss::SS5MatrixHolder* transform);
	~MenuRoot();

	void action(int up, int down, int left, int right, int enter, int cancel) override;
	void draw(std::ostream &oss) override;

private:
	std::vector<MenuInterface*> m_childMenu;
	int m_activeIndex;
};


//--
struct AnimeChanger : public SS5Base {
	AnimeChanger(ss::SS5Player* p);
	void action(int up, int down, int left, int right, int enter, int cancel) override;
	void draw(std::ostream &oss) override;
	
	std::vector<std::string> m_animationList;
	std::string m_currentAnimeName;
	int m_select;
};

struct FrameChanger : public SS5Base {
	FrameChanger(ss::SS5Player* p) : SS5Base(p), m_frame(0){}
	void action(int up, int down, int left, int right, int enter, int cancel) override;
	void draw(std::ostream &oss) override;

	int m_frame;
};

struct PositionChanger : public TransformBase {
	enum class XYZ{ X, Y/*, Z*/ };
	PositionChanger(ss::SS5MatrixHolder* transform, XYZ xyz, int pos) : TransformBase(transform), m_xyz(xyz), m_position(pos), INIT_POS(pos){}
	void action(int up, int down, int left, int right, int enter, int cancel) override;
	void draw(std::ostream &oss) override;

	XYZ m_xyz;
	int m_position;
	const int INIT_POS;
};

struct RotationChanger : public TransformBase {
	enum class XYZ{ X, Y, Z };
	RotationChanger(ss::SS5MatrixHolder* transform, XYZ xyz) : TransformBase(transform), m_xyz(xyz), m_rotation(0){}
	void action(int up, int down, int left, int right, int enter, int cancel) override;
	void draw(std::ostream &oss) override;

	XYZ m_xyz;
	int m_rotation;
};

struct ScaleChanger : public TransformBase {
	enum class XYZ{ X, Y/*, Z*/ };
	ScaleChanger(ss::SS5MatrixHolder* transform, XYZ xyz) : TransformBase(transform), m_xyz(xyz), m_scale(0.5){}
	void action(int up, int down, int left, int right, int enter, int cancel) override;
	void draw(std::ostream &oss) override;

	XYZ m_xyz;
	double m_scale;
};

struct AlphaChanger : public SS5Base {
	AlphaChanger(ss::SS5Player* p) : SS5Base(p), m_alpha(1.0){}
	void action(int up, int down, int left, int right, int enter, int cancel) override;
	void draw(std::ostream &oss) override;

	double m_alpha;
};

