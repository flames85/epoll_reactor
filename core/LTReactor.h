//
// Created by shaoqi on 17-3-8.
//

#ifndef REACTOR_REACTOR_H
#define REACTOR_REACTOR_H

#define MAX_EPOLL_SIZE 1024
#define MAX_EPOLL_EVENTS_SIZE 128

#include "NonCopyable.h"
#include "EventHanderSet.h"

class TimerEventHanderBase;

class Reactor: public NonCopyable
{
public:
    Reactor() {
    }
    
    virtual ~Reactor() {
    }
    
    virtual bool Init() = 0;
    virtual bool Run() = 0;
    virtual bool Stop() = 0;
    virtual int RegisterReadEvent(FDEventHanderBase* pBase) = 0;
    virtual int RegisterWriteEvent(FDEventHanderBase* pBase) = 0;
    virtual int UnRegisterReadEvent(FDEventHanderBase* pBase) = 0;
    virtual int UnRegisterWriteEvent(FDEventHanderBase* pBase) = 0;
    virtual int RegisterTimer(TimerEventHanderBase* pBase) = 0;
    virtual int UnRegisterTimer(TimerEventHanderBase* pBase) = 0;
};

//////////// LT epoll /////////////////
class LTReactor: public Reactor
{
public:
    LTReactor();
    virtual ~LTReactor();
    
    bool Init();
    bool Run();
    bool Stop();
    //注册读事件
    int RegisterReadEvent(FDEventHanderBase* pBase);
    //注册写事件
    int RegisterWriteEvent(FDEventHanderBase* pBase);
    
    int UnRegisterReadEvent(FDEventHanderBase* pBase);
    int UnRegisterWriteEvent(FDEventHanderBase* pBase);
    int UnRegisterAllEvent(FDEventHanderBase* pBase);
    // 注册定时器
    int RegisterTimer(TimerEventHanderBase* pBase);
    int UnRegisterTimer(TimerEventHanderBase* pBase);
private:
    int ScanTimer();
private:
    int             m_epfd; //epoll's fd
    bool            m_bRunning;
    timeval         m_stoptime; //停止时间
    EventHanderSet  m_handlerSet;
};


#endif //REACTOR_REACTOR_H
