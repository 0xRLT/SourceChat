#include <Windows.h>

#include "keyboard_layout_map.h"

// Singleton
CKeyboardLayoutMap KeyboardLayoutMap;

//-----------------------------------------------------------------------------
// Table of keyboard layout codes
//-----------------------------------------------------------------------------

CKeyboardLayoutMap::CKeyboardLayoutMap()
{
	Init();
}

const char *CKeyboardLayoutMap::GetLayoutName( int iCode ) const
{
	auto it = m_map.find( iCode );

	if ( it != m_map.end() )
	{
		return m_map.at( iCode );
	}

	return NULL;
}

const char *CKeyboardLayoutMap::GetCurrentLayoutName( void ) const
{
	static char lang[ 16 ];

	if ( !GetKeyboardLayoutNameA( lang ) )
		return NULL;

	return GetLayoutName( strtoul( lang, NULL, 16 ) );
}

void CKeyboardLayoutMap::Init( void )
{
	AddKeyboardLayoutNamed( 0x401, "AR" );
	AddKeyboardLayoutNamed( 0x402, "BG" );
	AddKeyboardLayoutNamed( 0x404, "CN" );
	AddKeyboardLayoutNamed( 0x405, "CZ" );
	AddKeyboardLayoutNamed( 0x406, "DA" );
	AddKeyboardLayoutNamed( 0x407, "DE" );
	AddKeyboardLayoutNamed( 0x408, "GK" );
	//AddKeyboardLayoutNamed( 0x409, "US" );
	AddKeyboardLayoutNamed( 0x40A, "SP" );
	AddKeyboardLayoutNamed( 0x40B, "FI" );
	AddKeyboardLayoutNamed( 0x40C, "FR" );
	AddKeyboardLayoutNamed( 0x40D, "HE" );
	AddKeyboardLayoutNamed( 0x40E, "HU" );
	AddKeyboardLayoutNamed( 0x40F, "IS" );
	AddKeyboardLayoutNamed( 0x410, "IT" );
	AddKeyboardLayoutNamed( 0x411, "JP" );
	AddKeyboardLayoutNamed( 0x412, "KO" );
	AddKeyboardLayoutNamed( 0x413, "NL" );
	AddKeyboardLayoutNamed( 0x414, "NO" );
	AddKeyboardLayoutNamed( 0x415, "PL" );
	AddKeyboardLayoutNamed( 0x416, "BR" );
	AddKeyboardLayoutNamed( 0x418, "RO" );
	AddKeyboardLayoutNamed( 0x419, "RU" );
	AddKeyboardLayoutNamed( 0x41A, "HR" );
	AddKeyboardLayoutNamed( 0x41B, "SK" );
	AddKeyboardLayoutNamed( 0x41C, "AL" );
	AddKeyboardLayoutNamed( 0x41D, "SV" );
	AddKeyboardLayoutNamed( 0x41E, "TH" );
	AddKeyboardLayoutNamed( 0x41F, "TR" );
	AddKeyboardLayoutNamed( 0x420, "UR" );
	AddKeyboardLayoutNamed( 0x422, "UA" );
	AddKeyboardLayoutNamed( 0x423, "BR" );
	AddKeyboardLayoutNamed( 0x424, "SL" );
	AddKeyboardLayoutNamed( 0x425, "ES" );
	AddKeyboardLayoutNamed( 0x426, "LV" );
	AddKeyboardLayoutNamed( 0x427, "LT" );
	AddKeyboardLayoutNamed( 0x428, "TG" );
	AddKeyboardLayoutNamed( 0x429, "IR" );
	AddKeyboardLayoutNamed( 0x42A, "VN" );
	AddKeyboardLayoutNamed( 0x42B, "AM" );
	AddKeyboardLayoutNamed( 0x42C, "AZ" );
	AddKeyboardLayoutNamed( 0x42F, "MA" );
	AddKeyboardLayoutNamed( 0x437, "GE" );
	AddKeyboardLayoutNamed( 0x43B, "NO" );
	AddKeyboardLayoutNamed( 0x43F, "KZ" );
	AddKeyboardLayoutNamed( 0x440, "KY" );
	AddKeyboardLayoutNamed( 0x442, "TK" );
	AddKeyboardLayoutNamed( 0x450, "MN" );
	//AddKeyboardLayoutNamed( 0x452, "UK" );
	AddKeyboardLayoutNamed( 0x454, "LO" );
	AddKeyboardLayoutNamed( 0x45A, "SY" );
	AddKeyboardLayoutNamed( 0x46E, "LB" );
	AddKeyboardLayoutNamed( 0x804, "CN" );
	AddKeyboardLayoutNamed( 0x807, "DE" );
	//AddKeyboardLayoutNamed( 0x809, "UK" );
	AddKeyboardLayoutNamed( 0x80A, "LA" );
	AddKeyboardLayoutNamed( 0x80C, "BE" );
	AddKeyboardLayoutNamed( 0x813, "BE" );
	AddKeyboardLayoutNamed( 0x816, "PT" );
	//AddKeyboardLayoutNamed( 0x81A, "US" ); // SR
	AddKeyboardLayoutNamed( 0x82C, "AZ" );
	AddKeyboardLayoutNamed( 0x843, "UZ" );
	AddKeyboardLayoutNamed( 0x850, "MN" );
	AddKeyboardLayoutNamed( 0xC04, "CN" );
	AddKeyboardLayoutNamed( 0xC0C, "FR" );
	AddKeyboardLayoutNamed( 0xC1A, "SR" );
	AddKeyboardLayoutNamed( 0x1004, "CN" );
	AddKeyboardLayoutNamed( 0x1009, "FR" );
	AddKeyboardLayoutNamed( 0x100C, "FR" );
	AddKeyboardLayoutNamed( 0x1404, "CN" );
	AddKeyboardLayoutNamed( 0x1809, "IE" );
	AddKeyboardLayoutNamed( 0x201A, "BS" );
	AddKeyboardLayoutNamed( 0x4009, "IN" );
	AddKeyboardLayoutNamed( 0x10401, "AB" );
	//AddKeyboardLayoutNamed( 0x10402, "US" ); // BG
	AddKeyboardLayoutNamed( 0x10405, "CZ" );
	AddKeyboardLayoutNamed( 0x10407, "DE" );
	AddKeyboardLayoutNamed( 0x10408, "GK" );
	AddKeyboardLayoutNamed( 0x10409, "DV" );
	AddKeyboardLayoutNamed( 0x1040A, "SP" );
	AddKeyboardLayoutNamed( 0x1040E, "HU" );
	AddKeyboardLayoutNamed( 0x10410, "IT" );
	AddKeyboardLayoutNamed( 0x10415, "PL" );
	AddKeyboardLayoutNamed( 0x10416, "BR" );
	AddKeyboardLayoutNamed( 0x10418, "RO" );
	AddKeyboardLayoutNamed( 0x10419, "RU" );
	AddKeyboardLayoutNamed( 0x1041B, "SL" );
	AddKeyboardLayoutNamed( 0x1041E, "TH" );
	AddKeyboardLayoutNamed( 0x1041F, "TR" );
	//AddKeyboardLayoutNamed( 0x10426, "US" );
	AddKeyboardLayoutNamed( 0x10427, "LT" );
	AddKeyboardLayoutNamed( 0x1042B, "AM" );
	AddKeyboardLayoutNamed( 0x1042C, "AZ" );
	AddKeyboardLayoutNamed( 0x1042F, "MA" );
	AddKeyboardLayoutNamed( 0x10439, "HI" );
	AddKeyboardLayoutNamed( 0x1045A, "SY" );
	AddKeyboardLayoutNamed( 0x1083B, "FI" );
	AddKeyboardLayoutNamed( 0x10850, "MN" );
	AddKeyboardLayoutNamed( 0x10C0C, "CF" );
	//AddKeyboardLayoutNamed( 0x10C1A, "US" );
	AddKeyboardLayoutNamed( 0x11009, "CA" );
	AddKeyboardLayoutNamed( 0x20402, "BG" );
	AddKeyboardLayoutNamed( 0x20405, "CZ" );
	AddKeyboardLayoutNamed( 0x20408, "GK" );
	//AddKeyboardLayoutNamed( 0x20409, "US" );
	AddKeyboardLayoutNamed( 0x20418, "RO" );
	AddKeyboardLayoutNamed( 0x20419, "RU" );
	AddKeyboardLayoutNamed( 0x2041E, "TH" );
	AddKeyboardLayoutNamed( 0x20422, "UA" );
	AddKeyboardLayoutNamed( 0x20426, "LV" );
	AddKeyboardLayoutNamed( 0x20427, "LT" );
	AddKeyboardLayoutNamed( 0x2042B, "AM" );
	AddKeyboardLayoutNamed( 0x20437, "GE" );
	AddKeyboardLayoutNamed( 0x30402, "BG" );
	AddKeyboardLayoutNamed( 0x30408, "GK" );
	AddKeyboardLayoutNamed( 0x30409, "DV" ); // USL
	AddKeyboardLayoutNamed( 0x3041E, "TH" );
	AddKeyboardLayoutNamed( 0x3042B, "AM" );
	AddKeyboardLayoutNamed( 0x30437, "GE" );
	AddKeyboardLayoutNamed( 0x40402, "BG" );
	AddKeyboardLayoutNamed( 0x40408, "GK" );
	AddKeyboardLayoutNamed( 0x40409, "DV" ); // USR
	AddKeyboardLayoutNamed( 0x40437, "GE" );
	AddKeyboardLayoutNamed( 0x50408, "GK" );
	AddKeyboardLayoutNamed( 0x50429, "IR" );
	AddKeyboardLayoutNamed( 0x60408, "GK" );
	AddKeyboardLayoutNamed( 0xF0C00, "IT" );
	AddKeyboardLayoutNamed( 0x110C00, "JV" );
}

void CKeyboardLayoutMap::AddKeyboardLayoutNamed( int iCode, const char *pszLayoutName )
{
	m_map.insert( std::pair<int, const char *>( iCode, pszLayoutName ) );
}
