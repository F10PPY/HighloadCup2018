wrk -c 300 -t 3 --latency http://localhost:80/accounts/filter/\?country_eq=%D0%A0%D0%BE%D1%81%D0%BC%D0%B0%D0%BB%D1%8C\&limit=100
wrk -c 300 -t 3 --latency http://localhost:80/accounts/filter/\?country_null=0\&limit=100
wrk -c 300 -t 3 --latency http://localhost:80/accounts/filter/\?country_null=1\&limit=100

