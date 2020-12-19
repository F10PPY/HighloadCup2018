wrk -c 300 -t 3 --latency http://localhost:80/accounts/filter/\?city_eq=%D0%90%D0%BC%D1%81%D1%82%D0%B5%D1%80%D0%B8%D0%BD%D1%81%D0%BA\&query_id=2231\&limit=100
wrk -c 300 -t 3 --latency http://localhost:80/accounts/filter/\?city_any=%D0%92%D0%BE%D0%BB%D0%BE%D0%B4%D0%B0%D0%BC%2C%D0%A0%D0%BE%D1%81%D0%BE%D1%80%D0%B5%D1%87%D1%81%D0%BA%2C%D0%A0%D0%BE%D1%81%D0%BE%D0%BB%D1%91%D0%B2\&query_id=2231\&limit=100



