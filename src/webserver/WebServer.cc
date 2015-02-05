#include "WebServer.hh"
#include "Simulation.hh"
#include "JsonReader.hh"

// Mutex
pthread_cond_t WebServer::condSockFdFull_;
pthread_cond_t WebServer::condSockFdEmpty_;

pthread_mutex_t WebServer::mutexSockFd_;

// Sockets
volatile int *WebServer::clientSockFd_ = NULL;
volatile int WebServer::clientSockFdRd_ = 0;
volatile int WebServer::clientSockFdWr_ = 0;
volatile int WebServer::clientSockFdCount_ = 0;

unsigned int WebServer::count_ = 0;

//------------------------------------------------------------------------------
WebServer::WebServer() : 
  config_(NULL),
  thread_(NULL),
  threadId_(NULL),
  serverSockFd_(0),
  sim_(NULL),
  port_(8080),
  numberOfThreads_(0),
  status_(STOPPED) {

}

//------------------------------------------------------------------------------
int WebServer::configure(const char *fileName, const char *path) {

  if (fileName == NULL) {
    ERROR("Missing file name");
    return -1;
  }

  if (path == NULL) {
    ERROR("Missing path");
    return -1;
  }

  JsonReader reader;
  JsonValue *config_ = NULL;

  if ( ! reader.parse("server.json", &config_)) {
    ERROR("Unable to parse server configuration file");
    return -1;
  }

  if (config_->isNull() || ! config_->isObject()) {
    ERROR("Web server must be a valid JSON object");
    return -1;
  }

  JsonValue *numThreads = config_->getValue("threads");
  numberOfThreads_ = 4;

  if (numThreads->isNumber() && numThreads->toInteger() > 0) {
    numberOfThreads_ = numThreads->toInteger();
    INFO("numberOfThreads(%d)", numberOfThreads_);
  } else {
    INFO("numberOfThreads[%d]", numberOfThreads_);
  }

  JsonValue *port = config_->getValue("port");
  port_ = 8080;

  if (port->isNumber() && port->toInteger() > 0) {
    port_ = port->toInteger();
    INFO("port(%u)", port_);
  } else  {
    INFO("port[%u]", port_);
  }

  JsonValue *documentRoot = config_->getValue("documentRoot");
  documentRoot_ = "./www";

  if (documentRoot != NULL && documentRoot->isString()) {
    // TODO remove trailing slash
    documentRoot_ = documentRoot->getString();
    INFO("documentRoot(%s)", documentRoot_.c_str());
  } else {
    INFO("documentRoot[%s]", documentRoot_.c_str());
  }

  JsonValue *defaultFile = config_->getValue("defaultFile");
  defaultFile_ = "index.html";

  if (defaultFile != NULL && defaultFile->isString()) {
    defaultFile_ = defaultFile->getString();
    INFO("defaultFile(%s)", defaultFile_.c_str());
  } else {
    INFO("defaultFile[%s]", defaultFile_.c_str());
  }

  JsonValue *timeout = config_->getValue("timeout");
  timeout_ = 5;

  if (timeout != NULL && timeout->isNumber()) {
    timeout_ = timeout->toInteger();
    INFO("timeout(%d)", timeout_);
  } else {
    INFO("timeout[%d]", timeout_);
  }

  return 0;
}

//------------------------------------------------------------------------------
void WebServer::initialize() {

  serverSockFd_ = socket(PF_INET, SOCK_STREAM, 0);

  memset(&serverAddr_, 0, sizeof(struct sockaddr_in));
  memset(&clientAddr_, 0, sizeof(struct sockaddr_in));

  serverAddr_.sin_family = AF_INET;
  serverAddr_.sin_port = htons(port_);

  serverAddr_.sin_addr.s_addr = htonl(INADDR_ANY);

  bind(serverSockFd_, (struct sockaddr *)&serverAddr_, sizeof(struct sockaddr_in));

  listen(serverSockFd_, MAX_LISTEN);

  INFO("Listening to port %u", port_);

  clientSize_ = sizeof(struct sockaddr_in);

  // Initialize threads mutex and conditions
  pthread_mutex_init(&mutexSockFd_, NULL);
  pthread_cond_init(&condSockFdEmpty_, NULL);
  pthread_cond_init(&condSockFdFull_, NULL);

  clientSockFdCount_ = 0;
  clientSockFdRd_ = 0;
  clientSockFdWr_ = 0;

  status_ = RUNNING;

  // Wake up threads
  thread_ = (pthread_t *)malloc(sizeof(pthread_t)*numberOfThreads_);
  threadId_ = (int *)malloc(sizeof(int)*numberOfThreads_);

  // Get as many client sockets as threads
  clientSockFd_ = (int *)malloc(sizeof(int)*numberOfThreads_);

  for (int i = 0; i < numberOfThreads_; i++) {

    threadId_[i] = i;

    if (pthread_create(&thread_[i], NULL, worker, this) != 0) {
      //DEBUG("Error creating thread");
    }
    INFO("Spawning worker(%u) thread", i);
  }
}

//------------------------------------------------------------------------------
void WebServer::finalize() {

  for (int i = 0; i < numberOfThreads_; i++) {
    pthread_join(thread_[i], NULL);
  }

  pthread_mutex_destroy(&mutexSockFd_);
  pthread_cond_destroy(&condSockFdEmpty_);
  pthread_cond_destroy(&condSockFdFull_);

  free(thread_);
  free(threadId_);
  free((void *)clientSockFd_);
}

//------------------------------------------------------------------------------
void *WebServer::run() {

  int sockfd;

  // Initialize() calls pthread_create, which in turn each thread
  // runs the handler() function. This function has a while loop using
  // the WebServer::quit variable.
  initialize();

  while (status_) {

    sockfd = accept(serverSockFd_, (struct sockaddr *)&clientAddr_, &clientSize_);

    // Server can be stopped while accepting connections, so check here for status
    if (status_ == STOPPED) break;

    // start of mutex area
    pthread_mutex_lock(&mutexSockFd_);

    while (clientSockFdCount_ > numberOfThreads_) {
        pthread_cond_wait(&condSockFdFull_, &mutexSockFd_);
    }

    clientSockFd_[clientSockFdWr_] = sockfd;

    clientSockFdCount_++;
    clientSockFdWr_++;
    clientSockFdWr_ = clientSockFdWr_ % numberOfThreads_;

    pthread_cond_broadcast(&condSockFdEmpty_);
    pthread_mutex_unlock(&mutexSockFd_);

    // end of mutex area
  }

  finalize();

  return 0;
}

//------------------------------------------------------------------------------
void WebServer::stop() {
  setStatus(STOPPED);
  shutdown(serverSockFd_, 2); 
}

//------------------------------------------------------------------------------
void *WebServer::worker(void *args) {

  WebServer *server = (WebServer *)args;

  int sockFd;

  sigset_t sigpipe_mask;
  sigemptyset(&sigpipe_mask);
  sigaddset(&sigpipe_mask, SIGPIPE);
  sigset_t saved_mask;

  if (pthread_sigmask(SIG_BLOCK, &sigpipe_mask, &saved_mask) == -1) {
    // DEBUG("pthread_sigmask error");
  }

  while (server->status_ == RUNNING) {

    // start of mutex area
    pthread_mutex_lock(&mutexSockFd_);

    while (clientSockFdCount_ == 0) {
      pthread_cond_wait(&condSockFdEmpty_, &mutexSockFd_);
    }

    sockFd = clientSockFd_[clientSockFdRd_];

    clientSockFdRd_++;
    clientSockFdRd_ = clientSockFdRd_ % server->numberOfThreads_;
    clientSockFdCount_--;

    pthread_cond_broadcast(&condSockFdFull_);
    pthread_mutex_unlock(&mutexSockFd_);

    DEBUG("Received connection %u", count_);
    count_++;

    // end of mutex area
    server->requestHandler(sockFd);

    INFO("Closing client socket %u", sockFd);
    close(sockFd);
  }

  DEBUG("Worker shutdown");

  return 0;
}

//------------------------------------------------------------------------------
int8_t WebServer::requestHandler(int sockfd) {

  int n, m;

  fd_set selectSet;

  struct timeval timeout;

  WebServer::request_t req;
  WebServer::response_t resp;

  timeout.tv_sec = timeout_;
  timeout.tv_usec = 0;

  string conn;

  // Wait for data to be received or connection timeout
  FD_ZERO(&selectSet);
  FD_SET(sockfd, &selectSet);

  n = select(sockfd + 1, &selectSet, NULL, NULL, &timeout);

  if (n < 0) {
    DEBUG("Client closed connection");
    FD_CLR(sockfd, &selectSet);
    FD_ZERO(&selectSet);
    return 0;
  } else if (n == 0) {
    // timeout
    DEBUG("Connection timed out");
    FD_CLR(sockfd, &selectSet);
    FD_ZERO(&selectSet);
    return 1;
  } else {

    if (FD_ISSET(sockfd, &selectSet)) {

      m = handleRequest(sockfd, &req);

      if (m < 0) {
        // handleRequest error
        clearRequest(&req);
        return -1;
      }
    }  
  }

  conn = getHeader(&req.headers, "Connection");

  if (conn.length() == 0 || conn.compare("close") == 0) {
    // Some http clients (e.g. curl) may not send the Connection header ...
    // Just handle it as a close connection
    m = handleResponse(sockfd, &req, &resp);

  } else {

    // Assume keep-alive connection... but keep it simple for now
    //timeout.tv_sec = 0;
    //timeout.tv_usec = 250;

    m = handleResponse(sockfd, &req, &resp);
  }

  clearRequest(&req);
  clearResponse(&resp);

  FD_CLR(sockfd, &selectSet);
  FD_ZERO(&selectSet);

  return 0;
}

//------------------------------------------------------------------------------
int8_t WebServer::handleRequest(int sockfd, request_t *req) {

  vectstr_t components;

  receiveRequest(sockfd, req);

  components = split(req->uri, '?');

  if (components.size() != 0) {
    req->resource = components[0];

    if (components.size() == 2) { 
      req->query = components[1]; 
    }
  } else {
    req->resource = req->uri;
  }

  // TODO: if Content-length header exists, get the request body part

  return 0;
}

//------------------------------------------------------------------------------
int8_t WebServer::handleResponse(int sockfd, request_t *req, response_t *res) {

  if (req == NULL || res == NULL) {
    ERROR("Empty parameters");
    return -1;
  }

  int8_t ret = -1;

  std::time_t t = std::time(NULL);
  char date[100];

  res->version = "HTTP/1.1";
  std::strftime(date, 100, "%c", std::localtime(&t));

  setHeader(&(res->headers), "Date", std::string(date));
  setHeader(&(res->headers), "Server", "Simplicity web server");

  if (req->method.compare("GET") == 0) {
    ret = handleGet(sockfd, req, res);
  } else if (req->method.compare("POST") == 0) {
    ret = handlePost(sockfd, req, res);
  } else if (req->method.compare("HEAD") == 0) {
    ret = handleHead(sockfd, req, res);
  }

  if (ret < 0) {
    handleError(sockfd, req, res);
  }

  return 0;
}

//------------------------------------------------------------------------------
int8_t WebServer::receiveRequest(int sockfd, request_t *req) {

  char chunk;

  int received, i, n;

  std::string line;
  std::stringstream ss;

  WebServer::header_t header;
  vectstr_t splitted;

  i = 0;
  received = 0;
  n = 0;

  ss.str("");

  // read request byte by byte
  while ((n = read(sockfd, &chunk, 1)) > 0) {

    ss.put(chunk);
    received += n;

    // TODO add upper limit!!
    if (received >= 4 && ss.str().find("\r\n\r\n") != std::string::npos) {
      break;
    }
  }

  // parse raw data
  while (std::getline(ss, line)) {

    if (i == 0) {

      splitted = split(line, ' ');

      req->method = trim(splitted[0]);
      req->uri = trim(splitted[1]);

      if (splitted.size() > 2) {
        req->version = trim(splitted[2]);
      }

      DEBUG("Request = %s %s %s", 
        req->method.c_str(),
        req->uri.c_str(),
        req->version.c_str());

    } else {

      splitted = split(line, ':');

      if (splitted.size() == 2) {
        header.name = trim(splitted[0]);
        header.value = trim(splitted[1]);

        req->headers.push_back(header);
        DEBUG("Header = %s: %s", header.name.c_str(), header.value.c_str());
      }
    }

    i++;
  }

  return 0;
}

//------------------------------------------------------------------------------
void WebServer::clearRequest(request_t *req) {

  if (req == NULL) {
    ERROR("Null request");
    return;
  }

  req->method.clear();
  req->uri.clear();
  req->version.clear();
  req->resource.clear();
  req->query.clear();
  req->headers.clear();
}

//------------------------------------------------------------------------------
void WebServer::clearResponse(response_t *res) {

  if (res == NULL) {
    ERROR("Null response");
    return;
  }

  res->statusCode = 0;
  res->reasonPhrase.clear();
  res->filePath.clear();
  res->headers.clear();
}

//------------------------------------------------------------------------------
std::string WebServer::getHeader(std::vector<header_t> *headers, std::string name) {

  std::string result("");

  if (headers == NULL) {
    ERROR("Null headers");
    return result;
  }

  std::vector<header_t>::iterator it;

  for (it = headers->begin(); it != headers->end(); ++it) {
    if (it->name.compare(name) == 0) {
      result = it->value;
      break;
    }
  }

  return result;
}

//------------------------------------------------------------------------------
void WebServer::setHeader(std::vector<header_t> *headers, std::string name, std::string value) {

  header_t header;

  header.name = name;
  header.value = value;

  headers->push_back(header);
}

//------------------------------------------------------------------------------
int8_t WebServer::handleGet(int sockfd, request_t *req, response_t *res) {

  uint32_t size = 0;

  uint32_t w = 0;

  std::string conn;
  std::string file;

  std::ifstream infile;
  std::stringstream ss;

  Buffer buf;

  ss << WebServer::documentRoot_;

  if (req->resource.at(req->resource.length()-1) == '/') {
    ss << req->resource << defaultFile_;
  } else {
    ss << req->resource;
  }

  file = ss.str();
  infile.open(file.c_str());

  ss.str("");

  if (infile) {

    res->statusCode = 200;
    res->reasonPhrase = "OK";

    infile.seekg(0, std::ifstream::end);
    size = infile.tellg();

    ss << size;
    setHeader(&(res->headers), "Length", ss.str());

    conn = getHeader(&(req->headers), "Connection");
    transform(conn.begin(), conn.end(), conn.begin(), ::tolower);

    setHeader(&(res->headers), "Connection", conn.compare("close") == 0 ? "close" : "keep-alive");
    setHeader(&(res->headers), "Content-type", "text/html");

    appendResponseHeaders(res, &buf);

    // read and append file
    FILE *fs = fopen(file.c_str(), "r");
    if (fs == NULL) {
      ERROR("Error opening file %s", file.c_str());
      return -1;
    }

    char tmp[1024];
    while ((size = fread(tmp, sizeof(char), 1024, fs)) > 0) {
      buf.append(tmp, size);
    }

    if ((w = send(sockfd, buf.getRaw(), buf.length(), 0)) != buf.length()) {
      ERROR("Error sending response");
    }

    fclose(fs);

  } else {

    res->statusCode = 404;
    res->reasonPhrase = "Not found";

    DEBUG("File %s not found", file.c_str());

    conn = getHeader(&(req->headers), "Connection");
    transform(conn.begin(), conn.end(), conn.begin(), ::tolower);

    setHeader(&(res->headers), "Connection", conn.compare("close") == 0 ? "close" : "keep-alive");

    appendResponseHeaders(res, &buf);

    if ((w = send(sockfd, buf.getRaw(), buf.length(), 0)) != buf.length()) {
      ERROR("Error sending response");
    }
  }

  infile.close();
  return 0;
}

//------------------------------------------------------------------------------
int8_t WebServer::handlePost(int sockfd, request_t *req, response_t *res) {

  return 0;
}

//------------------------------------------------------------------------------
int8_t WebServer::handleHead(int sockfd, request_t *req, response_t *res) {

  return 0;
}

//------------------------------------------------------------------------------
void WebServer::handleError(int sockfd, request_t *req, response_t *res) {

  if (res == NULL) {
    ERROR("Null response");
    return;
  }

  uint32_t w = 0;

  Buffer buf;

  clearResponse(res);

  res->statusCode = 500;
  res->reasonPhrase = "Internal Server Error";

  setHeader(&(res->headers), "Connection", "close");

  appendResponseHeaders(res, &buf);

  if ((w = send(sockfd, buf.getRaw(), buf.length(), 0)) != buf.length()) {
    ERROR("Error sending response headers");
  }
}

//------------------------------------------------------------------------------
void WebServer::appendResponseHeaders(response_t *res, Buffer *buf) {

  if (buf == NULL) {
    ERROR("Null buffer");
  }

  char codeStr[4];
  std::vector<header_t>::iterator it;

  snprintf(codeStr, 4, "%hu", res->statusCode);

  buf->append(res->version.c_str(), res->version.length());
  buf->append(" ", 1);
  buf->append(codeStr, 3);
  buf->append(" ", 1);
  buf->append(res->reasonPhrase.c_str(), res->reasonPhrase.length());
  buf->append("\r\n", 2);

  for (it = res->headers.begin(); it != res->headers.end(); ++it) {
    buf->append(it->name.c_str(), it->name.length());
    buf->append(": ", 2);
    buf->append(it->value.c_str(), it->value.length());
    buf->append("\r\n", 2);
  }

  buf->append("\r\n", 2);
}