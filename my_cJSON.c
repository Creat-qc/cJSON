#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <ctype.h>
#include "my_cJSON.h"
/* error print */
static const char *ep;

const char *cJSON_GetErrorPtr(void) { return ep; }

/* parse an object */
cJSON *cJSON_Parse(const char *value) { return cJSON_ParseWithOpts(value, 0, 0); }

/* Render a cJSON item/entity/structure to text. */
char *cJSON_Print(cJSON *item) { return print_value(item, 0, 1, 0); }

typedef struct
{
    char *buffer;
    int length;
    int offset;
} printbuffer;

static char *print_value(cJSON *item, int depth, int fmt, printbuffer *p)
{
    char *out = 0;
    if (!item)
        return 0;
    if (p)
    {
        /* TODO */
    }
    else
    {
        /* 将item->type转换成整数类型 */
        switch ((item->type) & 255)
        {
        case cJSON_NULL:
            out = cJSON_Strdup("null");
            break;
        case cJSON_False:
            out = cJSON_strdup("false");
            break;
        case cJSON_True:
            out = cJSON_strdup("true");
            break;

        default:
            break;
        }
    }
}

static char *cJSON_strdup(const char *str)
{
    size_t len;
    
}

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
    /* 解析失败 */
    if (!end)
    {
        /* cJSON_Delete hasn't realize */
        cJSON_Delete(c);
        return 0;
    }
}

/* 解析函数 核心代码 */
static const char *parse_value(cJSON *item, const char *value)
{
    /* 不太明白这里为什么空表示失败 */
    if (!value)
        return 0;
    /* strncmp返回0时表示两个字符串一致 */
    if (!strncmp(value, "null", 4))
    {
        item->type = cJSON_NULL;
        return value + 4;
    }
    if (!strncmp(value, "false", 5))
    {
        item->type = cJSON_False;
        return value + 5;
    }
    if (!strncmp(value, "true", 4))
    {
        /* TODO 不懂这里的valueint 为何要设置为1 */
        item->type = cJSON_True;
        item->valueint = 1;
        return value + 4;
    }
    if (*value == '\"')
    {
        return parse_string(item, value);
    }
    /* 在负数的情况下进行解析 */
    if (*value == '-' || (*value >= '0' && *value <= '9'))
    {
        return parse_number(item, value);
    }
    if (*value == '[')
    {
        return parse_array(item, value);
    }
    if (*value == '{')
    {
        return parse_object(item, value);
    }
    /* 这里不理解为何失败返回0，以及ep的作用 */
    ep = value;
    return 0;
}

static const char *parse_object(cJSON *item, const char *value)
{
    cJSON *child;
    if (*value != '{')
    {
        ep = value;
        return 0;
    }
    item->type = cJSON_Object;
    value = skip(value + 1);
    if (*value == '}')
        return value + 1;
    item->child = child = cJSON_New_Item();
    /* 分配内存失败 */
    if (!item->child)
        return 0;

    value = skip(parse_string(child, skip(value)));
    if (!value)
        return 0;
    child->string = child->valuestring;
    child->valuestring = 0;
    if (*value != ':')
    {
        ep = value;
        return 0;
    }
    value = skip(parse_value(child, skip(value + 1)));
    if (!value)
        return 0;

    while (*value == ',')
    {
        cJSON *new_item;
        if (!(new_item = cJSON_New_Item()))
            return 0;
        child->next = new_item;
        new_item->prev = child;
        child = new_item;
        value = skip(parse_string(child, skip(value + 1)));
        if (!value)
            return 0;
        child->string = child->valuestring;
        child->valuestring = 0;
        if (*value != ':')
        {
            ep = value;
            return 0;
        }
        value = skip(parse_value(child, skip(value + 1)));
        if (!value)
            return 0;
    }
    if (*value == '}')
        return value + 1;
    ep = value;
    return 0;
}

static const char *parse_array(cJSON *item, const char *value)
{
    cJSON *child;
    if (*value != '[')
    {
        /* 这里又是判断，感觉无用，而且ep也不知道是干啥的 */
        ep = value;
        return 0;
    }
    item->type = cJSON_Array;
    value = skip(value + 1);
    if (*value == ']')
        return value + 1;

    item->child = child = cJSON_New_Item();
    if (!item->child)
        return 0;
    value = skip(parse_value(child, skip(value)));
    if (!value)
        return 0;
    while (*value == ',')
    {
        cJSON *new_item;
        /* 分配空间并判断是否成功 */
        if (!(new_item = cJSON_New_Item()))
            return 0;
        child->next = new_item;
        new_item->prev = child;
        child = new_item;
        /* 在创立完双向链表之后，再次过滤中间的无用项 */
        value = skip(parse_value(child, skip(value + 1)));
        if (!value)
            return 0;
    }

    if (*value == ']')
        return value + 1;
    /* i still don't konw what is ep */
    ep = value;
    return 0;
}

/* 解析数字 */
static const char *parse_number(cJSON *item, const char *value)
{
    double n = 0, sign = 1, scale = 0;
    int subscale = 0, signsubscale = 1;

    if (*value == '-')
        sign = -1, value++;
    if (*value == '0')
        value++;
    /* 数值有意义时，进行每个字节的乘10累加，得到字符串中的数字 */
    if (*value >= '1' && *value <= '9')
        do
            n = (n * 10.0) + (*value++ - '0');
        while (*value >= '0' && *value <= '9');
    /* 数值为小数时进行小数的处理 */
    if (*value == '.' && value[1] >= '0' && value[1] <= '9')
    {
        value++;
        do
        {
            n = (n * 10.0) + (*value++ - '0'), scale--;
        } while (*value >= '0' && *value <= '9');
    }
    /* 科学计数法 */
    if (*value == 'e' || *value == 'E')
    {
        value++;
        if (*value == '+')
            value++;
        else if (*value == '-')
            signsubscale = -1, value++;
        while (*value >= '0' && *value <= '9')
            subscale = (subscale * 10) + (*value++ - '0');
    }
    n = sign * n * pow(10.0, (scale + subscale * signsubscale));

    item->valuedouble = n;
    item->valueint = (int)n;
    item->type = cJSON_Number;
    return value;
}

static const unsigned char firstByteMark[7] = {0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC};
static const char *parse_string(cJSON *item, const char *value)
{
    /* 这样使用const可以更改指针指向的位置，但是不能更改指针指向的值 */
    const char *ptr = value + 1;
    char *ptr2;
    char *out;
    int len = 0;
    unsigned int ui, ui2;
    /* TODO 我是没懂原项目为啥要加这个if，明明就是因为value等于'\'才进来的 */
    if (*value != '\"')
    {
        ep = value;
        return 0;
    }
    /* 这个while很经典，意思是当*ptr不等于"的时候，就执行*ptr等不等于null的判断，如果不为空再执行++len，并且也为true，再执行循环里的代码
    含义是要计算该字符串的长度 */
    while (*ptr != '\"' && *ptr && ++len)
    {
        /* *ptr++ 和 *(ptr++)是一样的意思 */
        /* TODO 不太明白这里是啥意思 */
        if (*(ptr++) == '\\')
            ptr++;
    }
    out = (char *)cJSON_malloc(len + 1);
    /* 每次用malloc分配内存空间后都要检查一下，是否分配成功 */
    if (!out)
        return 0;
    ptr = value + 1;
    ptr2 = out;
    while (*ptr != '\"' && *ptr)
    {
        if (*ptr != '\\')
            *ptr2++ = *ptr++;
        else
        /* ptr此时指向转义字符前面的\ */
        {
            ptr++;
            switch (*ptr)
            {
            case 'b':
                *ptr2++ = '\b';
                break;
            case 'f':
                *ptr2++ = '\f';
                break;
            case 'n':
                *ptr2++ = '\n';
                break;
            case 'r':
                *ptr2++ = '\r';
                break;
            case 't':
                *ptr2++ = '\t';
                break;
                /* 详细的看不懂 case u: 应该是处理unicode编码的一种方式，因为\u表示接下来的ascll码解析方式为unicode*/
            case 'u':
                ui = parse_hex4(ptr + 1);
                ptr += 4; /* get the unicode char. */

                if ((ui >= 0xDC00 && ui <= 0xDFFF) || ui == 0)
                    break; /* check for invalid.	*/

                if (ui >= 0xD800 && ui <= 0xDBFF) /* UTF16 surrogate pairs.	*/
                {
                    if (ptr[1] != '\\' || ptr[2] != 'u')
                        break; /* missing second-half of surrogate.	*/
                    ui2 = parse_hex4(ptr + 3);
                    ptr += 6;
                    if (ui2 < 0xDC00 || ui2 > 0xDFFF)
                        break; /* invalid second-half of surrogate.	*/
                    ui = 0x10000 + (((ui & 0x3FF) << 10) | (ui2 & 0x3FF));
                }

                len = 4;
                if (ui < 0x80)
                    len = 1;
                else if (ui < 0x800)
                    len = 2;
                else if (ui < 0x10000)
                    len = 3;
                ptr2 += len;

                switch (len)
                {
                case 4:
                    *--ptr2 = ((ui | 0x80) & 0xBF);
                    ui >>= 6;
                case 3:
                    *--ptr2 = ((ui | 0x80) & 0xBF);
                    ui >>= 6;
                case 2:
                    *--ptr2 = ((ui | 0x80) & 0xBF);
                    ui >>= 6;
                case 1:
                    *--ptr2 = (ui | firstByteMark[len]);
                }
                ptr2 += len;
                break;
            default:
                *ptr2++ = *ptr;
                break;
            }
            ptr++;
        }
    }
    *ptr2 = 0;
    if (*ptr == '\"')
        ptr++;
    item->valuestring = out;
    item->type = cJSON_String;
    return ptr;
}

/* TODO don't user const */
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