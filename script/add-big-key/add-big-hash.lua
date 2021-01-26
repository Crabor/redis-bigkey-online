local bulk = 1000
local fvs = {}
local j
for i = 1, ARGV[1] do
  j = i % bulk
  if j == 0 then
    fvs[2 * bulk - 1] = "field" .. i
    fvs[2 * bulk] = "value" .. i
    redis.call("HMSET", KEYS[1], unpack(fvs))
    fvs = {}
  else
    fvs[2 * j - 1] = "field" .. i
    fvs[2 * j] = "value" .. i
  end
end
if #fvs > 0 then
  redis.call("HMSET", KEYS[1], unpack(fvs))
end
return "OK"
