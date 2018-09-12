#include "SSPassitiveEffect.h"

namespace SceneServer{
	CSSPassiveEffect::CSSPassiveEffect():m_pEffectUniqueID(0),
	m_pcMasterGO(NULL),
	m_bIfKeeped(FALSE),
	m_effectType(EPassiveSkillType_None)
{}

CSSPassiveEffect::~CSSPassiveEffect(){

}
}