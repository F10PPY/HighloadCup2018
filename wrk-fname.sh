wrk -c 300 -t 3 --latency http://localhost:80/accounts/filter/\?fname_any=%D0%94%D0%B8%D0%BD%D0%B0%2C%D0%92%D0%B0%D0%BB%D0%B5%D1%80%D0%B8%D1%8F%2C%D0%9E%D0%BB%D0%B5%D1%81%D1%8F%2C%D0%9B%D0%B8%D0%B4%D0%B8%D1%8F%2C%D0%95%D0%BB%D0%B5%D0%BD%D0%B0\&limit=100
wrk -c 300 -t 3 --latency http://localhost:80/accounts/filter/\?fname_eq=%D0%94%D0%B0%D1%80%D0%B8%D1%8F\&limit=100
wrk -c 300 -t 3 --latency http://localhost:80/accounts/filter/\?fname_null=0\&limit=100
wrk -c 300 -t 3 --latency http://localhost:80/accounts/filter/\?fname_null=1\&limit=100

