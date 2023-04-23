#include "plugin.h"
#include "sourcechat.h"

#include <base_feature.h>

#include <dbg.h>
#include <convar.h>
#include <interface.h>

//-----------------------------------------------------------------------------
// Export the global interface
//-----------------------------------------------------------------------------

CSourceChatPlugin g_SourceChatPlugin;
CSourceChatPlugin *g_pSourceChatPlugin = &g_SourceChatPlugin;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CSourceChatPlugin, IClientPlugin, CLIENT_PLUGIN_INTERFACE_VERSION, g_SourceChatPlugin );

//-----------------------------------------------------------------------------
// Implement plugin methods
//-----------------------------------------------------------------------------

api_version_t CSourceChatPlugin::GetAPIVersion()
{
	return SVENMOD_API_VER;
}

bool CSourceChatPlugin::Load( CreateInterfaceFn pfnSvenModFactory, ISvenModAPI *pSvenModAPI, IPluginHelpers *pPluginHelpers )
{
	BindApiToGlobals( pSvenModAPI );

	if ( !LoadFeatures() )
	{
		Warning( "[Source Chat] Failed to initialize one of features. Check \"svenmod.log\" for detailed information\n" );
		return false;
	}

	ConVar_Register();

	ConColorMsg( { 40, 255, 40, 255 }, "[Source Chat] Successfully loaded\n" );

	g_pEngineFuncs->ClientCmd( "exec sourcechat.cfg" );

	return true;
}

void CSourceChatPlugin::PostLoad( bool bGlobalLoad )
{
	PostLoadFeatures();
}

void CSourceChatPlugin::Unload( void )
{
	ConVar_Unregister();
	UnloadFeatures();
}

bool CSourceChatPlugin::Pause( void )
{
	return false;
}

void CSourceChatPlugin::Unpause( void )
{
}

void CSourceChatPlugin::OnFirstClientdataReceived( client_data_t *pcldata, float flTime )
{
}

void CSourceChatPlugin::OnBeginLoading( void )
{

}

void CSourceChatPlugin::OnEndLoading( void )
{
	g_SourceChat.OnEnterToServer();
}

void CSourceChatPlugin::OnDisconnect( void )
{
	g_SourceChat.OnDisconnect();
}

void CSourceChatPlugin::GameFrame( client_state_t state, double frametime, bool bPostRunCmd )
{
	if ( bPostRunCmd )
	{

	}
	else
	{

	}
}

void CSourceChatPlugin::Draw( void )
{
}

void CSourceChatPlugin::DrawHUD( float time, int intermission )
{
}

const char *CSourceChatPlugin::GetName( void )
{
	return "Source Chat";
}

const char *CSourceChatPlugin::GetAuthor( void )
{
	return "Reality / Sw1ft";
}

const char *CSourceChatPlugin::GetVersion( void )
{
	return "0.1.4b";
}

const char *CSourceChatPlugin::GetDescription( void )
{
	return "Source-like chat";
}

const char *CSourceChatPlugin::GetURL( void )
{
	return "https://github.com/0Reality/SourceChat";
}

const char *CSourceChatPlugin::GetDate( void )
{
	return SVENMOD_BUILD_TIMESTAMP;
}

const char *CSourceChatPlugin::GetLogTag( void )
{
	return "SOURCECHAT";
}
