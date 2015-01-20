#include "stdafx.h"
#include "cCharInfo.h"

cCharInfo::cCharInfo()
{
	m_dwTotalBurden = 0;
	m_dwTotalPyreal = 0;
	m_qwTotalXP = 0;
	m_qwUnassignedXP = 0;
	m_dwUnassignedCredits = 0;
	m_dwLevel = 0;
	m_dwRank = 0;
	m_dwDeaths = 0;
	m_dwBirth = 0;
	m_dwAge = 0;

	m_bMovementDisabled = false;

	m_sName = "";
	m_sGender = "";
	m_sRace = "";
	m_sClass = "";

	m_lCorpses.clear();

	m_dwGUID = 0;
	m_dwPatron = 0;
	m_dwMonarch = 0;

	m_fVitae = 1.0f;

	m_lSpellbook.clear();

	for (int i=0;i<7;i++)
		m_lSpellBar[i].clear();

	m_lCompBuyer.clear();

	m_dwOptions = 0;

	for (int i=0;i<10;i++)
		m_dwShortcut[i] = 0;

	m_dwMaxSkill = 0;
//	ZeroMemory(m_siStat, sizeof(m_siStat));
//	ZeroMemory(m_siSecStat, sizeof(m_siSecStat));

	m_lEnchantments.clear();

	LoadSpellsTable();
	LoadSkillsTable();
	LoadStatsTable();
	LoadSecStatsTable();

	m_aiMonarch = m_aiPatron = m_aiChar = NULL;
	m_vVassals.clear();
}

cCharInfo::~cCharInfo()
{
	for (std::list<cEnchantment *>::iterator tpi = m_lEnchantments.begin(); tpi != m_lEnchantments.end(); tpi++)
		delete *tpi;
}

void cCharInfo::LoadSpellsTable()
{
	//Parse Spell Table
	cPortalFile *PF = m_Portal->OpenEntry(0x0E00000E);
	cByteStream *BS = new cByteStream(PF->data, PF->length);

	BS->ReadBegin();
	DWORD dwID = BS->ReadDWORD();
	WORD wCount = BS->ReadWORD();
	BS->ReadWORD();

	stInternalSpell IS;
	memset( &IS, 0, sizeof( IS ) );

	//FILE *out = fopen("c:\\spells.csv", "wt");

	// Loop through the spell table.
	for( int i = 0; i < wCount; i++ )
	{
		IS.dwSpellID = BS->ReadDWORD();

		char *temp = BS->ReadEncodedString(); 
		IS.szName = temp;
		delete []temp;

		temp = BS->ReadEncodedString();
		IS.szDesc = temp;
		delete []temp;

		IS.dwSchool = BS->ReadDWORD();
		IS.dwIcon = BS->ReadDWORD();
		IS.dwEffect = BS->ReadDWORD();
		IS.dwFlags = BS->ReadDWORD();
		IS.dwManaCost = BS->ReadDWORD();
		IS.fRangeBase = BS->ReadFloat();
		IS.fRangeModifier = BS->ReadFloat();
		IS.dwDifficulty = BS->ReadDWORD();
		IS.fEconomy = BS->ReadFloat();
		IS.dwVersion = BS->ReadDWORD();
		IS.fSpeed = BS->ReadFloat();

		IS.dwType = BS->ReadDWORD();
		IS.dwID2 = BS->ReadDWORD();

		if( IS.dwType == 1 || IS.dwType == 7 || IS.dwType == 12 )
		{
			IS.dDuration = BS->ReadDouble();
		} 

		if( IS.dwType == 1 || IS.dwType == 12 )
		{
			IS.dwUnk2 = BS->ReadDWORD();
			IS.dwUnk3 = BS->ReadDWORD();
		}

		memcpy(IS.pdwComps, BS->ReadGroup(sizeof(DWORD) * 8), sizeof(DWORD) * 8);

		IS.dwEffectOnCaster = BS->ReadDWORD();
		IS.dwEffectOnTarget = BS->ReadDWORD();

		memcpy(IS.pdwMisc, BS->ReadGroup(sizeof(DWORD) * 4), sizeof(DWORD) * 4);

		IS.dwSortOrder = BS->ReadDWORD();
		IS.dwTargetMask = BS->ReadDWORD();
		IS.dwUnk6 = BS->ReadDWORD();

		if( IS.dwFlags & 0x80 )
			IS.Researchable = false;
		else
			IS.Researchable = true;

		if (IS.dwDifficulty < 40) IS.iLevel = 1;
		else if (IS.dwDifficulty < 80) IS.iLevel = 2;
		else if (IS.dwDifficulty < 120) IS.iLevel = 3;
		else if (IS.dwDifficulty < 180) IS.iLevel = 4;
		else if (IS.dwDifficulty < 230) IS.iLevel = 5;
		else if (IS.dwDifficulty < 290) IS.iLevel = 6;
		else if (IS.dwDifficulty < 370) IS.iLevel = 7;
		else IS.iLevel = 8;

		m_mSpell[ IS.dwSpellID ] = IS;

/*		fprintf(out, "%i,%s,\"%04X\",\"%08X\",\"%08X\",\"%08X\",\"%08X\",\"%08X\",\"%08X\"\n",
			IS.dwSpellID, IS.szName.c_str(), IS.dwEffect, IS.dwFlags, IS.dwIcon, IS.dwTargetMask, IS.dwEffect,
			IS.dwEffectOnCaster, IS.dwEffectOnTarget);
		fflush(out);*/
	}

	//fclose(out);

	delete BS;
}

void cCharInfo::LoadSkillsTable()
{
	//Parse Spell Table
	cPortalFile *PF = m_Portal->OpenEntry(0x0E000004);
	cByteStream *BS = new cByteStream(PF->data, PF->length);

	BS->ReadBegin();
	DWORD dwID = BS->ReadDWORD();
	WORD wCount = BS->ReadWORD();
	BS->ReadWORD();

	stInternalSkill SI;
	memset( &SI, 0, sizeof( SI ) );

//	FILE *out = fopen("c:\\skills.csv","wt");

	for (int i=0; i<wCount; i++)
	{
		SI.dwID = BS->ReadDWORD();

		char *temp = BS->ReadString();
		SI.szDescription = temp;
		delete []temp;

		temp = BS->ReadString();
		SI.szName = temp;
		delete []temp;

		SI.dwIcon = BS->ReadDWORD();
		SI.dwTrainCost = BS->ReadDWORD();
		SI.dwSpecCost = BS->ReadDWORD();
		SI.dwType = BS->ReadDWORD();
		SI.dwUnk1 = BS->ReadDWORD();
		DWORD dwUse = BS->ReadDWORD();
		SI.bUsableUntrained = (dwUse == 1);
		SI.dwUnk0 = BS->ReadDWORD();
		DWORD dwAttrib1Valid = BS->ReadDWORD();
		DWORD dwAttrib2Valid = BS->ReadDWORD();
		SI.dwAttribDivisor = BS->ReadDWORD();
		SI.dwAttrib1 = (eAttributeID) BS->ReadDWORD();
		SI.dwAttrib2 = (eAttributeID) BS->ReadDWORD();
		if (!dwAttrib1Valid) SI.dwAttrib1 = eAttrNULL;
		if (!dwAttrib2Valid) SI.dwAttrib2 = eAttrNULL;
		SI.dXPTimerLimit = BS->ReadDouble();
		SI.dXPTimerStart = BS->ReadDouble();
		SI.dUnk1 = BS->ReadDouble();

		SI.dwInc = 0;
		SI.dwTrained = SI.bUsableUntrained ? eTrainUntrained : eTrainUnusable;
		SI.dwXP = 0;
		SI.dwBonus = 0;
		SI.dwBuffed = 0;

//		fprintf(out, "%i,%s\n",SI.dwID, SI.szName.c_str());

		m_mSkill[SI.dwID] = SI;

		if (SI.dwID > m_dwMaxSkill)
			m_dwMaxSkill = SI.dwID;
	}
	delete BS;
	
//	fclose(out);
}

void cCharInfo::LoadStatsTable()
{
	DWORD dwIcons[6] = {
		0x060002C8,
		0x060002C4,
		0x060002C6,
		0x060002C9,
		0x060002C5,
		0x060002C7
	};
	char szNames[6][25] = {
		"Strength",
		"Endurance",
		"Quickness",
		"Coordination",
		"Focus",
		"Self"
	};
	char szDescs[6][100] = {
		"Measures your character's muscular power.",
		"Measures how healthy your character is.",
		"Measures your character's reflexes.",
		"Measures how fast your character is.",
		"Measures your character's mind and senses.",
		"Measures your character's willpower."
	};

	stStatInfo SI;
	memset( &SI, 0, sizeof( SI ) );

	for (int i=1; i<=6; i++)
	{
		SI.dwID = i;
		SI.dwIcon = dwIcons[i-1];
		SI.szDescription = szDescs[i-1];
		SI.szName = szNames[i-1];

		m_mStat[SI.dwID] = SI;
	}
}

void cCharInfo::LoadSecStatsTable()
{
	DWORD dwIcons[3] = {
		0x060013B1,
		0x0600138B,
		0x06001380
	};
	char szNames[3][25] = {
		"Health",
		"Stamina",
		"Mana"
	};
	char szDescs[3][100] = {
		"If you run out of health, you will die!",
		"Affects your actions and movement.",
		"Affects how much magic you can cast."
	};
	eAttributeID aiAttribs[3] = {
		eAttrEndurance,
		eAttrEndurance,
		eAttrSelf
	};
	DWORD dwDivisors[3] = {
		2,
		1,
		1
	};

	stSecStatInfo SI;
	memset( &SI, 0, sizeof( SI ) );

	for (int i=2; i<=6; i+=2)
	{
		SI.dwID = i;
		SI.dwIcon = dwIcons[(i >> 1) - 1];
		SI.szDescription = szDescs[(i >> 1) - 1];
		SI.szName = szNames[(i >> 1) - 1];
		SI.dwAttrib = aiAttribs[(i >> 1) - 1];
		SI.dwAttribDivisor = dwDivisors[(i >> 1) - 1];

		m_mSecStat[SI.dwID] = SI;
	}
}

void cCharInfo::SetGUID(DWORD dwGUID)
{
	m_dwGUID = dwGUID;
}

void cCharInfo::ParseLogin(cMessage *Msg)
{
	Lock();

	//CharacterPropertyData start

	DWORD flags1 = Msg->ReadDWORD();
	DWORD unknown1 = Msg->ReadDWORD();	//always 0x0A

	if (flags1 & 0x00000001)	//dword properties
	{
		WORD countStats = Msg->ReadWORD();
		WORD unknownStats = Msg->ReadWORD();
		for (int i=0;i<countStats;i++)
		{
			DWORD key = Msg->ReadDWORD();
			DWORD value = Msg->ReadDWORD();

			UpdateStatisticDW(key, value);
		}
	}
	if (flags1 & 0x00000080)	//qword properties
	{
		WORD countQWord = Msg->ReadWORD();
		WORD unknownQWord = Msg->ReadWORD();
		for (int i=0;i<countQWord;i++)
		{
			DWORD key = Msg->ReadDWORD();
			QWORD value = Msg->ReadQWORD();

			UpdateStatisticQW(key, value);
		}
	}
	if (flags1 & 0x00000002)	//bool properties
	{
		WORD countBools = Msg->ReadWORD();
		WORD unknownBools = Msg->ReadWORD();
		for (int i=0;i<countBools;i++)
		{
			DWORD key = Msg->ReadDWORD();
			DWORD value = Msg->ReadDWORD();

			UpdateStatisticBool(key, (value == 1));
		}
	}
	if (flags1 & 0x00000004)	//double properties
	{
		WORD countDoubles = Msg->ReadWORD();
		WORD unknownDoubles = Msg->ReadWORD();
		for (int i=0;i<countDoubles;i++)
		{
			DWORD key = Msg->ReadDWORD();
			double value = Msg->ReadDouble();
		}
	}
	
	if (flags1 & 0x00000010)	//string properties
	{
		WORD countStrings = Msg->ReadWORD();
		WORD unknownStrings = Msg->ReadWORD();
		for (int i=0;i<countStrings;i++)
		{
			DWORD key = Msg->ReadDWORD();
			char * string = Msg->ReadString();

			switch (key)
			{
			case 1: m_sName = string; break;
//			case 3: m_sGender = string; break;
//			case 4: m_sRace = string; break;
			case 5: m_sClass = string; break;	//class = title
			}

			delete []string;
		}
	}
	if (flags1 & 0x00000040)	//resource prpoerties
	{
		WORD countResources = Msg->ReadWORD();
		WORD unknownResources = Msg->ReadWORD();
		for (int i=0;i<countResources;i++)
		{
			DWORD key = Msg->ReadDWORD();
			DWORD value = Msg->ReadDWORD();
		}
	}
	if (flags1 & 0x00000008)	//link properties
	{
		WORD countLinks = Msg->ReadWORD();
		WORD unknownLinks = Msg->ReadWORD();
		for (int i=0;i<countLinks;i++)
		{
			DWORD key = Msg->ReadDWORD();
			DWORD value = Msg->ReadDWORD();
		}
	}
	if (flags1 & 0x00000020)	//position properties (corpses?)
	{
		WORD countCorpse = Msg->ReadWORD();
		WORD unknownCorpse = Msg->ReadWORD();
		for (int i=0;i<countCorpse;i++)
		{
			DWORD key = Msg->ReadDWORD();

			stLocation tploc;
			memcpy(&tploc, Msg->ReadGroup(sizeof(stLocation)), sizeof(stLocation));
			m_lCorpses.push_back(tploc);
		}
	}

	//CharacterPropertyData end


	//CharacterVectorData start

	DWORD flags2 = Msg->ReadDWORD();
	DWORD unknown2 = Msg->ReadDWORD();	//always 1
	
	if (flags2 & 0x00000001)
	{
		DWORD attributeMask = Msg->ReadDWORD();	//mask it if we want but it's always 1FF (all of them...)

		for (int i=1;i<=6;i++)
		{
			m_mStat[i].dwInc = Msg->ReadDWORD();
			m_mStat[i].dwInitial = Msg->ReadDWORD();
			m_mStat[i].dwXP = Msg->ReadDWORD();
		}

		for (int i=2;i<=6;i+=2)
		{
			m_mSecStat[i].dwInc = Msg->ReadDWORD();
			m_mSecStat[i].dwUnknown = Msg->ReadDWORD();
			m_mSecStat[i].dwXP = Msg->ReadDWORD();
			m_mSecStat[i].dwCurrent = Msg->ReadDWORD();
		}
	}
	if (flags2 & 0x00000002)
	{
		WORD skillCount = Msg->ReadWORD();
		WORD skillUnknown = Msg->ReadWORD();
		for (int i=0;i<skillCount;i++)
		{
			DWORD skillID = Msg->ReadDWORD();

			if (GetSkillValid(skillID))
			{
				stInternalSkill *sk = GetSkillInfo(skillID);

				sk->dwInc = Msg->ReadWORD();	
				WORD unknown1 = Msg->ReadWORD();
				sk->dwTrained = (eTrainingType) Msg->ReadDWORD();
				if (sk->dwTrained == eTrainUntrained)
					sk->dwTrained = sk->bUsableUntrained ? eTrainUntrained : eTrainUnusable;
				sk->dwXP = Msg->ReadDWORD();
				sk->dwBonus = Msg->ReadDWORD();
				DWORD difficulty = Msg->ReadDWORD();
				double unknown2skill = Msg->ReadDouble();
			}
			else
			{
				Msg->ReadWORD();
				Msg->ReadWORD();
				Msg->ReadDWORD();
				Msg->ReadDWORD();
				Msg->ReadDWORD();
				Msg->ReadDWORD();
				Msg->ReadDouble();
				//weird unlisted skills..?
			}
		}
	}
	if (flags2 & 0x00000100)
	{
		//spellbook
		WORD spellbookCount = Msg->ReadWORD();
		WORD spellbookUnknown = Msg->ReadWORD();
		for (int i=0;i<spellbookCount;i++)
		{
			DWORD spell = Msg->ReadDWORD();
			float charge = Msg->ReadFloat();

			m_lSpellbook.insert(spell);
		}
	}
	if (flags2 & 0x00000200)
	{
		//enchantments
		DWORD enchantmentMask = Msg->ReadDWORD();
		if (enchantmentMask & 1)
		{
			//life magic
			DWORD lifeSpellCount = Msg->ReadDWORD();
			for (int i=0;i<(int)lifeSpellCount;i++)
			{
				cEnchantment *tpench = new cEnchantment();
				tpench->Unpack(Msg);
				AddEnchantment(tpench);
			}
		}
		if (enchantmentMask & 2)
		{
			//critter magic
			DWORD creatureSpellCount = Msg->ReadDWORD();
			for (int i=0;i<(int)creatureSpellCount;i++)
			{
				cEnchantment *tpench = new cEnchantment();
				tpench->Unpack(Msg);
				AddEnchantment(tpench);
			}
		}
		if (enchantmentMask & 4)
		{
			//vitae
			cEnchantment *tpench = new cEnchantment();
			tpench->Unpack(Msg);
			AddEnchantment(tpench);
		}
		if (enchantmentMask & 8)
		{
			//cooldowns
			DWORD cooldownCount = Msg->ReadDWORD();
			for (int i=0;i<(int)cooldownCount;i++)
			{
				cEnchantment *tpench = new cEnchantment();
				tpench->Unpack(Msg);
				AddEnchantment(tpench);
			}
		}
	}

	//CharacterVectorData End


	//CharacterOptionData Start

	DWORD flags3 = Msg->ReadDWORD();
	m_dwOptions = Msg->ReadDWORD();

	if (flags3 & 0x1)
	{
		DWORD shortcutCount = Msg->ReadDWORD();
		for (int i=0;i<(int)shortcutCount;i++)
		{
			DWORD position = Msg->ReadDWORD();
			m_dwShortcut[position] = Msg->ReadDWORD();
			DWORD unknown3 = Msg->ReadDWORD();
		}
	}

	//spellbars
	int spellBarCount = 8;
	//if (flags3 & 0x10)
	//	spellBarCount = 7;
	for (int i=0;i<spellBarCount;i++)
	{
		DWORD tabxCount = Msg->ReadDWORD();
		for (int h=0;h<(int)tabxCount;h++)
		{
			DWORD spell = Msg->ReadDWORD();
			m_lSpellBar[i].push_back(spell);
		}
	}

	if (flags3 & 0x8)
	{
		WORD compBuyerCount = Msg->ReadWORD();
		WORD compBuyerUnknown = Msg->ReadWORD();
		for (int i=0;i<compBuyerCount;i++)
		{
			DWORD compModel = Msg->ReadDWORD();
			DWORD compRefill = Msg->ReadDWORD();
			m_lCompBuyer.push_back(std::pair<DWORD,DWORD>(compModel, compRefill));
		}
	}
	if (flags3 & 0x20)
	{
		DWORD unk20mask3 = Msg->ReadDWORD();
	}
	if (flags3 & 0x40)
	{
		DWORD characterOptions2 = Msg->ReadDWORD();
	}
	if (flags3 & 0x00000100)
	{
		DWORD unk100_1 = Msg->ReadDWORD();
		WORD optionStringsCount = Msg->ReadWORD();
		WORD optionStringsUnknown = Msg->ReadWORD();
		for (int i=0;i<optionStringsCount;i++)
		{
			DWORD key = Msg->ReadDWORD();
			char * string = Msg->ReadString();
			delete []string;
		}
	}
	if (flags3 & 0x00000200)
	{
		DWORD unk200_1 = Msg->ReadDWORD();
		BYTE unk200_2 = Msg->ReadByte();
		BYTE optionPropertyCount = Msg->ReadByte();
		for (int i=0; i<optionPropertyCount; i++)
		{
			cPropertyType cPT;
			cPT.Unpack(Msg);
		}
		Msg->ReadAlign();
	}

	//CharacterOptionData Finish


	DWORD inventoryCount = Msg->ReadDWORD();
	for (int i=0;i<(int)inventoryCount;i++)
	{
		DWORD object = Msg->ReadDWORD();
		DWORD isContainer = Msg->ReadDWORD();
		
		//backpack tracking...
	}

	DWORD equipmentCount = Msg->ReadDWORD();
	for (int i=0;i<(int)equipmentCount;i++)
	{
		DWORD object = Msg->ReadDWORD();
		DWORD EquipMask = Msg->ReadDWORD();
		DWORD unknown3 = Msg->ReadDWORD();
		
		//equipped item tracking...
	}

	Unlock();

	RecalcStats();
	RecalcSecStats();
	RecalcSkills();
}

void cCharInfo::ParseAllegiance(cMessage *Msg)
{
	Lock();

	DWORD unk0 = Msg->ReadDWORD();
	DWORD allegSize = Msg->ReadDWORD();
	DWORD followers = Msg->ReadDWORD();
	WORD recordCount = Msg->ReadWORD();
	WORD unk1 = Msg->ReadWORD();

	Msg->ReadGroup(sizeof(DWORD)*17);
	char *allegianceName = Msg->ReadString();
	delete []allegianceName;

	Msg->ReadGroup(sizeof(DWORD)*2);
	if (recordCount == 0)
		Msg->ReadDWORD();

	//Clean up the last batch..
	if (m_aiMonarch)
	{
		delete m_aiMonarch;
		m_aiMonarch = NULL;
	}
	if (m_aiPatron)
	{
		delete m_aiPatron;
		m_aiPatron = NULL;
	}
	if (m_aiChar)
	{
		delete m_aiChar;
		m_aiChar = NULL;
	}
	for (std::vector<stAllegianceInfo *>::iterator i = m_vVassals.begin(); i != m_vVassals.end(); i++)
		delete *i;
	m_vVassals.clear();

	std::vector<stAllegianceInfo *> vRawAllegs;
	for (int i=0; i<recordCount; i++)
	{
		stAllegianceInfo *AI = new stAllegianceInfo;

		AI->dwParent = Msg->ReadDWORD();
		AI->dwGUID = Msg->ReadDWORD();
		AI->dwPendingXP = Msg->ReadDWORD();
		AI->qwXP = Msg->ReadQWORD();
		AI->bGender = Msg->ReadByte();
		AI->bRace = Msg->ReadByte();
		AI->bRank = Msg->ReadByte();
		Msg->ReadAlign();
		AI->bOnline = (Msg->ReadDWORD() > 0);
		AI->wLoyalty = Msg->ReadWORD();
		AI->wLeadership = Msg->ReadWORD();
		AI->dUnknown = Msg->ReadDouble();
		
		char *temp = Msg->ReadString();
		AI->szName = temp;
		delete []temp;

		vRawAllegs.push_back(AI);
	}

	if ( vRawAllegs.size() == 0 )
	{
		//this outta never happen
		__asm int 3;
		return;
	}

	//Now parse it out
	std::vector<stAllegianceInfo *>::iterator j = vRawAllegs.begin();

	//we now 'know' that the monarch is the first entry, always
	if ( (*j)->dwGUID != m_dwGUID ) //i'm not the monarch, so find out who is
	{
		if ( (*(j + 1))->dwGUID != m_dwGUID ) //i'm not a direct either
		{
			m_aiMonarch = *j;

			j = vRawAllegs.erase( j ); // this is my patron now
		}
		m_aiPatron = *j;

		j = vRawAllegs.erase( j );
	}
	// we've taken care of any possible patron & monarch,
	// so i'm all that's left
	m_aiChar = *j;

	j = vRawAllegs.erase( j );
	
	//Get race and gender from here now...  Only way to do it...
	if (m_aiChar->bGender == 1) m_sGender = "Male";
	if (m_aiChar->bGender == 2) m_sGender = "Female";

	if (m_aiChar->bRace == 1) m_sRace = "Aluvian";
	if (m_aiChar->bRace == 2) m_sRace = "Gharu'ndim";
	if (m_aiChar->bRace == 3) m_sRace = "Sho";

	//The remainder *should* all be vassals
	while( vRawAllegs.size() > 0 )
	{
		//Vassal
		m_vVassals.push_back(*j);

		j = vRawAllegs.erase( j );
	}
}

void cCharInfo::ParseFriendsUpdate(cMessage *Msg)
{
	DWORD friendsCount = Msg->ReadDWORD();
	for (int i=0; i<(int)friendsCount; i++)
	{
		DWORD friendID = Msg->ReadDWORD();
		bool online = Msg->ReadDWORD() == 1;
		DWORD unknown1 = Msg->ReadDWORD();
		char * name = Msg->ReadString();
		delete []name;
		DWORD outFriendsCount = Msg->ReadDWORD();
		for (int h=0; h<(int)outFriendsCount; h++)
		{
			DWORD outFriend = Msg->ReadDWORD();
		}
		DWORD inFriendsCount = Msg->ReadDWORD();
		for (int h=0; h<(int)inFriendsCount; h++)
		{
			DWORD inFriend = Msg->ReadDWORD();
		}
	}
	DWORD friendsUpdateType = Msg->ReadDWORD();
	/*	0x0000 Full friends list (at log in)
		0x0001 Friend added
		0x0002 Friend removed
		0x0004 Friend logged in or out*/
}

void cCharInfo::ParseTitleList(cMessage *Msg)
{
	DWORD unknown = Msg->ReadDWORD();
	DWORD current = Msg->ReadDWORD();
	DWORD count = Msg->ReadDWORD();
	for (int i=0; i<(int)count; i++)
	{
		DWORD title = Msg->ReadDWORD();
	}
}

void cCharInfo::ParseSquelches(cMessage *Msg)
{
	m_vSquelchedUsers.clear();

	stSquelchedUser SU;
	
	DWORD unknown0 = Msg->ReadDWORD();
	WORD squelchCount = Msg->ReadWORD();
	WORD squelchUnknown = Msg->ReadWORD();

	for (int i=0; i<squelchCount; i++)
	{
		SU.dwGUID = Msg->ReadDWORD();
		DWORD unknown1 = Msg->ReadDWORD();
		DWORD unknown2 = Msg->ReadDWORD();
		DWORD unknown3 = Msg->ReadDWORD();
		DWORD unknown4 = Msg->ReadDWORD();
		DWORD unknown5 = Msg->ReadDWORD();
		char *temp = Msg->ReadString();
		SU.szName = temp;
		delete []temp;
		DWORD unknown6 = Msg->ReadDWORD();
		m_vSquelchedUsers.push_back(SU);
	}
}

void cCharInfo::UpdateStatisticBool(DWORD Property, bool Value)
{
	//?..
}

void cCharInfo::UpdateStatisticQW(DWORD Statistic, QWORD Value)
{
	if (Statistic == 1) m_qwTotalXP = Value;
	else if (Statistic == 2) m_qwUnassignedXP = Value;
}

void cCharInfo::UpdateStatisticDW(DWORD Statistic, DWORD Value)
{
/*0x05	Total Burden
0x14	Total Pyreal
0x15	Total Experience
0x16	Unassigned Experience
0x18	Unassigned Skill Points
0x19	Level
0x1e	Rank
0x28	Movement Disabled (1 indicates movement should be ignored)
0x2B	Deaths
0x2F	Unknown2
0x62	Birth - Unix Time Seconds
0x7D	Age - Seconds
0x81	Unknown3
0x8B	Unknown4*/
	Lock();
	switch (Statistic)
	{
	case 0x05:
		m_dwTotalBurden = Value;
		break;
	case 0x14:
		m_dwTotalPyreal = Value;
		break;
/*	case 0x15:
		m_qwTotalXP = Value;
		break;
	case 0x16:
		m_qwUnassignedXP = Value;
		break;*/
	case 0x18:
		m_dwUnassignedCredits = Value;
		break;
	case 0x19:
		m_dwLevel = Value;
		break;
	case 0x1e:
		m_dwRank = Value;
		break;
	case 0x28:
		m_bMovementDisabled = (Value == 1);
		break;
	case 0x2B:
		m_dwDeaths = Value;
		break;
	case 0x2F:
		//unknown2 = Value;
		break;
	case 0x62:
		m_dwBirth = Value;
		break;
	case 0x7D:
		m_dwAge = Value;
		break;
	case 0x81:
		//unknown3 = Value;
		break;
	case 0x8B:
		//unknown4 = Value;
		break;
	};
	Unlock();
}
void cCharInfo::UpdateSkill(DWORD Skill, DWORD Inc, DWORD Trained, DWORD XP, DWORD Bonus)
{
	Lock();
	m_mSkill[Skill].dwInc = Inc;
	m_mSkill[Skill].dwTrained = (eTrainingType) Trained;
	if (m_mSkill[Skill].dwTrained == eTrainUntrained)
		m_mSkill[Skill].dwTrained = m_mSkill[Skill].bUsableUntrained ? eTrainUntrained : eTrainUnusable;
	m_mSkill[Skill].dwXP = XP;
	m_mSkill[Skill].dwBonus = Bonus;
	Unlock();
}

void cCharInfo::UpdateSkillTrain(DWORD Skill, DWORD Trained)
{
	Lock();
	m_mSkill[Skill].dwTrained = (eTrainingType) Trained;
	if (m_mSkill[Skill].dwTrained == eTrainUntrained)
		m_mSkill[Skill].dwTrained = m_mSkill[Skill].bUsableUntrained ? eTrainUntrained : eTrainUnusable;
	Unlock();
}


void cCharInfo::UpdateAttribute(DWORD Attribute, DWORD Increment, DWORD Initial, DWORD XP)
{
	Lock();
	m_mStat[Attribute].dwInc = Increment;
	m_mStat[Attribute].dwInitial = Initial;
	m_mStat[Attribute].dwXP = XP;
	Unlock();
}

void cCharInfo::UpdateSecondaryAttribute(DWORD Attribute, DWORD Increment, DWORD XP)
{
	Lock();
	m_mSecStat[Attribute].dwInc = Increment;
	m_mSecStat[Attribute].dwXP = XP;
	Unlock();
}

void cCharInfo::UpdateVital(DWORD Vital, DWORD Value)
{
	Lock();
	m_mSecStat[Vital].dwCurrent = Value;
	Unlock();
}

void cCharInfo::AddSpellToBook(DWORD Spell)
{
	Lock();
	m_lSpellbook.insert(Spell);
	Unlock();
}

void cCharInfo::RemoveSpellFromBook(DWORD Spell)
{
	Lock();
	m_lSpellbook.erase(Spell);
	Unlock();
}

void cCharInfo::AddEnchantment(cEnchantment *Enchantment)
{
	Lock();
	if (Enchantment->wSpellID == 666)
	{
		//vitae
		m_fVitae = Enchantment->fAdjustment;
	}
	m_lEnchantments.push_back(Enchantment);

	//(copied from charstats)
	Enchantment->dwExpireTime = (DWORD) time(NULL);
	// duration of the spell plus the (negative) number the server sends us indicating how many seconds have elapsed
	Enchantment->dwExpireTime += (long(Enchantment->dDuration + 0.5f) + long(Enchantment->dTimeElapsed + 0.5f));

	switch (Enchantment->dwAffectMask & 0x13)
	{
	case 0x01:	//Primary Stat
		{
			RecalcStat(Enchantment->dwAffected);
			RecalcSecStats();
			RecalcSkills();
		}
		break;
	case 0x02:	//Secondary Stat
		{
			RecalcSecStat(Enchantment->dwAffected);
		}
		break;
	case 0x10:	//Skill
		{
			RecalcSkill(Enchantment->dwAffected);
		}
		break;
	}
	Unlock();
}

void cCharInfo::RemoveEnchantment(WORD spell, WORD layer)
{
	Lock();
	for (std::list<cEnchantment *>::iterator i = m_lEnchantments.begin(); i != m_lEnchantments.end(); i++)
	{
		if (((*i)->wSpellID == spell) && ((*i)->wLayer == layer))
		{
			DWORD AffectMask = (*i)->dwAffectMask, Affected = (*i)->dwAffected;
			delete *i;
			m_lEnchantments.erase(i);
			Unlock();

			if ((AffectMask & 0x013) == 0x01)
			{
				RecalcStat(Affected);
				RecalcSecStats();
				RecalcSkills();
			}
			else if ((AffectMask & 0x013) == 0x02)
			{
				RecalcSecStats();
			}
			else if ((AffectMask & 0x013) == 0x10)
			{
				RecalcSkill(Affected);
			}
			return;
		}
	}
	Unlock();
}

void cCharInfo::RemoveAllEnchantments()
{
	Lock();
	for (std::list<cEnchantment *>::iterator i = m_lEnchantments.begin(); i != m_lEnchantments.end(); i++)
		delete *i;

	m_lEnchantments.clear();
	Unlock();

	RecalcStats();
	RecalcSecStats();
	RecalcSkills();
}

void cCharInfo::RecalcStats()
{
	for (int i=1;i<=6;i++)
		RecalcStat(i);
}

void cCharInfo::RecalcStat(int Stat)
{
	Lock();
	std::map<DWORD, float> FamilyMap;

	for (std::list< cEnchantment * >::iterator tpi = m_lEnchantments.begin(); tpi != m_lEnchantments.end(); tpi++)
	{
		if ((((*tpi)->dwAffectMask & 0x13) == 0x01) && ((*tpi)->dwAffected == Stat))
		{
			if (abs(FamilyMap[(*tpi)->dwFamily]) <= abs((*tpi)->fAdjustment))
				FamilyMap[(*tpi)->dwFamily] = (*tpi)->fAdjustment;
		}
	}

	//Calc Base
	m_mStat[Stat].dwBase = m_mStat[Stat].dwInitial + m_mStat[Stat].dwInc;

	//Calc Buffed
	float tpsf = (float) m_mStat[Stat].dwBase;

	for (std::map<DWORD, float>::iterator tpf = FamilyMap.begin(); tpf != FamilyMap.end(); tpf++)
		tpsf += (*tpf).second;

	m_mStat[Stat].dwBuffed = static_cast <DWORD> (tpsf);
	Unlock();
}

void cCharInfo::RecalcSecStats()
{
	for (int i=2;i<=6;i+=2)
		RecalcSecStat(i);
}

void cCharInfo::RecalcSecStat(int SecStat)
{
	Lock();
	std::map<DWORD, float> FamilyMap;

	for (std::list< cEnchantment * >::iterator tpi = m_lEnchantments.begin(); tpi != m_lEnchantments.end(); tpi++)
	{
		if( (((*tpi)->dwAffectMask & 0x13) == 0x02) && ((*tpi)->dwAffected == SecStat) )
		{
			if (abs(FamilyMap[(*tpi)->dwFamily]) <= abs((*tpi)->fAdjustment))
				FamilyMap[(*tpi)->dwFamily] = (*tpi)->fAdjustment;
		}
	}

	//Calc Base
	m_mSecStat[SecStat].dwBase = (DWORD) ceilf((float) m_mSecStat[SecStat].dwInc + (float) m_mStat[m_mSecStat[SecStat].dwAttrib].dwBase/(float) m_mSecStat[SecStat].dwAttribDivisor);

	//Calc Buffed
	float tpsf = (float) m_mSecStat[SecStat].dwInc;
	tpsf += (float) m_mStat[m_mSecStat[SecStat].dwAttrib].dwBuffed/(float) m_mSecStat[SecStat].dwAttribDivisor;

	for (std::map<DWORD, float>::iterator tpf = FamilyMap.begin(); tpf != FamilyMap.end(); tpf++)
		tpsf += (*tpf).second;

	m_mSecStat[SecStat].dwBuffed = (DWORD) ceilf(tpsf); //static_cast <DWORD> (tpsf);
	Unlock();
}
void cCharInfo::RecalcSkills()
{
	for (int i=0;i<MAX_SKILLS;i++)
	{
		if (GetSkillValid(i))
			RecalcSkill(i);
	}
}
void cCharInfo::RecalcSkill(int Skill)
{
	Lock();
	std::map<DWORD, float> FamilyMap;

	for (std::list<cEnchantment *>::iterator tpi = m_lEnchantments.begin(); tpi != m_lEnchantments.end(); tpi++)
	{
		if ((((*tpi)->dwAffectMask & 0x13) == 0x10) && ((*tpi)->dwAffected == Skill))
		{
			if (abs(FamilyMap[(*tpi)->dwFamily]) <= abs((*tpi)->fAdjustment))
				FamilyMap[(*tpi)->dwFamily] = (*tpi)->fAdjustment;
		}
	}

	//Calc Base
	float TPS = 0;

	if (m_mSkill[Skill].dwTrained != eTrainUnusable)
	{
		if (m_mSkill[Skill].dwAttrib1 != eAttrNULL)
			TPS += m_mStat[m_mSkill[Skill].dwAttrib1].dwBase;

		if (m_mSkill[Skill].dwAttrib2 != eAttrNULL)
			TPS += m_mStat[m_mSkill[Skill].dwAttrib2].dwBase;

		TPS /= (float) m_mSkill[Skill].dwAttribDivisor;
		TPS += m_mSkill[Skill].dwInc + m_mSkill[Skill].dwBonus/* + 0.5f*/;
	}

	m_mSkill[Skill].dwBase = (DWORD) ceilf(TPS);

	//Calc Buffed
	TPS = 0;

	if (m_mSkill[Skill].dwTrained != eTrainUnusable)
	{
		if (m_mSkill[Skill].dwAttrib1 != eAttrNULL)
			TPS += m_mStat[m_mSkill[Skill].dwAttrib1].dwBuffed;

		if (m_mSkill[Skill].dwAttrib2 != eAttrNULL)
			TPS += m_mStat[m_mSkill[Skill].dwAttrib2].dwBuffed;

		TPS /= (float) m_mSkill[Skill].dwAttribDivisor;
		TPS += m_mSkill[Skill].dwInc + m_mSkill[Skill].dwBonus/* + 0.5f*/;
	}


	for (std::map<DWORD, float>::iterator tpf = FamilyMap.begin(); tpf != FamilyMap.end(); tpf++)
		TPS += (*tpf).second;

	m_mSkill[Skill].dwBuffed = (DWORD) ceilf(TPS); //static_cast <DWORD> (TPS);
	Unlock();
}

void cCharInfo::AddCorpse(stLocation Loc)
{
	Lock();
	m_lCorpses.push_back(Loc);
	Unlock();
}

float cCharInfo::GetVitae()
{
	Lock();
	float toret = m_fVitae;
	Unlock();
	return toret;
}

std::list<DWORD> * cCharInfo::GetSpellBar(int iBar)
{
	return &m_lSpellBar[iBar];

/*	if (m_lSpellBar[iBar].begin() == m_lSpellBar[iBar].end())
		return 0;
	else
		return m_lSpellBar[iBar].begin();*/
}

int cCharInfo::GetSpellBarCount(int iBar)
{
	return (int) m_lSpellBar[iBar].size();
}

stInternalSpell * cCharInfo::GetSpellInfo(DWORD SpellID)
{
	return &m_mSpell[SpellID];
}

DWORD cCharInfo::GetMaxSkill()
{
	return m_dwMaxSkill;
}

int cCharInfo::GetSkillCount()
{
	return (int) m_mSkill.size();
}

bool cCharInfo::GetSkillValid(DWORD dwSkill)
{
	return (m_mSkill.find(dwSkill) != m_mSkill.end());
}

stInternalSkill * cCharInfo::GetSkillInfo(DWORD dwSkill)
{
	return &m_mSkill[dwSkill];
}

stStatInfo * cCharInfo::GetStat(DWORD dwStat)
{
	return &m_mStat[dwStat];
}

stSecStatInfo * cCharInfo::GetSecStat(DWORD dwSecStat)
{
	return &m_mSecStat[dwSecStat];
}

