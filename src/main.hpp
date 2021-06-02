#ifndef MYHTTP_MAIN_HPP
#define MYHTTP_MAIN_HPP
#include <GarrysMod/Lua/LuaBase.h>

#define MY_LUA_FUNCTION( FUNC )											\
    static int FUNC##__Imp( GarrysMod::Lua::ILuaBase* LUA );			\
    static int FUNC( lua_State* L )										\
	{																	\
		GarrysMod::Lua::ILuaBase* LUA = L->luabase;						\
		LUA->SetState(L);												\
		try {															\
			return FUNC##__Imp(LUA);									\
		} catch (const std::exception& ex) {							\
			LOG() << "C++ EXCEPTION!" << std::endl;						\
			LUA->ThrowError(ex.what());									\
			return 0;													\
		}																\
	}																	\

namespace MyHTTP {
	class Main;
	static Main* global_context;

	class Main {
	public:
		void Initialize(GarrysMod::Lua::ILuaBase* LUA);
		void Deinitialize(GarrysMod::Lua::ILuaBase* LUA);
	};
}

#endif