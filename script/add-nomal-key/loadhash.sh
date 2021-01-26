
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
nohup ./memtier_benchmark -s ${host} -p ${port} --select-db=${database} -t 1 -c 1 --command="hmset __key__ field1 __data__ field2 __data__ field3 __data__ field4 __data__ field5 __data__ field6 __data__ field7 __data__ field8 __data__ field9 __data__ field10 __data__" --random-data --command-key-pattern=S --command-ratio=1  --data-size=${data_size} --key-prefix=${key_prefix} --key-minimum=${keymin} --key-maximum=${keymax} -n $1 >tmp/hashf.txt &
else
for((i=0;i<${thread_num};i++))
do
   nohup ./memtier_benchmark -s ${host} -p ${port} --select-db=${database} -t 1 -c 1 --command="hmset __key__ field1 __data__ field2 __data__ field3 __data__ field4 __data__ field5 __data__ field6 __data__ field7 __data__ field8 __data__ field9 __data__ field10 __data__" --random-data --command-key-pattern=S --command-ratio=1  --data-size=${data_size} --key-prefix=${key_prefix} --key-minimum=${keymin} --key-maximum=${keymax} -n $num > tmp/hash${i}.txt &
done
if [ $1%${thread_num} != 0 ]
then
   nohup ./memtier_benchmark -s ${host} -p ${port} --select-db=${database} -t 1 -c 1 --command="hmset __key__ field1 __data__ field2 __data__ field3 __data__ field4 __data__ field5 __data__ field6 __data__ field7 __data__ field8 __data__ field9 __data__ field10 __data__" --random-data --command-key-pattern=S --command-ratio=1  --data-size=${data_size} --key-prefix=${key_prefix} --key-minimum=${keymin} --key-maximum=${keymax} -n $(($1-$((${thread_num}*${num})))) > tmp/hashl.txt &
fi
fi

