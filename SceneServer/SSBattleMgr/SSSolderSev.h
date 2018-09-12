#ifndef SSSOLDER_H_
#define SSSOLDER_H_

namespace SceneServer{
	class SSSolderSev
	{
		INT64											m_tNPCNextBornCDMilsec;			//出兵时间:延迟时间  出兵波数时间延迟
		//bool											m_ifAltarBrokern;				//祭坛是否被打爆 用于出超级兵

		std::map<INT32/*pathId*/, SMapCurtBornSolderInfo >		m_curtSolderBornInfo;
	public:
		SSSolderSev();
		~SSSolderSev();  

		std::map<INT32/*pathId*/, SMapCurtBornSolderInfo >&  GetMapCurtBornSolderInfo(){return		m_curtSolderBornInfo;}

		bool							CheckBornSolder(INT64  curttime);	 

		void							SetNPCNextBornCDMisec(INT64 tcfg);  

		void							SetAltarBrokenAboutSuperSolder(INT32 pathId, INT32 altarIdx, std::vector<INT32>& altarCfg);

	private: 
		bool								Comp2Vector(std::vector<INT32> & v1, std::vector<INT32> & v2);
	};
}
#endif