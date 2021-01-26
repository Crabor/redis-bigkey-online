local bulk = 1000
local fvs = {}
local j
for i = 1, ARGV[1] do
  j = i % bulk
  if j == 0 then
    fvs[bulk] = "value" .. i
    redis.call("SADD", KEYS[1], unpack(fvs))
    fvs = {}
  else
    fvs[j] = "value" .. i
  end
end
if #fvs > 0 then
  redis.call("SADD", KEYS[1], unpack(fvs))
end
return "OK"
