#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char *argv[])
{
    uint32_t* p = (uint32_t*)0x8dffe000;
    printf(1, "<%p>\n", p);
    *p = 0;
    for(int32_t i = 0; i < 0x800; ++i)
    {
        *p += 1;
        printf(1, "<%p> contains <%x>\n", p, *p);
    }
    exit();
}