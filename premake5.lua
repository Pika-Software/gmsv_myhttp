PROJECT_GENERATOR_VERSION = 3
include('third-party/garrysmod_common')

CreateWorkspace({ name = "myhttp", abi_compatible = false })
    group "Third Party"
        include('third-party/curl')
        include('third-party/mbedtls')

    CreateProject({ serverside = true })
        IncludeLuaShared()
        IncludeHelpersExtended()
        IncludeSDKCommon()
        IncludeSDKTier0()
        IncludeSDKTier1()

        files {"src/*.cpp", "src/*.hpp"}

        -- third-party libraries
        defines { "CURL_STATICLIB"}
        links { 'curl-lib', 'mbedtls-lib' }
        sysincludedirs{ 'third-party/curl/include' }