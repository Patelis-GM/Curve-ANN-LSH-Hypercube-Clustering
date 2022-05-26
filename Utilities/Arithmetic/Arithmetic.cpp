#include "Arithmetic.h"

/* Utility function to calculate the x modulo y */
long long int Arithmetic::mod(long long int x, long long int y) {
    return (x % y + y) % y;
}

