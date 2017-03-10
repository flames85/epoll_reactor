//
// Created by shaoqi on 17-3-8.
//

#ifndef REACTOR_MINHEAP_H
#define REACTOR_MINHEAP_H

#include <map>

template <class T>
class MinHeap {
public:
    void Insert(const T &t) {
        long value = t.first*1000000 + t.second;
        m_heap[value] = t;
    }
    bool IsEmpty() {
        return m_heap.empty();
    }
    T GetMin() {
        if(m_heap.empty()) {
            return T();
        }
        return m_heap.begin()->second;
    }
    void RemoveMin(){
        if(!m_heap.empty())
            m_heap.erase(m_heap.begin());
    }
private:
    std::map<long, T>   m_heap;
};


#endif //REACTOR_MINHEAP_H
