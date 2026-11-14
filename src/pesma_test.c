#include "pesma.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int counter = 0;
int g_failures = 0;

int tests_server();
int tests_read_write(PHandle* handle);
int tests_file();
void test_tcp_connect_fail(void);
void test_tcp_pingpong(void);

void printString(char* string);
void printChar(unsigned char c);

int main(int argc, char** argv)
{
    // Minimal test skeleton for pesma library
    // TODO: Improve testing kit, this is a jumbled mess

    printf("Pesma test skeleton running...\n");
    PHandle* handle = pesma_buffer_create(1024);
    int i = tests_read_write(handle);
    printf("%d\n", i);
    pesma_handle_free(handle);
    handle = NULL;

    printf("\n");
    i = tests_file();

    test_tcp_connect_fail();

    if(argc > 1 && strcmp(argv[1], "net") == 0)
        tests_server();

    test_tcp_pingpong();
    return g_failures ? 1 : 0;
}

void printString(char* string)
{
    int i = 0;
    while(string[i] != '\0') {
        printChar(string[i]);
        i++;
    }
}

void printChar(unsigned char theChar)
{

    switch(theChar) {

    case '\n': printf("\\n\n"); break;
    case '\r': printf("\\r"); break;
    case '\t': printf("\\t"); break;
    default:
        if((theChar < 0x20) || (theChar > 0x7f)) {
            printf("\\%03o", (unsigned char) theChar);
        }
        else {
            printf("%c", theChar);
        }
        break;
    }
}

#define CHECK_EQ(actual, expected)                          \
    do {                                                    \
        if((actual) != (expected)) {                        \
            fprintf(stderr,                                 \
                "[FAIL] %s:%d: %s = 0x%02X, want 0x%02X\n", \
                __FILE__,                                   \
                __LINE__,                                   \
                #actual,                                    \
                (unsigned) (actual),                        \
                (unsigned) (expected));                     \
            g_failures++;                                   \
        }                                                   \
    }                                                       \
    while(0)

void test_endianness(PHandle* handle)
{
    pesma_write_uint32(handle, 0x01020304);
    pesma_buffer_sync(handle);

    printf("[TESTS] Endianess test \n\n");
    uint8_t* data = handle->write_buffer.data;
    CHECK_EQ(data[0], 0x01); /* MSB first */
    CHECK_EQ(data[1], 0x02);
    CHECK_EQ(data[2], 0x03);
    CHECK_EQ(data[3], 0x04); /* LSB last */
    printf("[TESTS] Endianess test concluded \n\n");

    pesma_buffer_clear(handle, 'w');
    pesma_buffer_clear(handle, 'r');
}

void test_rdwr_file()
{
    printf("[TESTS] Read/Write mixed test \n\n");
    if(access("rwtest.txt", F_OK) == -1) {
        printf("File not exists, skipping...\n");
        return;
    }
    PHandle* handle = pesma_file_open("rwtest.txt", "rwc");

    char* str = "Good Luck!";
    char p = '*';
    int8_t i8 = -30;
    uint32_t u32 = 99999;
    uint16_t u16 = 12500;
    int64_t i64 = -1;
    char* str2 = "END";

    printf(
        "[TESTS] Mixed test. in order: string %s, char %c, int8 %d, uint32 %u, uint16 %u, int64 " "%ld, string %s\n",
        str,
        p,
        i8,
        u32,
        u16,
        i64,
        str2);

    pesma_write_string(handle, str, strlen(str));
    pesma_write_char(handle, p);
    pesma_write_int8(handle, i8);
    pesma_write_uint32(handle, u32);
    pesma_write_uint16(handle, u16);
    pesma_write_int64(handle, i64);
    pesma_write_string(handle, str2, strlen(str2));

    int bytecount = handle->write_buffer.used;

    pesma_send(handle, bytecount);
    pesma_file_seek(handle, 0, SEEK_SET);

    printf(" [TESTS] Mixed test. outputs in order:");

    pesma_recv(handle, bytecount);

    char buf[1024];
    pesma_read_string(handle, buf, strlen(str));
    buf[strlen(str)] = '\0';
    printf("%s,", buf);
    char c;
    pesma_read_char(handle, &c);
    printf("%c,", c);
    int8_t a;
    pesma_read_int8(handle, &a);
    printf("%d,", a);
    uint32_t b;
    pesma_read_uint32(handle, &b);
    printf("%u,", b);
    uint16_t d;
    pesma_read_uint16(handle, &d);
    printf("%u,", d);
    int64_t e;
    pesma_read_int64(handle, &e);
    printf("%ld,", e);

    pesma_read_string(handle, buf, strlen(str2));
    buf[strlen(str2)] = '\0';
    printf("%s", buf);

    pesma_buffer_clear(handle, 'w');
    pesma_buffer_clear(handle, 'r');

    pesma_handle_free(handle);

    printf("[TESTS] Read/Write mixed test done \n\n");

    return;
}

void test_read()
{

    printf("[TESTS] File test. Read chars from readme.txt, output: characters:");
    if(access("readme.txt", F_OK) == -1) {
        printf("File not exists, skipping...\n");
        return;
    }
    PHandle* handle = pesma_file_open("readme.txt", "r");
    pesma_recv(handle, 10);

    for(int i = 0; i < 10; i++) {
        char c;
        pesma_read_char(handle, &c);
        printf("%c", c);
    }
    printf("\n");

    pesma_handle_free(handle);

    return;
}

void test_write()
{
    printf("[TESTS] File test. Write data to writeme.txt, output: characters:");

    PHandle* handle = pesma_file_open("writeme.txt", "rwc");
    pesma_write_string(handle, "characters", strlen("characters"));
    pesma_send(handle, 5);
    pesma_send(handle, 5);
    pesma_write_char(handle, '\n');
    pesma_write_uint64(handle, 99999999);
    pesma_write_int64(handle, -99999999);
    pesma_send(handle, 17);

    pesma_handle_free(handle);

    return;
}

void test_mixed(PHandle* handle)
{
    char* str = "Good Luck!";
    char p = '*';
    int8_t i8 = -30;
    uint32_t u32 = 99999;
    uint16_t u16 = 12500;
    int64_t i64 = -1;
    char* str2 = "END";

    printf(
        "[TESTS] Mixed test. in order: string %s, char %c, int8 %d, uint32 %u, uint16 %u, int64 " "%ld, string %s\n",
        str,
        p,
        i8,
        u32,
        u16,
        i64,
        str2);

    pesma_write_string(handle, str, strlen(str));
    pesma_write_char(handle, p);
    pesma_write_int8(handle, i8);
    pesma_write_uint32(handle, u32);
    pesma_write_uint16(handle, u16);
    pesma_write_int64(handle, i64);
    pesma_write_string(handle, str2, strlen(str2));

    printf(" [TESTS] Mixed test. outputs in order:");

    pesma_buffer_sync(handle);

    char buf[1024];
    pesma_read_string(handle, buf, strlen(str));
    buf[strlen(str)] = '\0';
    printf("%s,", buf);

    char c;
    pesma_read_char(handle, &c);
    printf("%c,", c);
    int8_t a;
    pesma_read_int8(handle, &a);
    printf("%d,", a);
    uint32_t b;
    pesma_read_uint32(handle, &b);
    printf("%u,", b);
    uint16_t d;
    pesma_read_uint16(handle, &d);
    printf("%u,", d);
    int64_t e;
    pesma_read_int64(handle, &e);
    printf("%ld,", e);

    pesma_read_string(handle, buf, strlen(str2));
    buf[strlen(str2)] = '\0';
    printf("%s", buf);

    pesma_buffer_clear(handle, 'w');
    pesma_buffer_clear(handle, 'r');

    return;
}

void test_string(PHandle* handle)
{
    char* s1 = "Hello World!\n";
    char* s2 = "Test Test 123 123\n";
    char* s3 = "This is a C library\n";

    printf("[TESTS] String testing, string input order: %s,%s,%s\n", s1, s2, s3);

    pesma_write_string(handle, s1, strlen(s1));
    pesma_write_string(handle, s2, strlen(s2));
    pesma_write_string(handle, s3, strlen(s3));

    printf("[TESTS] string testing, tring output order: ");

    pesma_buffer_sync(handle);
    char buf[1028];

    pesma_read_string(handle, buf, strlen(s1));
    buf[strlen(s1)] = '\0';
    printf("%s", buf);
    memset(buf, 0, 1028);
    pesma_read_string(handle, buf, strlen(s2));
    buf[strlen(s2)] = '\0';
    printf("%s", buf);
    memset(buf, 0, 1028);
    pesma_read_string(handle, buf, strlen(s3));
    buf[strlen(s3)] = '\0';
    printf("%s\n", buf);
    memset(buf, 0, 1028);

    pesma_buffer_clear(handle, 'w');
    pesma_buffer_clear(handle, 'r');

    return;
}

void test_uint16(PHandle* handle)
{
    printf("[TESTS] Uint16 testing, uint16 input order: 203,17,58\n");

    pesma_write_uint16(handle, 203);
    pesma_write_uint16(handle, 17);
    pesma_write_uint16(handle, 58);

    printf("[TESTS] Uint16 testing, uint16 output order: ");

    pesma_buffer_sync(handle);

    uint16_t c;
    pesma_read_uint16(handle, &c);
    printf("%d\n", c);
    uint16_t b;
    pesma_read_uint16(handle, &b);
    printf("%d\n", b);
    uint16_t a;
    pesma_read_uint16(handle, &a);
    printf("%d\n", a);

    pesma_buffer_clear(handle, 'w');
    pesma_buffer_clear(handle, 'r');

    return;
}

void test_char(PHandle* handle)
{
    printf("[TESTS] Char testing, chars input order: c,b,a\n");

    pesma_write_char(handle, 'c');
    pesma_write_char(handle, 'b');
    pesma_write_char(handle, 'a');

    printf("[TESTS] Char testing, chars output order: ");

    pesma_buffer_sync(handle);
    char c;
    pesma_read_char(handle, &c);
    printf("%c,", c);
    char b;
    pesma_read_char(handle, &b);
    printf("%c,", b);
    char a;
    pesma_read_char(handle, &a);
    printf("%c\n", a);

    pesma_buffer_clear(handle, 'w');
    pesma_buffer_clear(handle, 'r');

    return;
}

int tests_read_write(PHandle* handle)
{
    test_char(handle);
    test_uint16(handle);
    test_string(handle);
    test_mixed(handle);
    test_endianness(handle);
    return 0;
}

int tests_file()
{
    test_write();
    test_read();
    test_rdwr_file();
    return 0;
}

int tests_server(void)
{
    PHandle* serv = pesma_tcp_listen(8080);
    if(!serv)
        return 1;

    printf("[TESTS] Server listening on :8080 — open http://localhost:8080\n");

    PHandle* client = pesma_tcp_accept(serv);
    if(!client) {
        pesma_handle_free(serv);
        return 1;
    }

    pesma_recv(client, client->read_buffer.size);

    char* buffer = malloc(client->read_buffer.used + 1);
    memset(buffer, 0, client->read_buffer.used + 1);
    memcpy(buffer, client->read_buffer.data, client->read_buffer.used);
    printString(buffer);
    printf("\n");
    free(buffer);

    char
        resp[] =
            "HTTP/1.0 200 OK\r\n" "Server: pesma-c\r\n" "Content-type: text/html\r\n\r\n" "<html>"
                                                                                          "hello," " world</" "html>\r\n";

    pesma_write_string(client, resp, sizeof(resp) - 1);
    pesma_send(client, client->write_buffer.used);

    pesma_handle_free(client);
    pesma_handle_free(serv);
    return 0;
}

void test_tcp_connect_fail(void)
{
    printf("[TESTS] TCP connect to closed port (expect NULL)\n");
    PHandle* h = pesma_tcp_connect("127.0.0.1", 1);
    CHECK_EQ(h == NULL, 1);
    if(h)
        pesma_handle_free(h); /* defensive: fail if connect unexpectedly succeeded */
    printf("[TESTS] TCP connect fail path OK\n\n");
}

#include <time.h>
#include <sys/wait.h>

void test_tcp_pingpong(void)
{
    printf("[TESTS] TCP ping-pong (fork)\n");

    pid_t pid = fork();
    if(pid == 0) {
        /* child = server */
        PHandle* serv = pesma_tcp_listen(9090);
        if(!serv)
            exit(1);
        PHandle* c = pesma_tcp_accept(serv);
        if(!c) {
            pesma_handle_free(serv);
            exit(1);
        }

        /* receive "ping" */
        pesma_recv(c, 64);
        char buf[16] = {0};
        pesma_read_string(c, buf, 4);
        if(strncmp(buf, "ping", 4) != 0) {
            pesma_handle_free(c);
            pesma_handle_free(serv);
            exit(1);
        }

        /* send "pong" back */
        pesma_write_string(c, "pong", 4);
        pesma_send(c, 4);

        pesma_handle_free(c);
        pesma_handle_free(serv);
        exit(0);
    }

    /* parent = client */
    struct timespec ts = {.tv_sec = 0, .tv_nsec = 50000000};
    nanosleep(&ts, NULL); /* let the listener bind */
    PHandle* h = pesma_tcp_connect("127.0.0.1", 9090);
    CHECK_EQ(h != NULL, 1);
    if(!h) {
        waitpid(pid, NULL, 0);
        return;
    }

    pesma_write_string(h, "ping", 4);
    pesma_send(h, 4);

    pesma_recv(h, 64);
    char buf[16] = {0};
    pesma_read_string(h, buf, 4);
    CHECK_EQ(strncmp(buf, "pong", 4) == 0, 1);

    pesma_handle_free(h);

    int status = 0;
    waitpid(pid, &status, 0);
    int child_ok = WIFEXITED(status) && WEXITSTATUS(status) == 0;
    CHECK_EQ(child_ok, 1);
    if(child_ok)
        printf("[TESTS] TCP ping-pong OK\n\n");
}
