﻿
“One step each day get closer to the dream —— MolyDB”  ————JakeSun

DONE
0.0.0.1  2014-08-03 JakeSun   搭建整体简单框架
0.0.2.0	 2014-08-04 JakeSun   完成网络IO模块1
0.0.2.1	 2014-08-05 JakeSun   完成网络IO模块2
0.0.2.1	 2014-08-06 JakeSun   完成网络IO模块3
0.0.3.0  2014-08-07 JakeSun   增加数据引擎的hash结构，并亲自测试
0.0.3.2	 2014-08-08 JakeSun   引入google开源unit test框架gtest
0.0.3.3	 2014-08-08 JakeSun   调整输出目录结构，调整工程依赖
0.0.3.4	 2014-08-09 JakeSun   client端的网络接口1
0.0.3.5	 2014-08-09 JakeSun   client端的网络接口2
0.0.3.6	 2014-08-11 JakeSun   调试通Server与Client之间的协议
0.0.4.0  2014-08-14 JakeSun   实现Dictionary结构及动态重置功能(rehash)  
0.0.4.1  2014-08-18 JakeSun   实现Client端命令解析&控制台彩色显示
0.0.4.2  2014-08-18 JakeSun   调试通过Client到Server的命令，可以初步用作一个缓存系统。
0.0.4.7  2014-08-21 JakeSun   开始实现持久化-二进制dump持久化
0.0.5.0  2014-08-26 JakeSun   完成无锁队列(lock-free-queue)的实现，支持多读多写
0.0.5.3  2014-08-26 JakeSun   调试通过无锁队列，持久化-append file开发结束
0.0.5.4  2014-08-26 JakeSun   使用控制台彩显
0.0.5.6  2014-08-27 JakeSun   开始实现replication的slave部分
0.0.5.8  2014-08-31 JakeSun   实现了消息包压缩，并解决了一个致命的内存BUG
0.0.6.0  2014-09-02 JakeSun   完成了从append文件恢复内存数据
0.0.6.5  2014-09-04 JakeSun   完成了replication
0.0.6.6  2014-09-06 JakeSun   完成了Hash iterator，使容器和算法解耦
0.0.6.8  2014-09-07 JakeSun   完成内存分配器, 并修正了一个内存泄漏BUG
0.0.6.9  2014-09-11 JakeSun   完成KEYS命令，支持*?通配符匹配
0.0.7.0  2014-09-12 JakeSun   多数据结构支持:单向链表
0.0.7.1  2014-09-13 JakeSun   多数据结构支持:双端链表
0.0.7.5  2014-09-14 JakeSun   完成了exists、lpush、rpush、lpop、rpop、llen等命令
0.0.7.6  2017-10-17 JakeSun   继续开始完善，创建dev分支
0.0.7.7  2017-10-26 JakeSun   增加最小优先堆，实现expire(过期键)命令
0.0.7.8  2018-09-01 JakeSun   增加启动参数功能
0.0.8.0  2018-10-10 JakeSun   支持LIST基础数据类型，及其相关CAUD命令
0.0.8.2  2018-10-11 JakeSun   支持HASH基础数据类型，及其相关CAUD命令
