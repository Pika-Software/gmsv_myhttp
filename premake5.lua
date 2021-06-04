PROJECT_GENERATOR_VERSION = 3
include('third-party/garrysmod_common')

CreateWorkspace({ name = "myhttp", abi_compatible = false })
    group "Third Party"
        project "curl"
            kind "StaticLib"
            language "C"
            defines     { "BUILDING_LIBCURL", "CURL_STATICLIB", "HTTP_ONLY" }
            warnings    "off"

            includedirs{ 'third-party/curl/include', 'third-party/curl/lib' }
            files { 'third-party/curl/lib/**.c', 'third-party/curl/lib/**.h' }

            filter { "system:windows" }
                defines { "USE_SCHANNEL", "USE_WINDOWS_SSPI" }
                links "crypt32"

            filter { "system:macosx" }
                defines { "USE_DARWINSSL" }

            filter { "system:not windows", "system:not macosx" }
                defines { "USE_MBEDTLS" }

            filter { "system:linux or bsd or solaris" }
                defines { "CURL_HIDDEN_SYMBOLS" }

    CreateProject({ serverside = true })
        IncludeHelpersExtended()
        IncludeSDKCommon()
        IncludeSDKTier0()
        IncludeSDKTier1()

        files {"src/*.cpp", "src/*.hpp"}

        -- third-party libraries
        links { 'curl' }
        sysincludedirs{ 'third-party/curl/include' }