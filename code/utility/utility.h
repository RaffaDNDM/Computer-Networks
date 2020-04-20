#include <stdint.h>

/*
 * Utility functions that are also implemented in 
 * standard C libraries as <string.h>
 */

int my_strlen(char *buf);

/*
 *  My implementation of htons() function 
 *  (host to network short) that performs the transformation
 *   of a number to the  correspondent Big Endian format)
 */
uint16_t my_htons(uint16_t number);
