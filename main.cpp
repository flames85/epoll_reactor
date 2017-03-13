#include <stdio.h>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <tcpserver/TcpServerEventHandler.h>
#include <list>
#include "core/FDEventHanderBase.h"
#include "core/TimerEventHanderBase.h"
#include "core/LTReactor.h"
#include "tcpserver/SocketDataDecoderBase.h"
#include "tcpserver/TcpServerBase.h"
// tid
#include <sys/syscall.h>
#define gettid() syscall(SYS_gettid)

// 基础代码源自 http://blog.chinaunix.net/uid-26975042-id-3979456.html


class FileFDEventHander;

FileFDEventHander* g_fHandler = NULL;
LTReactor * g_Reactor = NULL;


//////////////post的callback函数////////////////
typedef void (*p_callback)(const std::string &msg);

void callbackStop(const std::string &msg)
{
    printf("[tid:%ld] [fun:%s][msg:%s]\n", gettid(), __func__, msg.c_str());
    g_Reactor->Stop();
}

void callback1(const std::string &msg)
{
    printf("[tid:%ld] [fun:%s][msg:%s]\n", gettid(), __func__, msg.c_str());
}

void callback2(const std::string &msg)
{
    printf("[tid:%ld] [fun:%s][msg:%s]\n", gettid(), __func__, msg.c_str());
}

void callback3(const std::string &msg)
{
    printf("[tid:%ld] [fun:%s][msg:%s]\n", gettid(), __func__, msg.c_str());
}

void callback4(const std::string &msg)
{
    printf("[tid:%ld] [fun:%s][msg:%s]\n", gettid(), __func__, msg.c_str());
}
//////////////////////////////

class FileFDEventHander: public FDEventHanderBase
{
public:
    FileFDEventHander(int fd, Reactor* pReactor) :
            FDEventHanderBase(fd, pReactor)
    {
        
    }
    
    void post(void(*callback)(const std::string &), const std::string &msg)
    {
        std::pair<p_callback, std::string> p(callback, msg);
        m_callbackPairList.push_back(p);
        write(GetFD(), "x", 1);
        this->RegisterReadEvent();
    }
    
    // 读被触发时，调用该函数
    virtual void OnFDRead() {
        char buf[1024] = {0};
        int ret = read(GetFD(), buf, sizeof(buf));
        std::list< std::pair<p_callback, std::string> >::iterator it = m_callbackPairList.begin();
        for(; ret-- && it != m_callbackPairList.end(); )
        {
            std::pair<p_callback, std::string> p = *it;
            p.first(p.second);
            m_callbackPairList.erase(it++);
        }
        this->UnRegisterReadEvent();
    }
    // 写被触发时，调用该函数
    virtual void OnFDWrite() {
        char buf[] = "this is a test file !";
        write(GetFD(), buf, strlen(buf));
//        this->UnRegisterWriteEvent();
    }
    // 关闭fd
    virtual void Close() {
        close(GetFD());
    }
    
private:
    std::list< std::pair<p_callback, std::string> >   m_callbackPairList;
            
};




class TimerEventHandler_test: public TimerEventHanderBase
{
public:
    explicit TimerEventHandler_test(Reactor * pReactor, const std::string &timerID) :
        TimerEventHanderBase(pReactor),
        m_timerID(timerID)
    {

    }
    void OnTimeOut() {
//        printf("[tid:%ld] [fun:%s]\n", gettid(), __func__);
        g_fHandler->post(callback3, "I am in " + m_timerID + " OnTimeOut");
    }
    
    std::string m_timerID;
};

/* *********************
 * 处理客户端发过来的数据
 * ********************/
class TcpDataDecoder_echo_test: public SocketDataDecoderBase
{
public:
    // 真正的处理数据的函数
    virtual int OnProcess(FDEventHanderBase* pSocket,
                          const char* buf,
                          unsigned int buf_len)
    {
        // 这个地方，将客户端的数据回写
        if (0 == pSocket->RegisterWriteEvent())
        {
            printf("[tid:%ld] [fun:%s][socket:%p] [recv:%s]\n", gettid(), __func__, pSocket, buf);
            write(pSocket->GetFD(), buf, buf_len);
            pSocket->UnRegisterWriteEvent();
        }
        
        return 0;
    }
};

void *childThread(void *arg)
{
    printf("[tid:%ld] [fun:%s] <--- new thread\n", gettid(), __func__);
    
    for(int i = 0; i < 5; ++i)
    {
        g_fHandler->post(callback4, "I am in childThread");
        sleep(5);
    }
    
    g_fHandler->post(callbackStop, "bye");
    return NULL;
}

int main()
{
    // reactor对象
    g_Reactor = new LTReactor;
    g_Reactor->Init();
    
    // 1. 测试异步tcpserver的socket通信
    TcpDataDecoder_echo_test* socket = new TcpDataDecoder_echo_test;
    TcpServerEventHandler server("127.0.0.1", 12345, socket, 0, g_Reactor);
    if (0 != server.OnListen()) {
        return -2;
    }
    
    // 2. 测试用pipe来发送post消息
    const char* path = "/home/shaoqi/test.txt";
    if(0 == mkfifo(path, 0777)) // 创建一个pipe.
    {
        return -3;
    }
    int fd = open(path, O_RDWR);;
    g_fHandler = new FileFDEventHander(fd, g_Reactor);
    // 主线程中post
    g_fHandler->post(callback1, "I am in main");
    g_fHandler->post(callback2, "I am in main too");
    
    // 子线程中post
    pthread_t ntid;
    int err = pthread_create(&ntid, NULL, childThread, NULL);
    if (err != 0)
    {
        printf("can't create thread: %s\n", strerror(err));
        return -1;
    }

    // 3. 测试异步定时器
    TimerEventHandler_test* tHandler1 = new TimerEventHandler_test(g_Reactor, "1");
    tHandler1->RegisterTimer(2000, false);
    TimerEventHandler_test* tHandler2 = new TimerEventHandler_test(g_Reactor, "2");
    tHandler2->RegisterTimer(4000, false);
    TimerEventHandler_test* tHandler3 = new TimerEventHandler_test(g_Reactor, "3");
    tHandler3->RegisterTimer(6000, false);
    TimerEventHandler_test* tHandler4 = new TimerEventHandler_test(g_Reactor, "4");
    tHandler4->RegisterTimer(16000, false);

    // 运行
    g_Reactor->Run();
    
    printf("process exit");
    
    return 0;
}