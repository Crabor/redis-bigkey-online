# 功能

生成大key

# 使用

## string

```bash
./redis-cli --eval add-big-string.lua big-string , 200
```

其中KEY[1]为key名称，ARGV[1]为大小,单位MB

## list

```bash
./redis-cli --eval add-big-list.lua big-list , 1000000
```

其中KEY[1]为key名称，ARGV[1]为value数量

## set

```bash
./redis-cli --eval add-big-set.lua big-set , 1000000
```

其中KEY[1]为key名称，ARGV[1]为member数量

## zset

```bash
./redis-cli --eval add-big-zset.lua big-zset , 1000000
```

其中KEY[1]为key名称，ARGV[1]为member数量

## hash

```bash
./redis-cli --eval add-big-hash.lua big-hash , 1000000
```

其中KEY[1]为key名称，ARGV[1]为field数量

## add-big-key.sh

```bash
./add-big-key.sh host port database name string_size list_num set_num zset_num hash_num
```