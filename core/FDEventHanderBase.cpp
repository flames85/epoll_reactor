//
// Created by shaoqi on 17-3-8.
//

#include <fcntl.h>
#include "FDEventHanderBase.h"
#include "LTReactor.h"

/* **********************************
 *             处理FD类的基类
 * 当该fd被触发时，调用处理函数来进行处理
 * ***********************************/
// 注册读事件
int FDEventHanderBase::RegisterReadEvent() {
    return m_pReactor->RegisterReadEvent(this);
}
// 注册写事件
int FDEventHanderBase::RegisterWriteEvent() {
    return m_pReactor->RegisterWriteEvent(this);
}
// 注销读事件
int FDEventHanderBase::UnRegisterReadEvent() {
    return m_pReactor->UnRegisterReadEvent(this);
}
// 注销写事件
int FDEventHanderBase::UnRegisterWriteEvent() {
    return m_pReactor->UnRegisterWriteEvent(this);
}

int FDEventHanderBase::GetFD() const {
    return m_fd;
}
void FDEventHanderBase::SetFD(int fd) {
    m_fd = fd;
}
// 设置非阻塞，返回0，success.
int FDEventHanderBase::SetNonBlock(int fd, bool bNonBlock/* = true*/) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
    {
        return -1;
    }
    if (bNonBlock)
        flags |= O_NONBLOCK;
    else
        flags &= ~O_NONBLOCK;
    
    if (fcntl(fd, F_SETFL, flags) == -1)
    {
        return -1;
    }
    
    return 0;
}