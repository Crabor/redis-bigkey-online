cpu=0
cpucnt=0
mem=0
memcnt=0
for ((i=0;i<$2;++i))
do
    tempcpu=$(ps -aux | grep redis-rdb-bigkey | grep -v "grep" | awk '{print $3}')
    tempmem=$(ps -aux | grep redis-rdb-bigkey | grep -v "grep" | awk '{print $4}')
    if [ $(echo "$tempcpu > 50" | bc) -eq 1 ]
    then
        cpu=$(echo "$cpu+$tempcpu" | bc)
        cpucnt=$(expr $cpucnt + 1)    
    fi
    mem=$(echo "$mem+$tempmem" | bc)
    usleep $1
done
mem=$(echo "$mem/$2" | bc)
cpu=$(echo "$cpu/$cpucnt" | bc)
echo "mem:$mem"
echo "cpu:$cpu"
