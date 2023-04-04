#ifndef SOURCECHAT_H
#define SOURCECHAT_H
#endif

#ifdef _WIN32
#pragma once
#endif

#define IMGUI_USE_SDL ( 1 )
#define IMGUI_USE_GL3 ( 1 )

#define CHAT_HISTORY_BUFFER_SIZE ( 8192 )
#define CHAT_INPUT_BUFFER_SIZE ( 128 )

#include <messagebuffer.h>
#include <base_feature.h>
#include <IDetoursAPI.h>

#include <imgui.h>

#include "public/ISourceChat.h"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------

FUNC_SIGNATURE( float *, __cdecl, GetClientColorFn, int client );
FUNC_SIGNATURE( void, __thiscall, CClient_SoundEngine__Play2DSoundFn, void *thisptr, const char *sound, float volume );
FUNC_SIGNATURE( void *, __cdecl, GetClientVoiceMgrFn );
FUNC_SIGNATURE( bool, __thiscall, CVoiceStatus__IsPlayerBlockedFn, void *thisptr, int index );

#if IMGUI_USE_SDL
struct SDL_Window;
typedef union SDL_Event SDL_Event;
#endif

//-----------------------------------------------------------------------------
// Source chat
//-----------------------------------------------------------------------------

class CSourceChat : public CBaseFeature, ISourceChat
{
public:
	CSourceChat();

	// ISourceChat interface
	virtual bool IsOpened( void ) override;
	virtual bool IsTeamChat( void ) override;
	virtual void Clear( void ) override;

	// CBaseFeature abstract class
	virtual bool Load( void ) override;
	virtual void PostLoad( void ) override;

	virtual void Unload( void ) override;

public:
	// Callbacks
	void OnEnterToServer();
	void OnDisconnect();

	void Draw( void );

	void FadeThink( void );

	void PrintMessage( int client, const char *pszMessage, int src );

	void SendMessageFromChat( void );
	char *PushMessageToBuffer( char *buffer, int maxsize, const char *msg, int *shift_quantity );

private:
	void AddTextColor( const char *pszTextPos, float *pflColor );
	void ApplyShiftQuantityToTextColor( int shiftQuantity );
	void RemoveInvalidTextColor( void );
	
	void AddTextOpacity( const char *pszTextPos, float flTime );
	void ApplyShiftQuantityToTextOpacity( int shiftQuantity );
	void RemoveInvalidTextOpacity( void );

	void DrawKeyCodes( void );
	void DrawTextHistory( void );
	void DrawInputLineName( void );
	void DrawInputLine( void );
	void DrawKeyboardLayout( const char *pszLayoutName );

public:
#if IMGUI_USE_SDL
	void InitImGui( SDL_Window *window );
#else
	void InitImGui( HDC hdc );
#endif
	void InitStyle( void );
	void InitFont( void );

	void OnOpen( bool bTeamChat );
	void OnClose( void );

#if !IMGUI_USE_SDL
	inline WNDPROC GetGameWindowProc( void ) const { return m_hGameWndProc; };
#endif

	inline bool IsOpened_Internal( void ) const { return m_bOpened; };
	inline bool IsTeamChat_Internal( void ) const { return m_bTeamChat; };

	inline void SetOpened_Internal( bool bOpened ) { m_bOpened = bOpened; };
	inline void SetTeamChat_Internal( bool bTeamChat ) { m_bTeamChat = bTeamChat; };

private:
#if !IMGUI_USE_SDL
	HWND m_hGameWnd;
	WNDPROC m_hGameWndProc;
#endif

	float m_flWindowWidth;
	float m_flWindowHeight;

	char m_szInputBuffer[ CHAT_INPUT_BUFFER_SIZE ];
	char m_szHistoryBuffer[ CHAT_HISTORY_BUFFER_SIZE ];

	bool m_bOpened;
	bool m_bWasOpenedRightNow;
	bool m_bTeamChat;
	bool m_bCalcTextHistoryHeight;

	float m_flOpenTime;
	float m_flCloseTime;
	float m_flCurrentTime;

	float m_flTextHistoryHeight;
	float m_flTextHistoryDefaultColor[3];

	ImVector<ImTextColorStyle> m_ColorfulTextStyle;
	ImVector<ImTextOpacity> m_TextOpacity;

	ImFont *m_pFont;
	ImFont *m_pFontBitmap;
	ImFont *m_pFontSmall;

	cvar_t *hud_draw;

	GetClientColorFn m_pfnGetClientColor;
	CClient_SoundEngine__Play2DSoundFn m_pfnCClient_SoundEngine__Play2DSound;
	GetClientVoiceMgrFn m_pfnGetClientVoiceMgr;
	CVoiceStatus__IsPlayerBlockedFn m_pfnCVoiceStatus__IsPlayerBlocked;

	void **m_pSoundEngine;

	// Detour members
	void *m_pfnKey_Event;
	void *m_pfnIN_Move;
#if IMGUI_USE_SDL
	void *m_pfnSDL_PollEvent;
	void *m_pfnSDL_GL_SwapWindow;
#else
	void *m_pfnwglSwapBuffers;
#endif
	void *m_pfnSetCursorPos;

	DetourHandle_t m_hKey_Event;
	DetourHandle_t m_hIN_Move;
	DetourHandle_t m_hMessageMode;
	DetourHandle_t m_hMessageMode2;
	DetourHandle_t m_hUserMsgHook_SayText;
	DetourHandle_t m_hUserMsgHook_TextMsg;
	DetourHandle_t m_hNetMsgHook_TempEntity;
#if IMGUI_USE_SDL
	DetourHandle_t m_hSDL_PollEvent;
	DetourHandle_t m_hSDL_GL_SwapWindow;
#else
	DetourHandle_t m_hwglSwapBuffers;
#endif
	DetourHandle_t m_hSetCursorPos;
};

extern CSourceChat g_SourceChat;