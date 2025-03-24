#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char *argv[])
{
    int *p = (int *)(0x8dfff000 - (1 << 12));
    *p = 0xfeedface;
    
    printf(2, "shared <%x> contents <%x>\n", p, *p);
    exit();
}