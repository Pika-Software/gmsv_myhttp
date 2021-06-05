#include "main.hpp"
#include "lua_threading.hpp"
#include "curl_writers.hpp"

#include <GarrysMod/Lua/Interface.h>
#include <GarrysMod/InterfacePointers.hpp>
#include <lua.hpp>

using namespace std;
using namespace MyHTTP;
using GarrysMod::Lua::ILuaBase;
using MyHTTP::global_context;

int Main::ErrorHandler(lua_State* L)
{
	ILuaBase* LUA = L->luabase;
	LUA->SetState(L);

	const char* err = LUA->GetString(1);
	luaL_traceback(L, L, err, 2);
	Warning("ERROR: %s\n", LUA->GetString(-1));

	return 0;
}

void Main::PushErrorHandler(ILuaBase* LUA)
{
	LUA->PushCFunction(ErrorHandler);
}

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

struct DownloadFileData {
	string url;
	string filename;
	string pathID;
	int callback;

	bool ok = true;
	string err;
	size_t size;
};

int Main::DownloadFile(ILuaBase* LUA)
{
	const char* url = LUA->CheckString(1);
	const char* filename = LUA->CheckString(2);
	const char* pathID = LUA->CheckString(3);
	LUA->CheckType(4, GarrysMod::Lua::Type::Function);

	LUA->Push(4);
	int callback = LUA->ReferenceCreate();

	DownloadFileData* data = new DownloadFileData{ url, filename, pathID, callback };
	Threading::Thread::Create(LUA, data,
	[](void* ptr) { // process
		DownloadFileData* data = static_cast<DownloadFileData*>(ptr);

		CURL* curl = curl_easy_init();
		if (!curl) {
			data->ok = false;
			data->err = "failed to create http client";
			return;
		}

		curl_easy_setopt(curl, CURLOPT_URL, data->url.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		FileHandle_t fh = CurlWriters::WriteToFile(curl, data->filename, data->pathID.c_str());
		if (!fh) {
			curl_easy_cleanup(curl);
			data->ok = false;
			data->err = "failed to open file";
			return;
		}

		CURLcode res = curl_easy_perform(curl);
		if (res == CURLE_OK) {
			data->size = g_pFullFileSystem->Tell(fh);
		} else {
			data->ok = false;
			data->err = "failed to make http request";
		}

		curl_easy_cleanup(curl);
		g_pFullFileSystem->Close(fh);
	}, 
	[](ILuaBase* LUA, void* ptr) { // end
		DownloadFileData* data = static_cast<DownloadFileData*>(ptr);

		PushErrorHandler(LUA);
		LUA->ReferencePush(data->callback);
		LUA->PushBool(data->ok);
		if (data->ok)
			LUA->PushNil();
		else
			LUA->PushString(data->err.c_str());
		LUA->PushNumber(data->size);
		LUA->PCall(3, 0, -5);

		LUA->ReferenceFree(data->callback);
		delete data;
	});
	return 1;
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