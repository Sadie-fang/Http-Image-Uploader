#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "HttpClient.h"
#define MAX_BYTE 1024

using namespace N;
using namespace std;

class ImageUploader {
private:
  ifstream _image_file;
  string _ip;
  int _port;
public:
  /**
    * @brief  construct an image uploader.
    * @param  ip   uploader to server ip.
    * @param  port uploader to server port.
    */
  ImageUploader(string ip, int port) {
    _ip = ip;
    _port = port;
  }
  /**
    * @brief  get http post request packet.
    * @param  image_path image path ready to be open and read.
    * @return whether the image be opened successfully.
    */
  bool open(string image_path) {
    // read image
    _image_file.open(image_path, ios::binary);
    if (!_image_file) {
      cerr << "Error: Unable to open image file." << endl;
      return false;
    }
    return true;
  }
  /**
    * @brief  get http post request packet.
    * @param  out_reques save a post request packet.
    * @return whether the packet get successfully.
    */
  bool getHttpRequest(string& out_request) {
    // prepare HTTP request
    stringstream request;
    request << "POST /upload HTTP/1.1\r\n"
      << "Host: " << _ip << ":" << _port << "\r\n"
      << "Content-Type: image/jpeg\r\n"
      << "Content-Length: ";
    // get image size
    _image_file.seekg(0, ios::end);
    int fileSize = _image_file.tellg();
    _image_file.seekg(0, ios::beg);

    request << fileSize << "\r\n\r\n";
    out_request = request.str();
    return true;
  }
  /**
    * @brief  decide whether the reading header meet the end.
    * @return it meets end or not.
    */
  bool isReadEnd() {
    if (_image_file.eof()) return true;
    return false;
  }
  /**
    * @brief  read image with buffer size.
    * @param  buffer a buffer to save image data.
    * @return actually read image data size.
    */
  int read(char* buffer) {
    _image_file.read(buffer, sizeof(buffer));
    return _image_file.gcount();
  }
  /**
    * @brief close file reader.
    */
  void close() {
    _image_file.close();
  }
};
/**
  * @brief  upload an image using http post to server.
  * @param  image_path  image path in local.
  * @param  server_ip   server's ip.
  * @param  server_port server's port.
  * @return whether upload success or not.
  */
bool httpUploadImage(const string image_path, const string server_ip, int server_port) {
  HttpClient s(server_ip, server_port);
  ImageUploader uploader(server_ip, server_port);
  bool is_success = false;
  if (s.connectHttp()) {
    string request = "\0";
    if (uploader.open(image_path)) {
      if (uploader.getHttpRequest(request)) {
        cout << request << endl;
        if (s.sendData((char*)request.c_str(), request.length())) {
          is_success = true;
          // send all image data
          while (!uploader.isReadEnd()) {
            char buffer[MAX_BYTE] = { '\0' };
            int bytes_read = uploader.read(buffer);
            if (!s.sendData(buffer, bytes_read)) {
              // reconnect to server
              if (s.reconnect())
                if (s.sendData(buffer, bytes_read)) continue; // success
              cout << "Image uploaded fail, lost byte:" << bytes_read << endl;
              is_success = false;
            }
          }
        }
        else cout << "Client connects to server fail." << endl;
      }
      else cout << "Cannot get request packet." << endl;
    }
    else cout << "Cannot get image information." << endl;
  }
  uploader.close();
  s.disconnectHttp();
  return is_success;
}
int main() {
  // image path
  string image_path = "/home/abc/projects/vpgs/main.jpg";
  string server_ip = "127.0.0.1";
  int server_port = 80;
  // upload image
  if (!httpUploadImage(image_path, server_ip, server_port))
    cout << "fail" << endl;
  else
    cout << "success" << endl;
  return 0;
}