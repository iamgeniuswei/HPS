#include <iostream>

using namespace std;

#include <netinet/in.h>
#include <arpa/inet.h>
#include "basic_api.h"
enum BYTEORDER{
    BIGENDIAN,
    LITTLEENDIAN,
    UNKNOWN
};

BYTEORDER byteorder()
{
    union{
        short value;
        char bytes[sizeof (short)];
    }order;
    order.value = 0x0102;
    if((order.bytes[0] == 1) && (order.bytes[1] == 2))
        return BIGENDIAN;
    else if ((order.bytes[0] == 2) && (order.bytes[1] == 1)) {
        return LITTLEENDIAN;
    }
    else {
        return UNKNOWN;
    }
}


struct order{
    char bytes[4];
};


void byter_order_test()
{
    int src = 0x01020304;

    cout.setf(ios::showbase);
    cout.setf(ios_base::hex,   ios_base::basefield);
    order *host_order = (order*)&src;
    cout <<(int) host_order->bytes[0] << " "
                          << (int)host_order->bytes[1] << " "
                          << (int)host_order->bytes[2] << " "
                          << (int)host_order->bytes[3] << endl;
    int nsrc = htonl(src);
    order *net_order = (order*)&nsrc;
    cout <<(int) net_order->bytes[0] << " "
                          << (int)net_order->bytes[1] << " "
                          << (int)net_order->bytes[2] << " "
                          << (int)net_order->bytes[3] << endl;
    cout.unsetf(ios::hex);
}


void ip_str_test()
{
    in_addr_t ip_int = inet_addr("1.2.3.4");
    cout.setf(ios::showbase);
    cout.setf(ios_base::hex,   ios_base::basefield);
    order *host_order = (order*)&ip_int;
    cout <<(int) host_order->bytes[0] << " "
                          << (int)host_order->bytes[1] << " "
                          << (int)host_order->bytes[2] << " "
                          << (int)host_order->bytes[3] << endl;
    cout.unsetf(ios::hex);
    in_addr addr;
    addr.s_addr = ip_int;
    char *host_ip = inet_ntoa(addr);
    in_addr_t ip_int_2 = inet_addr("11.22.33.44");
    in_addr addr2;
    addr2.s_addr = ip_int_2;
    char *host_ip_2 = inet_ntoa(addr2);
    cout << host_ip << endl;
    cout << host_ip_2 << endl;
    int ip_int_3;
    int rst = inet_pton(AF_INET, "2.3.4.5", &ip_int_3);
    cout.setf(ios::showbase);
    cout.setf(ios_base::hex,   ios_base::basefield);
    order *host_order_2 = (order*)&ip_int_3;
    cout <<(int) host_order_2->bytes[0] << " "
                          << (int)host_order_2->bytes[1] << " "
                          << (int)host_order_2->bytes[2] << " "
                          << (int)host_order_2->bytes[3] << endl;
    cout.unsetf(ios::hex);


}

int main()
{
    cout << "Hello World!" << endl;
    cout << "Byte order " << byteorder() << endl;
    byter_order_test();
    ip_str_test();
//    test_backlog();
//    test_oob_recv();

//    test_gethostbyname();
//    test_cgi();
//    test_web_server();
//    test_cpu_copy();
    test_zero_copy();
    return 0;
}
