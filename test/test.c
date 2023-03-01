#include <stdio.h>

int x256(int i) {
    return i * 256;
}

int main(void) {
    printf("%d\n", x256(3));
    return 0;
}
