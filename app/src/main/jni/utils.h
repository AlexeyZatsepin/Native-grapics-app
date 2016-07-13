#include <sys/time.h>

long long now() {
    struct timeval tp;
    gettimeofday(&tp,NULL);
    long long ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    return ms;

}

