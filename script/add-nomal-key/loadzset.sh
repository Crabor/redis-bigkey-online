host=$2
port=$3
thread_num=$4
key_prefix=$5
data_size=$6
database=$7
keymin=$8
keymax=$9
num=$(($1/${thread_num}))
if [ ${num} == 0 ]
then
   nohup ./memtier_benchmark -s ${host} -p ${port} --select-db=${database} -t 1 -c 1 --command="zadd __key__ 1 __data__ 2 __data__ 3 __data__ 4 __data__ 5 __data__ 6 __data__ 7 __data__ 8 __data__ 9 __data__ 10 __data__" --command-key-pattern=S --command-ratio=1 --random-data --data-size=${data_size}  --key-prefix=${key_prefix} --key-minimum=${keymin} --key-maximum=${keymax} -n $1 > tmp/zsetf.txt &
else
for((i=0;i<${thread_num};i++))
do
   nohup ./memtier_benchmark -s ${host} -p ${port} --select-db=${database} -t 1 -c 1 --command="zadd __key__ 1 __data__ 2 __data__ 3 __data__ 4 __data__ 5 __data__ 6 __data__ 7 __data__ 8 __data__ 9 __data__ 10 __data__" --command-key-pattern=S --command-ratio=1 --random-data --data-size=${data_size} --key-prefix=${key_prefix} --key-minimum=${keymin} --key-maximum=${keymax} -n $num > tmp/zset${i}.txt &
done
if [ $1%${thread_num} != 0 ]
then
  nohup ./memtier_benchmark -s ${host} -p ${port} --select-db=${database} -t 1 -c 1 --command="zadd __key__ 1 __data__ 2 __data__ 3 __data__ 4 __data__ 5 __data__ 6 __data__ 7 __data__ 8 __data__ 9 __data__ 10 __data__" --command-key-pattern=S --command-ratio=1 --random-data --data-size=${data_size} --key-prefix=${key_prefix} --key-minimum=${keymin} --key-maximum=${keymax} -n $(($1-$((${thread_num}*${num})))) > tmp/zsetl.txt &
fi
fi

