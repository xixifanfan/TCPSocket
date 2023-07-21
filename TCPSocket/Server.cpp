#include <iostream>
#include "..\TcpSocket\tcpsocket.hpp"
const int MAX_N = 1024;//传输最大字节
int main()
{
    TCPSocket* tcp = TCPInit::getInstance();
    //if (tcp)cout << "我存在！！！" << endl;
    SOCKET serfd = tcp->createServerSocket();
    cout << "等待客户端连接~~~~~~~~" << '\n';
    SOCKET clifd = accept(serfd, nullptr, nullptr);
    if (clifd == INVALID_SOCKET)
    {
        cout << "连接失败~~~~~~~" << '\n';
        getchar();
        closesocket(clifd);
        closesocket(serfd);
        return 0;
    }
    cout << "客户端连接成功！！！" << '\n';
    while (1)
    {
        char buff[MAX_N] = {0};
        int ret = recv(clifd, buff, sizeof(buff), 0);
        if (ret > 0)
        {
            cout <<"来自客户端的信息：" << buff << '\n';
        }
        else
        {
            cout << "连接断开~~~~~~" << '\n';
            break;
        }
        char se[MAX_N] = { 0 };
        gets_s(se,MAX_N);
        if (string(se) == string("break"))break;
        send(clifd, se, strlen(se), 0);
    }
    cout << "与客户端连接断开~~~~" << '\n';
    closesocket(clifd);
    closesocket(serfd);
    getchar();
}
