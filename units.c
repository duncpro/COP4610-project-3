#include "units.h"
#include "math.h"
#include "stdlib.h"

long bytes(long double quantity, int unit) {
    if (unit == BYTE) return quantity;
    if (unit == KILOBYTE) return 1024 * quantity;
    if (unit == MEGABYTE) return quantity * bytes(1024, KILOBYTE);
    if (unit == GIGABYTE) return quantity * bytes(1024, MEGABYTE);
    exit(1); 
}

