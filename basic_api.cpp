#include "basic_api.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <iostream>

static bool stop = false;
static void handle_term(int sig)
{
    stop = true;
}


void test_backlog()
{
    signal(SIGINT, handle_term);
    std::cout << "test_backlog() is running..." << std::endl;
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock >= 0);
    sockaddr_in sin;
    bzero(&sin, sizeof (sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(12345);
    sin.sin_addr.s_addr = 0;

    int ret = bind(sock, (sockaddr*)&sin, sizeof (sin));
    assert(ret != -1);

    ret = listen(sock, 3);
    assert(ret != -1);

    while (!stop) {
        sleep(1);
    }
    std::cout << "recv sigint, exit()" << std::endl;
    close(sock);

}


void test_oob_recv()
{
    sockaddr_in address;
    bzero(&address, sizeof (address));
    address.sin_family = AF_INET;
    address.sin_port = htons(12345);
    address.sin_addr.s_addr = 0;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    assert(sock >= 0);

    int ret = bind(sock, (sockaddr*)&address, sizeof (address));
    assert(ret != -1);

    ret = listen(sock, 5);
    assert(ret != -1);
    sockaddr_in client;
    socklen_t client_len = sizeof (client);
    int conn = accept(sock, (sockaddr*)&client, &client_len);

    assert(conn >= 0);

    //getperrname()

    in_addr addr2 = client.sin_addr;
    char *host_ip_2 = inet_ntoa(addr2);

    std::cout << host_ip_2 << std::endl;

    char buffer[BUFSIZ];
    memset(buffer, 0, BUFSIZ);
    ret = recv(conn, buffer, BUFSIZ-1, 0);

    std::cout << ret << "  " << buffer << std::endl;

    memset(buffer, 0, BUFSIZ);
    ret = recv(conn, buffer, BUFSIZ-1, MSG_OOB);
    std::cout << ret << "  " << buffer << std::endl;

    memset(buffer, 0, BUFSIZ);
    ret = recv(conn, buffer, BUFSIZ-1, 0);

    std::cout << ret << "  " << buffer << std::endl;
//    close(conn);

//    close(sock);

}


#include <netdb.h>
using namespace std;
void test_gethostbyname()
{
    hostent *baidu = gethostbyname("www.baidu.com");
    std::cout << baidu->h_name << std::endl;
    std::cout << baidu->h_addrtype << std::endl;
    std::cout << baidu->h_length << std::endl;
    int index = 0;
    while (baidu->h_addr_list[index] != NULL)
    {
        struct order{
            char bytes[4];
        };

        cout.setf(ios::showbase);
        cout.setf(ios_base::hex,   ios_base::basefield);
        order *host_order = (order*)(in_addr*)baidu->h_addr_list[index];
        cout <<(int) host_order->bytes[0] << " "
                              << (int)host_order->bytes[1] << " "
                              << (int)host_order->bytes[2] << " "
                              << (int)host_order->bytes[3] << endl;

        std::cout << inet_ntoa(*((in_addr*)baidu->h_addr_list[index])) << std::endl;
        index++;
    }

}

void test_cgi()
{
    sockaddr_in addr;
    bzero(&addr, sizeof (addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = 0;
    addr.sin_port = htons(12345);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    assert(sock >= 0);
    int ret = bind(sock, (sockaddr*)&addr, sizeof (addr));
    assert(ret != -1);

    ret = listen(sock, 5);
    assert(ret != -1);
    sockaddr_in client;
    socklen_t client_addr_len = sizeof (client);
    int connfd = accept(sock, (sockaddr*)&client, &client_addr_len);
    assert(connfd >= 0);

    close(STDOUT_FILENO);
    dup(connfd);
    std::cout << "<title>百度一下，你就知道 </title>" << endl;
    close(connfd);
}

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/uio.h>
void test_web_server()
{
    const char* status_line[2] = {"200 OK",
                                 "500 Internal server error"};
    const char* file_name = "/home/developer/Documents/1.txt";
    int sock = initialize_server();
    sockaddr_in client;
    socklen_t client_addr_len = sizeof (client);
    int connfd = accept(sock, (sockaddr*)&client, &client_addr_len);
    assert(connfd >= 0);

    char header_buf[1024];
    memset(header_buf, 0, 1024);
    char *file_buf;
    struct stat file_stat;
    stat(file_name, &file_stat);

    bool valid = true;
    int len = 0;

    int fd = open(file_name, O_RDONLY);
    file_buf = new char [file_stat.st_size + 1];
    memset(file_buf, 0, file_stat.st_size + 1);
    if(read(fd, file_buf, file_stat.st_size) < 0)
        valid = false;

    if(valid)
    {
        int ret = snprintf(header_buf,
                           1023,
                           "%s %s\r\n",
                           "HTTP/1.1",
                           status_line[0]);
        len += ret;
        ret = snprintf(header_buf+len, 1023-len,
                       "Content-Length: %d\r\n",
                       file_stat.st_size);
        len += ret;
        ret = snprintf(header_buf+len,
                       1023-len,
                       "%s",
                       "\r\n");

        iovec iv[2];
        iv[0].iov_base = header_buf;
        iv[0].iov_len = strlen(header_buf);
        iv[1].iov_base = file_buf;
        iv[1].iov_len = file_stat.st_size;

        ret = writev(connfd, iv, 2);
    }
    else
    {
        int ret = snprintf(header_buf,
                           1023,
                           "%s %s\r\n",
                           "HTTP/1.1",
                           status_line[1]);
        ret = snprintf(header_buf+len,
                       1023-len,
                       "%s",
                       "\r\n");
        send(connfd, header_buf, strlen(header_buf), 0);
    }
    close(connfd);
    delete [] file_buf;
    close(sock);

}

int initialize_server()
{
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = 0;
    addr.sin_port = htons(12345);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof (opt));
    assert(sock >= 0);
    int ret = bind(sock, (sockaddr*)&addr, sizeof (addr));
    assert(ret != -1);
    ret = listen(sock, 5);
    assert(ret != -1);
    return  sock;
}
#include <fstream>
#include <chrono>
void test_cpu_copy()
{
    int sock = initialize_server();
    sockaddr_in client;
    socklen_t client_addr_len = sizeof (client);
    int connfd = accept(sock, (sockaddr*)&client, &client_addr_len);
    assert(connfd >= 0);
    fstream fs;
    fs.open("/home/developer/Downloads/1.iso",
            fstream::in|fstream::binary);
    fs.seekg(0, fs.end);


    ssize_t size = fs.tellg();
    fs.seekg(0, fs.beg);
    char buf[1024*1024];
    typedef chrono::time_point<chrono::system_clock, chrono::nanoseconds> nanoClock;
    nanoClock tp = chrono::time_point_cast<chrono::nanoseconds>(chrono::system_clock::now());
    cout << tp.time_since_epoch().count() << endl;
    while (size >0)
    {
        memset(buf, 0, 1024*1024);
        fs.read(buf, 1024*1024);
        int ret = send(connfd, buf, 1024*1024, 0);
        if(fs)
            size -= 1024*1024;
        else {
            size -= fs.gcount();
        }
    }
    tp = chrono::time_point_cast<chrono::nanoseconds>(chrono::system_clock::now());
    cout << tp.time_since_epoch().count() << endl;
}

#include <sys/sendfile.h>
void test_zero_copy()
{
    int sock = initialize_server();
    sockaddr_in client;
    socklen_t client_addr_len = sizeof (client);
    int connfd = accept(sock, (sockaddr*)&client, &client_addr_len);
    assert(connfd >= 0);

    int filefd = open("/home/developer/Downloads/1.iso", O_RDONLY);
    struct stat stat_file;
    fstat(filefd, &stat_file);
    typedef chrono::time_point<chrono::system_clock, chrono::nanoseconds> nanoClock;
    nanoClock tp = chrono::time_point_cast<chrono::nanoseconds>(chrono::system_clock::now());
    cout << tp.time_since_epoch().count() << endl;
    sendfile(connfd, filefd, NULL, stat_file.st_size);
    tp = chrono::time_point_cast<chrono::nanoseconds>(chrono::system_clock::now());
    cout << tp.time_since_epoch().count() << endl;
    close(connfd);
    close(sock);
}
