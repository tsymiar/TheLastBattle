
[![Build Status](https://tsymiar.visualstudio.com/TheLastBattle/_apis/build/status/TheLastBattle-ASP.NET%20Core%20(.NET%20Framework)-CI?repoName=tsymiar%2FTheLastBattle&branchName=main)](https://tsymiar.visualstudio.com/TheLastBattle/_build/latest?definitionId=61&repoName=tsymiar%2FTheLastBattle&branchName=main)
[![MIT Licence](https://tsymiar.github.io/MyAutomatic/image/unlicense.svg)](https://unlicense.org/)

-------
### Brief

**TheLastBattle** is a MOBA game system, like `Honor of Kings`.

It is a complete distributed server cluster written in **C++ (VS2017 / Win32)** plus a **Unity 4.6** client. The server side is a classic "scene-free gateway" MOBA architecture: a single `CentralServer` owns all persistent data and matchmaking, `SceneServer` instances run rooms/battles, `GateServer` proxies client traffic without parsing it, and several small support processes handle login, load balancing and centralized logging.

Below is the structure of this project:

![](https://github.com/tsymiar/BattleServer/blob/master/Document/image/LastBattle.jpg "LastBattle")

-------
### Architecture

```
        +--------------+        +-------------+
Unity   | LoginServer  |<------>|  SDK (HTTP) |      account / channel auth
Client  +--------------+        +-------------+
  |            ^ server list
  |            v
  |     +---------------+   pick least-loaded GS
  |     | BalanceServer |
  |     +---------------+
  |         ^       ^
  v         |       |
+-------------+   |       |          GS <-> SS/CS messages are forwarded
| GateServer  |---+       |          transparently (no re-serialization)
+-------------+           |
     |  ^                 |
     v  |                 v
+-------------+     +-----------------+
| SceneServer |<--->|  CentralServer  |<--- Redis 6379 / LogicRedis 6380
|  (rooms)    |     | (data/match/    |<--- MySQL  (async DB threads)
+-------------+     |  mail/battle)   |<--- RemoteConsole 10010
                    +-----------------+
                            |
                            v
                     +--------------+     all rumors battle reports / log
                     |   LogMgr     |---> MySQL `FBAll_LogDB_1`
                     +--------------+
```

All internal links are **TCP long connections**, framed with an **8 byte header**
`{ int32 size; int32 protocolId }` followed by a **protobuf** body
(see `Share/Net/INetSessionMgr.{h,cpp}` -> `ParsePacket`, and the client side
`Client/Assets/Scripts/Common/CMsg.cs`). No shared memory or pipes are used.

#### Processes

| Process | Entry point | Responsibility |
| --- | --- | --- |
| **CentralServer** (`CSBattleMgr`) | `CentralServer/CSBattleMgr/main.cpp` | Cluster core: user data / items / mail, match making & battle rooms, newbie guide, registration & scheduling of SS/GS, entry point for the ops console. Key classes: `CCSKernel`, `CCSBattleMgr`, `CCSUserMgr` (async MySQL via `DBActiveWrapper`), `CCSCfgMgr`, `CCSMailMgr`, `CSBattleRoom`/`CSBattle`, `BattleMatcher`, `CSWordFilter`, `CSVoipMgr`. Uses embedded **hiredis**. |
| **SceneServer** (`SSBattleMgr`) | `SceneServer/SSBattleMgr/main.cpp` | Runs battles inside a room: frame/timer driven simulation, hero/AI/behavior-tree/move-A*/skills/effects, reports results to CS. One `BattleLogicThreadActor` per core (`ELogging/SSActive.h` actor model). Key files: `SSBattle.cpp`, `SSHero.cpp`, `SSAI_*.cpp`, `SSBTreeAct_*`/`SSBTreeCon_*`, `SSMoveMgr`, `SSAStar`, `SSEffectMgr`, `SSVMMgr` (Lua), `SSightMgr`. |
| **GateServer** (`GSKernel` + `GSConsole` shell) | `GateServer/GSKernel/main.cpp` | Keeps client long connections; forwards `GC <-> SS` battle messages **without parsing** (`INetSessionMgr::TranMsgToSession`); opens/closes listening and kicks players on CS order. Config: `./GSKernel/GSCfg.xml`. |
| **LoginServer** | `LoginServer/LoginServer/LoginServer.cpp` | Account login, channel SDK verification over **HTTP** (`SdkConnector` + `SDKAsynHandler`, curl_multi + boost::asio, jsoncpp), delivers the server list, syncs with BalanceServer. Config: `LSConfig/Setup.ini`, `LSConfig/Serverlist.ini`. |
| **BalanceServer** | `BalanceServer/BalanceServer/BalanceServer.cpp` | On client request returns the least-loaded Gate address (internal + exported address); GS reports online counts; connects up to LoginServer. Config: `BSConfig/Setup.ini`. No DB. |
| **LogMgr** | `LogMgr/LogMgr/LogMgr.cpp` | The only log sink. `ToLog::WriteLog` -> `EncodeAndSendToDBThread` -> plain SQL insert into `FBAll_LogDB_1`. Listener port & DB settings from `./LGCfg/DBConfig.xml`. |
| **Robot** (`RobotConsole`) | `Robot/RobotConsole/RobotConsole.cpp` | Headless stress-test client that walks the real chain LS -> BS -> GS -> SS. Behavior AI in `PlayerAI`/`PlayerMoveAI`/`PlayerAtkAI`/..., crashes are dumped with Google **Breakpad**. Config: `robot.xml`. |
| **RemoteConsole** | `RemoteConsole/RemoteConsole/Program.cs` | WinForms ops console (C#): multi-server monitoring (online / rooms / battles), mail, announcements, CDKey, DB queries. Talks protobuf-net to CS TCP port **10010** with an `RSKey`; also queries MySQL directly. Config in `cfg/*.xml`. |

#### Directory map

| Directory | What it is |
| --- | --- |
| `CentralServer/`, `SceneServer/`, `GateServer/`, `LoginServer/`, `BalanceServer/`, `LogMgr/`, `Robot/` | Server processes (C++). ~300 first-party source files. |
| `RemoteConsole/` | C# WinForms ops tool (separate solution). |
| `Share/` | Cross-server shared code: session layer `Net/` (`INetSessionMgr`), `DbgLib/` (crash dump wrapper). |
| `CommonCPlus/` | Project common layer: `FBAll_Const.h`, `ServerMath.h`, MySQL wrapper (`CSDataSource`/`DBMgr`/`DBActiveWrapper`), plus vendored headers (`MySql`, `LuaBind`, ZeroC `ice`, `libiconv`). |
| `UCore/` | Base framework: containers/threads (`include/base`) and **IOCP** networking (`include/net`, `src/net/win`), plus vendored `libiconv`. |
| `ELogging/` | Lightweight async logging DLL, exposes `ELOG(level, ...)`, `SSActive` actor model, timers. |
| `ThirdFunc/` | Third-party sources/libs: protobuf 2.6, boost 1.60, openssl 1.1.1a, curl 7.64, libevent, tinyxml, libssh2, LuaPlus, jsoncpp, Intel TBB, gperftools, Google-Breakpad, VLD. Generated message code lives in `MsgLists/` (C++), `ClientProtobuf/` and `RemoteProtobuf/` (C#). |
| `lib-src/` | ~180MB archive of pre-built dependencies (boost, openssl, mysql-connector-c, pthreads-w32, ActivePerl, NASM, protoc...). Extracted at build time. |
| `Client/` | Unity client (see below). |
| `FBALLVoipAndroid/` | Android VoIP module used by the mobile build. |
| `Document/` | `BuildingInstructions/`, `image/`, `TestDoc/`. |

-------
### Ports used

* CentralServer (`CSBattleMgr`, see `CentralServer/CSBattleMgr/CSKernel_MainThread.cpp`)

  Listen Port | Server
  ------------: | -------------
  10010 | RC scene (RemoteConsole)
  10001 | SS scene (SceneServer)
  10002 | GS gateway (GateServer)
   1234 | LogMgr (outgoing)
   6379 | redis
   6380 | LogicRedis

* LogMgr

  Listen Port | Server
  ------------: | -------------
  1234 | Database [*FBAll_LogDB_1*]

* Other endpoints (configured per process, defaults below)

  Port | Server | Source
  ----: | ----- | ------
  49996 | LoginServer, listened for the client | `LSConfig/Setup.ini` `[MainGC] ListernPortForClient`; same default in `Client/Assets/Scripts/JxBlGame.cs`
  40001 | BalanceServer, default address used by the client | `NetworkManager` defaults (`m_IP=127.0.0.1`, `m_Port=40001`)
  `n32GCListenPort` | GateServer, listened for the client | `./GSKernel/GSCfg.xml`
  `GSPort` / `SSPort` | SceneServer <-> GateServer / CentralServer | `./SSKernel/SSKernelCfg.xml`, `./CSConfig/CSKernelCfg.xml`

-------
### Build

Only the **C++ server cluster** is built by `AllServer.sln`; the Unity client is built manually from the editor.

* Requirements (Windows): **Visual Studio 2017** (toolset `v141`), Windows 10 SDK, `7za` on `PATH`.
* One-shot script:

  ```
  clean.bat      # remove generated boost/openssl dirs and intermediates
  build.bat      # extract pthread + libmysql, build boost 1.60 (bjam),
                 # build OpenSSL 1.1.1a (needs ActivePerl + NASM),
                 # then: MSBuild AllServer.sln /t:build /p:configuration=Debug /m
  ```

  Output executables and required DLLs (`libmysql.dll`, `libcrypto-1_1.dll`) are copied to `Bin/`.
* CI: `.github/workflows/msbuild.yml` (GitHub Actions, `windows-latest`, branch `main`) and
  `azure-pipelines.yml` (Hosted VS2017) both run the same dependency preparation and then MSBuild.
* Solutions: `AllServer.sln` contains 14 projects (BalanceServer, GSKernel, SSBattleMgr, CSBattleMgr,
  ELogging, LoginServer, LogMgr, DbgLib, tinyxml, ucorebase, ucorenet, libiconv, libcurl, LuaPlus).
  `LoginServer.sln`, `BalanceServer.sln`, `GateServer.sln`, `LogMgr.sln`, `UCore/project/win/ucore.sln`,
  `Robot/Robot.sln` and `RemoteConsole/RemoteConsole.sln` are separate.
* Linux: **not fully supported**. Several per-project `CMakeLists.txt` exist
  (`CentralServer/`, `SceneServer/`, `GateServer/GSKernel`, `ELogging/`, ...) but they expect environment
  variables (`ServerRoot`, `Platform`, `Configuration`, `CompilerSetting`) and system-installed
  boost/openssl/libmysql/libprotobuf; there is no top-level CMake build or Linux build doc.

#### Protocol regeneration

All messages are defined in `ThirdFunc/ProtoFile/*.proto` (26 files named by direction:
`GCToCS`, `GSToGC`, `CSToSS`, `SSToCS`, `RCToCS`, `ToLog`, ...). Regenerate with:

```
ThirdFunc/ProtoFile/genProto.bat
```

It emits C++ into `ThirdFunc/MsgLists`, C# into `ThirdFunc/ClientProtobuf` and
`ThirdFunc/RemoteProtobuf`, and then copies the C# into `Client/Assets/ProtobuMsg`
and `RemoteConsole/RemoteConsole`.

-------
### Deployment notes

The **runtime configuration directories are not in this repository** and must be deployed next to the
executables:

| Path | Process |
| --- | --- |
| `CSConfig/CSKernelCfg.xml`, `CSConfig/DBConfig.xml`, `CSBattleMgr/*.xml` (Hero/Item/NPC/Skill/Rune/RobotAI/MapLoad cfg), `CSBattleMgr/MapConfig/*.xml` | CentralServer |
| `SSKernel/SSKernelCfg.xml`, `SSKernel/SSGuideCfg/*.xml`, `SSKernel/MapObjCfg.xml`, `Map/` | SceneServer |
| `GSKernel/GSCfg.xml` | GateServer |
| `LSConfig/Setup.ini`, `LSConfig/Serverlist.ini` | LoginServer |
| `BSConfig/Setup.ini` | BalanceServer |
| `LGCfg/DBConfig.xml` | LogMgr |
| `robot.xml` | Robot |
| `cfg/ServerConfig.xml`, `cfg/DbConfig.xml`, `cfg/MailTypeCfg.xml`, ... | RemoteConsole |

Start-up order that works: **LogMgr -> redis/LogicRedis -> MySQL -> CentralServer ->
SceneServer -> GateServer -> LoginServer -> BalanceServer**, then clients; `RemoteConsole` and `Robot`
can join at any time afterwards.

-------
### Client

[Client](https://github.com/tsymiar/TheLastBattle/tree/main/Client) is a standalone **Unity 4.6** project
(Windows target shipped prebuilt as `Client/FF.exe`).

* UI: **NGUI 3.7.4**; rendering: built-in forward renderer, old Unity4 lightmap pipeline.
* Networking: TCP long connection via `Client/Assets/Scripts/Common/NetworkManager.cs` on top of
  SuperSocket's `ClientSession`; serialization with **protobuf-net**
  (`Assets/ProtobuMsg/` mirror of `ThirdFunc/ClientProtobuf`), 2MB receive buffer, heartbeat timeout and auto-reconnect.
* Login flow: connect **LoginServer 49996** -> receive `LSToGC.ServerBSAddr` -> connect
  **BalanceServer** -> receive `BSToGC.AskGateAddressRet` -> reconnect to the assigned **GateServer**
  and stay there for the whole session (see `Scripts/GameCtrl/LoginCtrl.cs`).
* Code layout: `Assets/Scripts/GameLogic/` (battle logic, `CGLCtrl_GameLogic_MsgHandler.cs` is the giant
  message switch), `Assets/Scripts/GameEntity/`, `Assets/Scripts/Effect/`, `Assets/Scripts/Guis/`
  (Login/Lobby/Play/GameOver), `Assets/Scripts/ConfigReader/` over `Assets/Resources/Config/*.xml`,
  `Assets/Scripts/ResourceManager/` (AssetBundle archive + version update or `Resources.Load`,
  toggled by `UsedAssetBundle`).
* AssetBundles: legacy Unity4 API (`BuildPipeline.BuildAssetBundle` + `PushAssetDependencies`) through the
  menu `BoLong/Build AssetBundles/{Android,Ios,Windows}/...` (`Assets/Scripts/Tools/Editor/BuildProjectAssets.cs`).
  The iOS post-process hook is `Assets/Editor/PostProcessBuildPlayer`.
* Third-party in client: protobuf-net, HOTween, `Xeffect`, `fyParticleSystem`,
  UCGameSDK/支付宝/Analytics Android jars.

-------
### Database

Using SQL sentence below to solve `Authentication plugin 'caching_sha2_password' cannot be loaded` issue:

`ALTER USER root@localhost IDENTIFIED WITH mysql_native_password BY '123321';`

MySQL is accessed from the server side through `CommonCPlus/DBMgr.h` + `DBActiveWrapper` (one async DB
thread per wrapper). There is **no `.sql` schema file** in this repository: tables are created/altered by
code (`CentralServer/CSBattleMgr/CSUserMgr_*.cpp`, `CSUserDbDataMgr.cpp`) and by the ops console
(`RemoteConsole/DbFunc.cs`, `MailFunc.cs`); the connection string comes from `CSConfig/DBConfig.xml`.

-------
### License

[Unlicense](LICENSE) — public domain.
