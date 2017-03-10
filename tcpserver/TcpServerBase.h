//
// Created by shaoqi on 17-3-8.
//

#ifndef REACTOR_TCPSERVERBASE_H
#define REACTOR_TCPSERVERBASE_H


class SocketDataDecoderBase;
class Reactor;
/* *********************************
 *          TcpServerBase类
 * *********************************/
class TcpServerBase
{
public:
    TcpServerBase(const char* ip,
                  int port,
                  SocketDataDecoderBase* pDecoder);
    
    virtual int Init();
    virtual int Run();
    virtual int Exit();
    
private:
    char                        m_ip[16];//ipv4
    int                         m_port;
    Reactor                     *m_pReactor;
    SocketDataDecoderBase*      m_pDecoder;
};

#endif //REACTOR_TCPSERVERBASE_H
