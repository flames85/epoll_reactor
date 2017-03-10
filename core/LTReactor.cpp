//
// Created by shaoqi on 17-3-8.
//

#include <iostream>
#include <sys/epoll.h>
#include <errno.h>
#include <cstring>
#include <zconf.h>
#include <sys/time.h>
#include "LTReactor.h"
#include "FDEventHanderBase.h"


LTReactor::LTReactor() :
        m_epfd(-1),
        m_bRunning(false)
{
}

LTReactor::~LTReactor() {
}

bool LTReactor::Init() {
    m_epfd = epoll_create(MAX_EPOLL_SIZE);
    if (m_epfd == -1)
    {
        std::cout << __LINE__ << " epoll_ctl error: " << strerror(errno) << std::endl;
        return false;
    }
    
    m_bRunning = true;
    return true;
}

bool LTReactor::Run() {
    struct epoll_event events[MAX_EPOLL_EVENTS_SIZE];
    int nfds = 0; //返回被触发的事件的个数
    while (true)
    {
        // 需要优化成下次超时的时间
        // 超时时间，若m_bRunning，立即返回；否则100ms，即0.1s. 实际上，epoll_wait，只能够精确到毫秒(1/1000)
        nfds = epoll_wait(m_epfd, events, sizeof(events) / sizeof(events[0]),
                          m_bRunning ? 100 : 1);
        // 要停止epoll.
        if (m_bRunning == false)
        {
            timeval now;
            gettimeofday(&now, NULL); //获取当前时间
            if (timercmp(&now, &m_stoptime, >))
            {
                break;
            }
        }
        // epoll 出错
        if (nfds == -1)
        {
            std::cout << "epoll_wait error: " << strerror(errno) << std::endl;
            continue;
        }
        // 处理被触发的事件
        for (int i = 0; i < nfds; ++i)
        {
            int type = 0;
            int fd = events[i].data.fd;
            if (fd < 0) //fd出错
            {
                std::cout << "the FD is " << fd << std::endl;
                continue;
            }
            FDEventHanderBase* pBase = m_handlerSet.GetFDEventHandler(fd, type);
            if (NULL == pBase)
            {
                std::cout << "pBase is NULL, fd is" << fd << std::endl;
                continue;
            }
            
            if (events[i].events & EPOLLIN)//read events
            {
                pBase->OnFDRead();
            }
            if (events[i].events & EPOLLOUT) //write events
            {
                pBase->OnFDWrite();
            }
            if (events[i].events & EPOLLERR) // error events
            {
                pBase->Close();
            }
        }
        // 处理定时器事件
        ScanTimer();
        // 处理idle事件
    } // end of while
    
    std::cout << "stop epoll" << std::endl;
    close(m_epfd); //close epoll fd
    m_epfd = -1;
    
    return true;
}

bool LTReactor::Stop() {
    m_bRunning = false;
    
    timeval val, now;
    val.tv_usec = 50 * 1000; //50ms后停止
    gettimeofday(&now, NULL);
    timeradd(&val, &now, &m_stoptime);
    return true;
}
// 注册读事件,
// @return: 0, success. else failed.
int LTReactor::RegisterReadEvent(FDEventHanderBase* pBase) {
    int type = 0;
    FDEventHanderBase* pHander = m_handlerSet.GetFDEventHandler(pBase->GetFD(),
                                                                type);
    
    epoll_event event;
    event.data.fd = pBase->GetFD();
    event.events = type | EPOLLIN; // 注册读事件
    
    int iRet = epoll_ctl(m_epfd, NULL == pHander ? EPOLL_CTL_ADD
                                                 : EPOLL_CTL_MOD, pBase->GetFD(), &event);
    if (iRet == -1)
    {
        std::cout << __LINE__ << " epoll_ctl error: " << strerror(errno) << std::endl;
        return -1;
    }
    
    m_handlerSet.AddFDEventHandler(pBase, EPOLLIN);
    return 0;
}
// 注册写事件
// @return: 0, success. else failed.
int LTReactor::RegisterWriteEvent(FDEventHanderBase* pBase) {
    int type = 0;
    FDEventHanderBase* pHandler = m_handlerSet.GetFDEventHandler(pBase->GetFD(), type);
    
    epoll_event event;
    event.data.fd = pBase->GetFD();
    event.events = type | EPOLLOUT; //注册写事件
    
    int iRet = epoll_ctl(m_epfd, NULL == pHandler ? EPOLL_CTL_ADD
                                                  : EPOLL_CTL_MOD, pBase->GetFD(), &event);
    if (iRet == -1)
    {
        std::cout << __LINE__ << " epoll_ctl error: " << strerror(errno) << std::endl;
        return -1;
    }
    
    m_handlerSet.AddFDEventHandler(pBase, EPOLLOUT);
    return 0;
}
// 注销读事件
// @return: 0, success. else failed.
int LTReactor::UnRegisterReadEvent(FDEventHanderBase* pBase) {
    int type = 0;
    FDEventHanderBase* pHandler = m_handlerSet.GetFDEventHandler(pBase->GetFD(), type);
    epoll_event event;
    event.data.fd = pBase->GetFD();
    event.events = (type & ~EPOLLIN); // 取消读事件
    
    int iRet = epoll_ctl(m_epfd, NULL == pHandler ? EPOLL_CTL_DEL
                                                  : EPOLL_CTL_MOD, pBase->GetFD(), &event);
    if (iRet == -1)
    {
        std::cout << __LINE__ << " epoll_ctl error: " << strerror(errno) << std::endl;
        return -1;
    }
    
    // 这里不删除的原因是epoll_ctl也是不删除的逻辑,所以我们暂时先这么改应付测试
    m_handlerSet.DelFDEventHandler(pBase->GetFD(), EPOLLIN);
    return 0;
}

// 注销写事件
// @return: 0, success. else failed.
int LTReactor::UnRegisterWriteEvent(FDEventHanderBase* pBase) {
    int type = 0;
    FDEventHanderBase* pHandler = m_handlerSet.GetFDEventHandler(pBase->GetFD(), type);
    epoll_event event;
    event.data.fd = pBase->GetFD();
    event.events = (type & ~EPOLLOUT); // 取消写事件
    
    int iRet = epoll_ctl(m_epfd, NULL == pHandler ? EPOLL_CTL_DEL
                                                  : EPOLL_CTL_MOD, pBase->GetFD(), &event);
    if (iRet == -1)
    {
        std::cout << __LINE__ << " epoll_ctl error: " << strerror(errno) << std::endl;
        return -1;
    }
    
    m_handlerSet.DelFDEventHandler(pBase->GetFD(), EPOLLOUT);
    return 0;
}
// 注销读写事件
int LTReactor::UnRegisterAllEvent(FDEventHanderBase* pBase) {
    epoll_event event;
    event.data.fd = pBase->GetFD();
    event.events = EPOLLOUT | EPOLLIN; // 取消读写事件
    int iRet = epoll_ctl(m_epfd, EPOLL_CTL_DEL, pBase->GetFD(), &event);
    if (iRet == -1)
    {
        std::cout << __LINE__ << " epoll_ctl error: " << strerror(errno) << std::endl;
        return -1;
    }
    
    m_handlerSet.DelFDEventHandler(pBase->GetFD(), EPOLLOUT | EPOLLIN);
    return 0;
}
// 注册定时器
int LTReactor::RegisterTimer(TimerEventHanderBase* pBase) {
    m_handlerSet.AddTimerEventHandler(pBase);
    return 0;
}
// 注销定时器
int LTReactor::UnRegisterTimer(TimerEventHanderBase* pBase) {
    m_handlerSet.DelTimerEventHandler(pBase);
    return 0;
}
// 扫描，看是否有定时器的时间到了
void LTReactor::ScanTimer() {
    m_handlerSet.ScanTimer();
}
