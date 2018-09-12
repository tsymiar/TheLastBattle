/*
* file name			:SSIntersectObject.h
* file mark			:
* summary			:
*
* version			:1.0
* author			:LiuLu
* complete date		:August 4 2014
* summary			:
*
*/

#ifndef __SSIntersectObject_h__
#define __SSIntersectObject_h__

#include "stdafx.h"

namespace SceneServer{
//寻路系统用点对象，可以表现点和方向
struct ColVector{
	FLOAT x;
	FLOAT y;

	ColVector():x(0.0f), y(0.0f){}
	ColVector(FLOAT x, FLOAT y):x(x), y(y){}

	ColVector operator + (const ColVector& v){
		return ColVector(x + v.x, y + v.y);
	}
	
	ColVector operator - (const ColVector& v){
		return ColVector(x - v.x, y - v.y);
	}

	friend ColVector operator * (FLOAT a, const ColVector& p1){
		return ColVector(a * p1.x , a * p1.y);
	}
	friend ColVector operator * ( const ColVector& p1, FLOAT a){
		return ColVector(p1.x * a, p1.y * a);
	}
	friend ColVector operator / ( const ColVector& p1, FLOAT a){
		if (a == 0.0f){
			return ColVector();
		}
		return ColVector(p1.x / a, p1.y / a);
	}
	ColVector operator / (FLOAT a){
		if (a == 0.0f){
			return ColVector();
		}
		return ColVector(this->x / a, this->y / a); 
	}
	ColVector antiscale(ColVector *pV, FLOAT a){
		if (a == 0.0f){
			return ColVector();
		}
		return ColVector(pV->x / a, pV->y / a);
	}
	bool operator == (const ColVector& v)  { return this->x == v.x && this->y == v.y; }
	bool operator != (const ColVector& v)  { return this->x != v.x || this->y != v.y; }

	ColVector Unit(){
		return antiscale(this, Length());
	}

	FLOAT Length(){
		return sqrt(LengthSqrt());
	}

	FLOAT LengthSqrt(){
		return (double)x*x + (double)y*y;
	}

};
//寻路系统用圆对象，用点和半径表示一个圆
struct ColSphere{
	ColVector c;
	FLOAT r;
	ColSphere():r(0.0f){

	}
	ColSphere(ColVector& c, FLOAT r):c(c), r(r){

	}
};

//寻路系统用长方形对象，用左上角和右下角两个点表示平行于X和Y轴的长方形。
struct ColAABB{
	ColVector min;
	ColVector max;
};
}

#endif