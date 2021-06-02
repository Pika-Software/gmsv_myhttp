#include "main.hpp"
#include <GarrysMod/Lua/Interface.h>

using namespace MyHTTP;
using namespace std;

void Main::Initialize(GarrysMod::Lua::ILuaBase* LUA)
{

}

void Main::Deinitialize(GarrysMod::Lua::ILuaBase* LUA)
{

}

// GMOD ENTRY POINT
MyHTTP::Main* global_context = nullptr;
GMOD_MODULE_OPEN()
{
	MyHTTP::global_context = new Main();
	MyHTTP::global_context->Initialize(LUA);

	return 0;
}

GMOD_MODULE_CLOSE()
{
	if (MyHTTP::global_context) {
		MyHTTP::global_context->Deinitialize(LUA);

		delete MyHTTP::global_context;
		MyHTTP::global_context = nullptr;
	}

	return 0;
}