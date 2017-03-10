//
// Created by shaoqi on 17-3-8.
//

#ifndef REACTOR_TCPSOCKETHANDLER_H
#define REACTOR_TCPSOCKETHANDLER_H

#include "core/FDEventHanderBase.h"
class SocketDataDecoderBase;
class Reactor;

/* **************************
 * 处理TCP客户端发过来的数据
 * 这个地方，可以开辟2块缓冲区（读缓冲区和写缓冲区）
 * (1)读，当socket可读时，epoll会被触发，回调OnFDRead()，
 * 将读取到的数据进行处理，或者放到读缓冲区
 * (2)写，当有数据需要通过socket发出去时，先将数据放到写缓冲区中，
 * 注册RegisterWriteEvent，之后,OnFDWrite就会被回调，
 * 若LT模式，当数据没有写完时，OnFDWrite一直会被回调，直到写完后，
 * 用户调用UnRegisteWriteEvent.
 * ************************/
class TcpSocketHandler: public FDEventHanderBase
{
public:
    TcpSocketHandler(SocketDataDecoderBase* decoder,
                     int fd,
                     Reactor* pReactor);
    
    virtual ~TcpSocketHandler();
    
    // 读被触发时，调用该函数，处理客户端发过来的数据
    virtual void OnFDRead();
    // 写被触发时，调用该函数
    virtual void OnFDWrite();
    // 关闭fd
    virtual void Close();
    // 可以通过该函数，向客户端发送数据
    virtual int SendBuf(const char* buf, int buf_len);
    
private:
    SocketDataDecoderBase       *m_dataDecoder;
    //Buffer* pReadBuf;
    //Buffer* pWriteBuf;
};
#endif //REACTOR_TCPSOCKETHANDLER_H
