//////////////////////////////////////////////////////////////////////////////////////
// description: represents a 3-dimensional Vector
//
// author: runtimeTerror (runtimeterror@kaiundina.de - http://www.kaiundina.de)
// last modified: 2003-07-30 20-30
//

#pragma once
#include	<math.h>
#include	<iostream>

// Type to use for vector-components (should be double or float)
#define TYPE FLOAT

// often used parameters
#define PARAM_T TYPE m_fX, TYPE m_fY, TYPE m_fZ  // 3 Coordinates of TYPE
#define PARAM_O const CVector3D& v             // 1 Vector-object as alias called v
#define PARAM_P CVector3D* v             // 1 Vector-reference as pointer

// often used combinations of those parameters
#define PARAM_TT TYPE x1, TYPE y1, TYPE z1, TYPE x2, TYPE y2, TYPE z2
#define PARAM_TP TYPE x1, TYPE y1, TYPE z1, CVector3D* v2
#define PARAM_TO TYPE x1, TYPE y1, TYPE z1, CVector3D& v2
#define PARAM_PT CVector3D* v1,              TYPE x2, TYPE y2, TYPE z2
#define PARAM_PP CVector3D* v1,              CVector3D* v2
#define PARAM_PO CVector3D* v1,              CVector3D& v2
#define PARAM_OT CVector3D& v1,              TYPE x2, TYPE y2, TYPE z2
#define PARAM_OP CVector3D& v1,              CVector3D* v2
#define PARAM_OO CVector3D& v1,              CVector3D& v2

const double    dDegToRad = 3.14159265358 / 180.f;
const double    dRadToDeg = 180.f / 3.14159265358;

#define IsNan _isnan
#define Finite _finite

class CVector3D {

public:
	TYPE m_fX, m_fY, m_fZ; // the 3 vector-components
	//////////////////////////////////////////////////////////////////////////////////////
	// Constructors
	inline CVector3D(){this->m_fX = this->m_fY = this->m_fZ = 0;}
	inline CVector3D(TYPE xyz){this->m_fX = this->m_fY = this->m_fZ = xyz;}
	inline CVector3D(PARAM_T){this->m_fX = m_fX;this->m_fY = m_fY;this->m_fZ = m_fZ;}
	inline CVector3D(PARAM_P){this->m_fX = v->m_fX;this->m_fY = v->m_fY;this->m_fZ = v->m_fZ;}
	inline CVector3D(PARAM_O){this->m_fX = v.m_fX;this->m_fY = v.m_fY;this->m_fZ = v.m_fZ;}

	inline bool CanWatch(TYPE fSight, const CVector3D* pcTarget)const{
	    FLOAT fX = pcTarget->m_fX - this->m_fX;
		FLOAT fZ = pcTarget->m_fZ - this->m_fZ;
        return (fX * fX + fZ * fZ) < fSight * fSight;
    }

	inline bool CanWatch(TYPE fSight, const CVector3D& pcTarget)const{
		return GetWatchDistSqr(pcTarget) < fSight * fSight;
	}

	FLOAT	GetWatchDistSqr(const CVector3D& pcTarget)const{
		const FLOAT fX = pcTarget.m_fX - this->m_fX;
		const FLOAT fZ = pcTarget.m_fZ - this->m_fZ;
		return fX * fX + fZ * fZ;
	}

	inline TYPE GetSqrCrossDeep(TYPE fSight, const CVector3D* pcTarget)const{
		double dX = pcTarget->m_fX - m_fX;
		double dY = pcTarget->m_fY - m_fY;
		double dZ = pcTarget->m_fZ - m_fZ;
		double dSqrDistance = dX * dX + dY * dY + dZ * dZ;
		double dSqrSight = fSight * fSight;
		if (dSqrDistance > dSqrSight){
			return 0;
		}
		return (TYPE)(dSqrSight - dSqrDistance);
	}

	void Zero(){
		m_fX = 0;
		m_fY = 0;
		m_fZ = 0;
	}

	void Clear(){
		m_fX = 0;
		m_fY = 0;
		m_fZ = 0;
	}

	void LookAhead(){
		m_fX = -1;
		m_fY = 0;
		m_fZ = 1;
	}

	TYPE GetAbs(TYPE tValue){
		if (0 > tValue){
			return - tValue;
		}
		return tValue;
	}

	bool IsZero() {
		return m_fZ == 0 && m_fX == 0 && m_fY == 0;
	}
			//! Rotates the vector by a specified number of degrees around the Y axis and the specified center.
		/** \param degrees Number of degrees to rotate around the Y axis.
		\param center The center of the rotation. */
	//void RotateXZBy(double degrees, const CVector3D<T>& center=CVector3D<T>())
	void RotateXZBy(double degrees, const CVector3D& center = CVector3D())
	{
		degrees *= dDegToRad;
		double cs = cos(degrees);
		double sn = sin(degrees);
		m_fX -= center.m_fX;
		m_fZ -= center.m_fZ;
		TYPE tTempX = m_fX;
		TYPE tTempZ = m_fZ;
		m_fX = (TYPE)(tTempX * cs - tTempZ * sn);
		m_fZ = (TYPE)(tTempX * sn + tTempZ * cs);
		m_fX += center.m_fX;
		m_fZ += center.m_fZ;
	}

	bool IfValid()const{
		INT32 n32XIsNan = IsNan(m_fX);
		INT32 n32XFinit = Finite(m_fX);
		INT32 n32YIsNan = IsNan(m_fY);
		INT32 n32YFinit = Finite(m_fY);
		INT32 n32ZIsNan = IsNan(m_fZ);
		INT32 n32ZFinit = Finite(m_fZ);

		if (n32XIsNan == 1 || n32XFinit == 0 || n32XIsNan == 1 || n32YFinit == 0 || n32ZIsNan == 1 || n32ZFinit == 0){
			return false;
		}
		return true;
	}


	friend CVector3D operator * (FLOAT a, const CVector3D& p1){
		return CVector3D(a * p1.m_fX , a * p1.m_fY, a * p1.m_fZ);
	}
	friend CVector3D operator * ( const CVector3D& p1, FLOAT a){
		return CVector3D(p1.m_fX * a, p1.m_fY * a, p1.m_fZ * a);
	}

	friend CVector3D operator - (const CVector3D& p1, const CVector3D& p2){return CVector3D(p1.m_fX - p2.m_fX, p1.m_fY-p2.m_fY, p1.m_fZ-p2.m_fZ);}
	CVector3D operator + (PARAM_P){
		return CVector3D( this->m_fX + v->m_fX, this->m_fY + v->m_fY, this->m_fZ + v->m_fZ);
	}
	CVector3D operator + (PARAM_O){
		return CVector3D(m_fX + v.m_fX, m_fY + v.m_fY, m_fZ + v.m_fZ);
	}
	CVector3D* operator += (PARAM_P) { this->m_fX += v->m_fX; this->m_fY += v->m_fY; this->m_fZ += v->m_fZ; return this; }
	CVector3D* operator += (PARAM_O) { this->m_fX += v.m_fX;  this->m_fY += v.m_fY;  this->m_fZ += v.m_fZ;  return this; }
	CVector3D operator - (PARAM_P){
		return CVector3D(m_fX - v->m_fX, m_fY - v->m_fY, m_fZ - v->m_fZ);
	}
	CVector3D operator - (PARAM_O){
		return CVector3D(m_fX - v.m_fX, m_fY - v.m_fY, m_fZ - v.m_fZ);
	}
	CVector3D* operator -= (PARAM_P) { this->m_fX -= v->m_fX; this->m_fY -= v->m_fY; this->m_fZ -= v->m_fZ; return this; }
	CVector3D* operator -= (PARAM_O) { this->m_fX -= v.m_fX;  this->m_fY -= v.m_fY;  this->m_fZ -= v.m_fZ;  return this; }
	TYPE operator * (PARAM_P)  { return this->m_fX * v->m_fX  + this->m_fY * v->m_fY  + this->m_fZ * v->m_fZ;  }
	TYPE operator * (PARAM_O)  { return this->m_fX * v.m_fX   + this->m_fY * v.m_fY   + this->m_fZ * v.m_fZ;   }
	CVector3D operator *  (TYPE s)          { return CVector3D(this->m_fX * s, this->m_fY * s, this->m_fZ * s); }
	CVector3D* operator *= (TYPE s)         { this->m_fX *= s; this->m_fY *= s; this->m_fZ *= s; return this; }
	CVector3D operator / (TYPE s)          { return CVector3D(this->m_fX / s, this->m_fY / s, this->m_fZ / s); }
	CVector3D* operator /= (TYPE s)         { this->m_fX /= s; this->m_fY /= s; this->m_fZ /= s; return this; }
	bool operator == (PARAM_P)  { if (this->m_fX != v->m_fX ) return false; if (this->m_fY != v->m_fY ) return false; if (this->m_fZ != v->m_fZ ) return false; return true;  }
	bool operator == (PARAM_O)  { if (this->m_fX != v.m_fX  ) return false; if (this->m_fY != v.m_fY  ) return false; if (this->m_fZ != v.m_fZ  ) return false; return true;  }
	bool operator != (PARAM_P)  { if (this->m_fX != v->m_fX ) return true;  if (this->m_fY != v->m_fY ) return true;  if (this->m_fZ != v->m_fZ ) return true;  return false; }
	bool operator != (PARAM_O)  { if (this->m_fX != v.m_fX  ) return true;  if (this->m_fY != v.m_fY  ) return true;  if (this->m_fZ != v.m_fZ  ) return true;  return false; }
	CVector3D* operator = (PARAM_P) { this->m_fX = v->m_fX; this->m_fY = v->m_fY; this->m_fZ = v->m_fZ; return this; }
	CVector3D* operator = (PARAM_O) { this->m_fX = v.m_fX;  this->m_fY = v.m_fY;  this->m_fZ = v.m_fZ;  return this; }
	bool Equal(const CVector3D &crsVec){
		if (GetAbs(m_fX - crsVec.m_fX) > 0.0001 || GetAbs(m_fY - crsVec.m_fY) > 0.0001 || GetAbs(m_fZ - crsVec.m_fZ) > 0.0001){
			return false;
		}
		return true;
	}

	TYPE sqr ()        { return this->m_fX * this->m_fX + this->m_fY * this->m_fY + this->m_fZ * this->m_fZ; }


	///////////////////////////////////////////////////
	// scales a vector to ensure, it has a length of 1.0
	// vector mustn't be (0.0, 0.0, 0.0)
	static CVector3D unit (PARAM_T){ return antiscale(m_fX, m_fY, m_fZ, length(m_fX, m_fY, m_fZ)); }
	static CVector3D unit (PARAM_P){ return antiscale(v, length(v)); }
	static CVector3D unit (PARAM_O){ return antiscale(v, length(v)); }
	CVector3D* unit (){ if(this->length() == 0) return this; return this->antiscale(this->length()); }

	///////////////////////////////////////////////////
	// multiplies (scales) a vector with the reziprocal value of a number (1/s)
	// s mustn't be 0
	static CVector3D antiscale (PARAM_T, TYPE s){ return CVector3D(m_fX       / s, m_fY       / s, m_fZ       / s); }
	static CVector3D antiscale (PARAM_P, TYPE s){ return CVector3D(v->m_fX    / s, v->m_fY    / s, v->m_fZ    / s); }
	static CVector3D antiscale (PARAM_O, TYPE s){ return CVector3D(v.m_fX / s, v.m_fY / s, v.m_fZ / s); }
	CVector3D* antiscale (TYPE s){ this->m_fX /= s; this->m_fY /= s; this->m_fZ /= s; return this; }

	///////////////////////////////////////////////////
	// computes the length of a vector
	static TYPE length (PARAM_T){ return (TYPE)sqrt((double)(m_fX * m_fX) + (double)(m_fY * m_fY) + (double)(m_fZ * m_fZ)); }
	static TYPE length (PARAM_P){ return (TYPE)sqrt((double)(v->m_fX * v->m_fX) + (double)(v->m_fY * v->m_fY) + (double)(v->m_fZ * v->m_fZ)); }
	static TYPE length (PARAM_O){ return (TYPE)sqrt((double)(v.m_fX * v.m_fX) + (double)(v.m_fY * v.m_fY) + (double)(v.m_fZ * v.m_fZ)); }
	TYPE length (){ return (TYPE)sqrt((double)(this->m_fX * this->m_fX) + (double)(this->m_fY * this->m_fY) + (double)(this->m_fZ * this->m_fZ)); }

	///////////////////////////////////////////////////
	// determines the smallest angle between two vectors
	// none of the vectors must be (0.0, 0.0, 0.0)

	// returns result using two external vectors
	TYPE angle (PARAM_TT) { return (TYPE)acos((double)(x1    * x2    + y1    * y2    + z1    * z2   ) / sqrt((double)(CVector3D(x1, y1, z1).sqr() * CVector3D(x2, y2, z2).sqr()))); }
	TYPE angle (PARAM_TP) { return (TYPE)acos((double)(x1    * v2->m_fX + y1    * v2->m_fY + z1    * v2->m_fZ) / sqrt((double)(CVector3D(x1, y1, z1).sqr() * v2->sqr()                 ))); }
	TYPE angle (PARAM_TO) { return (TYPE)acos((double)(x1    * v2.m_fX  + y1    * v2.m_fY  + z1    * v2.m_fZ ) / sqrt((double)(CVector3D(x1, y1, z1).sqr() * v2.sqr()                  ))); }
	TYPE angle (PARAM_PT) { return (TYPE)acos((double)(v1->m_fX * x2    + v1->m_fY * y2    + v1->m_fZ * z2   ) / sqrt((double)(v1->sqr() * CVector3D(x2, y2, z2).sqr()))); }
	TYPE angle (PARAM_PP) { return (TYPE)acos((double)(v1->m_fX * v2->m_fX + v1->m_fY * v2->m_fY + v1->m_fZ * v2->m_fZ) / sqrt((double)(v1->sqr() * v2->sqr()	))); }
	TYPE angle (PARAM_PO) { return (TYPE)acos((double)(v1->m_fX * v2.m_fX  + v1->m_fY * v2.m_fY  + v1->m_fZ * v2.m_fZ ) / sqrt((double)(v1->sqr()	* v2.sqr()	))); }
	TYPE angle (PARAM_OT) { return (TYPE)acos((double)(v1.m_fX  * x2    + v1.m_fY  * y2    + v1.m_fZ  * z2   ) / sqrt((double)(v1.sqr()* CVector3D(x2, y2, z2).sqr()))); }
	TYPE angle (PARAM_OP) { return (TYPE)acos((double)(v1.m_fX  * v2->m_fX + v1.m_fY  * v2->m_fY + v1.m_fZ  * v2->m_fZ) / sqrt((double)(v1.sqr()	* v2->sqr()	))); }
	TYPE angle (PARAM_OO) { return (TYPE)acos((double)(v1.m_fX  * v2.m_fX  + v1.m_fY  * v2.m_fY  + v1.m_fZ  * v2.m_fZ ) / sqrt((double)(v1.sqr()	* v2.sqr()	))); }

	// applied with the current instance (no changes will be made)
	TYPE angle (PARAM_T) { return (TYPE)acos((double)(this->m_fX * m_fX    + this->m_fY * m_fY    + this->m_fZ * m_fZ   ) / sqrt((double)(this->sqr() * CVector3D(m_fX, m_fY, m_fZ).sqr()))); }
	TYPE angle (PARAM_P) { return (TYPE)acos((double)(this->m_fX * v->m_fX + this->m_fY * v->m_fY + this->m_fZ * v->m_fZ) / sqrt((double)(this->sqr() * v->sqr()))); }
};

