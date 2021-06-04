#ifndef LUA_THREADING_HPP
#define LUA_THREADING_HPP

#include <GarrysMod/Lua/Interface.h>
#include <thread>
#include <atomic>
#include <queue>
#include <iostream>

namespace Threading {
	class Core;
	static Core* global_threading;

	class Thread {
		std::atomic_bool destroy_me{ false };
		std::atomic_bool joined{ false };
		GarrysMod::Lua::ILuaBase* LUA = nullptr;

		static int __gc(lua_State* L);
		static int wait(lua_State* L);

		static void ThreadHandler(Thread* t, void* data, void (*process)(void*), void (*end)(GarrysMod::Lua::ILuaBase*, void*));
	public:
		static int META;
		std::thread t;

		struct Callback {
			Thread* t;
			void* data;
			void (*cb)(GarrysMod::Lua::ILuaBase*, void*);

			void Call(GarrysMod::Lua::ILuaBase* LUA);
		};

		static Thread* GetPtr(GarrysMod::Lua::ILuaBase* LUA, int iStackPos = 1);
		static void Create(GarrysMod::Lua::ILuaBase* LUA, void* data, void (*process)(void*), void (*end)(GarrysMod::Lua::ILuaBase*, void*));
		~Thread();

		static void Initialize(GarrysMod::Lua::ILuaBase* LUA);
	};

	class Core {
		std::queue<Thread::Callback> sync_queue;

		static int ThinkHandler(lua_State* L);
	public:
		void PushCallback(const Thread::Callback& cb);

		static void Initialize(GarrysMod::Lua::ILuaBase* LUA);
	};
}

#endif