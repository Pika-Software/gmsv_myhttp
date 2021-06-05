#include "main.hpp"
#include "lua_threading.hpp"
#include <GarrysMod/Lua/Interface.h>
#include <string>
#include <curl/curl.h>

using namespace std;
using namespace MyHTTP;
using GarrysMod::Lua::ILuaBase;
using MyHTTP::global_context;

static size_t writetest(void* contents, size_t size, size_t nmemb, FILE* userp)
{
	size_t realsize = size * nmemb;
	std::cout << realsize << endl;
	size_t written = fwrite(contents, size, nmemb, userp);
	return written;
}

#ifdef DEBUG
int Main::Test(ILuaBase* LUA)
{
	CURL* curl;
	CURLcode res;
	FILE* f;

	curl = curl_easy_init();
	if (curl) {
		f = fopen("D:\\Work\\garrysmod-server\\out.dat", "wb");
		curl_easy_setopt(curl, CURLOPT_URL, "https://discord.com/api/downloads/distributions/app/installers/latest?channel=stable&platform=win&arch=x86");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writetest);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, f);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		fclose(f);
	}

	return 0;
}
MY_LUA_FUNCTION(Test_LUA) { return global_context->Test(LUA); }
#endif

void Main::Initialize(ILuaBase* LUA)
{
	curl_global_init(CURL_GLOBAL_ALL);

	Threading::Core::Initialize(LUA);

	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
		LUA->CreateTable();
#ifdef DEBUG
			LUA->PushCFunction(Test_LUA);
			LUA->SetField(-2, "Test");
#endif
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