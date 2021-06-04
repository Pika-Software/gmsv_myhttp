#include "main.hpp"
#include <GarrysMod/Lua/Interface.h>
#include <string>
#include "lua_threading.hpp"

using namespace std;
using namespace MyHTTP;
using GarrysMod::Lua::ILuaBase;
using MyHTTP::global_context;

#ifdef DEBUG
int Main::Test(ILuaBase* LUA)
{
	int* a = new int(3);

	Threading::Thread::Create(LUA, a,
	[](void* ptr) {
		cout << "Hello " << *(int*)ptr << endl;
	},
	[](GarrysMod::Lua::ILuaBase* LUA, void* ptr) {
		cout << "now, it is end for our int :(" << endl;
		delete (int*)ptr;
		LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
		LUA->GetField(-1, "print");
		LUA->PushString("pososi");
		LUA->Call(1, 0);
		LUA->Pop();
	});

	return 1;
}
MY_LUA_FUNCTION(Test_LUA) { return global_context->Test(LUA); }
#endif

void Main::Initialize(ILuaBase* LUA)
{
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