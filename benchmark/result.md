# JS (NODE)

```
░▒▓ ~  ab -n 100000 -c 50 http://localhost:4000/                                                                                         ✔  11:04:45 PM ▓▒░
This is ApacheBench, Version 2.3 <$Revision: 1923142 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking localhost (be patient)
Completed 10000 requests
Completed 20000 requests
Completed 30000 requests
Completed 40000 requests
Completed 50000 requests
Completed 60000 requests
Completed 70000 requests
Completed 80000 requests
Completed 90000 requests
Completed 100000 requests
Finished 100000 requests


Server Software:        
Server Hostname:        localhost
Server Port:            4000

Document Path:          /
Document Length:        50 bytes

Concurrency Level:      50
Time taken for tests:   18.459 seconds
Complete requests:      100000
Failed requests:        0
Total transferred:      25000000 bytes
HTML transferred:       5000000 bytes
Requests per second:    5417.27 [#/sec] (mean)
Time per request:       9.230 [ms] (mean)
Time per request:       0.185 [ms] (mean, across all concurrent requests)
Transfer rate:          1322.57 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    0   0.1      0       2
Processing:     5    9   2.5      8      51
Waiting:        5    9   2.5      8      51
Total:          6    9   2.5      8      52

Percentage of the requests served within a certain time (ms)
  50%      8
  66%      9
  75%      9
  80%     10
  90%     11
  95%     14
  98%     17
  99%     20
 100%     52 (longest request)

░▒▓ ~    ```

# MY SERVER

```
# This C Server
```
░▒▓ ~  ab -n 100000 -c 50 <http://localhost:3000/>                                                                                         ✔  11:05:53 PM ▓▒░
This is ApacheBench, Version 2.3 <$Revision: 1923142 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, <http://www.zeustech.net/>
Licensed to The Apache Software Foundation, <http://www.apache.org/>

Benchmarking localhost (be patient)
Completed 10000 requests
Completed 20000 requests
Completed 30000 requests
Completed 40000 requests
Completed 50000 requests
Completed 60000 requests
Completed 70000 requests
Completed 80000 requests
Completed 90000 requests
Completed 100000 requests
Finished 100000 requests

Server Software:        C-Server
Server Hostname:        localhost
Server Port:            3000

Document Path:          /
Document Length:        50 bytes

Concurrency Level:      50
Time taken for tests:   4.666 seconds
Complete requests:      100000
Failed requests:        0
Total transferred:      16400000 bytes
HTML transferred:       5000000 bytes
Requests per second:    21432.46 [#/sec] (mean)
Time per request:       2.333 [ms] (mean)
Time per request:       0.047 [ms] (mean, across all concurrent requests)
Transfer rate:          3432.54 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    1   0.2      1       4
Processing:     0    1   0.3      1       6
Waiting:        0    1   0.3      1       4
Total:          1    2   0.3      2       7

Percentage of the requests served within a certain time (ms)
  50%      2
  66%      2
  75%      2
  80%      2
  90%      2
  95%      3
  98%      3
  99%      4
 100%      7 (longest request)
```
# Conclusions

## Raw data

## C Server

- RPS → 21,432.46
- Mean latency → 2.33 ms

## JS Server

- RPS → 5,417.27
- Mean latency → 9.23 ms

## Performance Difference

[\frac{21432.46 - 5417.27}{5417.27} \times 100] = [\frac{16015.19}{5417.27} \times 100 \approx 295.6%]

- C server is `~295.6%` faster than the JS server.

## Speed Ratio

21432.4/5417.27≈3.96

- C is ~4× faster than JS

C server is:

~4× more throughput
~4× lower latency
better consistency
