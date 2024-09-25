#include <stdio.h>

int main() {
    int error_code = 404;
    
    // Write an error message to stderr
    printf("Error: Could not find the resource (code %d).\n", error_code);
    
    return 0;
}
