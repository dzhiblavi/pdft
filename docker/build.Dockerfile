FROM ubuntu:22.04

COPY docker/install_dependencies.sh .
RUN sh install_dependencies.sh

ENV PATH="$PATH:/root/.local/bin"

ENV TZ=Europe/Moscow
ENV LANG C.UTF-8
ENV LC_ALL C.UTF-8

COPY docker/set_compiler.sh .
RUN sh set_compiler.sh
