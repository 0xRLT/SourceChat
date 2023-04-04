#include <SDL.h>
#include <string>

#include <dbg.h>
#include <convar.h>
#include <keydefs.h>
#include <ISvenModAPI.h>
#include <IMemoryUtils.h>

#include <hl_sdk/common/protocol.h>

#include "sourcechat.h"
#include "patterns.h"
#include "chat_scheme.h"
#include "keyboard_layout_map.h"

#if IMGUI_USE_GL3
#include <backends/imgui_impl_opengl3.h>
#else
#include <backends/imgui_impl_opengl2.h>
#endif

#if IMGUI_USE_SDL
#include <backends/imgui_impl_sdl2.h>
#else
#include <backends/imgui_impl_win32.h>
#endif

#include <misc/freetype/imgui_freetype.h>

// ImGui's WndProc / SDL events handler
#if IMGUI_USE_SDL
extern bool ImGui_ImplSDL2_ProcessEvent( const SDL_Event *event );
#else
extern LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
#endif

//-----------------------------------------------------------------------------
// Declare hooks
//-----------------------------------------------------------------------------

#if IMGUI_USE_SDL
DECLARE_HOOK( int, __cdecl, SDL_PollEvent, SDL_Event * );
DECLARE_HOOK( int, __cdecl, SDL_GL_SwapWindow, SDL_Window * );
#else
DECLARE_HOOK( BOOL, APIENTRY, wglSwapBuffers, HDC );
#endif

DECLARE_HOOK( BOOL, WINAPI, SetCursorPos, int, int );

DECLARE_HOOK( void, __cdecl, Key_Event, int, int );
DECLARE_HOOK( void, __cdecl, IN_Move, float, usercmd_t * );

CommandCallbackFn ORIG_messagemode = NULL;
CommandCallbackFn ORIG_messagemode2 = NULL;

UserMsgHookFn ORIG_UserMsgHook_SayText = NULL;
UserMsgHookFn ORIG_UserMsgHook_TextMsg = NULL;

NetMsgHookFn ORIG_NetMsgHook_TempEntity = NULL;

//-----------------------------------------------------------------------------
// Singleton
//-----------------------------------------------------------------------------

CSourceChat g_SourceChat;

//-----------------------------------------------------------------------------
// ConVars & ConCommands
//-----------------------------------------------------------------------------

ConVar sourcechat( "sourcechat", "1", FCVAR_CLIENTDLL, "Enable Source-like chat" );
ConVar sourcechat_width_fraction( "sourcechat_width_fraction", "0.0115", FCVAR_CLIENTDLL, "Screen's fraction of width" );
ConVar sourcechat_height_fraction( "sourcechat_height_fraction", "0.5732", FCVAR_CLIENTDLL, "Screen's fraction of height" );
ConVar sourcechat_fadein_duration( "sourcechat_fadein_duration", "0.3", FCVAR_CLIENTDLL, "Fade-in duration of chatbox" );
ConVar sourcechat_fadeout_duration( "sourcechat_fadeout_duration", "0.3", FCVAR_CLIENTDLL, "Fade-out duration of chatbox" );
ConVar sourcechat_text_stay_time( "sourcechat_text_stay_time", "10.0", FCVAR_CLIENTDLL, "Stay time of recently message" );
ConVar sourcechat_text_fade_duration( "sourcechat_text_fade_duration", "2.0", FCVAR_CLIENTDLL, "Fade-out duration of recently message" );
ConVar sourcechat_monsterinfo_width_fraction( "sourcechat_monsterinfo_width_fraction", "1.0", FCVAR_CLIENTDLL, "Text message's width fraction of monster info" );
ConVar sourcechat_monsterinfo_height_fraction( "sourcechat_monsterinfo_height_fraction", "0.8", FCVAR_CLIENTDLL, "Text message's height fraction of monster info" );

CON_COMMAND( sourcechat_clear, "Clear Source-like chat" )
{
	g_SourceChat.Clear();
}

//-----------------------------------------------------------------------------
// Purpose: process inputs from keyboard / mouse / joystick ...
//-----------------------------------------------------------------------------

#if IMGUI_USE_SDL
DECLARE_FUNC( int, __cdecl, HOOKED_SDL_PollEvent, SDL_Event *event )
#else
DECLARE_FUNC( LRESULT, CALLBACK, HOOKED_WndProc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
#endif
{
	if ( !sourcechat.GetBool() )
	{
		g_SourceChat.SetOpened_Internal( false );

	#if IMGUI_USE_SDL
		return ORIG_SDL_PollEvent( event );
	#else
		return CallWindowProc( g_SourceChat.GetGameWindowProc(), hWnd, uMsg, wParam, lParam );
	#endif
	}

	/*
#if IMGUI_USE_SDL
	if ( g_SourceChat.IsOpened_Internal() && event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_ESCAPE )
#else
	if ( g_SourceChat.IsOpened_Internal() && uMsg == WM_KEYDOWN && wParam == VK_ESCAPE )
#endif
	{
		g_SourceChat.OnClose();
		return 0;
	}
	*/

	if ( g_SourceChat.IsOpened_Internal() )
	{
	#if IMGUI_USE_SDL
		ImGui_ImplSDL2_ProcessEvent( event );
	#else
		ImGui_ImplWin32_WndProcHandler( hWnd, uMsg, wParam, lParam );
	#endif

		return 0;
	}

#if IMGUI_USE_SDL
	return ORIG_SDL_PollEvent( event );
#else
	return CallWindowProc( g_SourceChat.GetGameWindowProc(), hWnd, uMsg, wParam, lParam );
#endif
}

//-----------------------------------------------------------------------------
// Purpose: draw chat on every swap buffers
//-----------------------------------------------------------------------------

#if IMGUI_USE_SDL
DECLARE_FUNC( int, __cdecl, HOOKED_SDL_GL_SwapWindow, SDL_Window *window )
#else
DECLARE_FUNC( BOOL, APIENTRY, HOOKED_wglSwapBuffers, HDC hdc )
#endif
{
	static bool s_bImGuiInitialized = false;

	if ( !s_bImGuiInitialized )
	{
	#if IMGUI_USE_SDL
		g_SourceChat.InitImGui( window );
	#else
		g_SourceChat.InitImGui( hdc );
	#endif

		s_bImGuiInitialized = true;
	}

#if IMGUI_USE_GL3
	ImGui_ImplOpenGL3_NewFrame();
#else
	ImGui_ImplOpenGL2_NewFrame();
#endif

#if IMGUI_USE_SDL
	ImGui_ImplSDL2_NewFrame();
#else
	ImGui_ImplWin32_NewFrame();
#endif

	ImGui::NewFrame();

	g_SourceChat.Draw();

	ImGui::Render();

#if IMGUI_USE_GL3
	ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
#else
	ImGui_ImplOpenGL2_RenderDrawData( ImGui::GetDrawData() );
#endif

#if IMGUI_USE_SDL
	return ORIG_SDL_GL_SwapWindow( window );
#else
	return ORIG_wglSwapBuffers( hdc );
#endif
}

//-----------------------------------------------------------------------------
// Purpose: forgot why did we hook it :D
//-----------------------------------------------------------------------------

DECLARE_FUNC( BOOL, WINAPI, HOOKED_SetCursorPos, int X, int Y )
{
	if ( g_SourceChat.IsOpened_Internal() )
		return FALSE;

	return ORIG_SetCursorPos( X, Y );
}

//-----------------------------------------------------------------------------
// Purpose: stop key inputs when chat is open
//-----------------------------------------------------------------------------

DECLARE_FUNC( void, __cdecl, HOOKED_Key_Event, int key, int down )
{
	if ( g_SourceChat.IsOpened_Internal() )
	{
		if ( down )
		{
			if ( key == K_ESCAPE )
				g_SourceChat.OnClose();

			return;
		}
	}

	ORIG_Key_Event( key, down );
}

//-----------------------------------------------------------------------------
// Purpose: prevent mouse movements on game's camera
//-----------------------------------------------------------------------------

DECLARE_FUNC( void, __cdecl, HOOKED_IN_Move, float frametime, usercmd_t *cmd )
{
	if ( g_SourceChat.IsOpened_Internal() )
		return;

	ORIG_IN_Move( frametime, cmd );
}

//-----------------------------------------------------------------------------
// Purpose: intercept open of original game's chat
//-----------------------------------------------------------------------------

DECLARE_FUNC( void, __cdecl, HOOKED_messagemode )
{
	if ( !sourcechat.GetBool() )
		return ORIG_messagemode();

	g_SourceChat.OnOpen( false );
}

//-----------------------------------------------------------------------------
// Purpose: intercept open of original game's team chat
//-----------------------------------------------------------------------------

DECLARE_FUNC( void, __cdecl, HOOKED_messagemode2 )
{
	if ( !sourcechat.GetBool() )
		return ORIG_messagemode2();

	g_SourceChat.OnOpen( true );
}

//-----------------------------------------------------------------------------
// Purpose: intercept incoming chat messages
//-----------------------------------------------------------------------------

DECLARE_FUNC( int, __cdecl, UserMsgHook_SayText, const char *pszUserMsg, int iSize, void *pBuffer )
{
	if ( !sourcechat.GetBool() )
		return ORIG_UserMsgHook_SayText( pszUserMsg, iSize, pBuffer );
	
	CMessageBuffer message( pBuffer, iSize, true );

	int src;
	int client = message.ReadByte();
	const char *pszMessage = message.ReadString();

	if ( *pszMessage > 0 && *pszMessage <= 3 )
	{
		src = *pszMessage;
		pszMessage = pszMessage + 1;
	}
	else
	{
		src = 0;
		client = 0;
	}

	if ( pszMessage[ 0 ] != '\0' )
		g_SourceChat.PrintMessage( client, pszMessage, src );
	
	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: intercept incoming chat messages
//-----------------------------------------------------------------------------

DECLARE_FUNC( int, __cdecl, UserMsgHook_TextMsg, const char *pszUserMsg, int iSize, void *pBuffer )
{
	if ( !sourcechat.GetBool() )
		return ORIG_UserMsgHook_TextMsg( pszUserMsg, iSize, pBuffer );

	CMessageBuffer message( pBuffer, iSize, true );

	if ( message.ReadByte() == HUD_PRINTTALK )
	{
		static char buffer[ 256 ];

		const char *str;
		std::vector<std::string> formattingStrings;

		std::string msg = message.ReadString();

		// #1
		str = message.ReadString();

		if ( *str != '\0' )
			formattingStrings.push_back( str );
		
		// #2
		str = message.ReadString();

		if ( *str != '\0' )
			formattingStrings.push_back( str );
		
		// #3
		str = message.ReadString();

		if ( *str != '\0' )
			formattingStrings.push_back( str );
		
		// #4
		str = message.ReadString();

		if ( *str != '\0' )
			formattingStrings.push_back( str );

		switch ( formattingStrings.size() )
		{
		case 0:
			strcpy_s( buffer, M_ARRAYSIZE( buffer ), msg.c_str() );
			break;

		case 1:
			snprintf( buffer, M_ARRAYSIZE( buffer ), msg.c_str(), formattingStrings[ 0 ].c_str() );
			break;

		case 2:
			snprintf( buffer, M_ARRAYSIZE( buffer ), msg.c_str(), formattingStrings[ 0 ].c_str(), formattingStrings[ 1 ].c_str() );
			break;

		case 3:
			snprintf( buffer, M_ARRAYSIZE( buffer ), msg.c_str(), formattingStrings[ 0 ].c_str(), formattingStrings[ 1 ].c_str(), formattingStrings[ 2 ].c_str() );
			break;

		case 4:
			snprintf( buffer, M_ARRAYSIZE( buffer ), msg.c_str(), formattingStrings[ 0 ].c_str(), formattingStrings[ 1 ].c_str(), formattingStrings[ 2 ].c_str(), formattingStrings[ 3 ].c_str() );
			break;
		}

		if ( buffer[ 0 ] != '\0' )
			g_SourceChat.PrintMessage( -1, buffer, 0 );
		
		return 0;
	}

	return ORIG_UserMsgHook_TextMsg( pszUserMsg, iSize, pBuffer );
}

//-----------------------------------------------------------------------------
// Purpose: adjust text message's position of monster info
//-----------------------------------------------------------------------------

void NetMsgHook_TempEntity( void )
{
	auto FixedSigned16 = []( float value, float scale ) -> short
	{
		int output;

		output = value * scale;

		if ( output > 32767 )
			output = 32767;

		if ( output < -32768 )
			output = -32768;

		return (short)output;
	};

	CNetMessageParams *params = Utils()->GetNetMessageParams();
	CMessageBuffer message( params->buffer, params->readcount, params->badread );

	int entitytype = message.ReadByte();

	if ( entitytype != TE_TEXTMESSAGE )
	{
		ORIG_NetMsgHook_TempEntity();
		return;
	}

	int coords_offset = message.GetReadCount();
	sizebuf_t *buffer = const_cast<sizebuf_t *>( message.GetBuffer() );

	int channel = message.ReadByte();

	float x = message.ReadShort() * ( 1.f / ( 1 << 13 ) );
	float y = message.ReadShort() * ( 1.f / ( 1 << 13 ) );

	const float eps = 0.00001f;
	if ( !( 0.569946f - eps <= y && y <= 0.569946f + eps ) ) // monster info
	{
		ORIG_NetMsgHook_TempEntity();
		return;
	}

	x *= sourcechat_monsterinfo_width_fraction.GetFloat();
	y *= sourcechat_monsterinfo_height_fraction.GetFloat();

	// set x
	coords_offset += 1; // skip entity type, channel
	*(short *)( buffer->data + coords_offset ) = FixedSigned16( x, 1 << 13 );
	
	// set y
	coords_offset += 2; // skip x
	*(short *)( buffer->data + coords_offset ) = FixedSigned16( y, 1 << 13 );

	ORIG_NetMsgHook_TempEntity();
}

//-----------------------------------------------------------------------------
// CSourceChat methods
//-----------------------------------------------------------------------------

CSourceChat::CSourceChat()
{
#if !IMGUI_USE_SDL
	m_hGameWnd = NULL;
	m_hGameWndProc = NULL;
#endif

	m_flWindowWidth = 0.f;
	m_flWindowHeight = 0.f;

	m_szInputBuffer[ 0 ] = '\0';
	m_szHistoryBuffer[ 0 ] = '\0';

	m_bOpened = false;
	m_bWasOpenedRightNow = false;
	m_bTeamChat = false;
	m_bCalcTextHistoryHeight = false;

	m_flOpenTime = -1.f;
	m_flCloseTime = -1.f;
	m_flCurrentTime = -1.f;

	m_flTextHistoryHeight = 0.f;

	m_flTextHistoryDefaultColor[ 0 ] = 1.f;
	m_flTextHistoryDefaultColor[ 1 ] = 1.f;
	m_flTextHistoryDefaultColor[ 2 ] = 1.f;

	m_pFont = NULL;
	m_pFontBitmap = NULL;
	m_pFontSmall = NULL;

	hud_draw = NULL;

	m_pfnGetClientColor = NULL;
	m_pfnCClient_SoundEngine__Play2DSound = NULL;
	m_pfnGetClientVoiceMgr = NULL;
	m_pfnCVoiceStatus__IsPlayerBlocked = NULL;

	m_pSoundEngine = NULL;

	// Detour members
	m_pfnKey_Event = NULL;
	m_pfnIN_Move = NULL;
#if IMGUI_USE_SDL
	m_pfnSDL_PollEvent = NULL;
	m_pfnSDL_GL_SwapWindow = NULL;
#else
	m_pfnwglSwapBuffers = NULL;
#endif
	m_pfnSetCursorPos = NULL;

	m_hKey_Event = DETOUR_INVALID_HANDLE;
	m_hIN_Move = DETOUR_INVALID_HANDLE;
	m_hMessageMode = DETOUR_INVALID_HANDLE;
	m_hMessageMode2 = DETOUR_INVALID_HANDLE;
	m_hUserMsgHook_SayText = DETOUR_INVALID_HANDLE;
	m_hUserMsgHook_TextMsg = DETOUR_INVALID_HANDLE;
	m_hNetMsgHook_TempEntity = DETOUR_INVALID_HANDLE;
#if IMGUI_USE_SDL
	m_hSDL_PollEvent = DETOUR_INVALID_HANDLE;
	m_hSDL_GL_SwapWindow = DETOUR_INVALID_HANDLE;
#else
	m_hwglSwapBuffers = DETOUR_INVALID_HANDLE;
#endif
	m_hSetCursorPos = DETOUR_INVALID_HANDLE;
}

//-----------------------------------------------------------------------------
// Purpose: reset/change chat state when entering server
//-----------------------------------------------------------------------------

void CSourceChat::OnEnterToServer()
{
	if ( m_bOpened )
	{
		//OnClose();
		m_bWasOpenedRightNow = true;
	}

	m_TextOpacity.clear();

	m_flOpenTime = -1.f;
	m_flCloseTime = -1.f;
}

//-----------------------------------------------------------------------------
// Purpose: reset/change chat state when disconnecting
//-----------------------------------------------------------------------------

void CSourceChat::OnDisconnect()
{
	if ( m_bOpened )
	{
		OnClose();
	}

	m_TextOpacity.clear();

	m_flOpenTime = -1.f;
	m_flCloseTime = -1.f;
}

//-----------------------------------------------------------------------------
// Purpose: draw chat
//-----------------------------------------------------------------------------

void CSourceChat::Draw( void )
{
	m_flCurrentTime = g_pEngineFuncs->GetClientTime();

	ImGui::GetIO().MouseDrawCursor = m_bOpened;
	ImGui::DisableCursorBlinking = !m_bOpened;

	FadeThink();

	if ( sourcechat.GetBool() && // Source chat enabled
		 SvenModAPI()->GetClientState() == CLS_ACTIVE && // We're currently playing
		 !VGameUI()->GameUI()->IsGameUIActive() && // Menu is not active
		 *(unsigned long *)&( hud_draw->value ) != 0 ) // HUD is enabled
	{
		constexpr int window_flags = ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoBringToFrontOnFocus;

		ImVec4 *colors = ImGui::GetStyle().Colors;
		const float flChatPosX = floorf( m_flWindowWidth * sourcechat_width_fraction.GetFloat() );
		const float flChatPosY = floorf( m_flWindowHeight * sourcechat_height_fraction.GetFloat() );

		// Colors of main and child windows
		colors[ ImGuiCol_WindowBg ] = ImColor( ColorSchemeActive.WindowBackground );
		colors[ ImGuiCol_Border ] = ImColor( ColorSchemeActive.WindowBorder );
		colors[ ImGuiCol_ChildBg ] = ImColor( ColorSchemeActive.ChildBackground );

		// Use our font as default
		ImGui::PushFont( m_pFont );

		// Chatbox window
		ImGui::SetNextWindowPos( ImVec2( flChatPosX, flChatPosY ) );
		ImGui::SetNextWindowSize( ImVec2( ChatSchemeActive.ChatSizeX, ChatSchemeActive.ChatSizeY ) );

		ImGui::Begin( "chatbox", 0, window_flags );
		{
			// Display key codes as strings
			DrawKeyCodes();

			// Enable text shadow
			ImGui::TextShadow = true;

			// Draw chatbox's rich text
			DrawTextHistory();

			// Draw name of input line
			DrawInputLineName();

			// Draw input line
			DrawInputLine();

			// Disable text shadow
			ImGui::TextShadow = false;
		}
		ImGui::End();

		ImGui::PopFont();

		m_bWasOpenedRightNow = false;
	}
}

//-----------------------------------------------------------------------------
// Purpose: show pressed Ctrl / Alt
//-----------------------------------------------------------------------------

void CSourceChat::DrawKeyCodes( void )
{
	ImGui::SetCursorPosX( ChatSchemeActive.KeyCodesPosX );
	ImGui::SetCursorPosY( ChatSchemeActive.KeyCodesPosY );

	ImGui::PushFont( m_pFontSmall );
	ImGui::PushStyleColor( ImGuiCol_Text, ColorSchemeActive.KeyCodesText );

	bool bCtrlPressed = GetAsyncKeyState( VK_RCONTROL ) || GetAsyncKeyState( VK_LCONTROL );
	bool bAltPressed = GetAsyncKeyState( VK_LMENU ) || GetAsyncKeyState( VK_RMENU );

	if ( bCtrlPressed && bAltPressed )
		ImGui::Text( "[CTRL+ALT]" );
	else if ( bCtrlPressed )
		ImGui::Text( "[CTRL]" );
	else if ( bAltPressed )
		ImGui::Text( "[ALT]" );

	ImGui::PopStyleColor();
	ImGui::PopFont();
}

//-----------------------------------------------------------------------------
// Purpose: draw chatbox and its text
//-----------------------------------------------------------------------------

void CSourceChat::DrawTextHistory( void )
{
	ImGui::SetCursorPosX( ChatSchemeActive.TextHistoryPosX );
	ImGui::SetCursorPosY( ChatSchemeActive.TextHistoryPosY );

	ImGui::PushStyleColor( ImGuiCol_ScrollbarBg, ColorSchemeActive.ScrollbarBg );
	ImGui::PushStyleColor( ImGuiCol_ScrollbarGrab, ColorSchemeActive.ScrollbarGrab );
	ImGui::PushStyleColor( ImGuiCol_ScrollbarGrabHovered, ColorSchemeActive.ScrollbarGrabHovered ); 
	ImGui::PushStyleColor( ImGuiCol_ScrollbarGrabActive, ColorSchemeActive.ScrollbarGrabActive );

	ImGui::BeginChild( "text-history" , ImVec2( ChatSchemeActive.TextHistorySizeX, ChatSchemeActive.TextHistorySizeY ), false );

	ImGui::PopStyleColor( 4 );

	// Main body
	ImGui::SetCursorPosX( ImGui::GetCursorPosX() + ChatSchemeActive.RichTextPosX );
	ImGui::SetCursorPosY( ImGui::GetCursorPosY() + ChatSchemeActive.RichTextPosY );

	ImGui::PushStyleColor( ImGuiCol_Text, ColorSchemeActive.Text ); // Text
	ImGui::PushStyleColor( ImGuiCol_TextSelectedBg, ColorSchemeActive.SelectedText ); // Selected Text Background color
	ImGui::PushStyleColor( ImGuiCol_ChildBg, IM_COL32( 0, 0, 0, 0 ) ); // Background (should be invisible if we keep BeginChild)
	
	if ( m_TextOpacity.size() > 0 )
		ImGui::TextDontIgnoreColorAbsence = true;

	ImGui::TextLineSpacing = ChatSchemeActive.RichTextLineSpacing;
	ImGui::ColorfulTextStyle = &m_ColorfulTextStyle;
	ImGui::TextOpacity = &m_TextOpacity;

	// Calculate new height of text history
	if ( m_bCalcTextHistoryHeight )
	{
		m_flTextHistoryHeight = ImGui::CalcMultilineWordWrapTextHeight( m_szHistoryBuffer,
																		strlen( m_szHistoryBuffer ),
																		ChatSchemeActive.RichTextWidth ) + ChatSchemeActive.RichTextExtraSpace; // actual height + extra space

		m_flTextHistoryHeight = max( m_flTextHistoryHeight, ChatSchemeActive.TextHistorySizeY - 1.f );
		m_bCalcTextHistoryHeight = false;
	}

	// Text history
	ImGui::InputTextMultiline( "##chatbox",
							   m_szHistoryBuffer,
							   strlen( m_szHistoryBuffer ) + 1,
							   ImVec2( ChatSchemeActive.RichTextWidth, m_flTextHistoryHeight ),
							   ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_WordWrapping | ImGuiInputTextFlags_NoVerticalScroll, NULL, NULL, false );

	ImGui::TextOpacity = NULL;
	ImGui::ColorfulTextStyle = NULL;
	ImGui::TextLineSpacing = 0.f;
	ImGui::TextDontIgnoreColorAbsence = false;

	ImGui::PopStyleColor( 3 );

	// Scroll to end when we open chat or it's closed
	if ( m_bWasOpenedRightNow || !m_bOpened )
		ImGui::SetScrollY( ImGui::GetScrollMaxY() );

	ImGui::EndChild();
}

//-----------------------------------------------------------------------------
// Purpose: draw name of input line (team chat or not)
//-----------------------------------------------------------------------------

void CSourceChat::DrawInputLineName( void )
{
	ImGui::SetCursorPosX( ChatSchemeActive.InputLineNamePosX );
	ImGui::SetCursorPosY( ChatSchemeActive.InputLineNamePosY );

	ImGui::PushStyleVar( ImGuiStyleVar_TextUnformattedAlign, ImVec2( ChatSchemeActive.InputLineNameTextAlignX, ChatSchemeActive.InputLineNameTextAlignY ) ); // Unformatted text align
	ImGui::PushStyleColor( ImGuiCol_Text, ColorSchemeActive.InputLineNameText ); // Text

	if ( m_bTeamChat )
	{
		ImGui::BeginChild( "##say", ImVec2( ChatSchemeActive.InputLineNameTeamSizeX, ChatSchemeActive.InputLineNameTeamSizeY ) );
		ImGui::SetCursorPosX( ChatSchemeActive.InputLineNameTextPosX );
		ImGui::SetCursorPosY( ChatSchemeActive.InputLineNameTextPosY );
		ImGui::TextUnformatted( "Say (Team):" );
		ImGui::EndChild();
	}
	else
	{
		ImGui::BeginChild( "##sayteam", ImVec2( ChatSchemeActive.InputLineNameSizeX, ChatSchemeActive.InputLineNameSizeY ) );
		ImGui::SetCursorPosX( ChatSchemeActive.InputLineNameTextPosX );
		ImGui::SetCursorPosY( ChatSchemeActive.InputLineNameTextPosY );
		ImGui::TextUnformatted( "Say:" );
		ImGui::EndChild();
	}

	ImGui::PopStyleColor( 2 );
	ImGui::PopStyleVar();
}

//-----------------------------------------------------------------------------
// Purpose: draw input line
//-----------------------------------------------------------------------------

void CSourceChat::DrawInputLine( void )
{
	const char *pszLayoutName = KeyboardLayoutMap.GetCurrentLayoutName();

	//if ( pszLayoutName != NULL && ( !strcmp( pszLayoutName, "US" ) || !strcmp( pszLayoutName, "UK" ) ) )
	//	pszLayoutName = NULL;

	if ( m_bTeamChat )
	{
		ImGui::SetCursorPosX( pszLayoutName != NULL ? ChatSchemeActive.InputLineKeyboardLayoutTeamPosX : ChatSchemeActive.InputLineTeamPosX );
		ImGui::PushItemWidth( pszLayoutName != NULL ? ChatSchemeActive.InputLineKeyboardLayoutTeamWidth : ChatSchemeActive.InputLineTeamWidth );
	}
	else
	{
		ImGui::SetCursorPosX( pszLayoutName != NULL ? ChatSchemeActive.InputLineKeyboardLayoutPosX : ChatSchemeActive.InputLinePosX );
		ImGui::PushItemWidth( pszLayoutName != NULL ? ChatSchemeActive.InputLineKeyboardLayoutWidth : ChatSchemeActive.InputLineWidth );
	}

	ImGui::SetCursorPosY( ChatSchemeActive.InputLinePosY );

	ImGui::PushStyleColor( ImGuiCol_Text, ColorSchemeActive.InputLineText ); // Text
	ImGui::PushStyleColor( ImGuiCol_TextSelectedBg, ColorSchemeActive.SelectedText ); // Selected Text Background color
	ImGui::PushStyleColor( ImGuiCol_FrameBg, ColorSchemeActive.ChildBackground ); // Background of input line

	ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( ChatSchemeActive.InputLineFramePaddingX, ChatSchemeActive.InputLineFramePaddingY ) );

	// Focus on input line when we open chat
	if ( m_bWasOpenedRightNow || !m_bOpened )
		ImGui::SetKeyboardFocusHere();

	ImGui::CursorColor = ColorSchemeActive.Cursor;
	ImGui::FrameBackgroundOffset = ImVec2( ChatSchemeActive.InputLineFrameBgOffsetX, ChatSchemeActive.InputLineFrameBgOffsetY );

	if ( ImGui::InputText( "##input-line", m_szInputBuffer, CHAT_INPUT_BUFFER_SIZE, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll, NULL, NULL, true ) )
	{
		// Have some symbols to send
		if ( m_szInputBuffer[ 0 ] != '\0' )
		{
			SendMessageFromChat();
		}

		// Now close the chat
		OnClose();
		ImGui::GetIO().ClearInputKeys(); // prevent pogo stick
	}
	else if ( !m_bOpened && m_szInputBuffer[ 0 ] != '\0' )
	{
		// Clear the buffer if chat is inactive
		ImGui::ClearInputText( "##input-line" );
		m_szInputBuffer[ 0 ] = '\0';
	}

	// Show keyboard's current layout (not US / UK)
	if ( pszLayoutName != NULL )
	{
		// Draw dummy background
		if ( m_bTeamChat )
			ImGui::SetCursorPosX( ChatSchemeActive.KeyboardLayoutBackgroundTeamPosX );
		else
			ImGui::SetCursorPosX( ChatSchemeActive.KeyboardLayoutBackgroundPosX );

		ImGui::SetCursorPosY( ChatSchemeActive.KeyboardLayoutBackgroundPosY );

		ImGui::BeginChild( "##kb-layout-bg", ImVec2( ChatSchemeActive.KeyboardLayoutBackgroundSizeX, ChatSchemeActive.KeyboardLayoutBackgroundSizeY ) );
		ImGui::EndChild();

		// Now draw main part
		DrawKeyboardLayout( pszLayoutName );
	}

	ImGui::FrameBackgroundOffset = ImVec2( 0.f, 0.f );
	ImGui::CursorColor = IM_COL32_WHITE;

	ImGui::PopStyleVar();
	ImGui::PopStyleColor( 3 );

	ImGui::PopItemWidth();
}

//-----------------------------------------------------------------------------
// Purpose: draw current keyboard layout
//-----------------------------------------------------------------------------

void CSourceChat::DrawKeyboardLayout( const char *pszLayoutName )
{
	// Disable text shadow
	ImGui::TextShadow = false;

	ImGui::PushFont( m_pFontBitmap );

	if ( m_bTeamChat )
		ImGui::SetCursorPosX( ChatSchemeActive.KeyboardLayoutTeamPosX );
	else
		ImGui::SetCursorPosX( ChatSchemeActive.KeyboardLayoutPosX );

	ImGui::SetCursorPosY( ChatSchemeActive.KeyboardLayoutPosY );

	ImGui::PushStyleVar( ImGuiStyleVar_TextUnformattedAlign, ImVec2( ChatSchemeActive.KeyboardLayoutTextAlignX, ChatSchemeActive.KeyboardLayoutTextAlignY ) ); // Unformatted text align
	ImGui::PushStyleColor( ImGuiCol_Text, ColorSchemeActive.KeyboardLayoutText ); // Text
	ImGui::PushStyleColor( ImGuiCol_ChildBg, ColorSchemeActive.KeyboardLayoutBackground ); // Background color

	ImVec2 text_size = ImGui::CalcTextSize( pszLayoutName );

	ImGui::BeginChild( "##kb-layout", ImVec2( ChatSchemeActive.KeyboardLayoutSizeX, ChatSchemeActive.KeyboardLayoutSizeY ) );
	ImGui::SetCursorPosX( (float)( ( (int)ChatSchemeActive.KeyboardLayoutSizeX / 2 ) - (int)text_size.x / 2 )); // center alignment
	ImGui::SetCursorPosY( 0.f );
	ImGui::TextUnformatted( pszLayoutName );
	ImGui::EndChild();

	ImGui::PopStyleColor( 2 );
	ImGui::PopStyleVar();

	ImGui::PopFont();

	// Enable text shadow
	ImGui::TextShadow = true;
}

//-----------------------------------------------------------------------------
// Purpose: print incoming message from server
//-----------------------------------------------------------------------------

void CSourceChat::PrintMessage( int client, const char *pszMessage, int src )
{
	// Check for muted player
	if ( client > 0 )
	{
		const char *pszLevelName = g_pEngineFuncs->GetLevelName();
		cl_entity_t *pLocal = g_pEngineFuncs->GetLocalPlayer();

		if ( pszLevelName && *pszLevelName && pLocal && pLocal->index != client )
		{
			if ( m_pfnCVoiceStatus__IsPlayerBlocked( m_pfnGetClientVoiceMgr(), client ) )
			{
				return;
			}
		}
	}

	int shiftQuantity;
	const char *pszMessagePos;
	float *pflClientColor = m_pfnGetClientColor( client );

	pszMessagePos = PushMessageToBuffer( m_szHistoryBuffer, CHAT_HISTORY_BUFFER_SIZE - 1, pszMessage, &shiftQuantity );

	if ( shiftQuantity > 0 )
	{
		ApplyShiftQuantityToTextColor( shiftQuantity );
		RemoveInvalidTextColor();

		ApplyShiftQuantityToTextOpacity( shiftQuantity );
		RemoveInvalidTextOpacity();
	}

	AddTextOpacity( pszMessagePos, g_pEngineFuncs->GetClientTime() );

	switch ( src )
	{
	case 0: // just message from the Server
	{
		ConColorMsg( Color( m_flTextHistoryDefaultColor[ 0 ], m_flTextHistoryDefaultColor[ 1 ], m_flTextHistoryDefaultColor[ 2 ], 1.f ), pszMessage );
		AddTextColor( pszMessagePos, m_flTextHistoryDefaultColor );

		break;
	}

	case 1: // message from the Server's Console
	{
		const char *pszMessageSender = "<Server Console>";

		ConColorMsg( Color( pflClientColor[ 0 ], pflClientColor[ 1 ], pflClientColor[ 2 ], 1.f ), pszMessageSender );
		ConColorMsg( Color( m_flTextHistoryDefaultColor[ 0 ], m_flTextHistoryDefaultColor[ 1 ], m_flTextHistoryDefaultColor[ 2 ], 1.f ), pszMessage + strlen( pszMessageSender ) );

		AddTextColor( pszMessagePos, pflClientColor );
		AddTextColor( pszMessagePos + strlen( pszMessageSender ) + 1, m_flTextHistoryDefaultColor );

		break;
	}

	case 2: // message from the Player
	{
		auto CStringStartsWith = [](const char *str, const char *prefix) -> bool
		{
			return strncmp( prefix, str, strlen( prefix ) ) == 0;
		};

		player_info_t *pPlayerInfo = g_pEngineStudio->PlayerInfo( client - 1 );

		if ( pPlayerInfo != NULL )
		{
			std::string sMessageSender = pPlayerInfo->name;
			sMessageSender += ":";

			if ( CStringStartsWith( pszMessage, sMessageSender.c_str() ) )
			{
				sMessageSender[ sMessageSender.length() - 1 ] = '\0';

				ConColorMsg( Color( pflClientColor[ 0 ], pflClientColor[ 1 ], pflClientColor[ 2 ], 1.f ), sMessageSender.c_str() );
				ConColorMsg( Color( m_flTextHistoryDefaultColor[ 0 ], m_flTextHistoryDefaultColor[ 1 ], m_flTextHistoryDefaultColor[ 2 ], 1.f ), pszMessage + sMessageSender.length() - 1 );

				sMessageSender[ sMessageSender.length() - 1 ] = ':';

				AddTextColor( pszMessagePos, pflClientColor );
				AddTextColor( pszMessagePos + sMessageSender.length(), m_flTextHistoryDefaultColor );
			}
			else
			{
				// fuck this retarded TEAM
				char *msgpos = const_cast<char *>( pszMessagePos );
				msgpos[ 2 ] = 'e';
				msgpos[ 3 ] = 'a';
				msgpos[ 4 ] = 'm';

				sMessageSender = "(Team) ";
				sMessageSender += pPlayerInfo->name;
				sMessageSender += ":";

				sMessageSender[ sMessageSender.length() - 1 ] = '\0';

				ConColorMsg( Color( pflClientColor[ 0 ], pflClientColor[ 1 ], pflClientColor[ 2 ], 1.f ), sMessageSender.c_str() );
				ConColorMsg( Color( m_flTextHistoryDefaultColor[ 0 ], m_flTextHistoryDefaultColor[ 1 ], m_flTextHistoryDefaultColor[ 2 ], 1.f ), pszMessage + sMessageSender.length() - 1 );

				sMessageSender[ sMessageSender.length() - 1 ] = ':';

				AddTextColor( pszMessagePos, pflClientColor );
				AddTextColor( pszMessagePos + sMessageSender.length(), m_flTextHistoryDefaultColor );
			}
		}

		break;
	}

	case 3: // action message from the Player
	{
		std::string sMessageSender = "* ";
		player_info_t *pPlayerInfo = g_pEngineStudio->PlayerInfo( client - 1 );

		if ( pPlayerInfo != NULL )
		{
			sMessageSender += pPlayerInfo->name;

			ConColorMsg( Color( pflClientColor[ 0 ], pflClientColor[ 1 ], pflClientColor[ 2 ], 1.f ), sMessageSender.c_str() );
			ConColorMsg( Color( m_flTextHistoryDefaultColor[ 0 ], m_flTextHistoryDefaultColor[ 1 ], m_flTextHistoryDefaultColor[ 2 ], 1.f ), pszMessage + sMessageSender.length() );

			AddTextColor( pszMessagePos, pflClientColor );
			AddTextColor( pszMessagePos + sMessageSender.length() + 1, m_flTextHistoryDefaultColor );
		}

		break;
	}
	}

	// Play sound notification
	if ( *m_pSoundEngine != NULL )
	{
		m_pfnCClient_SoundEngine__Play2DSound( *m_pSoundEngine, "misc/talk.wav", 1.f );
	}

	m_bCalcTextHistoryHeight = true;
}

//-----------------------------------------------------------------------------
// Purpose: send message to server
//-----------------------------------------------------------------------------

void CSourceChat::SendMessageFromChat( void )
{
	static char cmdBuffer[ CHAT_INPUT_BUFFER_SIZE * 2 ];

	snprintf( cmdBuffer, M_ARRAYSIZE( cmdBuffer ), m_bTeamChat ? "say_team \"%s\"" : "say \"%s\"", m_szInputBuffer );
	g_pEngineFuncs->ClientCmd( cmdBuffer );
}

//-----------------------------------------------------------------------------
// Purpose: pushes back to buffer given message
//-----------------------------------------------------------------------------

char *CSourceChat::PushMessageToBuffer( char *buffer, int maxsize, const char *msg, int *shift_quantity )
{
	char *msg_pos = NULL;

	int cursize = strlen( buffer );
	int msgsize = strlen( msg );

	int leftSpace = maxsize - cursize;

	if ( cursize == maxsize ) // buffer is filled
	{
		//DevMsg( "cursize == maxsize\n" );

		int msgsize_tmp = msgsize;
		const char *msg_tmp = msg;

		// msg is too long, truncate when copy
		if ( msgsize > maxsize )
		{
			msgsize_tmp -= msgsize - maxsize;
			msg_tmp += msgsize - maxsize;
		}

		char *shift_src = (char *)buffer + msgsize_tmp;
		char *shift_dst = (char *)buffer;

		int shift_size = strlen( (char *)buffer + msgsize_tmp );

		char *add_dst = (char *)buffer + shift_size;

		memcpy( (char *)shift_dst, shift_src, shift_size );
		memcpy( (char *)add_dst, (char *)msg_tmp, msgsize_tmp );

		*shift_quantity = msgsize_tmp;
		msg_pos = add_dst;
	}
	else if ( leftSpace >= msgsize ) // we can just copy msg to buffer with some shift
	{
		//DevMsg( "leftSpace >= msgsize\n" );

		memcpy( buffer + cursize, msg, msgsize );

		*shift_quantity = 0;
		msg_pos = buffer + cursize;

		buffer[ cursize + msgsize ] = '\0';
	}
	else /* if ( leftSpace < msgsize ) */
	{
		//DevMsg( "leftSpace < msgsize\n" );

		if ( msgsize <= maxsize )
		{
			char *shift_src = (char *)buffer + ( cursize - ( maxsize - msgsize ) );
			char *shift_dst = (char *)buffer;

			int shift_size = strlen( shift_src );

			memcpy( (char *)shift_dst, shift_src, shift_size );
			memcpy( buffer + ( maxsize - msgsize ), (char *)msg, msgsize );

			*shift_quantity = cursize - ( maxsize - msgsize );
			msg_pos = buffer + ( maxsize - msgsize );
		}
		else // message larger than buffer, truncate
		{
			//DevMsg( "msgsize > maxsize\n" );

			memcpy( buffer, (char *)msg + ( msgsize - maxsize ), maxsize );

			*shift_quantity = maxsize;
			msg_pos = buffer;
		}

		buffer[ maxsize ] = '\0';
	}
	/*
	else if ( msgsize == maxsize ) // message size matches buffer size
	{
		//DevMsg("msgsize == maxsize\n");

		memcpy( (char *)buffer, (char *)msg, msgsize );
		buffer[maxsize] = '\0';
	}
	*/

	return msg_pos;
}

//-----------------------------------------------------------------------------
// Purpose: add text style color
//-----------------------------------------------------------------------------

void CSourceChat::AddTextColor( const char *pszTextPos, float *pflColor )
{
	m_ColorfulTextStyle.push_back( { pszTextPos, pflColor } );
}

//-----------------------------------------------------------------------------
// Purpose: apply shift quantity to text style color from CSourceChat::PushMessageToBuffer
//-----------------------------------------------------------------------------

void CSourceChat::ApplyShiftQuantityToTextColor( int shiftQuantity )
{
	for ( int i = 0; i < m_ColorfulTextStyle.size(); i++ )
	{
		m_ColorfulTextStyle[ i ].pos -= shiftQuantity;
	}
}

//-----------------------------------------------------------------------------
// Purpose: remove out of bounds text style colors
//-----------------------------------------------------------------------------

void CSourceChat::RemoveInvalidTextColor( void )
{
	int firstTextColor = -1;
	int lastInvalidTextColor = -1;

	for ( int i = 0; i < m_ColorfulTextStyle.size(); i++ )
	{
		if ( m_szHistoryBuffer >= m_ColorfulTextStyle[ i ].pos )
		{
			firstTextColor = i;
			break;
		}

		lastInvalidTextColor = i;
	}

	if ( lastInvalidTextColor != -1 && firstTextColor != -1 )
	{
		if ( m_ColorfulTextStyle[ firstTextColor ].pos > m_szHistoryBuffer ) // last "invalid' text color still captures some text that belong to it
		{
			m_ColorfulTextStyle[ lastInvalidTextColor ].pos = m_szInputBuffer; // adjust pos
			lastInvalidTextColor -= 1; // the actual last invalid text color
		}

		if ( lastInvalidTextColor == -1 )
			return;

		// remove invalid text colors
		if ( lastInvalidTextColor != 0 )
		{
			m_ColorfulTextStyle.erase( &m_ColorfulTextStyle[ 0 ], &m_ColorfulTextStyle[ lastInvalidTextColor ] );
		}
		else
		{
			m_ColorfulTextStyle.erase( &m_ColorfulTextStyle[ 0 ] );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: add text opacity
//-----------------------------------------------------------------------------

void CSourceChat::AddTextOpacity( const char *pszTextPos, float flTime )
{
	m_TextOpacity.push_back( { pszTextPos, flTime, 0 } );
}

//-----------------------------------------------------------------------------
// Purpose: apply shift quantity to text opacity from CSourceChat::PushMessageToBuffer
//-----------------------------------------------------------------------------

void CSourceChat::ApplyShiftQuantityToTextOpacity( int shiftQuantity )
{
	for ( int i = 0; i < m_TextOpacity.size(); i++ )
	{
		m_TextOpacity[ i ].pos -= shiftQuantity;
	}
}

//-----------------------------------------------------------------------------
// Purpose: remove out of bounds text opacity
//-----------------------------------------------------------------------------

void CSourceChat::RemoveInvalidTextOpacity( void )
{
	int firstTextOpacity = -1;
	int lastInvalidTextOpacity = -1;

	for ( int i = 0; i < m_TextOpacity.size(); i++ )
	{
		if ( m_szHistoryBuffer >= m_TextOpacity[ i ].pos )
		{
			firstTextOpacity = i;
			break;
		}

		lastInvalidTextOpacity = i;
	}

	if ( lastInvalidTextOpacity != -1 && firstTextOpacity != -1 )
	{
		if ( m_TextOpacity[ firstTextOpacity ].pos > m_szHistoryBuffer ) // last "invalid' text color still captures some text that belong to it
		{
			m_TextOpacity[ lastInvalidTextOpacity ].pos = m_szInputBuffer; // adjust pos
			lastInvalidTextOpacity -= 1; // the actual last invalid text color
		}

		if ( lastInvalidTextOpacity == -1 )
			return;

		// remove invalid text colors
		if ( lastInvalidTextOpacity != 0 )
		{
			m_TextOpacity.erase( &m_TextOpacity[ 0 ], &m_TextOpacity[ lastInvalidTextOpacity ] );
		}
		else
		{
			m_ColorfulTextStyle.erase( &m_ColorfulTextStyle[ 0 ] );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: calculate chatbox's alpha depending on fade duration
//-----------------------------------------------------------------------------

void CSourceChat::FadeThink( void )
{
	//constexpr auto TextOffset = offsetof( CChatColorScheme, Text ) / sizeof( ImU32 );

	float flFadeInDuration = sourcechat_fadein_duration.GetFloat();
	float flFadeOutDuration = sourcechat_fadeout_duration.GetFloat();

	float flFadeInTime = m_flOpenTime + flFadeInDuration;
	float flFadeOutTime = m_flCloseTime + flFadeOutDuration;

	ImU32 *colors_active = (ImU32 *)&ColorSchemeActive;
	ImU32 *colors_scheme = (ImU32 *)&ColorScheme;

	//ImU32 *TextOffsetPtr = colors_active + TextOffset;

	bool bFadeIn = ( flFadeInDuration > 0.f && m_flOpenTime != -1.f && flFadeInTime >= m_flCurrentTime );
	bool bFadeOut = ( flFadeOutDuration > 0.f && m_flCloseTime != -1.f && flFadeOutTime >= m_flCurrentTime );

	if ( m_bOpened )
	{
		if ( bFadeIn )
		{
			int alphaInt;
			float desiredAlpha, realAlpha;
			float fadeOutFraction, fraction = 1.f - ( ( flFadeInTime - m_flCurrentTime ) / flFadeInDuration );

			for ( int i = 0; i < sizeof( CChatColorScheme ) / sizeof( ImU32 ); i++ )
			{
				// Skip CChatColorScheme::Text
				//if ( colors_active + i == TextOffsetPtr )
				//	continue;

				// Still closing... use alpha value from the close event
				if ( flFadeOutTime > m_flOpenTime )
				{
					fadeOutFraction = ( flFadeOutTime - m_flCurrentTime ) / flFadeOutDuration;

					realAlpha = float( ( colors_scheme[ i ] >> IM_COL32_A_SHIFT ) & 0xFF );
					desiredAlpha = realAlpha * fadeOutFraction;

					alphaInt = int( desiredAlpha + ( realAlpha - desiredAlpha ) * fraction ) & 0xFF;
				}
				else
				{
					desiredAlpha = float( ( colors_scheme[ i ] >> IM_COL32_A_SHIFT ) & 0xFF );
					alphaInt = int( desiredAlpha * fraction ) & 0xFF;
				}

				colors_active[ i ] = ( ( alphaInt << IM_COL32_A_SHIFT ) | ( colors_scheme[ i ] & 0x00FFFFFF ) );
			}
		}
		else
		{
			for ( int i = 0; i < sizeof( CChatColorScheme ) / sizeof( ImU32 ); i++ )
			{
				// Skip CChatColorScheme::Text
				//if ( colors_active + i == TextOffsetPtr )
				//	continue;

				colors_active[ i ] = ( ( colors_scheme[ i ] & IM_COL32_A_MASK ) | ( colors_scheme[ i ] & 0x00FFFFFF ) );
			}
		}
	}
	else
	{
		if ( bFadeOut )
		{
			int alphaInt;
			float desiredAlpha, realAlpha;
			float fadeInFraction, fraction = ( flFadeOutTime - m_flCurrentTime ) / flFadeOutDuration;

			for ( int i = 0; i < sizeof( CChatColorScheme ) / sizeof( ImU32 ); i++ )
			{
				// Skip CChatColorScheme::Text
				//if ( colors_active + i == TextOffsetPtr )
				//	continue;

				// Still opening... use alpha value from the open event
				if ( flFadeInTime > m_flCloseTime )
				{
					fadeInFraction = 1.f - ( ( flFadeInTime - m_flCurrentTime ) / flFadeInDuration );

					realAlpha = float( ( colors_scheme[ i ] >> IM_COL32_A_SHIFT ) & 0xFF );
					desiredAlpha = realAlpha * fadeInFraction;
				}
				else
				{
					desiredAlpha = float( ( colors_scheme[ i ] >> IM_COL32_A_SHIFT ) & 0xFF );
				}

				alphaInt = int( desiredAlpha * fraction ) & 0xFF;

				colors_active[ i ] = ( ( alphaInt << IM_COL32_A_SHIFT ) | ( colors_scheme[ i ] & 0x00FFFFFF ) );
			}
		}
		else
		{
			for ( int i = 0; i < sizeof( CChatColorScheme ) / sizeof( ImU32 ); i++ )
			{
				// Skip CChatColorScheme::Text
				//if ( colors_active + i == TextOffsetPtr )
				//	continue;

				colors_active[ i ] &= 0x00FFFFFF;
			}
		}
	}

	// Calc alpha for each text line
	// * ImTextOpacity::userdata is a print time

	float fraction;

	int textAlphaInt = ( ColorScheme.Text >> IM_COL32_A_SHIFT ) & 0xFF;
	int textRealAlphaInt = ( ColorSchemeActive.Text >> IM_COL32_A_SHIFT ) & 0xFF;
	float textAlpha = float( textAlphaInt );
	float textRealAlpha = float( textRealAlphaInt );

	float flStayTime = sourcechat_text_stay_time.GetFloat();
	float flFadeDuration = sourcechat_text_fade_duration.GetFloat();

	float flActualStayTime = flStayTime + flFadeDuration;

	for ( int i = 0; i < m_TextOpacity.size(); i++ )
	{
		ImTextOpacity *pTextOpacity = &m_TextOpacity[ i ];

		if ( pTextOpacity->userdata + flActualStayTime < m_flCurrentTime )
		{
			m_TextOpacity.erase( pTextOpacity );
			i--;
		}
	}

	if ( m_bOpened )
	{
		for ( int i = 0; i < m_TextOpacity.size(); i++ )
		{
			ImTextOpacity &textOpacity = m_TextOpacity[ i ];

			//if ( bFadeIn )
			//{
			//	if ( textOpacity.userdata + flActualStayTime <= flFadeInTime )
			//	{
			//		textOpacity.alpha = textRealAlphaInt;
			//	}
			//	else if ( m_flCurrentTime > textOpacity.userdata + flStayTime )
			//	{
			//		float startAlpha = textAlpha * ( ( m_flOpenTime - ( textOpacity.userdata + flStayTime ) ) / flFadeDuration );

			//		fraction = 1.f - ( ( flFadeInTime - m_flCurrentTime ) / flFadeInDuration );

			//		textOpacity.alpha = int( startAlpha + ( textAlpha - startAlpha ) * fraction );
			//	}
			//	else
			//	{
			//		textOpacity.alpha = textAlphaInt;
			//	}
			//}
			//else
			//{
				textOpacity.alpha = textAlphaInt;
			//}
		}
	}
	else
	{
		for ( int i = 0; i < m_TextOpacity.size(); i++ )
		{
			ImTextOpacity &textOpacity = m_TextOpacity[ i ];

			if ( textOpacity.userdata + flStayTime < m_flCurrentTime )
			{
				if ( bFadeOut )
				{
					if ( textOpacity.userdata + flActualStayTime <= flFadeOutTime )
					{
						textOpacity.alpha = textRealAlphaInt;
					}
					else if ( m_flCurrentTime > textOpacity.userdata + flStayTime )
					{
						if ( flFadeInTime > m_flCloseTime )
						{
							float actualAlpha = textAlpha * ( ( ( flFadeInTime - m_flCurrentTime ) / flFadeInDuration ) );
							float targetAlpha = textAlpha * ( 1.f - ( ( flFadeOutTime - ( textOpacity.userdata + flStayTime ) ) / flFadeDuration ) );

							fraction = ( flFadeOutTime - m_flCurrentTime ) / flFadeOutDuration;

							textOpacity.alpha = int( targetAlpha + ( actualAlpha - targetAlpha ) * fraction );
						}
						else
						{
							float targetAlpha = textAlpha * ( 1.f - ( ( flFadeOutTime - ( textOpacity.userdata + flStayTime ) ) / flFadeDuration ) );

							fraction = ( flFadeOutTime - m_flCurrentTime ) / flFadeOutDuration;

							textOpacity.alpha = int( targetAlpha + ( textAlpha - targetAlpha ) * fraction );
						}
					}
					else
					{
						textOpacity.alpha = textAlphaInt;
					}
				}
				else
				{
					fraction = ( ( textOpacity.userdata + flActualStayTime ) - m_flCurrentTime ) / flFadeDuration;

					textOpacity.alpha = int( textAlpha * fraction );
				}
			}
			else
			{
				textOpacity.alpha = textAlphaInt;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: initialize ImGui
//-----------------------------------------------------------------------------

#if IMGUI_USE_SDL
void CSourceChat::InitImGui( SDL_Window *window )
#else
void CSourceChat::InitImGui( HDC hdc )
#endif
{
#if !IMGUI_USE_SDL
	m_hGameWnd = WindowFromDC( hdc );
	m_hGameWndProc = (WNDPROC)SetWindowLong( m_hGameWnd, GWL_WNDPROC, (LONG)HOOKED_WndProc );
#endif

	ImGui::CreateContext();

#if IMGUI_USE_SDL
	ImGui_ImplSDL2_InitForOpenGL( window, NULL );
#else
	ImGui_ImplWin32_Init( m_hGameWnd );
#endif

#if IMGUI_USE_GL3
	ImGui_ImplOpenGL3_Init();
#else
	ImGui_ImplOpenGL2_Init();
#endif

	ImGui::StyleColorsDark();

	ImGuiIO &io = ImGui::GetIO();
	io.IniFilename = NULL;
	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

	InitFont();
	InitStyle();
}

//-----------------------------------------------------------------------------
// Purpose: initialize ImGui's style
//-----------------------------------------------------------------------------

void CSourceChat::InitStyle( void )
{
	ImGuiStyle *style = &ImGui::GetStyle();
	ImVec4 *colors = style->Colors;

	style->WindowRounding = 5;
	style->ChildRounding = 0;
	style->ScrollbarRounding = 0;
	style->ScrollbarSize = 24.5f;
	style->LogSliderDeadzone = 4;
}

//-----------------------------------------------------------------------------
// Purpose: initialize fonts for chat
//-----------------------------------------------------------------------------

void CSourceChat::InitFont( void )
{
	const ImWchar ranges[] =
	{
		0x0020, 0xFFFF,
		0
	};

	ImFontConfig cfg;
	ImGuiIO &io = ImGui::GetIO();

	// Load font Tahoma
	m_pFontBitmap = io.Fonts->AddFontFromFileTTF( "svencoop/resource/sourcechat/tahoma.ttf", ChatSchemeActive.FontBitmapSize, &cfg, io.Fonts->GetGlyphRangesDefault() );

	if ( m_pFontBitmap == NULL )
	{
		Sys_Error( "Unable to load font \"../svencoop/resource/sourcechat/tahoma.ttf\"" );
		return;
	}

	// Load small font Tahoma
	m_pFontSmall = io.Fonts->AddFontFromFileTTF( "svencoop/resource/sourcechat/tahoma.ttf", ChatSchemeActive.FontSmallSize, &cfg, ranges ); // TODO: custom font?

	if ( m_pFontSmall == NULL )
	{
		Sys_Error( "Unable to load font \"../svencoop/resource/sourcechat/tahoma.ttf\"" );
		return;
	}

	// Use freetype for anti-aliasing font
	cfg.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_MonoHinting | ImGuiFreeTypeBuilderFlags_Monochrome;

	// Load default font Tahoma Bold
	m_pFont = io.Fonts->AddFontFromFileTTF( "svencoop/resource/sourcechat/tahomabd.ttf", ChatSchemeActive.FontSize, &cfg, ranges );

	if ( m_pFont == NULL )
	{
		Sys_Error( "Unable to load font \"../svencoop/resource/sourcechat/tahomabd.ttf\"" );
		return;
	}

	// Merge m_pFont with font Arial Unicode MS
	cfg.MergeMode = true;

	if ( io.Fonts->AddFontFromFileTTF( "svencoop/resource/sourcechat/ARIALUNI.ttf", ChatSchemeActive.FontUnicodeSize, &cfg, ranges ) == NULL )
	{
		Sys_Error( "Unable to load font \"../svencoop/resource/sourcechat/ARIALUNI.ttf\"" );
		return;
	}

	io.Fonts->Build();
}

//-----------------------------------------------------------------------------
// Purpose: open callback
//-----------------------------------------------------------------------------

void CSourceChat::OnOpen( bool bTeamChat )
{
	m_bOpened = true;
	m_bTeamChat = bTeamChat;

	m_bWasOpenedRightNow = true;

	m_flOpenTime = g_pEngineFuncs->GetClientTime();
}

//-----------------------------------------------------------------------------
// Purpose: close callback
//-----------------------------------------------------------------------------

void CSourceChat::OnClose( void )
{
	m_bOpened = false;
	m_szInputBuffer[ 0 ] = '\0'; // clear buffer

	g_pClientFuncs->IN_ClearStates();
	SetCursorPos( int( m_flWindowWidth ) / 2, int( m_flWindowHeight ) / 2 );

	m_flCloseTime = g_pEngineFuncs->GetClientTime();
}

//-----------------------------------------------------------------------------
// Feature methods
//-----------------------------------------------------------------------------

bool CSourceChat::Load( void )
{
	int patternIndex;
	bool ScanOK = true;

	void *pfnSoundEngine;

	DEFINE_PATTERNS_FUTURE( fKey_Event );
	DEFINE_PATTERNS_FUTURE( fm_pSoundEngine );
	DEFINE_PATTERNS_FUTURE( fIN_Move );
	DEFINE_PATTERNS_FUTURE( fGetClientColor );
	DEFINE_PATTERNS_FUTURE( fCClient_SoundEngine__Play2DSound );
	DEFINE_PATTERNS_FUTURE( fGetClientVoiceMgr );
	DEFINE_PATTERNS_FUTURE( fCVoiceStatus__IsPlayerBlocked );

	// Find signatures
	MemoryUtils()->FindPatternAsync( SvenModAPI()->Modules()->Hardware, Patterns::Hardware::Key_Event, fKey_Event );
	MemoryUtils()->FindPatternAsync( SvenModAPI()->Modules()->Client, Patterns::Client::m_pSoundEngine, fm_pSoundEngine );
	MemoryUtils()->FindPatternAsync( SvenModAPI()->Modules()->Client, Patterns::Client::IN_Move, fIN_Move );
	MemoryUtils()->FindPatternAsync( SvenModAPI()->Modules()->Client, Patterns::Client::GetClientColor, fGetClientColor );
	MemoryUtils()->FindPatternAsync( SvenModAPI()->Modules()->Client, Patterns::Client::CClient_SoundEngine__Play2DSound, fCClient_SoundEngine__Play2DSound );
	MemoryUtils()->FindPatternAsync( SvenModAPI()->Modules()->Client, Patterns::Client::GetClientVoiceMgr, fGetClientVoiceMgr );
	MemoryUtils()->FindPatternAsync( SvenModAPI()->Modules()->Client, Patterns::Client::CVoiceStatus__IsPlayerBlocked, fCVoiceStatus__IsPlayerBlocked );

	if ( ( m_pfnKey_Event = MemoryUtils()->GetPatternFutureValue( fKey_Event, &patternIndex ) ) == NULL )
	{
		Warning("[Source Chat] Couldn't find function \"Key_Event\"\n");
		ScanOK = false;
	}
	else
	{
		DevMsg( "[Source Chat] Found function \"Key_Event\" for version \"%s\"\n", GET_PATTERN_NAME_BY_INDEX( Patterns::Hardware::Key_Event, patternIndex ) );
	}
	
	if ( ( pfnSoundEngine = MemoryUtils()->GetPatternFutureValue( fm_pSoundEngine, &patternIndex ) ) == NULL )
	{
		Warning("[Source Chat] Couldn't find variable \"m_pSoundEngine\"\n");
		ScanOK = false;
	}
	else
	{
		DevMsg( "[Source Chat] Found variable \"m_pSoundEngine\" for version \"%s\"\n", GET_PATTERN_NAME_BY_INDEX( Patterns::Client::m_pSoundEngine, patternIndex ) );
	}
	
	if ( ( m_pfnIN_Move = MemoryUtils()->GetPatternFutureValue( fIN_Move, &patternIndex ) ) == NULL )
	{
		Warning("[Source Chat] Couldn't find function \"IN_Move\"\n");
		ScanOK = false;
	}
	else
	{
		DevMsg( "[Source Chat] Found function \"IN_Move\" for version \"%s\"\n", GET_PATTERN_NAME_BY_INDEX( Patterns::Client::IN_Move, patternIndex ) );
	}
	
	if ( ( m_pfnGetClientColor = (GetClientColorFn)MemoryUtils()->GetPatternFutureValue( fGetClientColor, &patternIndex ) ) == NULL )
	{
		Warning("[Source Chat] Couldn't find function \"GetClientColor\"\n");
		ScanOK = false;
	}
	else
	{
		DevMsg( "[Source Chat] Found function \"GetClientColor\" for version \"%s\"\n", GET_PATTERN_NAME_BY_INDEX( Patterns::Client::GetClientColor, patternIndex ) );
	}
	
	if ( ( m_pfnCClient_SoundEngine__Play2DSound = (CClient_SoundEngine__Play2DSoundFn)MemoryUtils()->GetPatternFutureValue( fCClient_SoundEngine__Play2DSound, &patternIndex ) ) == NULL )
	{
		Warning("[Source Chat] Couldn't find function \"CClient_SoundEngine::Play2DSound\"\n");
		ScanOK = false;
	}
	else
	{
		DevMsg( "[Source Chat] Found function \"CClient_SoundEngine::Play2DSound\" for version \"%s\"\n", GET_PATTERN_NAME_BY_INDEX( Patterns::Client::CClient_SoundEngine__Play2DSound, patternIndex ) );
	}
	
	if ( ( m_pfnGetClientVoiceMgr = (GetClientVoiceMgrFn)MemoryUtils()->GetPatternFutureValue( fGetClientVoiceMgr, &patternIndex ) ) == NULL )
	{
		Warning("[Source Chat] Couldn't find function \"GetClientVoiceMgr\"\n");
		ScanOK = false;
	}
	else
	{
		m_pfnGetClientVoiceMgr = (GetClientVoiceMgrFn)MemoryUtils()->CalcAbsoluteAddress( m_pfnGetClientVoiceMgr ); // CALL [GetClientVoiceMgr]

		DevMsg( "[Source Chat] Found function \"GetClientVoiceMgr\" for version \"%s\"\n", GET_PATTERN_NAME_BY_INDEX( Patterns::Client::GetClientVoiceMgr, patternIndex ) );
	}
	
	if ( ( m_pfnCVoiceStatus__IsPlayerBlocked = (CVoiceStatus__IsPlayerBlockedFn)MemoryUtils()->GetPatternFutureValue( fCVoiceStatus__IsPlayerBlocked, &patternIndex ) ) == NULL )
	{
		Warning("[Source Chat] Couldn't find function \"CVoiceStatus::IsPlayerBlocked\"\n");
		ScanOK = false;
	}
	else
	{
		DevMsg( "[Source Chat] Found function \"CVoiceStatus::IsPlayerBlocked\" for version \"%s\"\n", GET_PATTERN_NAME_BY_INDEX( Patterns::Client::CVoiceStatus__IsPlayerBlocked, patternIndex ) );
	}

	if ( !ScanOK )
		return false;

#if IMGUI_USE_SDL
	m_pfnSDL_PollEvent = Sys_GetProcAddress( SvenModAPI()->Modules()->SDL2, "SDL_PollEvent" );

	if ( !m_pfnSDL_PollEvent )
	{
		Warning( "Couldn't get function \"SDL_PollEvent\"\n" );
		return false;
	}

	m_pfnSDL_GL_SwapWindow = Sys_GetProcAddress( SvenModAPI()->Modules()->SDL2, "SDL_GL_SwapWindow" );

	if ( !m_pfnSDL_GL_SwapWindow )
	{
		Warning( "Couldn't get function \"SDL_GL_SwapWindow\"\n" );
		return false;
	}
#else
	m_pfnwglSwapBuffers = Sys_GetProcAddress( Sys_GetModuleHandle( "opengl32.dll" ), "wglSwapBuffers" );

	if ( !m_pfnwglSwapBuffers )
	{
		Warning( "Couldn't get function \"wglSwapBuffers\"\n" );
		return false;
	}
#endif

	m_pfnSetCursorPos = Sys_GetProcAddress( Sys_GetModuleHandle( "user32.dll" ), "SetCursorPos" );

	if ( !m_pfnSetCursorPos )
	{
		Warning( "Couldn't get function \"SetCursorPos\"\n" );
		return false;
	}

	if ( *(unsigned char *)m_pfnSetCursorPos == 0xE9 ) // JMP opcode, hooked by gameoverlayrenderer.dll
	{
		m_pfnSetCursorPos = MemoryUtils()->CalcAbsoluteAddress( m_pfnSetCursorPos );
	}

	// Get SDL functions
#if IMGUI_USE_SDL
	// Check for JMP opcode
	if ( *(unsigned char *)m_pfnSDL_PollEvent == 0xE9 ||
		 ( *(unsigned char *)m_pfnSDL_PollEvent == 0xFF && *( (unsigned char *)m_pfnSDL_PollEvent + 1 ) == 0x25 ) )
	{
		if ( *(unsigned char *)m_pfnSDL_PollEvent == 0xE9 )
			m_pfnSDL_PollEvent = MemoryUtils()->CalcAbsoluteAddress( m_pfnSDL_PollEvent );
		else
			m_pfnSDL_PollEvent = (void *)( **(unsigned long **)( (unsigned char *)m_pfnSDL_PollEvent + 2 ) );
	}

	// Check for JMP opcode
	if ( *(unsigned char *)m_pfnSDL_GL_SwapWindow == 0xE9 ||
		 ( *(unsigned char *)m_pfnSDL_GL_SwapWindow == 0xFF && *( (unsigned char *)m_pfnSDL_GL_SwapWindow + 1 ) == 0x25 ) )
	{
		if ( *(unsigned char *)m_pfnSDL_GL_SwapWindow == 0xE9 )
			m_pfnSDL_GL_SwapWindow = MemoryUtils()->CalcAbsoluteAddress( m_pfnSDL_GL_SwapWindow );
		else
			m_pfnSDL_GL_SwapWindow = (void *)( **(unsigned long **)( (unsigned char *)m_pfnSDL_GL_SwapWindow + 2 ) );
	}
#else
	if ( *(unsigned char *)m_pfnwglSwapBuffers == 0xE9 )
	{
		m_pfnwglSwapBuffers = MemoryUtils()->CalcAbsoluteAddress( m_pfnwglSwapBuffers );
	}
#endif

	// Get pointer to sound engine
	m_pSoundEngine = (void **)*(unsigned long *)( (unsigned char *)pfnSoundEngine + 1 );

	return true;
}

void CSourceChat::PostLoad( void )
{
	// Get screen size
	m_flWindowWidth = Utils()->GetScreenWidth();
	m_flWindowHeight = Utils()->GetScreenHeight();

	// Set initial text height
	m_flTextHistoryHeight = ChatSchemeActive.TextHistorySizeY - 1.f;

	// Set default text color of text history
	m_flTextHistoryDefaultColor[ 0 ] = (float)( ( ColorSchemeActive.Text >> IM_COL32_R_SHIFT ) & 0xFF ) / 255.f;
	m_flTextHistoryDefaultColor[ 1 ] = (float)( ( ColorSchemeActive.Text >> IM_COL32_G_SHIFT ) & 0xFF ) / 255.f;
	m_flTextHistoryDefaultColor[ 2 ] = (float)( ( ColorSchemeActive.Text >> IM_COL32_B_SHIFT ) & 0xFF ) / 255.f;
	
	// Get some cvars
	hud_draw = CVar()->FindCvar( "hud_draw" );

	Assert( hud_draw != NULL );

	// Attach detours
	m_hKey_Event = DetoursAPI()->DetourFunction( m_pfnKey_Event, HOOKED_Key_Event, GET_FUNC_PTR( ORIG_Key_Event ) );
	m_hIN_Move = DetoursAPI()->DetourFunction( m_pfnIN_Move, HOOKED_IN_Move, GET_FUNC_PTR( ORIG_IN_Move ) );

	m_hMessageMode = Hooks()->HookConsoleCommand( "messagemode", HOOKED_messagemode, &ORIG_messagemode );
	m_hMessageMode2 = Hooks()->HookConsoleCommand( "messagemode2", HOOKED_messagemode2, &ORIG_messagemode2 );

	m_hUserMsgHook_SayText = Hooks()->HookUserMessage( "SayText", UserMsgHook_SayText, &ORIG_UserMsgHook_SayText );
	m_hUserMsgHook_TextMsg = Hooks()->HookUserMessage( "TextMsg", UserMsgHook_TextMsg, &ORIG_UserMsgHook_TextMsg );

	m_hNetMsgHook_TempEntity = Hooks()->HookNetworkMessage( SVC_TEMPENTITY, NetMsgHook_TempEntity, &ORIG_NetMsgHook_TempEntity );

#if IMGUI_USE_SDL
	m_hSDL_PollEvent = DetoursAPI()->DetourFunction( m_pfnSDL_PollEvent, HOOKED_SDL_PollEvent, GET_FUNC_PTR( ORIG_SDL_PollEvent ) );
	m_hSDL_GL_SwapWindow = DetoursAPI()->DetourFunction( m_pfnSDL_GL_SwapWindow, HOOKED_SDL_GL_SwapWindow, GET_FUNC_PTR( ORIG_SDL_GL_SwapWindow ) );
#else
	m_hwglSwapBuffers = DetoursAPI()->DetourFunction( m_pfnwglSwapBuffers, HOOKED_wglSwapBuffers, GET_FUNC_PTR( ORIG_wglSwapBuffers ) );
#endif

	m_hSetCursorPos = DetoursAPI()->DetourFunction( m_pfnSetCursorPos, HOOKED_SetCursorPos, GET_FUNC_PTR( ORIG_SetCursorPos ) );
}

void CSourceChat::Unload( void )
{
	DetoursAPI()->RemoveDetour( m_hKey_Event );
	DetoursAPI()->RemoveDetour( m_hIN_Move );

	Hooks()->UnhookConsoleCommand( m_hMessageMode );
	Hooks()->UnhookConsoleCommand( m_hMessageMode2 );

	Hooks()->UnhookUserMessage( m_hUserMsgHook_SayText );
	Hooks()->UnhookUserMessage( m_hUserMsgHook_TextMsg );

	Hooks()->UnhookNetworkMessage( m_hNetMsgHook_TempEntity );

#if IMGUI_USE_SDL
	DetoursAPI()->RemoveDetour( m_hSDL_PollEvent );
	DetoursAPI()->RemoveDetour( m_hSDL_GL_SwapWindow );
#else
	if ( m_hGameWnd != NULL && m_hGameWndProc != NULL )
	{
		SetWindowLong( m_hGameWnd, GWL_WNDPROC, (LONG)m_hGameWndProc );
	}

	DetoursAPI()->RemoveDetour( m_hwglSwapBuffers );
#endif

	DetoursAPI()->RemoveDetour( m_hSetCursorPos );
}

//-----------------------------------------------------------------------------
// ISourceChat interface methods
//-----------------------------------------------------------------------------

bool CSourceChat::IsOpened( void )
{
	return m_bOpened;
}

bool CSourceChat::IsTeamChat( void )
{
	return m_bTeamChat;
}

void CSourceChat::Clear( void )
{
	m_szHistoryBuffer[ 0 ] = '\0';
	m_ColorfulTextStyle.clear();
	m_TextOpacity.clear();
}

// Expose interface
EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CSourceChat, ISourceChat, SOURCE_CHAT_INTERFACE_VERSION, g_SourceChat );