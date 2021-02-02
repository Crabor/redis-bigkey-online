# redis-bigkey-online项目

## 项目简介

正如项目名称一样，这是一个以在线形式分析redis内的大key的工具。灵感主要是看了redis的--bigkey选项的源码实现，发现--bigkey虽然通过scan命令遍历了数据库内的所有key，并通过strlen、hlen、llen等命令获取他们的大小数量信息，但是最后只输出每种数据类型“最大的那一个key“,我的想法是能不能输出前N个key，于是就有了redis-bigkey-online项目。

具体关于原redis是如何实现``--bigkeys``选项的以及我是如何修改源码的，强烈推荐先查看我写的文章（近12000字的详细介绍）：[一文读懂Redis6的--bigkeys选项源码以及redis-bigkey-online项目介绍](https://zhuanlan.zhihu.com/p/348820603)

该工具因为是直接修改的源码，所以使用起来无需额外的工具，只需由原来的

```bash
./redis-cli -h 127.0.0.1 -p 6379 --bigkeys
```

改为

```bash
./redis-cli -h 127.0.0.1 -p 6379 --bigkeys bigkeys.conf
```

就行。其中``bigkeys.conf``保存了用户自定义的配置信息。

## 如何使用

首先得进入redis-6.0的文件夹把``redis-cli``make出来并make install，然后就可以直接使用了。

``bigkeys.conf``文件格式如下：

```yml
#redis-bigkey-online配置文件
#使用方式：./redis-cli --bigkeys bigkey.conf

#output_file:输出文件位置，默认stdout
output_file redis-bigkey-online.output

#string
#string_need_scan:是否需要输出，0不用，1用
string_need_scan 1
#string_output_num:输出的bigkey的数量
string_output_num 9999999
#string_thro_size:判断大key阈值（必须带单位）B，支持B、KB、MB，支持大小写
string_thro_size 0B

#list
#list_need_scan:是否需要输出，0不用，1用
list_need_scan 1
#list_output_num:输出的bigkey的数量
list_output_num 9999999
#list_thro_size:判断大key阈值(item数量)
list_thro_size 0

#set
#set_need_scan:是否需要输出，0不用，1用
set_need_scan 0
#set_output_num:输出的bigkey的数量
set_output_num 9999999
#set_thro_size:判断大key阈值(member数量)
set_thro_size 0

#zset
#zset_need_scan:是否需要输出，0不用，1用
zset_need_scan 1
#zset_output_num:输出的bigkey的数量
zset_output_num 9999999
#zset_thro_size:判断大key阈值(member数量)
zset_thro_size 0

#hash
#hash_need_scan:是否需要输出，0不用，1用
hash_need_scan 1
#hash_output_num:输出的bigkey的数量
hash_output_num 9999999
#hash_thro_size:判断大key阈值(field数量)
hash_thro_size 0

#stream
#stream_need_scan:是否需要输出，0不用，1用
stream_need_scan 1
#stream_output_num:输出的bigkey的数量
stream_output_num 9999999
#stream_thro_size:判断大key阈值(entry数量)
stream_thro_size 0
```

同时，项目还支持``--memkeys``选项，对它也进行了优化，方便大家不仅可以从数量上判断bigkey，也能在实际内存占用大小上判断bigkey。使用方式如同``--bigkeys``选项一样：

```bash
./redis-cli -h 127.0.0.1 -p 6379 --memkeys memkeys.conf
```

``memkeys.conf``格式如下：

```yml
#redis-bigkey-online配置文件
#使用方式：./redis-cli --bigkeys bigkey.conf

#output_file:输出文件位置，默认stdout
output_file stdout

#string
#string_need_scan:是否需要输出，0不用，1用
string_need_scan 1
#string_output_num:输出的key的数量
string_output_num 3
#string_thro_size:判断大key阈值（必须带单位），默认0B，支持B、KB、MB，支持大小写
string_thro_size 2000B

#list
#list_need_scan:是否需要输出，0不用，1用
list_need_scan 1
#list_output_num:输出的key的数量
list_output_num 3
#list_thro_size:判断大key阈值（必须带单位），默认0B，支持B、KB、MB，支持大小写
list_thro_size 0B

#set
#set_need_scan:是否需要输出，0不用，1用
set_need_scan 1
#set_output_num:输出的key的数量
set_output_num 3
#set_thro_size:判断大key阈值（必须带单位），默认0B，支持B、KB、MB，支持大小写
set_thro_size 2000B

#zset
#zset_need_scan:是否需要输出，0不用，1用
zset_need_scan 1
#zset_output_num:输出的key的数量
zset_output_num 3
#zset_thro_size:判断大key阈值（必须带单位），默认0B，支持B、KB、MB，支持大小写
zset_thro_size 2000B

#hash
#hash_need_scan:是否需要输出，0不用，1用
hash_need_scan 1
#hash_output_num:输出的key的数量
hash_output_num 3
#hash_thro_size:判断大key阈值（必须带单位），默认0B，支持B、KB、MB，支持大小写
hash_thro_size 2000B

#stream
#stream_need_scan:是否需要输出，0不用，1用
stream_need_scan 1
#stream_output_num:输出的key的数量
stream_output_num 3
#stream_thro_size:判断大key阈值（必须带单位），默认0B，支持B、KB、MB，支持大小写
stream_thro_size 2000B
```

## 项目性能

这里比较解析能力，就把bigkey阈值设为0，输出数量也设为无上限，并且全部数据类型都要解析。事先通过脚本向redis服务中string、list、set、zset、hash中各插入10000个normalkey和2两个bigkey，stream类型不插入数据。并且通过``/usr/bin/time -v``获取进程执行时间、cpu利用率等信息。

### redis-bigkey-online

可以看到**用户运行时间为0.23秒，系统运行时间为0.12秒，cpu占用率为58%，最大占用内存为11332字节**。

```bash
[root@ecs-7e58 redis-6.0]# /usr/bin/time -v redis-cli --bigkeys bigkeys.conf
        Command being timed: "redis-cli --bigkeys bigkeys.conf"
        User time (seconds): 0.23
        System time (seconds): 0.12
        Percent of CPU this job got: 58%
        Elapsed (wall clock) time (h:mm:ss or m:ss): 0:00.61
        Average shared text size (kbytes): 0
        Average unshared data size (kbytes): 0
        Average stack size (kbytes): 0
        Average total size (kbytes): 0
        Maximum resident set size (kbytes): 11332
        Average resident set size (kbytes): 0
        Major (requiring I/O) page faults: 0
        Minor (reclaiming a frame) page faults: 850
        Voluntary context switches: 14459
        Involuntary context switches: 3
        Swaps: 0
        File system inputs: 0
        File system outputs: 2400
        Socket messages sent: 0
        Socket messages received: 0
        Signals delivered: 0
        Page size (bytes): 4096
        Exit status: 0
```

### python脚本

```py
import sys
import redis

if __name__ == '__main__':
    if len(sys.argv) != 4:
        print('Usage: python ', sys.argv[0], ' host port outputfile ')
        exit(1)
    host = sys.argv[1]
    port = sys.argv[2]
    outputfile = sys.argv[3]
    r = redis.StrictRedis(host=host, port=int(port))
    f = open(outputfile, "w")

    for k in r.scan_iter():
        length = 0
        try:
            type = r.type(k)
            if type == b'string':
                length = r.strlen(k)
            elif type == b'hash':
                length = r.hlen(k)
            elif type == b'list':
                length = r.llen(k)
            elif type == b'set':
                length = r.scard(k)
            elif type == b'zset':
                length = r.zcard(k)
            elif type == b'stream':
                length = r.xlen(k)
        except:
            sys.exit(1)
        if length > 0:
            print(k, type, length, file=f)
```

虽然代码足够精简，但是可以看到**用户运行时间为4.97秒，系统运行时间为1.14秒，cpu占用率为79%，最大占用内存为13060字节**。

```bash
[root@ecs-7e58 script]# /usr/bin/time -v python3 find_bigkeys.py 127.0.0.1 6379 find_bigkeys.output
        Command being timed: "python3 find_bigkeys.py 127.0.0.1 6379 find_bigkeys.output"
        User time (seconds): 4.97
        System time (seconds): 1.14
        Percent of CPU this job got: 79%
        Elapsed (wall clock) time (h:mm:ss or m:ss): 0:07.67
        Average shared text size (kbytes): 0
        Average unshared data size (kbytes): 0
        Average stack size (kbytes): 0
        Average total size (kbytes): 0
        Maximum resident set size (kbytes): 13060
        Average resident set size (kbytes): 0
        Major (requiring I/O) page faults: 0
        Minor (reclaiming a frame) page faults: 6877
        Voluntary context switches: 104842
        Involuntary context switches: 6
        Swaps: 0
        File system inputs: 0
        File system outputs: 2672
        Socket messages sent: 0
        Socket messages received: 0
        Signals delivered: 0
        Page size (bytes): 4096
        Exit status: 0
```

#### redis-rdb-tools（已安装python-lzf）

redis-rdb-tools是github非常受欢迎的一款分析rdb文件的工具，有4k+的star数。并且由于其是离线方式分析redis的持久化文件，避免了客户端命令查询的网络IO消耗，理论上速度是快于脚本的。redis-rdb-tools的``-c justkeys``选项是其最快的解析命令，只输出键名不输出其他信息，下面为测试结果：

惨不忍睹！可以看到**用户运行时间为18.55秒，系统运行时间为0.16秒，cpu占用率为99%，最大占用内存为60548字节**。由于redis-rdb-tools实现的功能过于冗杂繁多，所以反而导致其速度远低于存python脚本。

```bash
[root@ecs-7e58 redis-6.0]# /usr/bin/time -v rdb -c justkeys dump.rdb -f redis-rdb-tools.output
        Command being timed: "rdb -c justkeys dump.rdb -f redis-rdb-tools.output"
        User time (seconds): 18.55
        System time (seconds): 0.16
        Percent of CPU this job got: 99%
        Elapsed (wall clock) time (h:mm:ss or m:ss): 0:18.76
        Average shared text size (kbytes): 0
        Average unshared data size (kbytes): 0
        Average stack size (kbytes): 0
        Average total size (kbytes): 0
        Maximum resident set size (kbytes): 60548
        Average resident set size (kbytes): 0
        Major (requiring I/O) page faults: 0
        Minor (reclaiming a frame) page faults: 11416
        Voluntary context switches: 8
        Involuntary context switches: 254
        Swaps: 0
        File system inputs: 0
        File system outputs: 63552
        Socket messages sent: 0
        Socket messages received: 0
        Signals delivered: 0
        Page size (bytes): 4096
        Exit status: 0
```
