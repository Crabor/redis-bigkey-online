# 添加普通键（需要事先安装memtier-benchmark）

## 加载命令

```bash
sh 脚本名 key数量 ip 端口 并发写入进程数 key前缀 value大小 数据库号 键名最小值 键名最大值
```

脚本名：对应shell脚本名，string为loadstring.sh,hash为loadstring.sh，list为loadlist.sh，set为loadset.sh，zset为loadzest.sh

ip：灌入数据的实例ip

端口：灌入数据的实例端口

并发写入进程数：并发灌入数据的进程数

## 示例

string加载命令示例

```bash
sh loadstring.sh 100000000 10.35.206.11 6379 10 128_string_redisplus_ 128 0 1 1000000
```

hash加载命令示例

```bash
sh loadhash.sh 100000000 10.35.206.11 6379 10 128_hash_redisplus_ 128 0 1 1000000
```

list加载命令示例

```bash
sh loadlist.sh 100000000 10.35.206.11 6379 10 128_list_redisplus_ 128 0 1 1000000
```

set加载命令示例

```bash
sh loadset.sh 100000000 10.35.206.11 6379 10 128_set_redisplus_ 128 0 1 1000000
```

zset加载命令示例

```bash
sh loadzset.sh 100000000 10.35.206.11 6379 10 128_zset_redisplus_ 128 0 1 1000000
```