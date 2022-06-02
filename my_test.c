#include <stdio.h>
#include <stdlib.h>
#include "my_cJSON.h"
/* print a block of char in JSON */
void doit(char *text)
{
    char *out;
    cJSON *json;
    json = cJSON_Parse(text);
    if (!json)
    {
        /* TODO cJSON_GetErrorPtr hasn't realize */
        printf("Error before: [%s]\n", cJSON_GetErrorPtr());
    }
    else
    {
        /* TODO cJSON_Print hasn't realize */
        out = cJSON_Print(json);
        /* TODO cJSON_Delete hasn't realize */
        cJSON_Delete(json);
        printf("%s\n",out);
        free(out);
    }
}

/* print the file in JSON */
void dofile(char *filename)
{
    FILE *fp;
    long len;
    char *data;
    fp = fopen(filename, "rb");
    /* put the pointer to the end */
    fseek(fp, 0, SEEK_END);
    /* get the current position of pointer, but the real is len + 1 */
    len = ftell(fp);
    /* needn't use sizeof to get the size_long,ftell returns a size of bytes, but i don't konw why the len must +1*/
    data = (char *)malloc(len + 1);
    fread(data, 1, len, fp);
    fclose(fp);
    doit(data);
    free(data);
}
int main(void)
{
    dofile("tests/test1");
    return 0;
}