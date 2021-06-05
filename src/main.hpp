#ifndef MYHTTP_MAIN_HPP
#define MYHTTP_MAIN_HPP
#include <GarrysMod/Lua/LuaBase.h>
#include <iostream>

#define MY_LUA_FUNCTION( FUNC )											\
    static int FUNC##__Imp( GarrysMod::Lua::ILuaBase* LUA );			\
    static int FUNC( lua_State* L )										\
	{																	\
		GarrysMod::Lua::ILuaBase* LUA = L->luabase;						\
		LUA->SetState(L);												\
		try {															\
			return FUNC##__Imp(LUA);									\
		} catch (const std::exception& ex) {							\
			std::cout << "C++ EXCEPTION!" << std::endl;					\
			LUA->ThrowError(ex.what());									\
			return 0;													\
		}																\
	}																	\
	static int FUNC##__Imp( GarrysMod::Lua::ILuaBase* LUA )

#define DEFINE_LUAFUNC(FUNC)			\
	LUA->PushCFunction(FUNC##_LUA);		\
	LUA->SetField(-2, #FUNC);		

namespace MyHTTP {
	class Main;
	static Main* global_context;

	class Main {
	public:
		static int ErrorHandler(lua_State* L);
		static void PushErrorHandler(GarrysMod::Lua::ILuaBase* LUA);

#ifdef DEBUG
		int Test(GarrysMod::Lua::ILuaBase* LUA);
#endif
		
		int DownloadFile(GarrysMod::Lua::ILuaBase* LUA);

		void Initialize(GarrysMod::Lua::ILuaBase* LUA);
		void Deinitialize(GarrysMod::Lua::ILuaBase* LUA);
	};
}

#endif