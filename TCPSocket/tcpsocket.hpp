#ifndef _TCPSOCKET_H_
#define _TCPSOCKET_H_

#include<iostream>
#include<shared_mutex>
#include<thread>

#include<winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib,"ws2_32.lib")
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
using namespace std;
class TCPInit;
class TCPSocket
{
    friend class TCPInit;
private:
    shared_mutex sharedMtx;
    
    u_short port;//端口
    
    TCPSocket(u_short port = 1024) :port(port) {}
public:
    
    TCPSocket(const TCPSocket&) = delete;
    TCPSocket& operator=(const TCPSocket&) = delete;
    TCPSocket(TCPSocket&&) = delete;
    TCPSocket& operator=(TCPSocket&&) = delete;

    void setPort(u_short newPort)noexcept { shared_lock<shared_mutex>lock(sharedMtx);port = newPort; }
    u_short getPort()noexcept { shared_lock<shared_mutex>lock(sharedMtx);return port; }
    SOCKET createServerSocket(u_short backlog=10)
    {
        shared_lock<shared_mutex>lock(sharedMtx);
        SOCKET fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (fd == INVALID_SOCKET)
        {
            cout << "服务端socket创建失败" << '\n';
            return INVALID_SOCKET;
        }
        //绑定服务端IP与端口
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.S_un.S_addr = ADDR_ANY;

        if (bind(fd, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
        {
            cout << "端口绑定失败" << '\n';
            return SOCKET_ERROR;
        }
        //监听客户端
        listen(fd, backlog);
        return fd;
    }
    SOCKET createClientSocket(const wchar_t* ip=L"127.0.0.1")
    {
        shared_lock<shared_mutex>lock(sharedMtx);
        //PCWSTR s = CString(ip);
        SOCKET fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (fd == INVALID_SOCKET)
        {
            cout << "客户端socket创建失败" << '\n';
            return INVALID_SOCKET;
        }
        //绑定客户端IP与端口
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        //addr.sin_addr.S_un.S_addr = inet_addr(ip);
        InetPton(AF_INET,ip,&addr.sin_addr);
        if (connect(fd, (sockaddr*)&addr, sizeof(addr)) == INVALID_SOCKET)
        {
            cout << "客户端连接服务器失败" << '\n';
            return INVALID_SOCKET;
        }
        return fd;
    }
};
shared_mutex sharedMtx;
class TCPInit
{
private:
    static TCPSocket* tcp;
    struct GCForTCPSocket
    {
        ~GCForTCPSocket()
        {
            delete tcp;
            if (WSACleanup() != 0)
            {
                cout << "关闭socket服务失败！" << '\n';
            }
           // cout << "我无了" << endl;
        }
    };
    
    
    TCPInit();
public:
    static TCPSocket* getInstance()
    {
        if (tcp == nullptr)
        {
            shared_lock<shared_mutex>lock(sharedMtx);
            if (tcp == nullptr)
            {
                tcp = new TCPSocket();
                static GCForTCPSocket gc;
                WSADATA wsadata;
                if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)     //windows异步套接字启动
                {
                    cout << "启动socket服务失败" << '\n';
                }
            }
        }
        return tcp;
    }
};
TCPSocket* TCPInit::tcp = nullptr;


#endif // !_TCPSOCKET_H_
