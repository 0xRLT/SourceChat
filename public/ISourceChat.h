#ifndef ISOURCECHAT_H
#define ISOURCECHAT_H

#ifdef _WIN32
#pragma once
#endif

#include <platform.h>

//-----------------------------------------------------------------------------
// Purpose: Source chat control interface
//-----------------------------------------------------------------------------

abstract_class ISourceChat
{
public:
	virtual ~ISourceChat() {}

	virtual bool IsOpened( void ) = 0;
	virtual bool IsTeamChat( void ) = 0;
	virtual void Clear( void ) = 0;
};

extern ISourceChat *g_pSourceChat;

#define SOURCE_CHAT_INTERFACE_VERSION "SourceChat001"

#endif