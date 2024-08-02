#ifndef INCLUDE_H
#define INCLUDE_H

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <windows.h>
typedef SOCKET socket_t;

#else
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
typedef int socket_t;
#endif

#endif
