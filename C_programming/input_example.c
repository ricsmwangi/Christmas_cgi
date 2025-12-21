#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int i;
    float f;

    printf("Enter an integer: ");
    if (scanf("%d", &i) != 1) {
        fprintf(stderr, "Invalid integer input.\n");
        return 1;
    }

    printf("Enter a float: ");
    if (scanf("%f", &f) != 1) {
        fprintf(stderr, "Invalid float input.\n");
        return 1;
    }

    printf("You entered i=%d and f=%g\n", i, f);

    // Robust line-based input alternative
    char buf[128];
    int i2; float f2;
    printf("Enter integer then float separated by space (line-based): ");
    if (fgets(buf, sizeof buf, stdin) && sscanf(buf, "%d %f", &i2, &f2) == 2) {
        printf("Parsed i2=%d f2=%g\n", i2, f2);
    } else {
        fprintf(stderr, "Failed to parse line-based input.\n");
    }

    return 0;
}
