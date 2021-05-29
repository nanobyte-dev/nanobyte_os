#include "stdint.h"
#include "stdio.h"

void _cdecl cstart_(uint16_t bootDrive)
{
    puts("Hello world from C!\r\n");
    printf("I am printing %s %s %% %c text\r\n", "formatted", "beautiful", 'a');
    printf("%d %d %d %d\r\n", 1, 2, 3, 4);
    printf("%lu = 0x%lx = 0o%lo\r\n", 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF);
    for (;;);
}
