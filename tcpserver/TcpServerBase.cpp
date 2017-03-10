//
// Created by shaoqi on 17-3-8.
//

#include <cstring>
#include "TcpServerBase.h"
#include "core/LTReactor.h"
#include "TcpServerEventHandler.h"

TcpServerBase::TcpServerBase(const char* ip,
              int port,
              SocketDataDecoderBase* pDecoder)
{
    memset(m_ip, 0x00, sizeof(m_ip));
    strncpy(m_ip, ip, strlen(ip));
    m_port = port;
    m_pReactor = NULL;
    m_pDecoder = pDecoder;
}
int TcpServerBase::Init(){
    m_pReactor = new LTReactor;
    return m_pReactor->Init();
}
int TcpServerBase::Run(){
    TcpServerEventHandler h(m_ip, m_port, m_pDecoder, 0, m_pReactor);
    if (0 == h.OnListen()) {
        m_pReactor->Run();
    }
    
    return 0;
}
int TcpServerBase::Exit(){
    delete m_pReactor;
    return 0;
}