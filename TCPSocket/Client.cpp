#include <iostream>
#include <iostream>
#include "..\TcpSocket\tcpsocket.hpp"
const int MAX_N = 1024;
int main()
{
    TCPSocket* tcp = TCPInit::getInstance();
    SOCKET fd = tcp->createClientSocket();
    cout << "连接服务器成功！！！" << '\n';
    while (1)
    {
        char se[MAX_N] = { 0 };
        gets_s(se, MAX_N);
        if (string(se) == string("break"))break;
        send(fd, se, strlen(se),0);
        char buff[MAX_N] = { 0 };
        int ret = recv(fd, buff, sizeof(buff), 0);
        if (ret > 0)
        {
            cout << "来自服务端的信息：" << buff << '\n';
        }
        else
        {
            cout << "连接断开~~~~~~" << '\n';
            break;
        }
    }
    cout << "与服务端断开连接~~~" << '\n';
    closesocket(fd);
    
    getchar();
}

