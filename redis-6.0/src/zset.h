#ifndef ZSET_H
#define ZSET_H

#include "dict.h"
#include "sds.h"

/* Error codes */
#define C_OK 0
#define C_ERR -1

/* We can print the stacktrace, so our assert is defined this way: */
#define serverAssertWithInfo(_c,_o,_e) ((_e)?(void)0 : (_serverAssertWithInfo(_c,_o,#_e,__FILE__,__LINE__),_exit(1)))
#define serverAssert(_e) ((_e)?(void)0 : (_serverAssert(#_e,__FILE__,__LINE__),_exit(1)))
#define serverPanic(...) _serverPanic(__FILE__,__LINE__,__VA_ARGS__),_exit(1)

/* Hash table parameters */
#define HASHTABLE_MIN_FILL 10      /* Minimal hash table fill 10% */

#define ZSKIPLIST_MAXLEVEL 32 /* Should be enough for 2^64 elements */
#define ZSKIPLIST_P 0.25      /* Skiplist P = 1/4 */

/* Output flags. */
#define ZADD_NOP (1<<3)     /* Operation not performed because of conditionals.*/
#define ZADD_NAN (1<<4)     /* Only touch elements already existing. */
#define ZADD_ADDED (1<<5)   /* The element was new and was added. */
#define ZADD_UPDATED (1<<6) /* The element already existed, score updated. */

#define BIT_STRING 0    
#define BIT_LIST 1      
#define BIT_SET 2       
#define BIT_ZSET 3     
#define BIT_HASH 4      
#define BIT_STREAM 5   

//扫描类型
#define SCAN_TYPE_MASK_STRING (1<<BIT_STRING)
#define SCAN_TYPE_MASK_LIST (1<<BIT_LIST)
#define SCAN_TYPE_MASK_SET (1<<BIT_SET)
#define SCAN_TYPE_MASK_ZSET (1<<BIT_ZSET)
#define SCAN_TYPE_MASK_HASH (1<<BIT_HASH)
#define SCAN_TYPE_MASK_STREAM (1<<BIT_STREAM)

//大key数量
#define BIGKEY_NUM_INF INT64_MAX

//大key阈值
#define KB_TO_BYTE 1024
#define MB_TO_BYTE (KB_TO_BYTE*KB_TO_BYTE)

/* ZSETs use a specialized version of Skiplists */
typedef struct zskiplistNode {
    sds ele;
    double score;
    struct zskiplistNode *backward;
    struct zskiplistLevel {
        struct zskiplistNode *forward;
        unsigned long span;
    } level[];
} zskiplistNode;

typedef struct zskiplist {
    struct zskiplistNode *header, *tail;
    unsigned long length;
    int level;
} zskiplist;

typedef struct zset {
    dict *dict;
    zskiplist *zsl;
} zset;

typedef struct bigkeyConfig_t{
    int64_t num;
    uint32_t thro_size;
    int flag;
}bigkeyConfig_t;


zskiplistNode *zslCreateNode(int level, double score, sds ele);
zskiplist *zslCreate(void);
void zslFreeNode(zskiplistNode *node);
void zslFree(zskiplist *zsl);
int zslRandomLevel(void);
zskiplistNode *zslInsert(zskiplist *zsl, double score, sds ele);
zskiplistNode *zslUpdateScore(zskiplist *zsl, double curscore, sds ele, double newscore);
void zslDeleteNode(zskiplist *zsl, zskiplistNode *x, zskiplistNode **update);
int zslDelete(zskiplist *zsl, double score, sds ele, zskiplistNode **node);

zset *zsetCreate(void);
void zsetFree(zset *zs);
unsigned long zsetLength(const zset *zs);
sds zsetMin(const zset *zs);
int zsetScore(zset *zs, sds member, double *score);
int zsetAdd(zset *zs, double score, sds ele);
int zsetDel(zset *zs, sds ele);

#endif