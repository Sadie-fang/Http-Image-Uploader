#ifndef HTTP_CLIENT_H 
#define HTTP_CLIENT_H
#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#define MAX_BYTE 1024

namespace N
  {
  class HttpClient {
  private:
    std::string _server_ip;
    int _server_port;
    int _sockfd;
    struct sockaddr_in _server_addr;
  public:
    HttpClient(std::string ip, int port);
    ~HttpClient();
    bool connectHttp();
    bool sendData(char* request, int len);
    void disconnectHttp();
    bool recieveData(unsigned char* in_data, int len);
    bool reconnect();
  };
}
#endif /* HTTP_CLIENT_H */