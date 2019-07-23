#include "stdafx.h"
#include "cWObject.h"

#include <math.h>

cWObject::cWObject()
{
	//init all values to zero...
	GUID = 0;
	wielder = 0;
	m_wStance = 0x003D;
	location.xOffset = 0;
	location.yOffset = 0;
	location.zOffset = 0;
	location.wHeading = 0;
	location.aHeading = 0;
	location.bHeading = 0;
	location.cHeading = 0;
	location.landblock = 0;
	objectName = "";

	animCount = 0;

	unknown5 = 0;
	category = 0;

	m_fScale = 1.0f;

	moveCount = 0;
	m_bMoving = false;
	Velocity = cPoint3D(0,0,0);
	fVelocityTurn = 0;
	fVelocityStrafe = 0;
	fVelocityFB = 0;
	CalcPosition = cPoint3D(0,0,0);
	//m_fHeading = 0;

	m_bModelUpdate = false;
	m_mgModel = 0;
}

cWObject::~cWObject()
{
	delete m_mgModel;
}

int cWObject::Draw()
{
	if (m_mgModel)
		return m_mgModel->Draw();
	else
		return 0;
}

void cWObject::SetMoveVelocities(float fFB, float fStrafe, float fTurn)
{
	fVelocityFB = fFB;
	fVelocityStrafe = fStrafe;
	fVelocityTurn = fTurn;
	m_bMoving = ((fFB != 0) || (fStrafe != 0) || (fTurn != 0));
}

void cWObject::UpdatePosition(float fTimeDiff)
{
	Lock();
	if (m_bMoving)
	{
		CalcPosition += Velocity*fTimeDiff;

		//Turning...
 		float fCurHeading = GetHeading();
		fCurHeading -= (float) (fVelocityTurn*(M_PI/2)*fTimeDiff);
		if (fCurHeading > 2*M_PI)
			fCurHeading -= (float) (2*M_PI);
		if (fCurHeading < 0)
			fCurHeading += (float) (2*M_PI);
		LoadLocationHeading(fCurHeading);

		//Forwards/Backwards
		cPoint3D tpstrafe(0,1,0);
		tpstrafe.RotateAround(cPoint3D(0,0,0), cPoint3D(0,0,fCurHeading));
		CalcPosition += tpstrafe*(fVelocityFB*fTimeDiff/240.0f);

		//Strafing
		tpstrafe.RotateAround(cPoint3D(0,0,0), cPoint3D(0,0,(float) -M_PI/2));
		CalcPosition += tpstrafe*(fVelocityStrafe*fTimeDiff/240.0f);

		CalcHeading();
	}
	if (m_bModelUpdate)
	{
		bool FirstSet = false;
		if (!m_mgModel)
		{
			FirstSet = true;
			m_mgModel = new cModelGroup();
		}
		m_mgModel->ReadModel(modelNumber, &palettes, &textures, &models);
		m_mgModel->SetScale((1/240.0f) * m_fScale);

		if (FirstSet)
		{
			if (m_mAnims.find(0x003D0003) != m_mAnims.end())
			{
				stAnimSet asTemp = m_mAnims[0x003D0003];	//idle anim?
				m_mgModel->SetDefaultAnim(asTemp.vAnims[0].dwAnim);
			}
		}

		m_bModelUpdate = false;
	}
	m_mgModel->SetTranslation(CalcPosition);
	m_mgModel->SetRotation(location.wHeading, location.aHeading, location.bHeading, location.cHeading);
	m_mgModel->UpdateAnim(fTimeDiff);
	Unlock();
}

void cWObject::PlayAnimation(WORD wAnim, WORD wStance, float fPlaySpeed, bool bSetDefault)
{
	//fix this to cache...
	if (!m_mgModel)
		return;

	if (m_mAnims.find(((DWORD) wStance << 16) | wAnim) != m_mAnims.end())
	{
		stAnimSet *asTemp = &m_mAnims[((DWORD) wStance << 16) | wAnim];
		stAnimInfo aiTemp = asTemp->vAnims[0];

		if (bSetDefault)
			m_mgModel->SetDefaultAnim(aiTemp.dwAnim);
		m_mgModel->PlayAnimation(aiTemp.dwAnim, aiTemp.dwStartFrame, aiTemp.dwEndFrame, aiTemp.fPlaySpeed * fPlaySpeed);
	}
}

void cWObject::ParseF74C(cMessage * Message)
{
	Lock();
	numLogins = Message->ReadWORD();
	WORD sequence = Message->ReadWORD();
	animCount = Message->ReadWORD();
	WORD activity = Message->ReadWORD();

	BYTE animation_type = Message->ReadByte();
	BYTE type_flags = Message->ReadByte();
	WORD wStance = Message->ReadWORD();
//	if (m_wStance != wStance)
//		f74csequence++;
	m_wStance = wStance;

	switch (animation_type)
	{
	case 0x00:
		{
			//general animation
			DWORD flags = Message->ReadDWORD();

			WORD wAnimToPlay = 3;
			bool bSetDefault = true;
			float fSpeed = 1.0f;
			m_bMoving = false;

			if (flags & 0x1)
			{
				WORD stance2 = Message->ReadWORD();
				if (m_wStance != stance2)
					int a = 4;
			}
			if (flags & 0x2)
			{
				//hold animation until stopped
				//holding out hand does this
				wAnimToPlay = Message->ReadWORD();
			}
			if (flags & 0x8)
			{
				//strafing
				wAnimToPlay = Message->ReadWORD();
				m_bMoving = true;
			}
			else
				fVelocityStrafe = 0;

			if (flags & 0x20)
			{
				//turning...
				wAnimToPlay = Message->ReadWORD();
				m_bMoving = true;
			}
			else
				fVelocityTurn = 0;

			if (flags & 0x4)
			{
				//movement speed (forward/backwards)
                fVelocityFB = Message->ReadFloat();
				fSpeed = fabs(fVelocityFB);
				fVelocityFB *= 3;
				m_bMoving = true;
			}
			else
				fVelocityFB = 0;

			if (flags & 0x10)
			{
				//strafe speed
				fVelocityStrafe = Message->ReadFloat();
				fSpeed = fabs(fVelocityStrafe);
				m_bMoving = true;
			}
			if (flags & 0x40)
			{
				//turn speed
				fVelocityTurn = Message->ReadFloat();
				fSpeed = fabs(fVelocityTurn);
				m_bMoving = true;
			}
			if (flags & 0x80)
			{
				//anim sequence
				wAnimToPlay = Message->ReadWORD();
				WORD sequence = Message->ReadWORD();
				fSpeed = Message->ReadFloat();
				bSetDefault = false;
			}

			if (!m_bMoving)
			{
//				fVelocityStrafe = 0;
//				fVelocityTurn = 0;
//				fVelocityFB = 0;
			}

			PlayAnimation(wAnimToPlay, m_wStance, fSpeed, bSetDefault);

			break;
		}
	case 0x06:
		{
			//move to object
			DWORD object = Message->ReadDWORD();

			stLocation tpLoc;
			memcpy(&tpLoc, Message->ReadGroup(sizeof(tpLoc)), sizeof(tpLoc));
			float animation_speed = Message->ReadFloat();
			float float_4 = Message->ReadFloat();
			float heading = Message->ReadFloat();
			DWORD unknown_value = Message->ReadDWORD();
			cPoint3D GoTo;
			GoTo.CalcFromLocation(&tpLoc);
			Velocity = (GoTo - CalcPosition);
			Velocity.Normalize();
			Velocity *= animation_speed/240;
			m_bMoving = true;

			break;
		}
	case 0x07:
		{
			stLocation tpLoc;
			memcpy(&tpLoc, Message->ReadGroup(sizeof(tpLoc)), sizeof(tpLoc));
			float animation_speed = Message->ReadFloat();
			float float_4 = Message->ReadFloat();
			float heading = Message->ReadFloat();
			DWORD unknown_value = Message->ReadDWORD();
			cPoint3D GoTo;
			GoTo.CalcFromLocation(&tpLoc);
			Velocity = (GoTo - CalcPosition);
			Velocity.Normalize();
			Velocity *= animation_speed/240;
			m_bMoving = true;

			break;
		}
	case 0x08:
		{
			int a = 4;
			break;
		}
	case 0x09:
		{
			int a = 4;
			break;
		}
	default:
		{
			int a = 4;
			break;
		}
	};

	//TODO: more stuff after this... check protocol and figure it all out...
	Message->ReadAlign();
	Unlock();
}

void cWObject::ParseF625(cMessage * Message)
{
	Lock();
	Message->ReadByte();	//eleven
	int paletteCount = Message->ReadByte();
	int textureCount = Message->ReadByte();
	int modelCount = Message->ReadByte();
	
	palettes.clear();
	textures.clear();
	models.clear();

	if (paletteCount)
	{
		DWORD palflags = Message->ReadPackedDWORD();

		for (int i=0;i<paletteCount;i++)
		{
			stPaletteSwap tpPal;
			tpPal.newPalette = Message->ReadPackedDWORD();
			tpPal.offset = Message->ReadByte();
			tpPal.length = Message->ReadByte();
			palettes.push_back(tpPal);
		}
	}

	for (int i=0;i<textureCount;i++)
	{
		stTextureSwap tpTex;
		tpTex.modelIndex = Message->ReadByte();
		tpTex.oldTexture = Message->ReadPackedDWORD();
		tpTex.newTexture = Message->ReadPackedDWORD();
		textures.push_back(tpTex);
	}

	for (int i=0;i<modelCount;i++)
	{
		stModelSwap tpMod;
		tpMod.modelIndex = Message->ReadByte();
		tpMod.newModel = Message->ReadPackedDWORD();
		models.push_back(tpMod);
	}

	Message->ReadAlign();

	m_bModelUpdate = true;
	Unlock();
}

void cWObject::ParseF745(cMessage * Message)
{
	Lock();
	GUID = Message->ReadDWORD();

	//mmm, code reuse
	ParseF625(Message);

	DWORD flags = Message->ReadDWORD();
	portalMode = Message->ReadWORD();
	unknown_1 = Message->ReadWORD();

	//flags mask..
	if (flags & 0x00010000)
	{
		DWORD unknownCount = Message->ReadDWORD();
		for (int i=0;i<(int) unknownCount;i++)
		{
			BYTE unknownByte = Message->ReadByte();
		}
		DWORD unknownDword = Message->ReadDWORD();
	}
	if (flags & 0x00020000)
	{
		DWORD unknown = Message->ReadDWORD();
	}
	if (flags & 0x00008000)
	{
		memcpy(&location, Message->ReadGroup(sizeof(stLocation)), sizeof(stLocation));
	}
	if (flags & 0x00000002)
	{
		animConfig = Message->ReadDWORD();
		
		//parse animset!
		LoadAnimset();
	}
	if (flags & 0x00000800)
	{
		soundset = Message->ReadDWORD();
	}
	if (flags & 0x00001000)
	{
		unknown_blue = Message->ReadDWORD();
	}
	if (flags & 0x00000001)
	{
		modelNumber = Message->ReadDWORD();
	}
	if (flags & 0x00000020)
	{
		wielder = Message->ReadDWORD();
		wieldingSlot = Message->ReadDWORD();
	}
	if (flags & 0x00000040)
	{
		int equipCount = Message->ReadDWORD();
		for (int i=0;i<equipCount;i++)
		{
			stEquipped tpEquip;
			tpEquip.equipID = Message->ReadDWORD();
			tpEquip.equipSlot = Message->ReadDWORD();
			equipped.push_back(tpEquip);
		}
	}
	if (flags & 0x00000080)
	{
		m_fScale = Message->ReadFloat();
	}
	if (flags & 0x00000100)
	{
		unknown_darkbrown = Message->ReadDWORD();
	}
	if (flags & 0x00000200)
	{
		unknown_brightpurple = Message->ReadDWORD();
	}
	if (flags & 0x00040000)
	{
		unknown_lightgrey = Message->ReadFloat();
	}
	if (flags & 0x00000004)
	{
		unknown_trio1_1 = Message->ReadFloat();
		unknown_trio1_2 = Message->ReadFloat();
		unknown_trio1_3 = Message->ReadFloat();
	}
	if (flags & 0x00000008)
	{
		unknown_trio2_1 = Message->ReadFloat();
		unknown_trio2_2 = Message->ReadFloat();
		unknown_trio2_3 = Message->ReadFloat();
	}
	if (flags & 0x00000010)
	{
		unknown_trio3_1 = Message->ReadFloat();
		unknown_trio3_2 = Message->ReadFloat();
		unknown_trio3_3 = Message->ReadFloat();
	}
	if (flags & 0x00002000)
	{
		unknown_medgrey = Message->ReadDWORD();
	}
	if (flags & 0x00004000)
	{
		unknown_bluegrey = Message->ReadFloat();
	}
	//end of flags section

	//balh?
	if ((flags & 0x4004) == 0x4004)
	{
		Velocity = cPoint3D(unknown_trio1_1, unknown_trio1_2, unknown_trio1_3);
		Velocity *= 1.0f/240;
//		Velocity *= unknown_bluegrey/240;	//unknown_bluegrey's connection hasn't been explored yet
		m_bMoving = true;
	}
	//blah!

	//one of these might not exist...
	numMovements = Message->ReadWORD();
	numAnimInteract = Message->ReadWORD();
	numBubbleMode = Message->ReadWORD();
	numJumps = Message->ReadWORD();
	numPortals = Message->ReadWORD();
	animCount = Message->ReadWORD();
	numOverride = Message->ReadWORD();
	unknown_seagreen8 = Message->ReadWORD();
	numLogins = Message->ReadWORD();
//	unknown_seagreen10 = Message->ReadWORD();
	Message->ReadAlign();

	//Game data
	DWORD flags1 = Message->ReadDWORD();
	
	char *tpString = Message->ReadString();
	objectName = tpString;
	delete []tpString;

	model = Message->ReadPackedDWORD();
	icon = Message->ReadPackedDWORD();

	category = Message->ReadDWORD();
	behavior = Message->ReadDWORD();
	Message->ReadAlign();

	DWORD flags2 = 0;
	if (behavior & 0x04000000)
		flags2 = Message->ReadDWORD();

	//flags1 masks...
	if (flags1 & 0x00000001)
	{
		namePlural = Message->ReadString();
	}
	if (flags1 & 0x00000002)
	{
		itemSlots = Message->ReadByte();
	}
	if (flags1 & 0x00000004)
	{
		packSlots = Message->ReadByte();
	}
	if (flags1 & 0x00000100)
	{
		ammunition = Message->ReadWORD();
	}
	if (flags1 & 0x00000008)
	{
		value = Message->ReadDWORD();
	}
	if (flags1 & 0x00000010)
	{
		unknown_v2 = Message->ReadDWORD();
	}
	if (flags1 & 0x00000020)
	{
		approachDistance = Message->ReadFloat();
	}
	if (flags1 & 0x00080000)
	{
		usableOn = Message->ReadDWORD();
	}
	if (flags1 & 0x00000080)
	{
		iconHighlight = Message->ReadDWORD();
	}
	if (flags1 & 0x00000200)
	{
		wieldType = Message->ReadByte();
	}
	if (flags1 & 0x00000400)
	{
		usesLeft = Message->ReadWORD();
	}
	if (flags1 & 0x00000800)
	{
		totalUses = Message->ReadWORD();
	}
	if (flags1 & 0x00001000)
	{
		stackCount = Message->ReadWORD();
	}
	if (flags1 & 0x00002000)
	{
		stackMax = Message->ReadWORD();
	}
	if (flags1 & 0x00004000)
	{
		container = Message->ReadDWORD();
	}
	if (flags1 & 0x00008000)
	{
		owner = Message->ReadDWORD();
	}
	if (flags1 & 0x00010000)
	{
		coverage1 = Message->ReadDWORD();
	}
	if (flags1 & 0x00020000)
	{
		coverage2 = Message->ReadDWORD();
	}
	if (flags1 & 0x00040000)
	{
		coverage3 = Message->ReadDWORD();
	}
	if (flags1 & 0x00100000)
	{
		unknown5 = Message->ReadByte();
	}
	if (flags1 & 0x00800000)
	{
		unknown_v6 = Message->ReadByte();
	}
	if (flags1 & 0x08000000)
	{
		unknown800000 = Message->ReadWORD();
	}
	if (flags1 & 0x01000000)
	{
		workmanship = Message->ReadFloat();
	}
	if (flags1 & 0x00200000)
	{
		burden = Message->ReadWORD();
	}
	if (flags1 & 0x00400000)
	{
		associatedSpell = Message->ReadWORD();
	}
	if (flags1 & 0x02000000)
	{
		houseOwnerID = Message->ReadDWORD();
	}
	if (flags1 & 0x04000000)
	{
		dwellingaccess = Message->ReadDWORD();
	}
	if (flags1 & 0x20000000)
	{
		hookTypeUnknown = Message->ReadWORD();
		hookType = Message->ReadWORD();
	}
	if (flags1 & 0x00000040)
	{
		monarch = Message->ReadDWORD();
	}
	if (flags1 & 0x10000000)
	{
		hookableOn = Message->ReadWORD();
	}
	if (flags1 & 0x40000000)
	{
		iconOverlay = Message->ReadPackedDWORD();
	}
	if (behavior & 0x04000000)
	{
//		if (flags2 & 0x00000001)	//yes this should be flags2
//		{
			iconUnderlay = Message->ReadPackedDWORD();
//		}
	}
	if (flags1 & 0x80000000)
	{
		material = Message->ReadDWORD();
	}
	//end flags1

	//now precalc shit
	CalcPosition.CalcFromLocation(&location);
	CalcHeading();

	Unlock();
}

void cWObject::ParseF748(cMessage * Message)
{
	Lock();

	DWORD flags = Message->ReadDWORD();

	location.landblock = Message->ReadDWORD();
	location.xOffset = Message->ReadFloat();
	location.yOffset = Message->ReadFloat();
	location.zOffset = Message->ReadFloat();
	if (~flags & 0x08) location.wHeading = Message->ReadFloat();
	if (~flags & 0x10) location.aHeading = Message->ReadFloat();// else location.aHeading = 0;
	if (~flags & 0x20) location.bHeading = Message->ReadFloat();// else location.bHeading = 0;
	if (~flags & 0x40) location.cHeading = Message->ReadFloat();
	if (flags & 0x01)
	{
		//velocity
		float tx = Message->ReadFloat();
		float ty = Message->ReadFloat();
		float tz = Message->ReadFloat();
		Velocity = cPoint3D(tx, ty, tz);
	}
	if (flags & 0x02)
	{
		DWORD unknown = Message->ReadDWORD();
	}
	if (flags & 0x04)
	{
		numLogins = Message->ReadWORD();
		moveCount = Message->ReadWORD();
		numPortals = Message->ReadWORD();
		numOverride = Message->ReadWORD();
	}

	CalcPosition.CalcFromLocation(&location);
	CalcHeading();

	Unlock();
}

void cWObject::AdjustStack(DWORD Count, DWORD Value)
{
	//hmmmm, make sure this is right at some point
	Lock();
	stackCount = (WORD) Count;
	value = Value;
	Unlock();
}

void cWObject::Set229(DWORD Type, DWORD Value)
{
	Lock();
	switch (Type)
	{
	case 0x0A:
		{
			//set coverage
			coverage2 = Value;
			break;
		}
	case 0x5C:
		{
			//uses remaining
			usesLeft = (WORD) Value;
			break;
		}
	case 0xC1:
		{
			//keys on keyring
			usesLeft = (WORD) Value;//same variable?
			break;
		}
	};
	Unlock();
}

void cWObject::Set22D(DWORD Type, DWORD Value)
{
	Lock();
	switch (Type)
	{
	case 0x02:
		{
			//set as container
			wielder = 0;
			container = Value;
			break;
		}
	case 0x03:
		{
			//set as wielder
			wielder = Value;
			container = 0;
			break;
		}
	};
	Unlock();
}

DWORD cWObject::GetGUID()
{
	return GUID;
}

DWORD cWObject::GetWielder()
{
	return wielder;
}

cPoint3D cWObject::GetPosition()
{
	return CalcPosition;
}

float cWObject::GetHeading()
{
	return m_fHeading;
}

DWORD cWObject::GetObjectFlags2()
{
	return category;
}

DWORD cWObject::GetRadarOverride()
{
	return unknown5;
}

void cWObject::CalcHeading()
{
	//trimmed way down quat->euler algo

	double matrix[3][3];
	double sy;
	double cz,sz;

	// CONVERT QUATERNION TO MATRIX - I DON'T REALLY NEED ALL OF IT
	matrix[0][0] = 1.0f - (2.0f * location.bHeading * location.bHeading) - (2.0f * location.cHeading * location.cHeading);
//	matrix[0][1] = (2.0f * location.aHeading * location.bHeading) - (2.0f * location.wHeading * location.cHeading);
//	matrix[0][2] = (2.0f * location.aHeading * location.cHeading) + (2.0f * location.wHeading * location.bHeading);
	matrix[1][0] = (2.0f * location.aHeading * location.bHeading) + (2.0f * location.wHeading * location.cHeading);
//	matrix[1][1] = 1.0f - (2.0f * location.aHeading * location.aHeading) - (2.0f * location.cHeading * location.cHeading);
//	matrix[1][2] = (2.0f * location.bHeading * location.cHeading) - (2.0f * location.wHeading * location.aHeading);
	matrix[2][0] = (2.0f * location.aHeading * location.cHeading) - (2.0f * location.wHeading * location.bHeading);
//	matrix[2][1] = (2.0f * location.bHeading * location.cHeading) + (2.0f * location.wHeading * location.aHeading);
//	matrix[2][2] = 1.0f - (2.0f * location.aHeading * location.aHeading) - (2.0f * location.bHeading * location.bHeading);

	sy = -matrix[2][0];

	if ((sy != 1.0f) && (sy != -1.0f))	
	{
		double cy = sqrt(1 - (sy * sy));
		cz = matrix[0][0] / cy;
		sz = matrix[1][0] / cy;
		m_fHeading = (float)atan2(sz,cz);
	}
	else
	{
		cz = 1.0f;
		sz = 0.0f;
		m_fHeading = (float)atan2(sz,cz);
	}
}

void cWObject::LoadLocationHeading(float fZ)
{
	//only 
	float c2 = cos(fZ/2);
	float s2 = sin(fZ/2);
	
//	w = c1 c2 c3 - s1 s2 s3
//	x = s1 s2 c3 +c1 c2 s3
//	y = s1 c2 c3 + c1 s2 s3
//	z = c1 s2 c3 - s1 c2 s3
	location.wHeading = c2;
	location.aHeading = 0;
	location.bHeading = 0;
	location.cHeading = s2;
}

std::string cWObject::GetName()
{
	return objectName;
}

DWORD cWObject::GetLandblock()
{
	return location.landblock;
}

stLocation * cWObject::GetLocation()
{
	return &location;
}

stMoveInfo cWObject::GetMoveInfo()
{
	stMoveInfo miTemp;
	miTemp.moveCount = moveCount;
	miTemp.numLogins = numLogins;
	miTemp.numOverride = numOverride;
	miTemp.numPortals = numPortals;
	return miTemp;
}

void cWObject::SetVelocity(cPoint3D NewVelocity)
{
	Velocity = NewVelocity;
}


void cWObject::LoadAnimset()
{
	cPortalFile *pf = m_Portal->OpenEntry(animConfig);
	if (!pf)
		return;

	cByteStream BS(pf->data, pf->length);
	BS.ReadBegin();

	//portal ID
	DWORD dwID = BS.ReadDWORD();

	//something that tends to always be 0x8000003D
	DWORD dwUnknown = BS.ReadDWORD();

	//first vector, pairs together different 0x800000**'s to 0x41000003?
	DWORD vec1count = BS.ReadDWORD();
	for (DWORD i=0; i<vec1count; i++)
	{
		DWORD unk1 = BS.ReadDWORD();
		DWORD unk2 = BS.ReadDWORD();
	}

    FILE *out = NULL;
	//FILE *out=fopen("aset.txt","wt");

	DWORD vec2count = BS.ReadDWORD();
	for (DWORD i=0; i<vec2count; i++)
	{
		//0x003D0141 ?? obj[0];
		stAnimSet asTemp;
		asTemp.dwID = BS.ReadDWORD();
        asTemp.dwFlags = BS.ReadDWORD();

		stAnimInfo aiTemp;
		aiTemp.dwAnim = BS.ReadDWORD(); //0x0300****
		aiTemp.dwStartFrame = BS.ReadDWORD(); //0x00000000
		aiTemp.dwEndFrame = BS.ReadDWORD(); //0xFFFFFFFF
		aiTemp.fPlaySpeed = BS.ReadFloat();

        if (out != NULL) {
            fprintf(out, "%08X (%08X): %08X\n", asTemp.dwID, asTemp.dwFlags, aiTemp.dwAnim);
        }

		if (aiTemp.fPlaySpeed == 0)
			aiTemp.fPlaySpeed = 30.0f;

		asTemp.vAnims.push_back(aiTemp);
		
		m_mAnims[asTemp.dwID] = asTemp;

		if (asTemp.dwFlags & 0x20000) {
			//3 floats? are here
			BS.ReadFloat();
			BS.ReadFloat();
			BS.ReadFloat();
		}
	}

	DWORD vec3count = BS.ReadDWORD();
	for (DWORD i=0; i<vec3count; i++)
	{
		//first word(low) is weird (0x0D, 0x0F, etc.)
		//second word(high) is animation stance?
		WORD stance = (WORD)(BS.ReadDWORD() >> 16);

		//probably not flags, probably 2 WORDS (high is usually 2, sometimes 1)
		DWORD flags = BS.ReadDWORD();

		//last 3 are floats? could be dependent on flags, who knows..
		BS.ReadFloat();
		BS.ReadFloat();
		BS.ReadFloat();
	}

	DWORD vec4count = BS.ReadDWORD();
	for (DWORD i=0; i<vec4count; i++)
	{
		//0x003D0141 ?? obj[0]
		WORD wUnk = BS.ReadWORD();
		WORD wStance = BS.ReadWORD();

        if (out != NULL) {
            fprintf(out, "%04X (%04X): \n", wStance, wUnk);
        }

		DWORD vec5count = BS.ReadDWORD();
		for (DWORD h=0; h<vec5count; h++)
		{
			stAnimSet asTemp;
			asTemp.dwID = BS.ReadWORD();
			asTemp.dwFlags = BS.ReadWORD();

            if (out != NULL) {
                fprintf(out, "  %04X (%04X):: ", asTemp.dwID, asTemp.dwFlags);
            }

			DWORD vec6count = BS.ReadDWORD();
			for (DWORD vec6it = 0; vec6it < vec6count; vec6it++)
			{
				stAnimInfo aiTemp;
				aiTemp.dwAnim = BS.ReadDWORD(); //0x0300****
				aiTemp.dwStartFrame = BS.ReadDWORD(); //0x00000000
				aiTemp.dwEndFrame = BS.ReadDWORD(); //0xFFFFFFFF
				aiTemp.fPlaySpeed = BS.ReadFloat();

                if (out != NULL)
                {
                    fprintf(out, "%08X ", aiTemp.dwAnim);
                }

				//?
				if (aiTemp.fPlaySpeed == 0)
					aiTemp.fPlaySpeed = 30.0f;

				asTemp.vAnims.push_back(aiTemp);
			}

            if (out != NULL) {
                fprintf(out, "\n");
            }

			//uhh, ick...
			if ((wUnk == 0) || (wUnk == 3))
				asTemp.dwID |= ((DWORD) wStance << 16);
			else
				asTemp.dwID |= ((DWORD) wUnk << 16);

			m_mAnims[asTemp.dwID] = asTemp;
		}
	}
    if (out != NULL) {
        fclose(out);
    }
}

WORD cWObject::GetStance()
{
	return m_wStance;
}
