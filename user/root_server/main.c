/*
 * Root Server - First User-Space Process
 */

/* Simple syscall wrapper */
static inline long syscall1(long num, long arg1)
{
    long ret;
    __asm__ volatile("int $0x80" : "=a"(ret) : "a"(num), "D"(arg1) : "memory");
    return ret;
}

static void debug_print(const char *msg)
{
    syscall1(99, (long) msg); /* SYS_DEBUG_PRINT */
}

void _start(void)
{
    debug_print("Hello from user space!\n");

    /* Infinite loop */
    while (1) {
        __asm__ volatile("hlt");
    }
}
