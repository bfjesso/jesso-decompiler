#include <check.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Include the PE handler header or declare the function prototype */
extern int scanPE(const char *buffer, unsigned int length, void *ctx);

START_TEST(test_pe_memcpy_bounds)
{
    /* Invariant: Buffer reads/writes in PE parsing never exceed declared length */

    /* Craft minimal PE-like buffers with malicious header values */
    struct {
        unsigned char *data;
        unsigned int len;
    } payloads[3];

    /* Payload 1: Oversized section header values causing openIndex+len > buffer size */
    unsigned int sz1 = 512;
    payloads[0].data = calloc(sz1, 1);
    payloads[0].len = sz1;
    /* MZ signature + PE offset pointing near end of buffer */
    payloads[0].data[0] = 'M'; payloads[0].data[1] = 'Z';
    payloads[0].data[0x3C] = 0x80; /* e_lfanew pointing to offset 128 */
    payloads[0].data[0x80] = 'P'; payloads[0].data[0x81] = 'E';
    /* Set NumberOfSections to large value */
    payloads[0].data[0x86] = 0xFF; payloads[0].data[0x87] = 0x7F;
    /* Set SizeOfOptionalHeader to overflow value */
    payloads[0].data[0x94] = 0xFE; payloads[0].data[0x95] = 0xFF;

    /* Payload 2: Boundary - e_lfanew points exactly at buffer end */
    unsigned int sz2 = 256;
    payloads[1].data = calloc(sz2, 1);
    payloads[1].len = sz2;
    payloads[1].data[0] = 'M'; payloads[1].data[1] = 'Z';
    payloads[1].data[0x3C] = 0xFC; /* e_lfanew at offset 252, near end of 256-byte buf */

    /* Payload 3: Valid minimal PE (should parse without crash) */
    unsigned int sz3 = 1024;
    payloads[2].data = calloc(sz3, 1);
    payloads[2].len = sz3;
    payloads[2].data[0] = 'M'; payloads[2].data[1] = 'Z';
    payloads[2].data[0x3C] = 0x80;
    payloads[2].data[0x80] = 'P'; payloads[2].data[0x81] = 'E';
    payloads[2].data[0x86] = 0x01; /* 1 section */
    payloads[2].data[0x94] = 0x70; /* Reasonable optional header size */

    for (int i = 0; i < 3; i++) {
        /* Should not crash or access out of bounds - return value doesn't matter */
        scanPE((const char *)payloads[i].data, payloads[i].len, NULL);
        free(payloads[i].data);
    }
}
END_TEST

Suite *security_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Security");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_pe_memcpy_bounds);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = security_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}