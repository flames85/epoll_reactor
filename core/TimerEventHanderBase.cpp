//
// Created by shaoqi on 17-3-8.
//

#include "TimerEventHanderBase.h"
#include "LTReactor.h"

/* **********************************
 *        处理timer类的基类
 *    当timer被触发时，调用该类的函数
 * **********************************/
// 注册定时器，@msec，多长时间响一次,单位:毫秒
// 由于epoll精度的限制，这个地方，精度只能达到 毫秒，但不是准确
int TimerEventHanderBase::RegisterTimer(unsigned int msec /*ms*/, bool restart)
{
    m_bRestart = restart;
    
    m_interval.tv_sec = msec / 1000;
    m_interval.tv_usec = msec % 1000;
    timeval now;
    gettimeofday(&now, NULL);
    timeradd(&now, &m_interval, &m_endtime);
    return m_pReactor->RegisterTimer(this);
}
int TimerEventHanderBase::UnRegisterTimer() {
    return m_pReactor->UnRegisterTimer(this);
}
int TimerEventHanderBase::RegisterTimerAgain() {
    if (timerisset(&m_endtime) && timerisset(&m_interval))
    {
        timeval now;
        gettimeofday(&now, NULL);
        timeradd(&now, &m_interval, &m_endtime);
        return m_pReactor->RegisterTimer(this);
    }
    return -1;
}