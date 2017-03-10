//
// Created by shaoqi on 17-3-8.
//

#include <zconf.h>
#include "TcpSocketHandler.h"
#include "SocketDataDecoderBase.h"

/* ***********************
 * 处理TCP客户端发过来的数据
 * ************************/

TcpSocketHandler::TcpSocketHandler(SocketDataDecoderBase* decoder,
                                   int fd,
                                   Reactor* pReactor) :
        FDEventHanderBase(fd, pReactor),
        m_dataDecoder(decoder)
{
    
}
TcpSocketHandler::~TcpSocketHandler() {
}

// 读被触发时，调用该函数，处理客户端发过来的数据
void TcpSocketHandler::OnFDRead() {
    // 在这个函数中，可以读取客户端发过来的数据，并对数据进行分析，TCP的数据，
    // 可能会出现半包的情况，所以一般会有一个缓存来保存没有处理完毕的数据
    char buf[1024] =
            { 0 };
    int iRet = read(m_fd, buf, sizeof(buf));
    if (0 == iRet)
    {
        Close();// 客户端关闭socket.
        return;
    }
    // 回调处理函数，对数据进行处理
    m_dataDecoder->OnProcess(this, buf, iRet);
}
// 写被触发时，调用该函数
void TcpSocketHandler::OnFDWrite() {
    
}
// 关闭fd
void TcpSocketHandler::Close() {
    UnRegisterReadEvent();
    UnRegisterWriteEvent();
    close(m_fd);
    delete this;
}
// 可以通过该函数，向客户端发送数据
int TcpSocketHandler::SendBuf(const char* buf, int buf_len) {
    RegisterWriteEvent();
    int iRet = write(m_fd, buf, buf_len);
    UnRegisterWriteEvent();
    return iRet;
}