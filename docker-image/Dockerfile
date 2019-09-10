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

# get the training dataset for word2vec
RUN apt-get install wget unzip -y
RUN wget http://mattmahoney.net/dc/text8.zip && unzip text8.zip -d /root/

# get the training dataset for MNIST
RUN wget http://yann.lecun.com/exdb/mnist/train-images-idx3-ubyte.gz && gunzip -c train-images-idx3-ubyte.gz > /root/train-images-idx3-ubyte
RUN wget http://yann.lecun.com/exdb/mnist/train-labels-idx1-ubyte.gz && gunzip -c train-labels-idx1-ubyte.gz > /root/train-labels-idx1-ubyte

WORKDIR /source
COPY . /source

RUN rm -Rf build &&\
    mkdir build &&\
    cd build &&\
    cmake .. &&\
    make -j example-ml-mnist-distributed-learning   \
            example-ml-word2vec-SGNS-dataloader     \
            example-ml-word2vec-distributed-learning 

# set up apps
RUN cp -r ./build/libs/ml/examples/ /apps

# set up data
RUN mkdir /data/
RUN cp /root/text8 /data/
RUN cp /root/train-images-idx3-ubyte /data/
RUN cp /root/train-labels-idx1-ubyte /data/

# set up scripts
RUN mkdir /apps/scripts/
RUN echo "#!/bin/bash \n\
          /apps/example-ml-word2vec-SGNS-dataloader /data/text8 \n\
         " > /apps/scripts/word2vec-SGNS.sh 
RUN echo "#!/bin/bash \n\
          /apps/example-ml-mnist-distributed-learning /data/train-images-idx3-ubyte /data/train-labels-idx1-ubyte \n\
         " > /apps/scripts/mnist-dist.sh
RUN echo "#!/bin/bash \n\
          echo 'Usage: ' \n\
          echo ' [+] Word2Vec   : docker-run.sh ... -- /apps/scripts/word2vec-SGNS.sh' \n\
          echo ' [+] MNIST Dist : docker-run.sh ... -- /apps/scripts/mnist-dist.sh' \n\
         " > /apps/scripts/help.sh
RUN chmod +x /apps/scripts/*.sh

CMD ["/apps/scripts/help.sh"]
