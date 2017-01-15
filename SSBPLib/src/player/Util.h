#pragma once
#include <algorithm>
#include <assert.h>

namespace ss{
/**
 * �萔
 */

static const double SS_PI = 3.14159265358979323846;
static const double SS_2PI = SS_PI * 2;
double SSRadToDeg(double rad);
double SSDegToRad(double deg);

void DebugPrintToConsole(const char *filename, int line, const char *format, ...);


/** �z��̃T�C�Y��Ԃ� */
template<class TYPE, size_t N>
size_t lengthof(const TYPE(&ar)[N]){
	return N;
}

/** val��[min:max]�͈̔͂ɂ������̂�Ԃ� */
template<class T>
T clamp(T val, T minVal, T maxVal){
	assert(minVal <= maxVal);
	return std::min(std::max(val, minVal), maxVal);
}

/** [minVal:maxVal)�͈̔͂Ń��[�v������B�����l���� */
template<class T>
T wrap(T val, T minVal, T maxVal){
	assert(minVal < maxVal);
	int n = (val - minVal) % (maxVal - minVal);
	return (n >= 0) ? (n + minVal) : (n + maxVal);
}

/** [minVal:maxVal)�͈̔͂Ń��[�v������B���������_������ */
template<class T>
T fwrap(T val, T minVal, T maxVal){
	assert(minVal < maxVal);
	double n = fmod(val - minVal, maxVal - minVal);
	return (n >= 0) ? (n + minVal) : (n + maxVal);
}


/** ���`��� t[0:1] */
template <typename T>
T lerp(const T &from, const T &to, double t){
	T diff = to - from;
	return from + diff*t;
}


} //namespace ss



#define SS_SAFE_DELETE(p)           { delete (p); (p) = nullptr; }
#define SS_SAFE_DELETE_ARRAY(p)     { delete[] (p); (p) = nullptr; }


#ifdef _DEBUG
#define SS_LOG(...)					ss::DebugPrintToConsole(__FILE__, __LINE__, __VA_ARGS__)
#define SS_ASSERT(cond)				assert(cond)
#define SS_ASSERT_LOG(cond, ...)	{ SS_LOG(__VA_ARGS__); SS_ASSERT(cond); }
#else
#define SS_LOG(...)
#define SS_ASSERT(cond)				
#define SS_ASSERT_LOG(cond, ...)	
#endif
