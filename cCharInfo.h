#pragma once

#include "cThread.h"
#include "cMessage.h"

#define MAX_SKILLS	41

const DWORD PrimStatArray[] = { 0, 110,277,501,784,1125,1527,1988,2511,3097,3746,4459,5238,6084,6998,7982,9038,10167,11372,12654,14015,15459,16988,18604,20311,22113,24012,26014,28122,30341,32676,35132,37716,40434,43293,46301,49465,52795,56300,59991,63878,67975,72295,76851,81659,86737,92102,97775,103775,110128,116858,123991,131559,139591,148124,157194,166843,177113,188053,199715,212153,225429,239609,254762,270967,288306,306870,326756,348070,370928,395453,421779,450054,480434,513091,548210,585992,626654,670432,717582,768378,823122,882136,945773,1014414,1088469,1168386,1254649,1347781,1448351,1556972,1674311,1801089,1938088,2086155,2246205,2419233,2606314,2808613,3027394,3264023,3519983,3796877,4096444,4420567,4771285,5150808,5561528,6006039,6487148,7007896,7571580,8181768,8842327,9557443,10331656,11169877,12077431,13060084,14124082,15276190,16523738,17874666,19337572,20921773,22637359,24495261,26507320,28686361,31046278,33602120,36370190,39368147,42615120,46131828,49940719,54066105,58534323,63373901,68615745,74293328,80442912,87103777,94318471,102133083,110597540,119765922,129696811,140453665,152105222,164725942,178396483,193204214,209243776,226617688,245437001,265822007,287903011,311821164,337729361,365793227,396192167,429120520,464788799,503425038,545276249,590610001,639716134,692908610,750527522,812941268,880548904,953782704,1033110914,1119040753,1212121655,1312948783,1422166831,1540474151,1668627219,1807445467,1957816530,2120701915,2297143157,2488268472,2695299977,2919561502,3162487055,3425629996,3710672964,4019438644 };
const DWORD SecondaryStatArray[] = { 0, 73,183,331,517,743,1008,1312,1658,2044,2472,2943,3457,4015,4619,5268,5965,6711,7505,8352,9250,10203,11212,12279,13406,14595,15848,17169,18561,20025,21566,23187,24893,26687,28574,30559,32647,34845,37158,39594,42160,44864,47715,50722,53895,57247,60788,64531,68492,72685,77126,81834,86829,92130,97762,103748,110116,116895,124115,131812,140021,148784,158142,168143,178838,190282,202534,215659,229726,244812,260999,278375,297036,317087,338640,361819,386755,413592,442486,473604,507130,543260,582210,624211,669513,718390,771135,828069,889536,955912,1027602,1105046,1188719,1279139,1376862,1482495,1596694,1720167,1853685,1998080,2154256,2323189,2505939,2703654,2917575,3149049,3399533,3670609,3963986,4281518,4625212,4997243,5399967,5835936,6307913,6818893,7372119,7971105,8619656,9321894,10082286,10905668,11797280,12762798,13808370,14940657,16166873,17494831,18932998,20490543,22177399,24004326,25982977,28125979,30447007,32960875,35683629,38632653,41826775,45286392,49033597,53092322,57488493,62250191,67407835,72994377,79045509,85599896,92699419,100389447,108719122,117741679,127514781,138100892,149567674,161988421,175442525,190015988,205801968,222901379,241423530,261486830,283219543,306760608,332260525,359882324,389802601,422212649,457319683,495348165,536541237,581162277,629496585,681853203,738566897,800000293,866546197,938630108,1016712940,1101293965,1192914009,1292158910,1399663264,1516114484,1642257192,1778897985,1926910591,2087241457,2260915797,2449044157,2652829505,2873574933,3112691986,3371709687,3652284316,3956210003,4285430197 };

enum eTrainingType
{
	eTrainUnusable = 0,
	eTrainUntrained = 1,
	eTrainTrained = 2,
	eTrainSpecialized = 3
};

enum eAttributeID
{
   eAttrStrength = 1,
   eAttrEndurance = 2,
   eAttrQuickness = 3,
   eAttrCoordination = 4,
   eAttrFocus = 5,
   eAttrSelf = 6,
   eAttrNULL = 255
};

enum eVitalID
{
	eVitalHealth = 2,
	eVitalStamina = 4,
	eVitalMana = 6
};

struct stInternalSpell
{
	DWORD dwSpellID;
	std::string szName;
	std::string szDesc;
	DWORD dwSchool;
	DWORD dwIcon;
	DWORD dwEffect;
	DWORD dwFlags;
	DWORD dwManaCost;
	float fRangeBase;
	float fRangeModifier;
	DWORD dwDifficulty;
	float fEconomy;
	DWORD dwVersion;
	float fSpeed;
	DWORD dwType;
	DWORD dwID2;
	double dDuration;
	DWORD dwUnk2;
	DWORD dwUnk3;
	DWORD pdwComps[8];
	DWORD dwEffectOnCaster;
	DWORD dwEffectOnTarget;
	DWORD pdwMisc[4];
	DWORD dwSortOrder;
	DWORD dwTargetMask;
	DWORD dwUnk6;
	bool Researchable;
	int ComponentCount;

	int iLevel;
};

struct stInternalSkill
{
	DWORD dwID;
	std::string szDescription;
	std::string szName;
	DWORD dwIcon;
	DWORD dwTrainCost;
	DWORD dwSpecCost;
	DWORD dwType;
	DWORD dwUnk1;
	bool bUsableUntrained;
	DWORD dwUnk0;
	DWORD dwAttribDivisor;
	eAttributeID dwAttrib1;
	eAttributeID dwAttrib2;
	double dXPTimerLimit;
	double dXPTimerStart;
	double dUnk1;

	DWORD dwInc;
	eTrainingType dwTrained;
	DWORD dwXP;
	DWORD dwBonus;

	DWORD dwBase;
	DWORD dwBuffed;
};

struct stStatInfo {
	DWORD dwID;
	DWORD dwIcon;
	std::string szDescription;
	std::string szName;

	DWORD dwInc;
	DWORD dwInitial;
	DWORD dwXP;

	DWORD dwBase;
	DWORD dwBuffed;
};

struct stSecStatInfo {
	DWORD dwID;
	DWORD dwIcon;
	std::string szDescription;
	std::string szName;
	DWORD dwAttribDivisor;
	eAttributeID dwAttrib;

	DWORD dwInc;
	DWORD dwUnknown;
	DWORD dwXP;

	DWORD dwBase;
	DWORD dwCurrent;
	DWORD dwBuffed;
};

class cEnchantment {
public:
	cEnchantment()
	{
	}
	~cEnchantment()
	{
	}

	void Unpack(cByteStream * BS)
	{
		wSpellID = BS->ReadWORD();			//2
		wLayer = BS->ReadWORD();			//4
		dwFamily = BS->ReadDWORD();			//8
		dwDifficulty = BS->ReadDWORD();		//12
		dTimeElapsed = BS->ReadDouble();		//20
		dDuration = BS->ReadDouble();		//28
		dwCaster = BS->ReadDWORD();			//32
		dwUnknown1 = BS->ReadDWORD();			//36
		dwUnknown2 = BS->ReadDWORD();			//40
		dStartTime = BS->ReadDouble();		//48
		dwAffectMask = BS->ReadDWORD();			//52
		dwAffected = BS->ReadDWORD();			//56
		fAdjustment = BS->ReadFloat();		//60
		dwUnknown3 = BS->ReadDWORD();		//64
	}

	WORD wSpellID;				//2
	WORD wLayer;			//4
	DWORD dwFamily;			//8
	DWORD dwDifficulty;		//12
	double dTimeElapsed;		//20
	double dDuration;		//28
	DWORD dwCaster;			//32
	DWORD dwUnknown1;			//36
	DWORD dwUnknown2;			//40
	double dStartTime;		//48
	DWORD dwAffectMask;			//52
	DWORD dwAffected;			//56
	float fAdjustment;		//60
	DWORD dwUnknown3;		//64?

	DWORD dwExpireTime;

private:
};

struct stAllegianceInfo {
	DWORD dwParent;
	DWORD dwGUID;
	DWORD dwPendingXP;
	QWORD qwXP;
	BYTE bGender;
	BYTE bRace;
	BYTE bRank;
	bool bOnline;
	WORD wLoyalty;
	WORD wLeadership;
	double dUnknown;
	std::string szName;
};

struct stSquelchedUser {
	DWORD dwGUID;
	std::string szName;
};

class cPropertyType {
public:
	cPropertyType()
	{
	}
	~cPropertyType()
	{
	}
	void Unpack(cByteStream *Msg)
	{
		DWORD PropType = Msg->ReadDWORD();
		switch (PropType)
		{
		case 0x1000007F:
			{
				//chat window display mask
				DWORD unknown = Msg->ReadDWORD();
				QWORD ChatDisplayMask = Msg->ReadQWORD();
			}
			break;
		case 0x10000080:
			{
				DWORD unknown = Msg->ReadDWORD();
				float inactiveOpacity = Msg->ReadFloat();
			}
			break;
		case 0x10000081:
			{
				DWORD unknown = Msg->ReadDWORD();
				float activeOpacity = Msg->ReadFloat();
			}
			break;
		case 0x10000086:
			{
				//chat window position (x)
				DWORD unknown = Msg->ReadDWORD();
				DWORD value = Msg->ReadDWORD();
			}
			break;
		case 0x10000087:
			{
				//chat window position (y)
				DWORD unknown = Msg->ReadDWORD();
				DWORD value = Msg->ReadDWORD();
			}
			break;
		case 0x10000088:
			{
				//chat window size (x)
				DWORD unknown = Msg->ReadDWORD();
				DWORD value = Msg->ReadDWORD();
			}
			break;
		case 0x10000089:
			{
				//chat window size (y)
				DWORD unknown = Msg->ReadDWORD();
				DWORD value = Msg->ReadDWORD();
			}
			break;
		case 0x1000008A:
			{
				//chat window enabled
				DWORD unknown = Msg->ReadDWORD();
				BYTE value = Msg->ReadByte();
			}
			break;
		case 0x1000008B:
			{
				BYTE unknown = Msg->ReadByte();
				BYTE propCount = Msg->ReadByte();
				for (int i=0; i<propCount; i++)
				{
					cPropertyType cPT;
					cPT.Unpack(Msg);
				}
			}
			break;
		case 0x1000008C:
			{
				DWORD unknown = Msg->ReadDWORD();
				DWORD windowCount = Msg->ReadDWORD();
				for (int i=0; i<(int)windowCount; i++)
				{
					cPropertyType cPT;
					cPT.Unpack(Msg);
				}
			}
			break;
		case 0x1000008D:
			{
				DWORD unknown = Msg->ReadDWORD();
				BYTE titleSource = Msg->ReadByte();
				if (titleSource == 0)
				{
					DWORD stringID = Msg->ReadDWORD();
					DWORD fileID = Msg->ReadDWORD();
				}
				else if (titleSource == 1)
				{
					wchar_t * value = Msg->ReadWString();
					delete []value;
				}
				DWORD unknown_1b = Msg->ReadDWORD();
				DWORD unknown_1c = Msg->ReadDWORD();
			}
			break;
		}
	}
};

class cCharInfo : public cLockable {
public:
	cCharInfo();
	~cCharInfo();

	void SetGUID(DWORD dwGUID);
	void ParseLogin(cMessage *Msg);
	void ParseAllegiance(cMessage *Msg);
	void ParseSquelches(cMessage *Msg);
	void ParseFriendsUpdate(cMessage *Msg);
	void ParseTitleList(cMessage *Msg);

	void UpdateStatisticBool(DWORD Property, bool Value);
	void UpdateStatisticDW(DWORD Statistic, DWORD Value);
	void UpdateStatisticQW(DWORD Statistic, QWORD Value);
	void UpdateAttribute(DWORD Attribute, DWORD Increment, DWORD Initial, DWORD XP);
	void UpdateSecondaryAttribute(DWORD Attribute, DWORD Increment, DWORD XP);
	void UpdateVital(DWORD Vital, DWORD Value);
	void UpdateSkill(DWORD Skill, DWORD Inc, DWORD Trained, DWORD XP, DWORD Bonus);
	void UpdateSkillTrain(DWORD Skill, DWORD Trained);

	float GetVitae();

	void AddCorpse(stLocation Loc);

	void AddSpellToBook(DWORD Spell);
	void RemoveSpellFromBook(DWORD Spell);

	void AddEnchantment(cEnchantment *Enchantment);
	void RemoveEnchantment(WORD spell, WORD layer);
	void RemoveAllEnchantments();
	
	//Spelldatabase
	std::list<DWORD> * GetSpellBar(int iBar);
	int GetSpellBarCount(int iBar);
	stInternalSpell * GetSpellInfo(DWORD SpellID);

	//Skilldatabase
	DWORD GetMaxSkill();
	int GetSkillCount();
	bool GetSkillValid(DWORD dwSkill);
	stInternalSkill * GetSkillInfo(DWORD dwSkill);

	//Stats
	stStatInfo * GetStat(DWORD dwStat);
	stSecStatInfo * GetSecStat(DWORD dwSecStat);

private:
	void RecalcStats();
	void RecalcStat(int Stat);
	void RecalcSecStats();
	void RecalcSecStat(int SecStat);
	void RecalcSkills();
	void RecalcSkill(int Skill);

	void LoadSpellsTable();
	void LoadSkillsTable();
	void LoadStatsTable();
	void LoadSecStatsTable();

	DWORD	m_dwGUID,
			m_dwTotalBurden,
			m_dwTotalPyreal,
			m_dwUnassignedCredits,
			m_dwLevel,
			m_dwRank,
			m_dwDeaths,
			m_dwBirth,
			m_dwAge;

	QWORD	m_qwTotalXP,
			m_qwUnassignedXP;

	bool m_bMovementDisabled;

	std::string m_sName, m_sGender, m_sRace, m_sClass;

	std::list<stLocation> m_lCorpses;

	DWORD m_dwPatron, m_dwMonarch;

	std::unordered_set<DWORD> m_lSpellbook;

	std::list<DWORD> m_lSpellBar[7];
	std::list<std::pair<DWORD,DWORD> > m_lCompBuyer;

	DWORD m_dwOptions;
	DWORD m_dwShortcut[10];

	std::list<cEnchantment *> m_lEnchantments;
	
	std::map<DWORD, stStatInfo> m_mStat;
	std::map<DWORD, stSecStatInfo> m_mSecStat;
	std::map<DWORD, stInternalSpell> m_mSpell;
	std::map<DWORD, stInternalSkill> m_mSkill;
	DWORD m_dwMaxSkill;

	float m_fVitae;

	stAllegianceInfo *m_aiMonarch, *m_aiPatron, *m_aiChar;
	std::vector<stAllegianceInfo *> m_vVassals;

	std::vector<stSquelchedUser> m_vSquelchedUsers;

};
