#ifndef CHAT_SCHEME_H
#define CHAT_SCHEME_H

#ifdef _WIN32
#pragma once
#endif

#include <imgui.h>

//-----------------------------------------------------------------------------
// CChatScheme
//-----------------------------------------------------------------------------

struct CChatScheme
{
	// Fonts
	float FontSize;
	float FontUnicodeSize;
	float FontBitmapSize;
	float FontSmallSize;

	// Chat size
	float ChatSizeX;
	float ChatSizeY;

	// Key codes
	float KeyCodesPosX;
	float KeyCodesPosY;

	// Text history
	float TextHistoryPosX;
	float TextHistoryPosY;
	float TextHistorySizeX;
	float TextHistorySizeY;
	float RichTextPosX;
	float RichTextPosY;
	float RichTextWidth;
	float RichTextLineSpacing;
	float RichTextExtraSpace;

	// Input line name
	float InputLineNamePosX;
	float InputLineNamePosY;
	float InputLineNameTextAlignX;
	float InputLineNameTextAlignY;
	float InputLineNameTextPosX;
	float InputLineNameTextPosY;
	float InputLineNameSizeX;
	float InputLineNameSizeY;
	float InputLineNameTeamSizeX; // if team chat is enabled
	float InputLineNameTeamSizeY;

	// Input line
	float InputLinePosX;
	float InputLineTeamPosX;
	float InputLineKeyboardLayoutPosX;
	float InputLineKeyboardLayoutTeamPosX;
	float InputLineWidth;
	float InputLineTeamWidth;
	float InputLineKeyboardLayoutWidth;
	float InputLineKeyboardLayoutTeamWidth;
	float InputLinePosY;
	float InputLineFramePaddingX;
	float InputLineFramePaddingY;
	float InputLineFrameBgOffsetX;
	float InputLineFrameBgOffsetY;

	// Keyboard layout name
	float KeyboardLayoutBackgroundPosX;
	float KeyboardLayoutBackgroundTeamPosX;
	float KeyboardLayoutBackgroundPosY;
	float KeyboardLayoutBackgroundSizeX;
	float KeyboardLayoutBackgroundSizeY;
	float KeyboardLayoutPosX;
	float KeyboardLayoutTeamPosX;
	float KeyboardLayoutPosY;
	float KeyboardLayoutSizeX;
	float KeyboardLayoutSizeY;
	float KeyboardLayoutTextAlignX;
	float KeyboardLayoutTextAlignY;
};

//-----------------------------------------------------------------------------
// CChatColorScheme
//-----------------------------------------------------------------------------

struct CChatColorScheme
{
	ImU32 WindowBackground;
	ImU32 WindowBorder;
	ImU32 ChildBackground;

	ImU32 ScrollbarBg;
	ImU32 ScrollbarGrab;
	ImU32 ScrollbarGrabHovered;
	ImU32 ScrollbarGrabActive;

	ImU32 Cursor;

	ImU32 Text;
	ImU32 InputLineText;
	ImU32 InputLineNameText;
	ImU32 SelectedText;
	ImU32 KeyCodesText;

	ImU32 KeyboardLayoutText;
	ImU32 KeyboardLayoutBackground;
};

//-----------------------------------------------------------------------------
// Access to chat schemes
//-----------------------------------------------------------------------------

extern CChatScheme ChatSchemeDefault;
extern CChatColorScheme ColorSchemeDefault;

extern CChatScheme ChatScheme;
extern CChatColorScheme ColorScheme;

extern CChatScheme ChatSchemeActive;
extern CChatColorScheme ColorSchemeActive;

#endif // CHAT_SCHEME_H