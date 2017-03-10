//
// Created by shaoqi on 17-3-8.
//

#ifndef REACTOR_SOCKETDATADECODERBASE_H
#define REACTOR_SOCKETDATADECODERBASE_H

class FDEventHanderBase;

class SocketDataDecoderBase
{
public:
    virtual int OnProcess(FDEventHanderBase* pSocket,
                          const char* buf,
                          unsigned int buf_len) = 0;
};

#endif //REACTOR_SOCKETDATADECODERBASE_H
