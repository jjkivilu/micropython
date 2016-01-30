#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "mcu_api.h"

void _exit(int status) {
    debug_print(DBG_WARNING, "_exit() not implemented");
    while (1)
        mcu_sleep(100000); // satisfy 'noreturn'
}

void *sbrk(ptrdiff_t increment) {
    debug_print(DBG_WARNING, "_sbrk() not implemented");
    errno = ENOSYS;
    return NULL;
}

int kill(pid_t pid, int sig) {
    debug_print(DBG_WARNING, "kill() not implemented");
    errno = ENOSYS;
    return -1;
}

pid_t getpid(void) {
    debug_print(DBG_WARNING, "getpid() not implemented");
    errno = ENOSYS;
    return -1;
}

ssize_t write(int fd, const void *buf, size_t count) {
    if (fd < 1 || fd > 2) {
        debug_print(DBG_WARNING, "write() not implemented outside stdin/out/err");
        errno = EBADF;
        return 0;
    }

    const void* p = buf;
    while (p < buf + count) {
        int n = buf + count - p;
        if (n > 255)
            n = 255;
        if (host_send((unsigned char*) p, n) != 0)
            break;
        p += n;
    }
    return p - buf;
}

int close(int fd) {
    debug_print(DBG_WARNING, "close() not implemented");
    errno = ENOSYS;
    return -1;
}

int fstat(int fd, struct stat* buf) {
    switch (fd) {
    case 0:
    case 1:
    case 2:
        if (buf) {
            buf->st_dev = (136 << 8) + fd;
            buf->st_ino = 0;
            buf->st_mode = S_IFCHR;
            buf->st_nlink = 0;
            buf->st_uid = 0;
            buf->st_gid = 0;
            buf->st_rdev = (136 << 8) + fd;
            buf->st_size = 0;
            buf->st_blksize = 0;
            buf->st_blocks = 0;
            return 0;
        }
        // fallthroguh
    default:
        debug_print(DBG_WARNING, "fstat() not implemented");
    }
    errno = EBADF;
    return 0;
}

int isatty(int fd) {
    if (fd >= 0 && fd <= 2)
        return 1;
    errno = EBADF;
    return 0;
}

off_t lseek(int fd, off_t offset, int whence) {
    debug_print(DBG_WARNING, "lseek() not implemented");
    errno = (fd >= 0 && fd <= 2) ? EBADF : ENOSYS;
    return -1;
}

ssize_t read(int fd, void *buf, size_t count) {
    if (fd == 1 || fd == 2)
        return host_receive(buf, count);
    debug_print(DBG_WARNING, "read() not implemented");
    errno = EBADF;
    return 0;
}

