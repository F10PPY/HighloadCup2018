for i in {1..1}; do     docker run -v $(pwd):/var/loadtest --net host -it --rm direvius/yandex-tank -c load_dbg/load_$i.ini; done

