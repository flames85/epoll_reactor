cmake_minimum_required(VERSION 2.8)
project(epoll_reactor)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

INCLUDE_DIRECTORIES(.)


set(SOURCE_FILES main.cpp
        core/MinHeap.h
        core/NonCopyable.h
        core/LTReactor.cpp
        core/LTReactor.h
        core/FDEventHanderBase.cpp
        core/FDEventHanderBase.h
        core/TimerEventHanderBase.cpp
        core/TimerEventHanderBase.h
        core/EventHanderSet.cpp
        core/EventHanderSet.h

        tcpserver/SocketDataDecoderBase.h
        tcpserver/TcpServerEventHandler.cpp
        tcpserver/TcpServerEventHandler.h
        tcpserver/TcpSocketHandler.cpp
        tcpserver/TcpSocketHandler.h
        tcpserver/TcpServerBase.cpp
        tcpserver/TcpServerBase.h)

add_executable(epoll_reactor ${SOURCE_FILES})
