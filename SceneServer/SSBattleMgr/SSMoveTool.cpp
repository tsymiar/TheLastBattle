#include "SSMoveTool.h"
namespace SceneServer{
INT32	CSSMoveTool::IntersectSphereSphere(ColSphere s1, ColVector v1, ColSphere s2, ColVector v2,float &time, ColVector &q){
	ColVector p = s1.c;
	ColVector d = v1 - v2;
	ColSphere s = s2;
	s.r = s1.r + s2.r;

	return IntersectRaySphere(p, d, s, time, q);
}

INT32	CSSMoveTool::IntersectRaySphere(ColVector p, ColVector d, ColSphere s, float &time, ColVector &q){
	ColVector m = p - s.c;
	FLOAT b = Dot(m,d);
	FLOAT c = Dot(m,m);
	//Exit if r's origin outside s(c > 0) and r pointing away from s (b > 0)
	if( c> 0.0f && b>0.0f) return 0;

	FLOAT discr = b*b - c;
	//A negative discriminant corresponds to ray missing sphere
	if(discr < 0.0f) return 0;

	time = -b - sqrt(discr);
	//if time is negatime, started inside sphere so clamp time to zero
	if(time < 0.0f) time = 0.0f;
	q = p + time * d;
	return 1;
}

float CSSMoveTool::SqDistPointAABB(ColVector& p, ColAABB& b)
{
	float sqDist = 0.0f;
	{
		float v = p.x;
		if (v < b.min.x) sqDist += (b.min.x - v) * (b.min.x - v);
		if (v > b.max.x) sqDist += (v - b.max.x) * (v - b.max.x);
	}
	{
		float v = p.y;
		if (v < b.min.y) sqDist += (b.min.y - v) * (b.min.y - v);
		if (v > b.max.y) sqDist += (v - b.max.y) * (v - b.max.y);
	}
	return sqDist;
}

bool	CSSMoveTool::IfImpact(ColSphere& s1, ColSphere& s2){
	if(abs(s1.c.x - s2.c.x) > s1.r + s2.r || abs(s1.c.y - s2.c.y) > s1.r + s2.r){
		return FALSE;
	}
	return (s1.c.x - s2.c.x)*(s1.c.x - s2.c.x) + (s1.c.y - s2.c.y)*(s1.c.y - s2.c.y) < (s1.r + s2.r)*(s1.r + s2.r);
}

bool	CSSMoveTool::IfImpact(ColSphere& s, ColAABB& aabb){
	float dist = SqDistPointAABB(s.c,aabb);
	return dist < s.r * s.r;
}


}