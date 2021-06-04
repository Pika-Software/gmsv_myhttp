#include "lua_threading.hpp"

using namespace Threading;
using namespace std;

Core* global_threading = nullptr;
int Thread::META = 0;

int Thread::__gc(lua_State* L)
{
	GarrysMod::Lua::ILuaBase* LUA = L->luabase;
	LUA->SetState(L);

	Thread* t = GetPtr(LUA);
	if (t) {
		if (t->destroy_me)
			delete t;
		else
			t->destroy_me = true;
	}

	return 0;
}

int Thread::wait(lua_State* L)
{
	GarrysMod::Lua::ILuaBase* LUA = L->luabase;
	LUA->SetState(L);
	
	Thread* t = GetPtr(LUA);
	if (!t) {
		return 0;
	}

	if (!t->joined && t->t.joinable()) {
		t->joined = true;
		t->LUA = LUA;
		t->t.join();
	}

	return 0;
}

void Thread::ThreadHandler(Thread* t, void* data, void (*process)(void*), void (*end)(GarrysMod::Lua::ILuaBase*, void*))
{
	process(data);
	Callback cb{ t, data, end };

	if (t->joined && t->LUA)
		cb.Call(t->LUA);
	else
		global_threading->PushCallback(cb);
}

void Thread::Callback::Call(GarrysMod::Lua::ILuaBase* LUA)
{
	cb(LUA, data);

	if (t->destroy_me)
		delete t;
	else
		t->destroy_me = true;
}

Thread* Thread::GetPtr(GarrysMod::Lua::ILuaBase* LUA, int iStackPos) {
	return LUA->GetUserType<Thread>(iStackPos, META);
}

void Thread::Create(GarrysMod::Lua::ILuaBase* LUA, void* data, void (*process)(void*), void (*end)(GarrysMod::Lua::ILuaBase*, void*))
{
	Thread* t = new Thread;
	t->t = std::thread(ThreadHandler, t, data, process, end);

	LUA->PushUserType(t, META);
}

Thread::~Thread()
{
	if (t.joinable())
		t.detach();
}

void Thread::Initialize(GarrysMod::Lua::ILuaBase* LUA)
{
	META = LUA->CreateMetaTable("CThread");
		LUA->Push(-1);
		LUA->SetField(-2, "__index");

		LUA->PushCFunction(__gc);
		LUA->SetField(-2, "__gc");

		LUA->PushCFunction(wait);
		LUA->SetField(-2, "wait");
	LUA->Pop();
}

int Core::ThinkHandler(lua_State* L)
{
	GarrysMod::Lua::ILuaBase* LUA = L->luabase;
	LUA->SetState(L);

	if (global_threading->sync_queue.empty())
		return 0;

	while (!global_threading->sync_queue.empty()) {
		Thread::Callback& cb = global_threading->sync_queue.front();
		cb.Call(LUA);
		global_threading->sync_queue.pop();
	}

	return 0;
}

void Core::PushCallback(const Thread::Callback& cb)
{
	sync_queue.push(cb);
}

void Core::Initialize(GarrysMod::Lua::ILuaBase* LUA)
{
	global_threading = new Core;
	Thread::Initialize(LUA);

	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
		LUA->GetField(-1, "timer");
			LUA->GetField(-1, "Create");
			LUA->PushString("_LUA_THREADING_THINK_MYHTTP");
			LUA->PushNumber(0);
			LUA->PushNumber(0);
			LUA->PushCFunction(ThinkHandler);
			LUA->PCall(4, 0, 0);
		LUA->Pop();
	LUA->Pop();
}