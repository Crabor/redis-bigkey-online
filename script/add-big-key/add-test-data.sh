##添加大key
redis-cli -p 6379 -n 0 --eval add-big-string.lua bigstring00 , 20
redis-cli -p 6379 -n 0 --eval add-big-string.lua bigstring01 , 20
redis-cli -p 6379 -n 0 --eval add-big-list.lua biglist00 , 100000
redis-cli -p 6379 -n 0 --eval add-big-list.lua biglist01 , 100000
redis-cli -p 6379 -n 0 --eval add-big-set.lua bigset00 , 100000
redis-cli -p 6379 -n 0 --eval add-big-set.lua bigset01 , 100000
redis-cli -p 6379 -n 0 --eval add-big-zset.lua bigzset10 , 100000
redis-cli -p 6379 -n 0 --eval add-big-zset.lua bigzset11 , 100000
redis-cli -p 6379 -n 0 --eval add-big-hash.lua bighash10 , 100000
redis-cli -p 6379 -n 0 --eval add-big-hash.lua bighash11 , 100000
# ##添加normalkey
# redis-cli -p 6379 -n 0 set normalstring00 val1
# redis-cli -p 6379 -n 0 set normalstring01 val1
# redis-cli -p 6379 -n 0 lpush normallist00 val1
# redis-cli -p 6379 -n 0 lpush normallist01 val1
# redis-cli -p 6379 -n 0 sadd normalset00 val1
# redis-cli -p 6379 -n 0 sadd normalset01 val1
# redis-cli -p 6379 -n 1 zadd normalzset10 1 val1
# redis-cli -p 6379 -n 1 zadd normalzset11 1 val1
# redis-cli -p 6379 -n 1 hset normalhash10 field1 val1
# redis-cli -p 6379 -n 1 hset normalhash11 field1 val1
# ##添加过期时间
# redis-cli -p 6379 -n 0 expire bigstring01 30
# redis-cli -p 6379 -n 0 expire normalstring01 30
# redis-cli -p 6379 -n 0 expire biglist01 30
# redis-cli -p 6379 -n 0 expire normallist01 30
# redis-cli -p 6379 -n 0 expire bigset01 30
# redis-cli -p 6379 -n 0 expire normalset01 30
# redis-cli -p 6379 -n 1 expire bigzset11 30
# redis-cli -p 6379 -n 1 expire normalzset11 30
# redis-cli -p 6379 -n 1 expire bighash11 30
# redis-cli -p 6379 -n 1 expire normalhash11 30