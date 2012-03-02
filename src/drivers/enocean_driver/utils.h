#ifndef UTILS_H_
#define UTILS_H_

#define LOG_UTILS 1
/**
 * return a ready to use server-socket
 */
int mk_sock(int port, int addr, int flags);

/**
 * return a ready to use client-socket
 */
int connect_to(int addr, int c_port, int proto);

#endif
