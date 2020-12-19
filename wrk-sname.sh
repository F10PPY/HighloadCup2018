wrk -c 300 -t 3 --latency http://localhost:80/accounts/filter/\?sname_eq=%D0%9B%D1%83%D0%BA%D0%BE%D0%BB%D0%BE%D0%B2%D0%B5%D0%BD\&limit=170
wrk -c 300 -t 3 --latency http://localhost:80/accounts/filter/\?sname_null=0\&limit=100
wrk -c 300 -t 3 --latency http://localhost:80/accounts/filter/\?sname_null=1\&limit=150
wrk -c 300 -t 3 --latency http://localhost:80/accounts/filter/\?sname_starts=%D0%A4%D0%B0%D1%83%D1%88%D1%83\&limit=100


