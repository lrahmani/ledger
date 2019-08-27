FROM ubuntu:18.04

USER root

RUN apt-get update && apt-get upgrade -y

RUN apt-get install -y clang && \
    apt-get install -y gcc && \
    apt-get install -y g++

RUN apt-get install -y make
RUN apt-get install -y cmake

ENV CC=/usr/bin/clang
ENV CXX=/usr/bin/clang++

# from https://docs.fetch.ai/getting-started/installation-ubuntu/
RUN apt-get install build-essential clang git cmake libssl-dev doxygen python3-dev python3-pip python3-venv -y

# get the training dataset
RUN apt-get install wget unzip -y
RUN wget http://mattmahoney.net/dc/text8.zip && unzip text8.zip -d /root/

WORKDIR /source
COPY . /source

RUN rm -Rf build &&\
    mkdir build &&\
    cd build &&\
    cmake .. &&\
    make -j constellation

RUN cd build/libs/ml/examples/ && \
    make -j4

RUN cp -r ./build/libs/ml/examples/ /apps
RUN cp /root/text8 /apps

#EXPOSE 3333
ENTRYPOINT ["/apps/example-ml-word2vec-SGNS-dataloader", "/apps/text8"]
