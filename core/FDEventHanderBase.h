//
// Created by shaoqi on 17-3-8.
//

#ifndef REACTOR_FDEVENTHANDERBASE_H
#define REACTOR_FDEVENTHANDERBASE_H

#include "NonCopyable.h"
class Reactor;
/* **********************************
 *             处理FD类的基类
 * 当该fd被触发时，调用处理函数来进行处理
 * ***********************************/
class FDEventHanderBase: public NonCopyable
{
public:
    FDEventHanderBase(int fd, Reactor* pReactor) :
            m_fd(fd),
            m_pReactor(pReactor)
    {
    }
    virtual ~FDEventHanderBase() {
    }
    // 读被触发时，调用该函数
    virtual void OnFDRead() = 0;
    // 写被触发时，调用该函数
    virtual void OnFDWrite() = 0;
    // 关闭fd
    virtual void Close() = 0;
    // 注册读事件
    int RegisterReadEvent();
    // 注册写事件
    int RegisterWriteEvent();
    // 注销读事件
    int UnRegisterReadEvent();
    // 注销写事件
    int UnRegisterWriteEvent();
    int GetFD() const;
    void SetFD(int fd);
    // 设置非阻塞，返回0，success.
    int SetNonBlock(int fd, bool bNonBlock = true);
protected:
    int                 m_fd;//该handler所对应的fd.
    Reactor             *m_pReactor;
};

#endif //REACTOR_FDEVENTHANDERBASE_H
