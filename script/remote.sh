#!/bin/bash
REMOTE_PATH="/home"
LOCAL_CONF_FILE="bigkeys.conf"
REMOTE_CONF_FILE=${REMOTE_PATH}${LOCAL_CONF_FILE}
HOST=""
USER=""
PASSWORD=""
REDIS_SERVER_PORT="6379"

#sshpass省去了手动输入密码的麻烦，但是同时将密码以明文形式暴露在互联网
#更安全的做法是将本地的id_rsa.pub添加到远程主机的authorized_keys中

#复制配置文件到远程主机
sshpass -p ${PASSWORD} scp ${LOCAL_CONF_FILE} ${USER}@${HOST}:${REMOTE_CONF_FILE}
#本地执行远程主机的命令,需先保证远程主机已经install了redis-cli并且/usr/local/bin已添加到了$PATH中
sshpass -p ${PASSWORD} ssh ${USER}@${HOST} "redis-cli -h 127.0.0.1 -p ${REDIS_SERVER_PORT} --bigkeys ${REMOTE_CONF_FILE}"