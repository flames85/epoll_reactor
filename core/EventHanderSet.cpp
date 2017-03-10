//
// Created by shaoqi on 17-3-8.
//
#include <sys/time.h>
#include "EventHanderSet.h"
#include "FDEventHanderBase.h"
#include "TimerEventHanderBase.h"

/* **********************************
 *             处理类的集合
 *         有个对应关系 -- fd -- 处理函数 -- read/write.
 * *********************************/
// 增加fd事件, @type : EPOLLIN, EPOLLOUT，
void EventHanderSet::AddFDEventHandler(FDEventHanderBase* pHandler, int type) {
    const int fd = pHandler->GetFD();
    std::map<int, FDHandler>::iterator it = m_fdmap.find(fd);
    if (it != m_fdmap.end())
    {
        it->second.type |= type;
        it->second.pHandler = pHandler;
    }
    else
    {
        FDHandler th;
        th.type = type;
        th.pHandler = pHandler;
        m_fdmap.insert(std::make_pair(fd, th));
    }
}
// @type : EPOLLIN, EPOLLOUT
void EventHanderSet::DelFDEventHandler(int fd, int type) {
    std::map<int, FDHandler>::iterator it = m_fdmap.find(fd);
    if (it != m_fdmap.end())
    {
        it->second.type &= ~type;//去掉该属性
        // 为了与epoll_ctl一致, 不删除
//        if (0 == it->second.type) //若所有的属性都去掉了，从map中删除该handler.
//        {
//            m_fdmap.erase(it);
//        }
    }
}
// 根据fd，找到 FDEventHanderBase，及type.
FDEventHanderBase* EventHanderSet::GetFDEventHandler(int fd, int& type) {
    std::map<int, FDHandler>::iterator it = m_fdmap.find(fd);
    if (it != m_fdmap.end())
    {
        type = it->second.type;
        return it->second.pHandler;
    }
    
    return NULL;
}

void EventHanderSet::AddTimerEventHandler(TimerEventHanderBase* pHandler) {
    timeval endtime = pHandler->GetEndTime();
    std::pair<long int, long int> t(endtime.tv_sec, endtime.tv_usec);
//    printf("addTimer: [%u:%u] \n", (unsigned int) t.first, (unsigned int) t.second);
    m_timerMinHeap.Insert(t);
    m_timerMultiMap.insert(std::make_pair(t, pHandler));
}
void EventHanderSet::DelTimerEventHandler(TimerEventHanderBase* pHandler) {
    timeval endtime = pHandler->GetEndTime();
    std::pair<long int, long int> t(endtime.tv_sec, endtime.tv_usec);
    std::multimap<std::pair<long int, long int>, TimerEventHanderBase*>::iterator
    beg = m_timerMultiMap.lower_bound(t);
    std::multimap<std::pair<long int, long int>, TimerEventHanderBase*>::iterator
    end = m_timerMultiMap.upper_bound(t);
    for (; beg != end; ++beg)
    {
        if (beg->second == pHandler)
        {
            m_timerMultiMap.erase(beg);
            break;
        }
    }
}


int EventHanderSet::ScanTimer()
{
    timeval minTimeVal;
    while (true)
    {
        // printf("m_timerMinHeap: m_max_size[%d] m_cur_size[%d] \n",
        // m_timerMinHeap.GetMaxSize(), m_timerMinHeap.GetCurSize());
        if(m_timerMinHeap.IsEmpty()) return -1;
    
        std::pair<long int, long int> minTime = m_timerMinHeap.GetMin(); //获取最小时间
        minTimeVal.tv_sec = minTime.first;
        minTimeVal.tv_usec = minTime.second;
    
        timeval now = { 0, 0 };
        gettimeofday(&now, NULL);
        if (timercmp(&now, &minTimeVal, >=)) //当前时间>=触发时间
        {
            m_timerMinHeap.RemoveMin(); // 去掉最小堆里面的时间
            std::multimap<std::pair<long int, long int>,
                    TimerEventHanderBase*>::iterator beg = m_timerMultiMap.lower_bound(minTime);
        
            for (; beg != m_timerMultiMap.upper_bound(minTime);)
            {
                TimerEventHanderBase* pHandler = beg->second;
                pHandler->OnTimeOut(); // 回调超时函数
                if (pHandler->IsRestart()) //需要重启
                {
                    AddTimerEventHandler(pHandler);
                }
                m_timerMultiMap.erase(beg++);//去掉multimap中的句柄
            }
        }
        else
        {
            timeval tv;
            tv.tv_sec = minTimeVal.tv_sec - now.tv_sec;;
            tv.tv_usec = minTimeVal.tv_usec - now.tv_usec;
            if ( tv.tv_usec < 0 )
            {
                tv.tv_usec += 1000000;
                tv.tv_sec--;
            }
            int timeoutMilliseconds = tv.tv_sec*1000 + tv.tv_usec/1000;
            return timeoutMilliseconds;
        }
    }
}