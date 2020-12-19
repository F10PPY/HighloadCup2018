wrk -c 300 -t 3 --latency http://localhost:80/accounts/filter/\?prem_null=1\&limit=170
wrk -c 300 -t 3 --latency http://localhost:80/accounts/filter/\?prem_null=0\&limit=80
wrk -c 300 -t 3 --latency http://localhost:80/accounts/filter/\?prem_now=1\&limit=80

