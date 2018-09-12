ALTER USER root@localhost IDENTIFIED WITH mysql_native_password BY ‘123321’;
https://blog.csdn.net/zixiao217/article/details/80156362


CSBattleMgr
侦听端口：
10010 for RC 场景
10001 for SS 场景
10002 for GS 网关
连接1234 for LogServer

6379 for redis
6380 for LogicRedis


LogServer
侦听端口1234
连接数据库FBAll_LogDB_1