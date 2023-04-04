#include <string.h>

#include "chat_scheme.h"

//-----------------------------------------------------------------------------
// Chat schemes
//-----------------------------------------------------------------------------

CChatScheme ChatSchemeDefault; // stores default settings
CChatColorScheme ColorSchemeDefault;

CChatScheme ChatScheme; // stores current settings
CChatColorScheme ColorScheme;

CChatScheme ChatSchemeActive; // stores current active (or adaptive) settings
CChatColorScheme ColorSchemeActive;

//-----------------------------------------------------------------------------
// Initialize scheme settings at DLL load
//-----------------------------------------------------------------------------

class CStaticChatSchemeInitializer
{
public:
	CStaticChatSchemeInitializer();

private:
	void InitDefaultChatScheme();
	void InitDefaultColorScheme();
};

static CStaticChatSchemeInitializer ChatSchemeInitializer;

//-----------------------------------------------------------------------------
// CStaticChatSchemeInitializer::CStaticChatSchemeInitializer
//-----------------------------------------------------------------------------

CStaticChatSchemeInitializer::CStaticChatSchemeInitializer()
{
	InitDefaultChatScheme();
	InitDefaultColorScheme();

	memcpy( &ChatScheme, &ChatSchemeDefault, sizeof( CChatScheme ) );
	memcpy( &ColorScheme, &ColorSchemeDefault, sizeof( CChatColorScheme ) );

	memcpy( &ChatSchemeActive, &ChatSchemeDefault, sizeof( CChatScheme ) );
	memcpy( &ColorSchemeActive, &ColorSchemeDefault, sizeof( CChatColorScheme ) );
}

//-----------------------------------------------------------------------------
// CStaticChatSchemeInitializer::InitDefaultChatScheme
//-----------------------------------------------------------------------------

void CStaticChatSchemeInitializer::InitDefaultChatScheme()
{
	// Fonts
	ChatSchemeDefault.FontSize = 19.f;
	ChatSchemeDefault.FontUnicodeSize = 21.f;
	ChatSchemeDefault.FontBitmapSize = 19.f;
	ChatSchemeDefault.FontSmallSize = 14.f;

	// Chat size
	ChatSchemeDefault.ChatSizeX = 630.f;
	ChatSchemeDefault.ChatSizeY = 270.f;

	// Key codes
	ChatSchemeDefault.KeyCodesPosX = 22.f;
	ChatSchemeDefault.KeyCodesPosY = 6.f;

	// Text history
	ChatSchemeDefault.TextHistoryPosX = 22.f;
	ChatSchemeDefault.TextHistoryPosY = 38.f;
	ChatSchemeDefault.TextHistorySizeX = 585.f;
	ChatSchemeDefault.TextHistorySizeY = 191.f;
	ChatSchemeDefault.RichTextPosX = 5.f;
	ChatSchemeDefault.RichTextPosY = 1.f;
	ChatSchemeDefault.RichTextWidth = 555.f;
	ChatSchemeDefault.RichTextLineSpacing = 5.f;
	ChatSchemeDefault.RichTextExtraSpace = 22.f;

	// Input line name
	ChatSchemeDefault.InputLineNamePosX = 22.f;
	ChatSchemeDefault.InputLineNamePosY = 238.f;
	ChatSchemeDefault.InputLineNameTextAlignX = 0.f;
	ChatSchemeDefault.InputLineNameTextAlignY = 0.2f;
	ChatSchemeDefault.InputLineNameTextPosX = 2.f;
	ChatSchemeDefault.InputLineNameTextPosY = 0.f;
	ChatSchemeDefault.InputLineNameSizeX = 37.f;
	ChatSchemeDefault.InputLineNameSizeY = 18.f;
	ChatSchemeDefault.InputLineNameTeamSizeX = 102.f; // if team chat is enabled
	ChatSchemeDefault.InputLineNameTeamSizeY = 18.f;

	// Input line
	ChatSchemeDefault.InputLinePosX = 61.f;
	ChatSchemeDefault.InputLineTeamPosX = 126.f;
	ChatSchemeDefault.InputLineKeyboardLayoutPosX = 88.f;
	ChatSchemeDefault.InputLineKeyboardLayoutTeamPosX = 153.f;
	ChatSchemeDefault.InputLineWidth = 547.f;
	ChatSchemeDefault.InputLineTeamWidth = 482.f;
	ChatSchemeDefault.InputLineKeyboardLayoutWidth = 520.f;
	ChatSchemeDefault.InputLineKeyboardLayoutTeamWidth = 455.f;
	ChatSchemeDefault.InputLinePosY = 238.f;
	ChatSchemeDefault.InputLineFramePaddingX = 3.f;
	ChatSchemeDefault.InputLineFramePaddingY = 0.1f;
	ChatSchemeDefault.InputLineFrameBgOffsetX = 1.f;
	ChatSchemeDefault.InputLineFrameBgOffsetY = 1.f;

	// Keyboard layout name
	ChatSchemeDefault.KeyboardLayoutBackgroundPosX = 61.f;
	ChatSchemeDefault.KeyboardLayoutBackgroundTeamPosX = 126.f;
	ChatSchemeDefault.KeyboardLayoutBackgroundPosY = 238.f;
	ChatSchemeDefault.KeyboardLayoutBackgroundSizeX = 27.f;
	ChatSchemeDefault.KeyboardLayoutBackgroundSizeY = 18.f;
	ChatSchemeDefault.KeyboardLayoutPosX = 61.f;
	ChatSchemeDefault.KeyboardLayoutTeamPosX = 126.f;
	ChatSchemeDefault.KeyboardLayoutPosY = 240.f;
	ChatSchemeDefault.KeyboardLayoutSizeX = 25.f;
	ChatSchemeDefault.KeyboardLayoutSizeY = 14.f;
	ChatSchemeDefault.KeyboardLayoutTextAlignX = 0.f;
	ChatSchemeDefault.KeyboardLayoutTextAlignY = -2.f;
}

//-----------------------------------------------------------------------------
// CStaticChatSchemeInitializer::InitDefaultColorScheme
//-----------------------------------------------------------------------------

void CStaticChatSchemeInitializer::InitDefaultColorScheme()
{
	ColorSchemeDefault.WindowBackground = IM_COL32( 115, 115, 115, 200 );
	ColorSchemeDefault.WindowBorder = IM_COL32( 115, 115, 115, 200 );
	ColorSchemeDefault.ChildBackground = IM_COL32( 60, 60, 60, 128 );

	ColorSchemeDefault.ScrollbarBg = IM_COL32( 38, 38, 38, 255 );
	ColorSchemeDefault.ScrollbarGrab = IM_COL32( 79, 79, 79, 255 );
	ColorSchemeDefault.ScrollbarGrabHovered = IM_COL32( 105, 105, 105, 255 );
	ColorSchemeDefault.ScrollbarGrabActive = IM_COL32( 130, 130, 130, 255 );

	ColorSchemeDefault.Cursor = IM_COL32( 217, 217, 217, 255 );

	ColorSchemeDefault.Text = IM_COL32( 210, 200, 152, 255 );
	ColorSchemeDefault.InputLineText = IM_COL32( 255, 255, 255, 255 );
	ColorSchemeDefault.InputLineNameText = IM_COL32( 217, 217, 217, 255 );
	ColorSchemeDefault.SelectedText = IM_COL32( 255, 156, 0, 255 );
	ColorSchemeDefault.KeyCodesText = IM_COL32( 217, 217, 217, 255 );

	ColorSchemeDefault.KeyboardLayoutText = IM_COL32( 0, 0, 0, 255 );
	ColorSchemeDefault.KeyboardLayoutBackground = IM_COL32( 255, 255, 255, 255 );
}