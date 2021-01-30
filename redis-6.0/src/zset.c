#include "zset.h"

/* _serverAssert is needed by dict */
static void _serverAssert(const char *estr, const char *file, int line) {
    fprintf(stderr, "=== ASSERTION FAILED ===");
    fprintf(stderr, "==> %s:%d '%s' is not true",file,line,estr);
    *((char*)-1) = 'x';
}

/* Create a skiplist node with the specified number of levels.
 * The SDS string 'ele' is referenced by the node after the call. */
zskiplistNode *zslCreateNode(int level, double score, sds ele) {
    zskiplistNode *zn =
        zmalloc(sizeof(*zn)+level*sizeof(struct zskiplistLevel));
    zn->score = score;
    zn->ele = ele;
    return zn;
}

/* Create a new skiplist. */
zskiplist *zslCreate(void) {
    int j;
    zskiplist *zsl;

    zsl = zmalloc(sizeof(*zsl));
    zsl->level = 1;
    zsl->length = 0;
    zsl->header = zslCreateNode(ZSKIPLIST_MAXLEVEL,0,NULL);
    for (j = 0; j < ZSKIPLIST_MAXLEVEL; j++) {
        zsl->header->level[j].forward = NULL;
        zsl->header->level[j].span = 0;
    }
    zsl->header->backward = NULL;
    zsl->tail = NULL;
    return zsl;
}

/* Free the specified skiplist node. The referenced SDS string representation
 * of the element is freed too, unless node->ele is set to NULL before calling
 * this function. */
void zslFreeNode(zskiplistNode *node) {
    sdsfree(node->ele);
    zfree(node);
}

/* Free a whole skiplist. */
void zslFree(zskiplist *zsl) {
    zskiplistNode *node = zsl->header->level[0].forward, *next;

    zfree(zsl->header);
    while(node) {
        next = node->level[0].forward;
        zslFreeNode(node);
        node = next;
    }
    zfree(zsl);
}

/* Returns a random level for the new skiplist node we are going to create.
 * The return value of this function is between 1 and ZSKIPLIST_MAXLEVEL
 * (both inclusive), with a powerlaw-alike distribution where higher
 * levels are less likely to be returned. */
int zslRandomLevel(void) {
    int level = 1;
    while ((random()&0xFFFF) < (ZSKIPLIST_P * 0xFFFF))
        level += 1;
    return (level<ZSKIPLIST_MAXLEVEL) ? level : ZSKIPLIST_MAXLEVEL;
}

/* Insert a new node in the skiplist. Assumes the element does not already
 * exist (up to the caller to enforce that). The skiplist takes ownership
 * of the passed SDS string 'ele'. */
zskiplistNode *zslInsert(zskiplist *zsl, double score, sds ele) {
    zskiplistNode *update[ZSKIPLIST_MAXLEVEL], *x;
    unsigned int rank[ZSKIPLIST_MAXLEVEL];
    int i, level;

    serverAssert(!isnan(score));
    x = zsl->header;
    for (i = zsl->level-1; i >= 0; i--) {
        /* store rank that is crossed to reach the insert position */
        rank[i] = i == (zsl->level-1) ? 0 : rank[i+1];
        while (x->level[i].forward &&
                (x->level[i].forward->score < score ||
                    (x->level[i].forward->score == score &&
                    sdscmp(x->level[i].forward->ele,ele) < 0)))
        {
            rank[i] += x->level[i].span;
            x = x->level[i].forward;
        }
        update[i] = x;
    }
    /* we assume the element is not already inside, since we allow duplicated
     * scores, reinserting the same element should never happen since the
     * caller of zslInsert() should test in the hash table if the element is
     * already inside or not. */
    level = zslRandomLevel();
    if (level > zsl->level) {
        for (i = zsl->level; i < level; i++) {
            rank[i] = 0;
            update[i] = zsl->header;
            update[i]->level[i].span = zsl->length;
        }
        zsl->level = level;
    }
    x = zslCreateNode(level,score,ele);
    for (i = 0; i < level; i++) {
        x->level[i].forward = update[i]->level[i].forward;
        update[i]->level[i].forward = x;

        /* update span covered by update[i] as x is inserted here */
        x->level[i].span = update[i]->level[i].span - (rank[0] - rank[i]);
        update[i]->level[i].span = (rank[0] - rank[i]) + 1;
    }

    /* increment span for untouched levels */
    for (i = level; i < zsl->level; i++) {
        update[i]->level[i].span++;
    }

    x->backward = (update[0] == zsl->header) ? NULL : update[0];
    if (x->level[0].forward)
        x->level[0].forward->backward = x;
    else
        zsl->tail = x;
    zsl->length++;
    return x;
}

/* Update the score of an element inside the sorted set skiplist.
 * Note that the element must exist and must match 'score'.
 * This function does not update the score in the hash table side, the
 * caller should take care of it.
 *
 * Note that this function attempts to just update the node, in case after
 * the score update, the node would be exactly at the same position.
 * Otherwise the skiplist is modified by removing and re-adding a new
 * element, which is more costly.
 *
 * The function returns the updated element skiplist node pointer. */
zskiplistNode *zslUpdateScore(zskiplist *zsl, double curscore, sds ele, double newscore) {
    zskiplistNode *update[ZSKIPLIST_MAXLEVEL], *x;
    int i;

    /* We need to seek to element to update to start: this is useful anyway,
     * we'll have to update or remove it. */
    x = zsl->header;
    for (i = zsl->level-1; i >= 0; i--) {
        while (x->level[i].forward &&
                (x->level[i].forward->score < curscore ||
                    (x->level[i].forward->score == curscore &&
                     sdscmp(x->level[i].forward->ele,ele) < 0)))
        {
            x = x->level[i].forward;
        }
        update[i] = x;
    }

    /* Jump to our element: note that this function assumes that the
     * element with the matching score exists. */
    x = x->level[0].forward;
    serverAssert(x && curscore == x->score && sdscmp(x->ele,ele) == 0);

    /* If the node, after the score update, would be still exactly
     * at the same position, we can just update the score without
     * actually removing and re-inserting the element in the skiplist. */
    if ((x->backward == NULL || x->backward->score < newscore) &&
        (x->level[0].forward == NULL || x->level[0].forward->score > newscore))
    {
        x->score = newscore;
        return x;
    }

    /* No way to reuse the old node: we need to remove and insert a new
     * one at a different place. */
    zslDeleteNode(zsl, x, update);
    zskiplistNode *newnode = zslInsert(zsl,newscore,x->ele);
    /* We reused the old node x->ele SDS string, free the node now
     * since zslInsert created a new one. */
    x->ele = NULL;
    zslFreeNode(x);
    return newnode;
}

/* Internal function used by zslDelete, zslDeleteRangeByScore and
 * zslDeleteRangeByRank. */
void zslDeleteNode(zskiplist *zsl, zskiplistNode *x, zskiplistNode **update) {
    int i;
    for (i = 0; i < zsl->level; i++) {
        if (update[i]->level[i].forward == x) {
            update[i]->level[i].span += x->level[i].span - 1;
            update[i]->level[i].forward = x->level[i].forward;
        } else {
            update[i]->level[i].span -= 1;
        }
    }
    if (x->level[0].forward) {
        x->level[0].forward->backward = x->backward;
    } else {
        zsl->tail = x->backward;
    }
    while(zsl->level > 1 && zsl->header->level[zsl->level-1].forward == NULL)
        zsl->level--;
    zsl->length--;
}

/* Delete an element with matching score/element from the skiplist.
 * The function returns 1 if the node was found and deleted, otherwise
 * 0 is returned.
 *
 * If 'node' is NULL the deleted node is freed by zslFreeNode(), otherwise
 * it is not freed (but just unlinked) and *node is set to the node pointer,
 * so that it is possible for the caller to reuse the node (including the
 * referenced SDS string at node->ele). */
int zslDelete(zskiplist *zsl, double score, sds ele, zskiplistNode **node) {
    zskiplistNode *update[ZSKIPLIST_MAXLEVEL], *x;
    int i;

    x = zsl->header;
    for (i = zsl->level-1; i >= 0; i--) {
        while (x->level[i].forward &&
                (x->level[i].forward->score < score ||
                    (x->level[i].forward->score == score &&
                     sdscmp(x->level[i].forward->ele,ele) < 0)))
        {
            x = x->level[i].forward;
        }
        update[i] = x;
    }
    /* We may have multiple elements with the same score, what we need
     * is to find the element with both the right score and object. */
    x = x->level[0].forward;
    if (x && score == x->score && sdscmp(x->ele,ele) == 0) {
        zslDeleteNode(zsl, x, update);
        if (!node)
            zslFreeNode(x);
        else
            *node = x;
        return 1;
    }
    return 0; /* not found */
}

int htNeedsResize(dict *dict) {
    long long size, used;

    size = dictSlots(dict);
    used = dictSize(dict);
    return (size > DICT_HT_INITIAL_SIZE &&
            (used*100/size < HASHTABLE_MIN_FILL));
}

static uint64_t dictSdsHash(const void *key) {
    return dictGenHashFunction((unsigned char*)key, sdslen((char*)key));
}

static int dictSdsKeyCompare(void *privdata, const void *key1,
        const void *key2)
{
    int l1,l2;
    DICT_NOTUSED(privdata);

    l1 = sdslen((sds)key1);
    l2 = sdslen((sds)key2);
    if (l1 != l2) return 0;
    return memcmp(key1, key2, l1) == 0;
}

/* Sorted sets hash (note: a skiplist is used in addition to the hash table) */
dictType zsetDictType = {
    dictSdsHash,               /* hash function */
    NULL,                      /* key dup */
    NULL,                      /* val dup */
    dictSdsKeyCompare,         /* key compare */
    NULL,                      /* Note: SDS string shared & freed by skiplist */
    NULL                       /* val destructor */
};

zset *zsetCreate(void){
    zset *zs = zmalloc(sizeof(*zs));
    zs->dict = dictCreate(&zsetDictType,NULL);
    zs->zsl = zslCreate();
    return zs;
}

void zsetFree(zset *zs){
    dictRelease(zs->dict);
    zslFree(zs->zsl);
    zfree(zs);
}

unsigned long zsetLength(const zset *zs) {
    return (unsigned long)zs->zsl->length;
}

sds zsetMin(const zset *zs){
    if(!zs || !zsetLength(zs))
        return NULL;
    return zs->zsl->header->level[0].forward->ele;
}

sds zsetMax(const zset *zs){
    if(!zs || !zsetLength(zs))
        return NULL;
    return zs->zsl->tail->ele;
}

/* Return (by reference) the score of the specified member of the sorted set
 * storing it into *score. If the element does not exist C_ERR is returned
 * otherwise C_OK is returned and *score is correctly populated.
 * If 'zobj' or 'member' is NULL, C_ERR is returned. */
int zsetScore(zset *zs, sds member, double *score) {
    if (!zs || !member || !zsetLength(zs)) return C_ERR;

    dictEntry *de = dictFind(zs->dict, member);
    if (de == NULL) return C_ERR;
    *score = *(double*)dictGetVal(de);

    return C_OK;
}

int zsetAdd(zset *zs, double score, sds ele) {
    double curscore;

    /* NaN as input is an error regardless of all the other parameters. */
    if (isnan(score)) {
        return ZADD_NAN;
    }

    /* Update the sorted set. */
    zskiplistNode *znode;
    dictEntry *de;

    de = dictFind(zs->dict,ele);
    if (de != NULL) {
        curscore = *(double*)dictGetVal(de);

        /* Remove and re-insert when score changes. */
        if (score != curscore) {
            znode = zslUpdateScore(zs->zsl,curscore,ele,score);
            /* Note that we did not removed the original element from
                * the hash table representing the sorted set, so we just
                * update the score. */
            dictGetVal(de) = &znode->score; /* Update score ptr. */
        }
        return ZADD_UPDATED;
    } else{
        ele = sdsdup(ele);//TODO:是否有必要？
        znode = zslInsert(zs->zsl,score,ele);
        serverAssert(dictAdd(zs->dict,ele,&znode->score) == DICT_OK);
        return ZADD_ADDED;
    }

    return 0; /* Never reached. */
}

/* Delete the element 'ele' from the sorted set, returning 1 if the element
 * existed and was deleted, 0 otherwise (the element was not there). */
int zsetDel(zset *zs, sds ele) {
    dictEntry *de;
    double score;

    de = dictUnlink(zs->dict,ele);
    if (de != NULL) {
        /* Get the score in order to delete from the skiplist later. */
        score = *(double*)dictGetVal(de);

        /* Delete from the hash table and later from the skiplist.
            * Note that the order is important: deleting from the skiplist
            * actually releases the SDS string representing the element,
            * which is shared between the skiplist and the hash table, so
            * we need to delete from the skiplist as the final step. */
        dictFreeUnlinkedEntry(zs->dict,de);

        /* Delete from skiplist. */
        int retval = zslDelete(zs->zsl,score,ele,NULL);
        serverAssert(retval);

        if (htNeedsResize(zs->dict)) dictResize(zs->dict);
        return 1;
    }

    return 0; /* No such element found. */
}