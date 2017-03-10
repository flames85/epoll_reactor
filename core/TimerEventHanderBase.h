//
// Created by shaoqi on 17-3-8.
//

#ifndef REACTOR_TIMEREVENTHANDERBASE_H
#define REACTOR_TIMEREVENTHANDERBASE_H

#include "NonCopyable.h"
#include <sys/time.h>

class Reactor;

/* **********************************
 *        处理timer类的基类
 *    当timer被触发时，调用该类的函数
 * **********************************/
class TimerEventHanderBase: public NonCopyable
{
public:
    TimerEventHanderBase(Reactor* reactor) :
            m_pReactor(reactor),
            m_bRestart(false)
    {
        timerclear(&m_interval);
        timerclear(&m_endtime);
    }
    virtual ~TimerEventHanderBase() {
    }
    virtual void OnTimeOut() = 0;
    // 注册定时器，@msec，多长时间响一次,单位:毫秒
    // 由于epoll精度的限制，这个地方，精度只能达到 毫秒，但不是准确
    int RegisterTimer(unsigned int msec /*ms*/, bool restart = false);
    int UnRegisterTimer();
    int RegisterTimerAgain();
    // 获取结束时间
    const timeval& GetEndTime() const {
        return m_endtime;
    }
    // 该定时器是否需要重启
    bool IsRestart() const {
        return m_bRestart;
    }
protected:
    Reactor             *m_pReactor;
    timeval             m_interval;// 时间间隔
    timeval             m_endtime; // 定时器到期时间
    bool                m_bRestart;// 是否自动重启
};


#endif //REACTOR_TIMEREVENTHANDERBASE_H
