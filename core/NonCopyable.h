//
// Created by shaoqi on 17-3-8.
//

#ifndef REACTOR_NONCOPYABLE_H
#define REACTOR_NONCOPYABLE_H


class NonCopyable
{
protected:
    NonCopyable() {
    }
    virtual ~NonCopyable() {
    }
private:
    NonCopyable(const NonCopyable &); //禁止拷贝
    const NonCopyable & operator=(const NonCopyable &); //禁止赋值
};



#endif //REACTOR_NONCOPYABLE_H
