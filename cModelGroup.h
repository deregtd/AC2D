#pragma once

#include "cModel.h"

class cModelGroup {
public:
	cModelGroup();
	~cModelGroup();

	void SetTranslation(cPoint3D Translation);
	void SetRotation(float Rot1, float Rot2, float Rot3, float Rot4);
	void SetScale(float fScale);

	void SetDefaultAnim(DWORD dwAnim);
	void PlayAnimation(DWORD dwAnim, DWORD dwStartFrame, DWORD dwEndFrame, float fPlaySpeed);
	void UpdateAnim(float fTime);

	int Draw();

	bool ReadModel(DWORD dwModel, std::vector<stPaletteSwap> *vPaletteSwaps = 0, std::vector<stTextureSwap> *vTextureSwaps = 0, std::vector<stModelSwap> *vModelSwaps = 0);
	bool ReadDungeon(DWORD dwDungeon, WORD wDungeonPart, std::vector<WORD> * v_Textures);

private:
	//anims
	cPortalFile *m_pfAnim;
	float m_fAnimT;
	float m_fPlaySpeed;
	float *m_fKeyData;
	DWORD m_iNumFrames, m_iNumParts;
	DWORD m_dwEndFrame;
    
	DWORD m_dwCurAnim, m_dwDefaultAnim;

	std::vector<cModel *> m_vModels;

	float m_fScale;

	cPoint3D m_pTranslation;
	float m_fRotation[4];

};
