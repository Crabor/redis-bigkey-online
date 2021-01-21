# redis-bigkey-online

## 项目简介

正如项目名称一样，这是一个以在线形式分析redis内的大key的工具。灵感主要是看了redis的--bigkey选项的源码实现，发现--bigkey虽然通过scan命令遍历了数据库内的所有key，并通过strlen、hlen、llen等命令获取他们的大小数量信息，但是最后只输出每种数据类型“最大的那一个key“，而不是输出“超过阈值的所有key”。

## 项目功能

## 项目性能