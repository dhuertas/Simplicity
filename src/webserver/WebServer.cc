#include "WebServer.hh"

int WebServer::status_ = WebServer::STOPPED;

// Mutex
pthread_cond_t WebServer::condSockFdFull_;
pthread_cond_t WebServer::condSockFdEmpty_;

pthread_mutex_t WebServer::mutexSockFd_;

// Sockets
volatile int *WebServer::clientSockFd_ = NULL;
volatile int WebServer::clientSockFdRd_ = 0;
volatile int WebServer::clientSockFdWr_ = 0;
volatile int WebServer::clientSockFdCount_ = 0;

//------------------------------------------------------------------------------
void *startWebServer(void *sim) {
    
    WebServer *server = new WebServer();
    
    server->setSimulation((Simulation *)sim);
    
    server->start();
    
    // Server should poll simulation state in order to close
    server->finalize();
    
    delete server;
    
    return 0;
}

//------------------------------------------------------------------------------
void WebServer::initialize() {

  int i = 0;

  numberOfThreads_ = 10;

  port_ = 8080;

  serverSockFd_ = socket(PF_INET, SOCK_STREAM, 0);

  memset(&serverAddr_, 0, sizeof(struct sockaddr_in));
  memset(&clientAddr_, 0, sizeof(struct sockaddr_in));

  serverAddr_.sin_family = AF_INET;
  serverAddr_.sin_port = htons(port_);

  serverAddr_.sin_addr.s_addr = htonl(INADDR_ANY);

  bind(serverSockFd_, (struct sockaddr *)&serverAddr_, sizeof(struct sockaddr_in));

  listen(serverSockFd_, MAX_LISTEN);

  printf("Listening to port 8080\n");

  clientSize_ = sizeof(struct sockaddr_in);

  // Initialize threads mutex and conditions
  pthread_mutex_init(&mutexSockFd_, NULL);
  pthread_cond_init(&condSockFdEmpty_, NULL);
  pthread_cond_init(&condSockFdFull_, NULL);

  clientSockFdCount_ = 0;
  clientSockFdRd_ = 0;
  clientSockFdWr_ = 0;

  documentRoot_ = "./www";

  // Wake up threads
  thread_ = (pthread_t *)malloc(sizeof(pthread_t)*numberOfThreads_);
  threadId_ = (int *)malloc(sizeof(int)*numberOfThreads_);

  // Get as many client sockets as threads
  clientSockFd_ = (int *)malloc(sizeof(int)*numberOfThreads_);

  for (i = 0; i < MAX_THREADS; i++) {

    threadId_[i] = i;

    if (pthread_create(&thread_[i], NULL, worker, &threadId_[i]) != 0) {
      //DEBUG("Error creating thread");
    }
    printf("Spawning worker(%d) thread\n", i);
  }

  status_ = RUNNING;
}

//------------------------------------------------------------------------------
void WebServer::finalize() {

  free(thread_);
  free(threadId_);
  free((void *)clientSockFd_);
}

//------------------------------------------------------------------------------
void WebServer::start() {

  // Initialize() calls pthread_create, which in turn each thread
  // runs the handler() function. This function has a while loop using
  // the WebServer::quit variable.
  initialize();

  run();

  pthread_exit(NULL);

}

void WebServer::run() {

  int sockfd;

  fd_set selectSet;

  struct timeval timeout;

  timeout.tv_sec = QUIT_TIME_OUT;
  timeout.tv_usec = 0;

  while (status_) {

    sockfd = accept(serverSockFd_, (struct sockaddr *)&clientAddr_, &clientSize_);

    printf("Received request\n");

    FD_ZERO(&selectSet);
    FD_SET(sockfd, &selectSet);

    select(sockfd + 1, &selectSet, NULL, NULL, &timeout);

    if (FD_ISSET(sockfd, &selectSet)) {

      // start of mutex area
      pthread_mutex_lock(&mutexSockFd_);

      while (clientSockFdCount_ > MAX_THREADS) {
          pthread_cond_wait(&condSockFdFull_, &mutexSockFd_);
      }

      clientSockFd_[clientSockFdWr_] = sockfd;

      clientSockFdCount_++;
      clientSockFdWr_++;
      clientSockFdWr_ = clientSockFdWr_ % MAX_THREADS;

      pthread_cond_broadcast(&condSockFdEmpty_);
      pthread_mutex_unlock(&mutexSockFd_);

      // end of mutex area

    }

    FD_CLR(sockfd, &selectSet);
  }

}

//------------------------------------------------------------------------------
void *WebServer::worker(void *args) {

  int *tid, sockFd;

  tid = (int *)args;

  sigset_t sigpipe_mask;
  sigemptyset(&sigpipe_mask);
  sigaddset(&sigpipe_mask, SIGPIPE);
  sigset_t saved_mask;

  if (pthread_sigmask(SIG_BLOCK, &sigpipe_mask, &saved_mask) == -1) {
    // DEBUG("pthread_sigmask error");
  }

  while (WebServer::status_ > 0) {

    // start of mutex area
    pthread_mutex_lock(&mutexSockFd_);

    while (clientSockFdCount_ == 0) {
      pthread_cond_wait(&condSockFdEmpty_, &mutexSockFd_);
    }

    sockFd = clientSockFd_[clientSockFdRd_];

    clientSockFdRd_++;
    clientSockFdRd_ = clientSockFdRd_ % MAX_THREADS;
    clientSockFdCount_--;

    pthread_cond_broadcast(&condSockFdFull_);
    pthread_mutex_unlock(&mutexSockFd_);

    // end of mutex area
    requestHandler(*tid, sockFd);

  }

  return 0;
}

//------------------------------------------------------------------------------
void WebServer::requestHandler(int tid, int clientSockFd) {

  int n, m;

  fd_set selectSet;

  struct timeval timeout;

  //WebServer::request_t req;
  //WebServer::response_t resp;

  timeout.tv_sec = 1;
  timeout.tv_usec = 0;

  string conn;

  // Wait for data to be received or connection timeout
  FD_ZERO(&selectSet);
  FD_SET(clientSockFd, &selectSet);

  n = select(clientSockFd + 1, &selectSet, NULL, NULL, &timeout);

  if (n < 0) {

    // Client closed connection
    close(clientSockFd);
    return;

  } else if (n == 0) {

    // timeout
    close(clientSockFd);
    return;

  } else {

    if (FD_ISSET(clientSockFd, &selectSet)) {

      //m = WebServer::handleRequest(clientSockFd, &req);
      m = -1;

      if (m < 0) {

        //WebServer::clearRequest(&req);
        //WebServer::clearResponse(&resp);

        close(clientSockFd);
        return;

      }

    }
  
  }

/*
  conn = WebServer::getRequestHeader(&req, "Connection");

  if (conn.length() == 0) {
    // Some http clients (e.g. curl) may not send the Connection header ...
  } else if (strncasecmp(conn.c_str(), "close", strlen("close")) == 0) {

    m = WebServer::handleResponse(clientSockFd, &req, &resp);

  } else {
    
    // Assume keep-alive connection... but keep it simple for now
    timeout.tv_sec = 0;
    timeout.tv_usec = 250;

    //WebServer::handleResponse(clientSockFd, &req, &resp);
  }
*/
  send(clientSockFd, "Hi there", strlen("Hi there"), 0);

  //WebServer::clearRequest(&req);
  //WebServer::clearResponse(&resp);

  FD_CLR(clientSockFd, &selectSet);
  FD_ZERO(&selectSet);

  close(clientSockFd);
}

//------------------------------------------------------------------------------
int startWebServer() {

  WebServer *server = new WebServer();

  server->start();

  server->finalize();

  delete server;

  return 0;
}