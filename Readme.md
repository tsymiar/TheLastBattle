
   [![Build status](https://tsymiar.visualstudio.com/BattleServer/_apis/build/status/BattleServer-ASP.NET%20Core%20(.NET%20Framework)-CI)](https://tsymiar.visualstudio.com/BattleServer/_build/latest?definitionId=61)
   [![Codacy Badge](https://api.codacy.com/project/badge/Grade/7c414d19d53b4d6a9e30f741505f6dad)](https://www.codacy.com/app/tsymiar/BattleServer?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=tsymiar/BattleServer&amp;utm_campaign=Badge_Grade)
   [![MIT Licence](https://tsymiar.github.io/MyAutomatic/image/unlicense.svg)](https://unlicense.org/)

-------
### Introduction

   **BattleServer** is a MOBA game server, like `Srike of Kings`.
   
   Below is the structure of this server project:

   ![](https://github.com/tsymiar/BattleServer/blob/master/Document/image/LastBattle.jpg "LastBattle")

### Client

   Source files include [Client](https://github.com/tsymiar/BattleServer/tree/master/Client), which is also a standalone project.

-------
### Database

   Using SQL sentence below to solve `Authentication plugin 'caching_sha2_password' cannot be loaded` issue:
    
   `ALTER USER root@localhost IDENTIFIED WITH mysql_native_password BY '123321';`

### PORTs used:

* CSBattleMgr

  Listen Port | Server
  ------------: | ------------- 
  10010 | RC scene
  10001 | SS scene
  10002 | GS gateway
   1234 | LogMgr
   6379 | redis
   6380 | LogicRedis
   
* LogMgr

  Listen Port | Server
  ------------: | ------------- 
  1234 | database[FBAll_LogDB_1]
-------
