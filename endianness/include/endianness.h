/**
 * @brief Evaluation of Byte Order of the machine.
 * @return 1 if Little Endian 0 if Big Endian
 */
int is_little_endian();

/**
 * @brief Host to Network Byte Order convertion of 2-bytes number.
 * @param num number to be converted
 * @return num in Network Byte Order
 */
short int htons(short int num);

/**
 * @brief Host to Network Byte Order convertion of 4-bytes number.
 * @param num number to be converted
 * @return num in Network Byte Order
 */
int htonl(int num);
