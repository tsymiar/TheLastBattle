
-------
* [数据库初始化](https://blog.csdn.net/zixiao217/article/details/80156362):

  `ALTER USER root@localhost IDENTIFIED WITH mysql_native_password BY '123321';`

* CSBattleMgr

  侦听端口 | 服务
  ------------: | ------------- 
  10010 | RC 场景
  10001 | SS 场景
  10002 | GS 网关
   1234 | LogMgr
   6379 | redis
   6380 | LogicRedis
   
* LogMgr

  侦听端口 | 服务
  ------------: | ------------- 
  1234 | 数据库FBAll_LogDB_1
-------