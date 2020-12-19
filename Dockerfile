FROM ubuntu:latest


RUN apt-get update && apt-get upgrade -y
RUN apt-get install sudo


RUN sudo apt-get install build-essential software-properties-common -y  
RUN sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y  
RUN sudo apt-get update -y  
RUN sudo apt-get install gcc-8 g++-8 -y  
RUN sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 60 --slave /usr/bin/g++ g++ /usr/bin/g++-8  
RUN sudo update-alternatives --config gcc




RUN apt-get install -y g++
RUN apt-get install unzip





WORKDIR /root
COPY . /root
#RUN g++-8 -Irapidjson worker.cpp post_id.cpp post_likes.cpp post_new.cpp index.cpp get_filter.cpp get_group.cpp get_recommend.cpp get_suggest.cpp load_data.cpp main.cpp util.cpp req_handler.cpp resp_templates.cpp -std=c++2a -std=gnu++2a -lpthread -lstdc++fs  -Ofast -march=nehalem -flto

EXPOSE 80

CMD lscpu && cat /proc/version  && uname -r &&\
g++-8 -Irapidjson worker.cpp post_id.cpp post_likes.cpp post_new.cpp index.cpp get_filter.cpp get_group.cpp get_recommend.cpp get_suggest.cpp load_data.cpp main.cpp util.cpp req_handler.cpp resp_templates.cpp -std=c++2a -std=gnu++2a -lpthread -lstdc++fs  -Ofast -march=native -flto -msse4.2 -w &&\
 ./a.out

