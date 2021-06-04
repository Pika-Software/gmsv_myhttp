#include "main.hpp"
#include <GarrysMod/Lua/Interface.h>

using namespace std;
using namespace MyHTTP;
using GarrysMod::Lua::ILuaBase;
using MyHTTP::global_context;

#ifdef DEBUG
int Main::Test(ILuaBase* LUA)
{
	cout << "HELLO WORLD!" << endl;
	return 0;
}
MY_LUA_FUNCTION(Test_LUA) { return global_context->Test(LUA); }
#endif

void Main::Initialize(ILuaBase* LUA)
{
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