#include <stdio.h>
#include <getopt.h>

#include <cstdlib>
#include <cstring>

#include "Log.hh"
#include "Simulation.hh"
#include "JsonReader.hh"
#include "JsonValue.hh"

typedef struct Options {
  int quiet;
  int verbosity;
  char *configFile;
  char *path;
  bool web;
} options_t;

static const struct option longOpts[] = {
  { "config",   required_argument,  NULL, 'c' },
  { "quiet",    no_argument,        NULL, 'q' },
  { "verbose",  no_argument,        NULL, 'v' },
  { "web",      no_argument,        NULL, 'w' },
  { "help",     no_argument,        NULL, 'h' },
  { NULL,       no_argument,        NULL, 0 }
};

static const char *optString = "c:qvwh?";

//------------------------------------------------------------------------------
void printHelp(const char *name) {
  
  printf("Usage: %s -c config-file\n", name);
  printf("\t-c, --config: configuration file\n");
  printf("\t-q, --quiet: no output\n");
  printf("\t-v, --verbosity: increase verbosity\n");
  printf("\t-w, --web: enable web interface\n");
  printf("\t-h, --help: this help\n");
}

//------------------------------------------------------------------------------
int getOptions(int argc, char *argv[], options_t *options) {

  int index = 0;
  int longIndex = 0;
  int c = 0;

  opterr = 0;

  while ((c = getopt_long(argc, argv, optString, longOpts, &longIndex)) != -1) {

    switch (c) {

      case 'c':
        options->configFile = optarg;
        break;
      case 'q':
        options->quiet++;
        break;
      case 'v':
        options->verbosity++;
        break;
      case 'w':
        options->web = true;
        break;
      case 'h':
      case '?':
        if (strcmp("config", longOpts[longIndex].name) == 0) {
          fprintf(stderr, "Option --%s requires an argument.\n", longOpts[longIndex].name);
        } else if (optopt == 'c') {
          fprintf(stderr, "Option -%c requires an argument.\n", optopt);
        }
        printHelp(argv[0]);
        exit(1);
      case 0: /* long option without a short arg */
        // if( strcmp( "randomize", longOpts[longIndex].name ) == 0 ) {
        //  globalArgs.randomized = 1;
        // }
        break;
      default:
        abort();
    }
  }

  for (index = optind; index < argc; index++)
    printf("Non-option argument %s\n", argv[index]);

  return 0;
}

//------------------------------------------------------------------------------
int main(int argc, char *argv[]) {

  options_t options;

  // Get options from command line
  options.verbosity = 0;
  options.quiet = 0;
  options.configFile = NULL;
  options.path = argv[0];
  options.web = false;

  getOptions(argc, argv, &options);

  if (options.configFile == NULL) {
    printf("Error: missing configuration file\n");
    printHelp(argv[0]);
    exit(1);
  }

  Simulation *sim = Simulation::getInstance();

  // Set log level
  if (options.quiet) {

    Logger::setLevel(Logger::LOG_NONE);
  
  } else {

    switch (options.verbosity) {
      case 1:
        Logger::setLevel(Logger::LOG_DEBUG);
        break;
      default:
        Logger::setLevel(Logger::LOG_INFO);
        break;
    }
  }

  int configResult = 0;

  if (options.web) {

    // Interactive simulation
    WebServer *server = new WebServer();

    configResult = server->configure(
      "server.json",
      options.path);

    if (configResult != 0) {
      ERROR("Error(%d): unable to configure web server", configResult);

      delete server;
      exit(configResult);
    }

    server->setSimulation(sim);
    server->start();

    INFO("web server started (press ctrl+c to exit)");

    server->join();
    delete server;
    server = NULL;

  } else {

    // Single run
    configResult = sim->configure(
      options.configFile,
      options.path);

    // Configuration should go flawless
    if (configResult != 0) {
      ERROR("Error(%d): unable to configure simulation", configResult);
      exit(configResult);
    }

    sim->run();
    sim->finalize();
  }

  exit(0);
}
