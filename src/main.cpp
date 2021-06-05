#include "main.hpp"
#include "lua_threading.hpp"
#include "curl_writers.hpp"

#include <GarrysMod/Lua/Interface.h>
#include <GarrysMod/InterfacePointers.hpp>

using namespace std;
using namespace MyHTTP;
using GarrysMod::Lua::ILuaBase;
using MyHTTP::global_context;

#ifdef DEBUG
int Main::Test(ILuaBase* LUA)
{
	CURL* curl;
	CURLcode res;

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "https://discord.com/api/downloads/distributions/app/installers/latest?channel=stable&platform=win&arch=x86");
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		FileHandle_t fh = CurlWriters::WriteToFile(curl, "out.dat", "MOD");
		if (!fh) {
			cout << "FAILED TO OPEN FILE" << endl;
			curl_easy_cleanup(curl);
			return 0;
		}

		res = curl_easy_perform(curl);
		g_pFullFileSystem->Close(fh);
		if (res != CURLE_OK) {
			cout << "CODE ISN'T 200" << endl;
			curl_easy_cleanup(curl);
			return 0;
		}

		curl_easy_cleanup(curl);
	}

	return 0;
}
MY_LUA_FUNCTION(Test_LUA) { return global_context->Test(LUA); }
#endif

int Main::DownloadFile(ILuaBase* LUA)
{
	const char* url = LUA->CheckString(1);
	const char* filename = LUA->CheckString(2);
	const char* pathID = LUA->CheckString(3);
	LUA->CheckType(4, GarrysMod::Lua::Type::Function);

	return 0;
}
MY_LUA_FUNCTION(DownloadFile_LUA) { return global_context->DownloadFile(LUA); }

void Main::Initialize(ILuaBase* LUA)
{
	curl_global_init(CURL_GLOBAL_ALL);
	g_pFullFileSystem = InterfacePointers::FileSystem();

	Threading::Core::Initialize(LUA);

	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
		LUA->CreateTable();
#ifdef DEBUG
			DEFINE_LUAFUNC(Test);
#endif

			DEFINE_LUAFUNC(DownloadFile);
		LUA->SetField(-2, "myhttp");
	LUA->Pop();
}

void Main::Deinitialize(ILuaBase* LUA)
{
	curl_global_cleanup();

	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
		LUA->PushNil();
		LUA->SetField(-2, "myhttp");
	LUA->Pop();
}

// GMOD ENTRY POINT
GMOD_MODULE_OPEN()
{
	global_context = new Main();
	global_context->Initialize(LUA);

	return 0;
}

GMOD_MODULE_CLOSE()
{
	if (global_context) {
		global_context->Deinitialize(LUA);

		delete global_context;
		global_context = nullptr;
	}

	return 0;
}