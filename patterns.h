#ifndef PATTERNS_H
#define PATTERNS_H

#ifdef _WIN32
#pragma once
#endif

#include <memutils/patterns.h>

namespace Patterns
{
	namespace Hardware
	{
		EXTERN_PATTERNS( Key_Event );
	}

	namespace Client
	{
		EXTERN_PATTERNS( m_pSoundEngine );

		EXTERN_PATTERNS( IN_Move );
		EXTERN_PATTERNS( GetClientColor );
		EXTERN_PATTERNS( CHudTextMessage__MsgFunc_TextMsg );
		EXTERN_PATTERNS( CClient_SoundEngine__Play2DSound );

		EXTERN_PATTERNS( GetClientVoiceMgr );
		EXTERN_PATTERNS( CVoiceStatus__IsPlayerBlocked );
	}
}

#endif // PATTERNS_H