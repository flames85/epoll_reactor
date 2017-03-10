//
// Created by shaoqi on 17-3-8.
//

#ifndef REACTOR_TCPSERVEREVENTHANDLER_H
#define REACTOR_TCPSERVEREVENTHANDLER_H

#include "core/FDEventHanderBase.h"

class SocketDataDecoderBase;
class Reactor;

/********************************
 * 处理tcpServer事件的类
 ********************************/
class TcpServerEventHandler: public FDEventHanderBase
{
public:
    TcpServerEventHandler(const char* ip,
                          int port,
                          SocketDataDecoderBase* decoder,
                          int fd,
                          Reactor* pReactor);
    
    virtual ~TcpServerEventHandler();
    
    // create listen socket. return 0, success.
    int OnListen();
    int OnAccept(int fd);
    // 当epoll中有新的连接过来的时候，就会调用该函数
    virtual void OnFDRead();
    virtual void OnFDWrite();
    virtual void Close();
protected:
    char                            m_ip[16]; //ipv4
    int                             m_port;
    SocketDataDecoderBase           *m_pDecoder;
};

#endif //REACTOR_TCPSERVEREVENTHANDLER_H
