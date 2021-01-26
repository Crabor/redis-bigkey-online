host=$1
port=$2
database=$3
name=$4
string_size=$5
list_num=$6
set_num=$7
zset_num=$8
hash_num=$9

echo "redis-cli -h $host -p $port -n $database --eval add-big-string.lua big-string$name , $string_size"
redis-cli -h $host -p $port -n $database --eval add-big-string.lua big-string$name , $string_size
echo "redis-cli -h $host -p $port -n $database --eval add-big-list.lua big-list$name , $list_num"
redis-cli -h $host -p $port -n $database --eval add-big-list.lua big-list$name , $list_num
echo "redis-cli -h $host -p $port -n $database --eval add-big-set.lua big-set$name , $set_num"
redis-cli -h $host -p $port -n $database --eval add-big-set.lua big-set$name , $set_num
echo "redis-cli -h $host -p $port -n $database --eval add-big-zset.lua big-zset$name , $zset_num"
redis-cli -h $host -p $port -n $database --eval add-big-zset.lua big-zset$name , $zset_num
echo "redis-cli -h $host -p $port -n $database --eval add-big-hash.lua big-hash$name , $hash_num"
redis-cli -h $host -p $port -n $database --eval add-big-hash.lua big-hash$name , $hash_num