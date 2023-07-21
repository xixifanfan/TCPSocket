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
    
    u_short port;//�˿�
    
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
            cout << "�����socket����ʧ��" << '\n';
            return INVALID_SOCKET;
        }
        //�󶨷����IP��˿�
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.S_un.S_addr = ADDR_ANY;

        if (bind(fd, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
        {
            cout << "�˿ڰ�ʧ��" << '\n';
            return SOCKET_ERROR;
        }
        //�����ͻ���
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
            cout << "�ͻ���socket����ʧ��" << '\n';
            return INVALID_SOCKET;
        }
        //�󶨿ͻ���IP��˿�
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        //addr.sin_addr.S_un.S_addr = inet_addr(ip);
        InetPton(AF_INET,ip,&addr.sin_addr);
        if (connect(fd, (sockaddr*)&addr, sizeof(addr)) == INVALID_SOCKET)
        {
            cout << "�ͻ������ӷ�����ʧ��" << '\n';
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
                cout << "�ر�socket����ʧ�ܣ�" << '\n';
            }
           // cout << "������" << endl;
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
                if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)     //windows�첽�׽�������
                {
                    cout << "����socket����ʧ��" << '\n';
                }
            }
        }
        return tcp;
    }
};
TCPSocket* TCPInit::tcp = nullptr;


#endif // !_TCPSOCKET_H_
