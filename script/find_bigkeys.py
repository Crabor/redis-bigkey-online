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
