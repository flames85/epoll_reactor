//
// Created by shaoqi on 17-3-8.
//

#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <zconf.h>
#include <arpa/inet.h>
#include "TcpServerEventHandler.h"
#include "TcpSocketHandler.h"

/********************************
 * 处理tcpServer事件的类
 ********************************/

TcpServerEventHandler::TcpServerEventHandler(const char* ip,
                      int port,
                      SocketDataDecoderBase* decoder,
                      int fd,
                      Reactor* pReactor) :
        FDEventHanderBase(fd, pReactor),
        m_pDecoder(decoder)
{
    memset(m_ip, 0x00, sizeof(m_ip));
    strncpy(m_ip, ip, strlen(ip));
    m_port = port;
}
TcpServerEventHandler::~TcpServerEventHandler() {
}


// create listen socket. return 0, success.
int TcpServerEventHandler::OnListen() {
    m_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (m_fd == -1)
    {
        std::cout << "create socket error." << strerror(errno) << std::endl;
        return -1;
    }
    sockaddr_in addr;
    memset(&addr, 0x00, sizeof(sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_port);
    socklen_t len = sizeof(sockaddr_in);
    
    if (1 != inet_pton(AF_INET, m_ip, &addr.sin_addr))
    {
        std::cout << "inet_pton error." << strerror(errno) << std::endl;
        close(m_fd);
        return -1;
    }
    // 允许在同一端口上启动同一服务器的多个实例
    int opVal = 1;
    if (0 != setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &opVal, sizeof(opVal)))
    {
        std::cout << "socketopt error." << strerror(errno) << std::endl;
        close(m_fd);
        return -1;
    }
    // 设置非阻塞模式
    SetNonBlock(m_fd, true);
    // bind
    if (-1 == bind(m_fd, (struct sockaddr*) &addr, len))
    {
        std::cout << "bind error." << strerror(errno) << std::endl;
        close(m_fd);
        return -1;
    }
    // listen
    if (-1 == listen(m_fd, 200))
    {
        std::cout << "listen error." << strerror(errno) << std::endl;
        close(m_fd);
        return -1;
    }
    
    // 将该fd加入到 epoll中，监听客户端的连接
    if (0 != RegisterReadEvent())
    {
        std::cout << "RegisterReadEvent error." << std::endl;
        close(m_fd);
        return -1;
    }
    
    return 0;
}

int TcpServerEventHandler::OnAccept(int fd) {
    TcpSocketHandler* pHandler = new TcpSocketHandler(m_pDecoder, fd,
                                                      this->m_pReactor);
    // 注册一个读事件，从TCP的客户端读取数据.
    if (0 != pHandler->RegisterReadEvent())
    {
        pHandler->Close();
        delete pHandler;
        pHandler = NULL;
    }
    // 需要delete pHandler
    return 0;
}
// 当epoll中有新的连接过来的时候，就会调用该函数
void TcpServerEventHandler::OnFDRead() {
    sockaddr_in clientAddr;
    socklen_t len = sizeof(clientAddr);
    // 客户端的fd，这个地方，其实可以建立一个(fd -- 客户端)对应关系
    int sockFD = accept(m_fd, (struct sockaddr*) &clientAddr, &len);
    if (sockFD < 0)
    {
        std::cout << "accept error." << std::endl;
        return;
    }
    else
    {
        int port = ntohs(clientAddr.sin_port);
        char addr[256] =
                { 0 };
        inet_ntop(AF_INET, &clientAddr.sin_addr.s_addr, addr,
                  (socklen_t) sizeof(addr));
        printf("accept: [%s:%d] \n", addr, port);
    }
    
    OnAccept(sockFD);
}
void TcpServerEventHandler::OnFDWrite() {
    
}
void TcpServerEventHandler::Close() {
    UnRegisterReadEvent();
    UnRegisterWriteEvent();
    close(m_fd);
}