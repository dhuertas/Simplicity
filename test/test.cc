#include <stdio.h>
#include <iostream>
#include <fstream>

#include "Event.hh"
#include "BinaryHeap.hh"
#include "JsonReader.hh"

#define BEGIN_TEST do { \
  total++;\
  printf("BEGIN TEST: %s\n", __FUNCTION__);\
} while(0);
#define END_TEST printf("END TEST: %s\n\n", __FUNCTION__);

#define OK  1
#define NOK 0 

unsigned int total = 0;

//------------------------------------------------------------------------------
int binaryHeapWorks() {

  BEGIN_TEST

  Time::setScaleExp(-9);

  Event *a = new Event(0.15612);
  Event *b = new Event(7.12345);
  Event *d = new Event(2.12359);
  Event *c = new Event(3.43541);
  Event *e = new Event(1.23456);
  Event *f = new Event(-2.6);
  Event *g = new Event(0.0);

  BinaryHeap *fes = new BinaryHeap();
  fes->setComparator(Event::compare);

  DEBUG("Event a(%p) t_(%f)", a, a->getTime()->dbl());
  DEBUG("Event b(%p) t_(%f)", b, b->getTime()->dbl());
  DEBUG("Event c(%p) t_(%f)", c, c->getTime()->dbl());
  DEBUG("Event d(%p) t_(%f)", d, d->getTime()->dbl());
  DEBUG("Event e(%p) t_(%f)", e, e->getTime()->dbl());
  DEBUG("Event f(%p) t_(%f)", f, f->getTime()->dbl());
  DEBUG("Event g(%p) t_(%f)", g, g->getTime()->dbl());

  fes->push(a);
  fes->push(b);
  fes->push(c);
  fes->push(d);
  fes->push(e);
  fes->push(f);
  fes->push(g);

  while (fes->size() > 0) {
    Event *ev = (Event *)fes->pop();
    DEBUG("Event time %f", ev->getTime()->dbl());
  }

  delete fes;

  delete g;
  delete f;
  delete e;
  delete d;
  delete c;
  delete b;
  delete a;

  DEBUG("Remaining objects: %u (%s)",
    Object::count,
    Object::count == 0 ? "OK" : "OOPS!");

  END_TEST
  return Object::count == 0 ? OK : NOK;
}

//------------------------------------------------------------------------------
int readJsonWorks() {

  BEGIN_TEST

  JsonReader *reader = new JsonReader();
  JsonValue *root = NULL;

  bool parsed = false;

  parsed = reader->parse("test.json", &root);

  if ( ! parsed) {
    DEBUG("Unable to parse file");
    if (root != NULL) delete root;
    return 0;
  }

  // Access array item
  char buf[1024];
  JsonValue *val = (*root)[1];
  DEBUG("array[%u]: %s", 1, val->toString(buf, 1024));
/*
  // Access object attribute
  char buf[1024];
  JsonObject *obj = (JsonObject *)root;
  printf("object[\"object\"]: %s\n", obj->find("object")->toString(buf, 1024));
*/
  if (root != NULL) {
    delete root;
  }

  delete reader;

  DEBUG("Remaining objects: %u (%s)",
    Object::count,
    Object::count == 0 ? "OK" : "OOPS!");

  END_TEST
  return Object::count == 0 ? OK : NOK;
}

//------------------------------------------------------------------------------
int loggerWorks() {

  BEGIN_TEST

  Logger::setLevel(Logger::LOG_DEBUG);

  DEBUG("This is a test");
  DEBUG("This is a test with a number %d and a string \"%s\"", 5, "yeah!");

  Logger::setLevel(Logger::LOG_NONE);

  DEBUG("This string should not appear");

  DEBUG("Remaining objects: %u (%s)",
    Object::count,
    Object::count == 0 ? "OK" : "OOPS!");

  END_TEST
  return Object::count == 0 ? OK : NOK;
}

//------------------------------------------------------------------------------
int main(int argc, char* argv[]) {

  uint32_t count = 0;

  Logger::setLevel(Logger::LOG_DEBUG);

  count += binaryHeapWorks();
  count += readJsonWorks();
  count += loggerWorks();

  printf("%u TOTAL / %u OK / %u NOK\n", total, count, total-count);
  printf("\n");

  return 0;
}
