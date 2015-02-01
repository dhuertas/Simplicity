#ifndef __WEBSERVER_HH__
#define __WEBSERVER_HH__

#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <ctime>
#include <list>
#include <algorithm>

// integers
#include <stdint.h>

// connection
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// file
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

// others
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include "JsonValue.hh"
#include "Buffer.hh"
#include "Thread.hh"
#include "Utils.hh"

#define SOCK_RD             0
#define SOCK_WR             1
#define TIME_OUT            10 // Keep Alive timeout 10 seconds
#define RECV_TIME_OUT       10
#define KEEPALIVE_TIME_OUT  1
#define QUIT_TIME_OUT       10

#define MAX_LISTEN          30
#define BUFFER_SIZE         2048

using namespace std;

class Simulation;

class WebServer : public Thread {

 public:

  enum {
    STOPPED,
    RUNNING
  };

  enum {
    GET,
    HEAD,
    POST,
    PUT,
    DELETE,
    TRACE,
    CONNECT
  };

  typedef struct header {
    string name;
    string value;
  } header_t;

  typedef struct request {
    string method;
    string uri;
    string version;
    string resource;
    string query;
    vector<header_t> headers;
  } request_t;

  typedef struct response {
    uint16_t statusCode;
    string reasonPhrase;
    string version;
    string filePath;
    vector<header_t> headers;
  } response_t;

 private:

 protected:

  static unsigned int count_;

  // Mutex
  static pthread_cond_t condSockFdFull_;
  static pthread_cond_t condSockFdEmpty_;

  static pthread_mutex_t mutexSockFd_;

  JsonValue *config_;

  pthread_t *thread_;
  int *threadId_;

  // Sockets
  static volatile int *clientSockFd_;
  static volatile int clientSockFdRd_;
  static volatile int clientSockFdWr_;
  static volatile int clientSockFdCount_;

  int serverSockFd_;

  // Network
  socklen_t clientSize_;

  struct sockaddr_in serverAddr_;
  struct sockaddr_in clientAddr_;

  Simulation *sim_;

  // Parameters
  unsigned short port_;

  int timeout_;

  int numberOfThreads_;

  string documentRoot_;

  string defaultFile_;

 public:

  int status_;

  // Constructor
  WebServer();

  // Destructor
  ~WebServer() {}

  void initialize();

  void finalize();

  void *run();

  void setSimulation(Simulation *sim) { sim_ = sim; };

  void setStatus(int status) { status_ = status; }

  void stop();

  //
  // Worker methods
  //
  static void *worker(void *id);

  int8_t requestHandler(int clientSockFd);

  int8_t handleRequest(int sockfd, request_t *req);

  int8_t handleResponse(int sockfd, request_t *req, response_t *res);

  static int8_t receiveRequest(int sockfd, request_t *req);

  static void clearRequest(request_t *req);

  static void clearResponse(response_t *res);

  static std::string getHeader(std::vector<header_t> *headers, std::string name);

  static void setHeader(std::vector<header_t> *headers, std::string name, std::string value);

  int8_t handleGet(int sockfd, request_t *req, response_t *res);

  int8_t handlePost(int sockfd, request_t *req, response_t *res);

  int8_t handleHead(int sockfd, request_t *req, response_t *res);

  void handleError(int sockfd, request_t *req, response_t *res);

  void appendResponseHeaders(response_t *res, Buffer *buf);
};

#endif