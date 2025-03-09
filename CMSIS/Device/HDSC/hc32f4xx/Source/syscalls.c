// syscalls.c
#include <sys/types.h>  // 定义 off_t
#include <errno.h>

int _close(int fd) { return -1; }
int _isatty(int fd) { return 1; }  // 假设是终端设备
int _lseek(int fd, _off_t offset, int whence) { return -1; }
int _read(int fd, char *buf, int len) { return -1; }
int _write(int fd, const char *buf, int len) {
    // 实现串口输出（示例，需根据硬件修改）
    for (int i = 0; i < len; i++) {
        // USART_SendData(USART1, buf[i]);
        // while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    }
    return len;
}
void _exit(int status) { while (1); }  // 死循环
int _kill(int pid, int sig) { return -1; }
int _getpid(void) { return 1; }
void *_sbrk(ptrdiff_t incr) {
    extern char _end;  // 链接脚本定义的堆起始地址
    static char *heap_end = &_end;
    char *prev_heap_end = heap_end;
    heap_end += incr;
    return prev_heap_end;
}