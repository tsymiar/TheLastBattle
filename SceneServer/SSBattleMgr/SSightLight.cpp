#include "SSightLight.h"
#include "SSightTool.h"
#include "ISSightLight.h"

namespace SceneServer{
	CSSightLight::CSSightLight():/*m_tRecountMil(0),
								m_x(0),
								m_y(0),*/
	m_iterface(NULL)
{
	//memset(m_sightCellArr,0,sizeof(UINT32)*c_SightCellMaxSize);
}

CSSightLight::~CSSightLight(){

}

void CSSightLight::Clear(){
	//memset(m_sightCellArr,0,sizeof(UINT32)*c_SightCellMaxSize);
	//m_tRecountMil = 0;
	//m_x = 0;
	//m_y = 0;
	m_iterface = NULL;
}

void CSSightLight::SetLightInterface(ISSightLight* pInterface){
	m_iterface = pInterface;
}

//判断一个点是否在视野中
bool CSSightLight::IsInSight(float x, float y){
	float diffX = m_iterface->GetSightX() - x;
	float diffY = m_iterface->GetSightY() - y;

	float	lightDist = m_iterface->GetLightDistance();

	if(diffX > lightDist || diffX < -lightDist || diffY > lightDist || diffY < -lightDist )
		return FALSE;

	float	rHoh = lightDist * lightDist;

	return diffX*diffX + diffY*diffY < rHoh;
}
}