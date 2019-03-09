
   [![Build status](https://tsymiar.visualstudio.com/BattleServer/_apis/build/status/BattleServer-.NET%20Desktop-CI)](https://tsymiar.visualstudio.com/BattleServer/_build/latest?definitionId=1)
   [![MIT Licence](https://tsymiar.github.io/MyAutomatic/image/unlicense.svg)](https://unlicense.org/)

-------
### Database

   TRY `Authentication plugin 'caching_sha2_password' cannot be loaded` to solve issue below:
    
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
