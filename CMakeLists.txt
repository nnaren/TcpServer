cmake_minimum_required(VERSION 3.8)
Set(This TcpServer)
project(${This} CXX)

Set(Sources
    TcpServer_multipthread.cpp
    TcpServer_normal.cpp
    TcpServer_select.cpp
    main.cpp
    TcpServer_nonblocking.cpp
)

add_executable(${This} ${Sources})