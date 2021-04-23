cmake_minimum_required(VERSION 3.8)
Set(This TcpServer)
project(${This} CXX)

Set(Sources
    TcpServer_multipthread.cpp
)

add_executable(${This} ${Sources})