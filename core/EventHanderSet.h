//
// Created by shaoqi on 17-3-8.
//

#ifndef REACTOR_EVENTHANDERSET_H
#define REACTOR_EVENTHANDERSET_H

#include <map>
#include "MinHeap.h"
class FDEventHanderBase;
class TimerEventHanderBase;

/* **********************************
 *             处理类的集合
 *     有个对应关系 -- fd -- 处理函数 -- read/write.
 * *********************************/
class EventHanderSet
{
public:
    // 增加fd事件, @type : EPOLLIN, EPOLLOUT，
    void AddFDEventHandler(FDEventHanderBase* pHandler, int type);
    // @type : EPOLLIN, EPOLLOUT
    void DelFDEventHandler(int fd, int type);
    // 根据fd，找到 FDEventHanderBase，及type.
    FDEventHanderBase* GetFDEventHandler(int fd, int& type);
public:
    void AddTimerEventHandler(TimerEventHanderBase* pHandler);
    void DelTimerEventHandler(TimerEventHanderBase* pHandler);
    void ScanTimer();
private:
    struct FDHandler
    {
        int type; //EPOLLIN, EPOLLOUT
        FDEventHanderBase* pHandler;
    };
    std::map<int, FDHandler>                                                m_fdmap; // fd -- type -- handler
    
    // 用于保存timer，在同一时刻，可能有多个handler.
    MinHeap<std::pair<long int, long int> >                                 m_timerMinHeap;
    std::multimap<std::pair<long int, long int>, TimerEventHanderBase*>     m_timerMultiMap;
};


#endif //REACTOR_EVENTHANDERSET_H
