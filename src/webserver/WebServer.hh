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
#include "Thread.hh"

#define SOCK_RD             0
#define SOCK_WR             1
#define TIME_OUT            10 // Keep Alive timeout 10 seconds
#define RECV_TIME_OUT       10
#define KEEPALIVE_TIME_OUT  1
#define QUIT_TIME_OUT       10

#define MAX_LISTEN          30
#define MAX_THREADS         10

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

  static int status_;

  // Mutex
  static pthread_cond_t condSockFdFull_;
  static pthread_cond_t condSockFdEmpty_;

  static pthread_mutex_t mutexSockFd_;

  JsonValue *config_;

  // Server port
  unsigned short port_;

  // Threads
  unsigned int numberOfThreads_;

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

  // Constants
  string documentRoot_;

 public:

  // Constructor
  WebServer();

  // Destructor
  ~WebServer() {}

  void initialize();

  void finalize();

  void *run();

  static void *worker(void *id);

  static void requestHandler(int tid, int clientSockFd);

  void setSimulation(Simulation *sim) { sim_ = sim; };

  void setStatus(int status) { status_ = status; }

  void stop();

};

#endif