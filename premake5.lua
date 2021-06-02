PROJECT_GENERATOR_VERSION = 3
include('third-party/garrysmod_common')

CreateWorkspace({ name = "myhttp", abi_compatible = false })
    CreateProject({ serverside = true })
        IncludeHelpersExtended()
        IncludeSDKCommon()
        IncludeSDKTier0()
        IncludeSDKTier1()

        files {"src/*.cpp", "src/*.hpp"}