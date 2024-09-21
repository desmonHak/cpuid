#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include <stdint.h>

jmp_buf buf;

void signal_handler(int sig) {
    printf("Caught signal %d\n", sig);
    longjmp(buf, 1);
}

uint64_t rdtsc() {
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

int main() {
    signal(SIGSEGV, signal_handler);  // Capture segmentation faults
    signal(SIGILL, signal_handler);   // Capture illegal instructions

    if (setjmp(buf)) {
        printf("RDTSC caused an exception!\n");
        return 1;
    } else {
        uint64_t tsc = rdtsc();
        printf("RDTSC value: %llu\n", tsc);
    }

    return 0;
}
