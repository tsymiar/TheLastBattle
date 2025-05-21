
   [![Build Status](https://tsymiar.visualstudio.com/TheLastBattle/_apis/build/status/TheLastBattle-ASP.NET%20Core%20(.NET%20Framework)-CI?repoName=tsymiar%2FTheLastBattle&branchName=master)](https://tsymiar.visualstudio.com/TheLastBattle/_build/latest?definitionId=61&repoName=tsymiar%2FTheLastBattle&branchName=master)
   [![Codacy Badge](https://app.codacy.com/project/badge/Grade/14f96c5374174585a1448b7ba1750db7)](https://app.codacy.com/gh/tsymiar/TheLastBattle/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)
   [![MIT Licence](https://tsymiar.github.io/MyAutomatic/image/unlicense.svg)](https://unlicense.org/)

-------
### Brief

   **TheLastBattle** is a MOBA game system, like `Honor of Kings`.
   
   Below is the structure of this project:

   ![](https://github.com/tsymiar/BattleServer/blob/master/Document/image/LastBattle.jpg "LastBattle")

### Client

   [Client](https://github.com/tsymiar/BattleServer/tree/master/Client) source files include a standalone project.

-------
### Database

   Using SQL sentence below to solve `Authentication plugin 'caching_sha2_password' cannot be loaded` issue:
    
   `ALTER USER root@localhost IDENTIFIED WITH mysql_native_password BY '123321';`

### Ports used:

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
  1234 | Database[*FBAll_LogDB_1*]
-------
