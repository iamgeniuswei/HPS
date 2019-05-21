#ifndef BASIC_API_H
#define BASIC_API_H
extern void test_backlog();
extern void test_oob_recv();
extern void test_gethostbyname();
extern void test_cgi();
extern void test_web_server();
extern int initialize_server();
extern void test_zero_copy();
extern void test_cpu_copy();
#endif // BASIC_API_H
