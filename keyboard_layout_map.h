#ifndef KB_LAYOUT_MAP_H
#define KB_LAYOUT_MAP_H

#ifdef _WIN32
#pragma once
#endif

#include <unordered_map>

//-----------------------------------------------------------------------------
// Table of keyboard layout codes
//-----------------------------------------------------------------------------

class CKeyboardLayoutMap
{
public:
	CKeyboardLayoutMap();

	const char *GetLayoutName( int iCode ) const;
	const char *GetCurrentLayoutName( void ) const;

private:
	void Init( void );
	void AddKeyboardLayoutNamed( int iCode, const char *pszLayoutName );

private:
	std::unordered_map<int, const char *> m_map;

};

extern CKeyboardLayoutMap KeyboardLayoutMap;

#endif // KB_LAYOUT_MAP_H