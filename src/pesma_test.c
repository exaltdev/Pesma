#include "pesma.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

int counter = 0;

int tests_read_write(PHandle* handle);
int tests_file();

int main(void)
{
    // Minimal test skeleton for pesma library
    // TODO: implement test logic here

    printf("Pesma test skeleton running...\n");
    PHandle* handle = pesma_buffer_create(1024);
    int i = tests_read_write(handle);
    printf("%d\n", i);
    pesma_handle_free(handle);
    handle = NULL;

    printf("\n");
    i = tests_file();
    return 0;
}


void test_read(){

    printf("[TESTS] File test. Read chars from readme.txt, output: characters:");
    PHandle* handle = pesma_file_open("readme.txt", "r");
    pesma_file_read(handle, 10);

    for(int i = 0;i<10;i++){
        printf("%c",pesma_read_char(handle));
    }
    printf("\n");

    return;
}

void test_write(){
    printf("[TESTS] File test. Write data to writeme.txt, output: characters:");
    PHandle* handle = pesma_file_open("writeme.txt", "rwc");
    pesma_write_string(handle, "characters");
    pesma_file_write(handle, 10);
    pesma_write_char(handle,'\n');
    pesma_write_uint64(handle, 99999999);
    pesma_write_int64(handle, -99999999);
    pesma_file_write(handle,17);

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

    printf("[TESTS] Mixed test. in order: string %s, char %c, int8 %d, uint32 %u, uint16 %u, int64 "
           "%ld, string %s\n",
        str,
        p,
        i8,
        u32,
        u16,
        i64,
        str2);

    pesma_write_string(handle, str);
    pesma_write_char(handle, p);
    pesma_write_int8(handle, i8);
    pesma_write_uint32(handle, u32);
    pesma_write_uint16(handle, u16);
    pesma_write_int64(handle, i64);
    pesma_write_string(handle, str2);

    printf(" [TESTS] Mixed test. outputs in order:");

    char buf[1024];
    pesma_read_string(handle, buf, strlen(str));
    printf("%s", buf);

    printf("%c,", pesma_read_char(handle));
    printf("%d,", pesma_read_int8(handle));
    printf("%u,", pesma_read_uint32(handle));
    printf("%u,", pesma_read_uint16(handle));
    printf("%ld,", pesma_read_int64(handle));

    pesma_read_string(handle, buf, strlen(str2));
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

    pesma_write_string(handle, s1);
    pesma_write_string(handle, s2);
    pesma_write_string(handle, s3);

    printf("[TESTS] string testing, tring output order: ");

    char buf[1028];

    pesma_read_string(handle, buf, strlen(s1));
    printf("%s", buf);
    memset(buf, 0, 1028);
    pesma_read_string(handle, buf, strlen(s2));
    printf("%s", buf);
    memset(buf, 0, 1028);
    pesma_read_string(handle, buf, strlen(s3));
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

    printf("%d,", pesma_read_uint16(handle));
    printf("%d,", pesma_read_uint16(handle));
    printf("%d\n", pesma_read_uint16(handle));

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

    printf("%c,", pesma_read_char(handle));
    printf("%c,", pesma_read_char(handle));
    printf("%c\n", pesma_read_char(handle));

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
    return 0;
}

int tests_file(){
    test_write();
    test_read();
    return 0;
}
