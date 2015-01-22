//
// This implementation is a port to C++ from Eloquent Javascript BinaryHeap.
// http://eloquentjavascript.net/1st_edition/appendix2.html
//

#ifndef __BINARYHEAP_HH__
#define __BINARYHEAP_HH__

#include <vector>
#include <cmath>
#include "Object.hh"

#define DEFAULT_HEAP_SIZE 1024

typedef int32_t (* comparator_t)(Object *a, Object *b);

class BinaryHeap : public Object {

 public:

 private:

 protected:

  std::vector<Object *> content_;

  comparator_t compare_;

 public:

  //
  // Constructor
  //
  BinaryHeap() {

  }

  //
  // Constructor
  //
  BinaryHeap(comparator_t sf) {
    compare_ = sf;
  }

  //
  // Destructor
  //
  ~BinaryHeap() {

  }

  void push(Object *obj);

  Object *pop(void);

  void remove(Object *obj);

  uint32_t size() { return content_.size(); };

  void bubbleUp(unsigned long n);

  void sinkDown(unsigned long n);

  //
  // Gets and sets
  //
  comparator_t getComparator(void) { return compare_; };

  std::vector<Object *> getContent(void) { return content_; };

  void setComparator(comparator_t sf) { compare_ = sf; };

};

#endif