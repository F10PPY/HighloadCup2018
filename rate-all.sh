for i in {1..3}; do     docker run -v $(pwd):/var/loadtest --net host -it --rm direvius/yandex-tank -c load/rating/load_$i.ini; done

