#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "HttpClient.h"
#define MAX_BYTE 1024

using namespace std;
using namespace N;
/**
  * @brief construct a http client class.
  * @param ip   host ip.
  * @param port host port.
  */
HttpClient::HttpClient(string ip, int port) {
  _server_ip = ip;
  _server_port = port;
  // set server addr
  _server_addr.sin_family = AF_INET;
  inet_aton(_server_ip.c_str(), &_server_addr.sin_addr);
  _server_addr.sin_port = htons(_server_port);
}
/**
  * @brief destruct a http client class.
  */
HttpClient::~HttpClient() {
  close(_sockfd);
}
/**
  * @brief   set socket and connect to host
  * @return  return true or false depend on socket's connection
  */
bool HttpClient::connectHttp() {
  // creat socket
  _sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (_sockfd == -1) {
    cerr << "Error: Failed to create socket." << endl;
    close(_sockfd);
    return false;
  }
  // connect to server
  int state = connect(_sockfd, (struct sockaddr*)&_server_addr, sizeof(_server_addr));
  if (state == -1) {
    cerr << "Error: Failed to connect to server." << endl;
    close(_sockfd);
    return false;
  }
  return true;
}
/**
  * @brief  disconnect to the server.
  */
void HttpClient::disconnectHttp() {
  close(_sockfd);
}
/**
  * @brief   reconnect to server.
  * @return  return true or false depend on socket's connection.
  */
bool HttpClient::reconnect() {
  disconnectHttp();
  if (!connectHttp()) {
    perror("Cannot connect Http server!\n");
    return false;
  }
  return true;
}
/**
  * @brief   send packet to host.
  * @param   request unsigned char - a packet ready to be send.
  * @param   len     total packet size.
  * @return  return true if user send the packet successfully.
  */
bool HttpClient::sendData(char* request, int len) {
  // send HTTP request
  if (send(_sockfd, request, len, 0) == -1) {
    cerr << "Error: Failed to send HTTP request." << endl;
    close(_sockfd);
    return false;
  }
  return true;
}
/**
  * @brief   get packet from host.
  * @param   in_data buffer to save packet.
  * @param   byte    total packet size.
  * @return  return true if user recieve the packet successfully.
  */
bool HttpClient::recieveData(unsigned char* in_data, int len) {
  int bytes_received = recv(_sockfd, in_data, len, 0);
  if (bytes_received <= 0) return false;
  return true;
}



