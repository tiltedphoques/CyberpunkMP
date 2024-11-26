# Cyberpunk Multiplayer

**CyberpunkMP** is a multiplayer mod for Cyberpunk 2077, created by Tilted 
Phoques SRL. This mod brings multiplayer functionality to the game, allowing 
players to synchronize their appearances, equipment, movements, and basic 
animations seamlessly. Additionally, vehicles and their passengers are fully 
synchronized, enabling cooperative or competitive experiences involving the 
game's dynamic vehicular systems.

CyberpunkMP also includes powerful tools for developers. We provide a .NET SDK 
for creating server-side plugins and support client-side plugins through an 
exposed Redscript SDK. The mod features a robust Remote Procedure Call (RPC) 
system, allowing plugins to invoke server-side functions from the client and 
vice versa. This system is completely automatic, requiring no additional code 
to handle RPC functionality.

## Building

### Requirements
- [Visual Studio 2022](https://visualstudio.microsoft.com/downloads/)
- [xmake](https://github.com/xmake-io/xmake/releases)
- [git](https://git-scm.com/downloads)

### Build
1. Navigate to the repository using a command prompt.
2. Check out the correct branch/tag if you're not working against main
3. Run `git submodule update --init` to pull in vendored dependencies
4. Run `xmake -y` (add `-v` for verbose output)

### Visual Studio

If you want visual studio projects execute `xmake project -k vsxmake` and you 
will find the sln in the newly created `vsxmake` folder.

In addition, if you want to debug the project directly from with Visual Studio
you can set the game path `xmake f --game="C:/.../Cyberpunk2077.exe"`. In Visual
Studio you will then have a project named `Cyberpunk2077`, debug this target in
`Debug` only, it will not work in other modes.

> [!IMPORTANT]
> On Windows, you'll need to use Windows SDK **below** v10.0.26100.0. An issue
> is currently breaking the build due to package `protobuf-cpp`.

### Additional configuration / troubleshooting

- [RED4ext](https://github.com/WopsS/RED4ext/releases)
- [CyberEngineTweaks](https://github.com/maximegmd/CyberEngineTweaks/releases)
- [Redscript](https://github.com/jac3km4/redscript/releases/)
- [ArchiveXL](https://github.com/psiberx/cp2077-archive-xl/releases/)
- [TweakXL](https://github.com/psiberx/cp2077-tweak-xl/releases/)
- [Codeware](https://github.com/psiberx/cp2077-codeware/releases/)
- [Input Loader](https://github.com/jackhumbert/cyberpunk2077-input-loader/releases)

