FROM ubuntu:18.04

RUN apt-get update && \
    apt-get install -y \
    git \
    build-essential \
    cmake \
    libssl-dev \
    gdb \
    vim-gtk

ENV LANG="C.UTF-8" LANGUAGE="en_US"
WORKDIR /cpp-dev

