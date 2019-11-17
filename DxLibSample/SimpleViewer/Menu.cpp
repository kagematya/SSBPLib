#include "Menu.h"
#include "EventListener.h"
#include <sstream>
#include <algorithm>
#include "ss/SS5Player.h"
#include "ss/SS5MatrixHolder.h"
#include "ss/math/Vector3.h"
using namespace std;
using namespace ss;


MenuRoot::MenuRoot(SS5Player* p, SS5MatrixHolder* transform)
	: m_activeIndex(0)
{
	m_childMenu.push_back(new AnimeChanger(p));
	m_childMenu.push_back(new FrameChanger(p));
	m_childMenu.push_back(new PositionChanger(transform, PositionChanger::XYZ::X, 400));
	m_childMenu.push_back(new PositionChanger(transform, PositionChanger::XYZ::Y, 150));
	m_childMenu.push_back(new RotationChanger(transform, RotationChanger::XYZ::X));
	m_childMenu.push_back(new RotationChanger(transform, RotationChanger::XYZ::Y));
	m_childMenu.push_back(new RotationChanger(transform, RotationChanger::XYZ::Z));
	m_childMenu.push_back(new ScaleChanger(transform, ScaleChanger::XYZ::X));
	m_childMenu.push_back(new ScaleChanger(transform, ScaleChanger::XYZ::Y));
	m_childMenu.push_back(new AlphaChanger(p));
}

MenuRoot::~MenuRoot()
{
	for(MenuInterface* child : m_childMenu){
		delete child;
	}
	m_childMenu.clear();
}

void MenuRoot::action(int up, int down, int left, int right, int enter, int cancel)
{
	//メニュー項目の選択
	if(down){
		m_activeIndex++;
	}
	else if(up){
		m_activeIndex--;
	}
	m_activeIndex = wrap<int>(m_activeIndex, 0, m_childMenu.size());
	
	MenuInterface *current = m_childMenu[m_activeIndex];
	current->action(up, down, left, right, enter, cancel);
}

void MenuRoot::draw(std::ostream &oss)
{
	for(int i = 0; i < m_childMenu.size(); ++i){
		if(i == m_activeIndex){
			oss << "> ";
		}
		else{
			oss << "  ";
		}
		
		m_childMenu[i]->draw(oss);
	}
}


//--
AnimeChanger::AnimeChanger(SS5Player* p) : SS5Base(p), m_currentAnimeName(""), m_select(0) {
	m_animationList = p->getAnimationList();
}

void AnimeChanger::action(int up, int down, int left, int right, int enter, int cancel){
	//再生するモーションを設定
	int animNum = m_animationList.size();

	if(left){  m_select--; }
	if(right){ m_select++; }
	m_select = wrap<int>(m_select, 0, animNum);

	if(enter){
		m_currentAnimeName = m_animationList[m_select];
		get()->play(m_currentAnimeName);
	}
}
void AnimeChanger::draw(std::ostream &oss){
	auto ssp = get();
	oss << "animation: " << m_currentAnimeName << " --> " << m_animationList[m_select] << endl;
}
//--
void FrameChanger::action(int up, int down, int left, int right, int enter, int cancel){
	SS5Player* ssp = get();
	int animax = ssp->getMaxFrame();

	if(left){  m_frame--; }
	if(right){ m_frame++; }
	m_frame = wrap<int>(m_frame, 0, animax);
	
	if(enter){
		ssp->setCurrentFrame(m_frame);
	}
}
void FrameChanger::draw(std::ostream &oss){
	SS5Player* ssp = get();
	oss << "frame: current=" << ssp->getCurrentFrame() << " --> " << m_frame << endl;
}
//--
void PositionChanger::action(int up, int down, int left, int right, int enter, int cancel){
	SS5MatrixHolder* transform = get();
	
	if(left){  m_position--; }
	if(right){ m_position++; }
	if(cancel){m_position = INIT_POS;}

	if(enter){
		Vector3 pos = transform->getPosition();

		switch(m_xyz){
		case XYZ::X:	pos.x = m_position;	break;
		case XYZ::Y:	pos.y = m_position;	break;
		}
		transform->setPosition(pos.x, pos.y);
	}
}
void PositionChanger::draw(std::ostream &oss){
	SS5MatrixHolder* transform = get();
	switch(m_xyz){
	case XYZ::X:
		oss << "position X: current=" << transform->getPosition().x << " --> " << m_position << endl;
		break;
	case XYZ::Y:
		oss << "position Y: current=" << transform->getPosition().y << " --> " << m_position << endl;
		break;
	}
}
//--
void RotationChanger::action(int up, int down, int left, int right, int enter, int cancel){
	SS5MatrixHolder* transform = get();
	
	if(left){  m_rotation--; }
	if(right){ m_rotation++; }
	if(cancel){m_rotation = 0;}

	if(enter){
		Vector3 rot = transform->getRotation();

		switch(m_xyz){
		case XYZ::X:	rot.x = m_rotation; break;
		case XYZ::Y:	rot.y = m_rotation; break;
		case XYZ::Z:	rot.z = m_rotation; break;
		}
		transform->setRotation(rot.x, rot.y, rot.z);
	}
}
void RotationChanger::draw(std::ostream &oss){
	SS5MatrixHolder* transform = get();
	Vector3 rot = transform->getRotation();
	
	std::string text;
	float current;
	switch(m_xyz){
	case XYZ::X:	text = "rotation X"; current=rot.x; break;
	case XYZ::Y:	text = "rotation Y"; current=rot.y; break;
	case XYZ::Z:	text = "rotation Z"; current=rot.z; break;
	}
	oss << text << ": current=" << current << " --> " << m_rotation << endl;
}
//--
void ScaleChanger::action(int up, int down, int left, int right, int enter, int cancel){
	SS5MatrixHolder* transform = get();
	
	if(left){  m_scale -= 0.1; }
	if(right){ m_scale += 0.1; }
	if(cancel){m_scale = 0.5;}

	if(enter){
		Vector3 scale = transform->getScale();

		switch(m_xyz){
		case XYZ::X:	scale.x = m_scale; break;
		case XYZ::Y:	scale.y = m_scale; break;
		}
		transform->setScale(scale.x, scale.y);
	}
}
void ScaleChanger::draw(std::ostream &oss){
	SS5MatrixHolder* transform = get();
	Vector3 scale = transform->getScale();
	
	std::string text;
	float current;
	switch(m_xyz){
	case XYZ::X:	text = "scale X"; current=scale.x; break;
	case XYZ::Y:	text = "scale Y"; current=scale.y; break;
	}
	oss << text << ": current=" << current << " --> " << m_scale << endl;
}
//--
void AlphaChanger::action(int up, int down, int left, int right, int enter, int cancel){
	SS5Player* ssp = get();

	if(left){  m_alpha -= 0.01; }
	if(right){ m_alpha += 0.01; }
	if(cancel){m_alpha = 1.0; }
	m_alpha = clamp<double>(m_alpha, 0.0, 1.0);
	
	if(enter){
		ssp->setAlpha(m_alpha);
	}
}
void AlphaChanger::draw(std::ostream &oss){
	SS5Player* ssp = get();
	oss << "alpha: current=" << ssp->getAlpha() << " --> " << m_alpha << endl;
}

