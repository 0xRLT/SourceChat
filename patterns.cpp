#include "patterns.h"

namespace Patterns
{
	namespace Hardware
	{
		DEFINE_PATTERNS_1( Key_Event,
						   "5.25",
						   "81 EC ? ? ? ? A1 ? ? ? ? 33 C4 89 84 24 00 04 00 00 56 8B B4 24 0C 04 00 00 81 FE" );
	}

	namespace Client
	{
		DEFINE_PATTERNS_1( m_pSoundEngine,
						   "5.25",
						   "A1 ? ? ? ? 85 C0 0F 85 ? ? ? ? 68 ? ? ? ? E8 ? ? ? ? 8B F0" );


		DEFINE_PATTERNS_2( IN_Move,
						   "5.25",
						   "83 3D ? ? ? ? ? 75 ? 83 3D ? ? ? ? ? 74 ? FF 74 24 08",
						   "5.25 Hooked",
						   "E9 ? ? ? ? 90 90 75 ? 83 3D ? ? ? ? ? 74 ? FF 74 24 08" );

		DEFINE_PATTERNS_2( GetClientColor,
						   "5.25",
						   "8B 4C 24 04 85 C9 7E 35 6B C1 ? 0F BF 80 ? ? ? ? 48 83 F8 03",
						   "5.25 Hooked",
						   "E9 ? ? ? ? 90 7E 35 6B C1 ? 0F BF 80 ? ? ? ? 48 83 F8 03" );

		DEFINE_PATTERNS_1( CHudTextMessage__MsgFunc_TextMsg,
						   "5.25",
						   "53 55 56 57 FF 74 24 18 FF 74 24 20" );
		
		DEFINE_PATTERNS_2( CClient_SoundEngine__Play2DSound,
						   "5.25",
						   "83 EC ? F3 0F 10 44 24 14",
						   "5.25 Hooked",
						   "E9 ? ? ? ? 90 90 90 90 8D 04 24" );


		DEFINE_PATTERNS_1( GetClientVoiceMgr,
						   "5.25",
						   "E8 ? ? ? ? 8B C8 E8 ? ? ? ? 84 C0 0F 85" );

		DEFINE_PATTERNS_2( CVoiceStatus__IsPlayerBlocked,
						   "5.25",
						   "83 EC ? A1 ? ? ? ? 33 C4 89 44 24 10 56",
						   "5.25 Hooked",
						   "E9 ? ? ? ? 90 90 90 33 C4 89 44 24 10 56" );
	}
}