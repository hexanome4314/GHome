#ifndef UTILS_H_
#define UTILS_H_

/**
 * return a ready to use server-socket
 */
int mk_sock(int port, int addr, int flags);

/**
 * return a ready to use client-socket
 */
int connect_to(int addr, int c_port, int proto);

/**
 * return the list of sensors id
 */
char** read_sensors_list_file(int* a_number_of_sensor);

#endif
