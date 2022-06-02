#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <ctype.h>
#include "cJSON.h"

static const char *ep;

/* parse an object */
cJSON *cJSON_Parse(const char *value) { return cJSON_ParseWithOpts(value, 0, 0); }

cJSON *cJSON_ParseWithOpts(const char *value, const char **return_parse_end, int require_null_terminated)
{
    const char *end = 0;
    cJSON *c = cJSON_New_Item();
    /*
        const int *p1;
        int const *p2;
        int * const p3;
        only the third option can't modify the pointer,the other can modify the pointer,but can't modify the value of pointer
    */
    ep = 0;
    /* 分配内存失败 */
    if (!c)
        return 0;
    end = parse_value(c, skip(value));
    /* TODO  parse_value hasn't realize*/
}
/* TODO don't user const terporary*/
static char *skip(const char *in)
{
    /* 检查指针是否为空，指针指向的值是否为空，指针指向的值的ascll码是否小于32，小于32被认为是无意义字符，可以跳过 */
    while (in && *in && (unsigned char)*in <= 32)
        in++;
    /* 跳至有意义字符时，返回相应的指针 */
    return in;
}
static cJSON *cJSON_New_Item(void)
{
    cJSON *node = (cJSON *)cJSON_malloc(sizeof(cJSON));
    if (node)
        memset(node, 0, sizeof(cJSON));
    return node;
}

/* 这是一个指针函数指针，(*cJSON_malloc)说明这是一个函数指针，加上*说明返回值是指针 */
static void *(*cJSON_malloc)(size_t sz) = malloc;