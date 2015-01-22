#include "BinaryHeap.hh"

//------------------------------------------------------------------------------
void BinaryHeap::push(Object *obj) {
  content_.push_back(obj);
  bubbleUp(content_.size() - 1);
}

//------------------------------------------------------------------------------
Object *BinaryHeap::pop() {

  Object *result = content_[0];
  Object *end = content_.back();

  if (content_.size() > 0) {
    content_.pop_back();
    content_[0] = end;
    sinkDown(0);
  }

  return result;
}

//------------------------------------------------------------------------------
void BinaryHeap::remove(Object *obj) {

  unsigned long length = content_.size();

  for (unsigned long i = 0; i < length; i++) {

    if (content_[i] != obj) continue;

    Object *end = content_.back();

    content_.pop_back();

    if (i == length - 1) break;

    content_[i] = end;
    bubbleUp(i);
    sinkDown(i);
    break;
  }
}

//------------------------------------------------------------------------------
void BinaryHeap::bubbleUp(unsigned long n) {

  Object *element = content_[n];

  while (n > 0) {

    //unsigned long parentN = floor((n + 1) / 2) - 1;
    unsigned long parentN = ((n + 1) / 2) - 1;
    Object *parent = content_[parentN];

    if (compare_(element, parent) >= 0) break;

    content_[parentN] = element;
    content_[n] = parent;
    n = parentN;
  }
}

//------------------------------------------------------------------------------
void BinaryHeap::sinkDown(unsigned long n) {

  unsigned long size = content_.size();
  unsigned long child2N = 0;
  unsigned long child1N = 0;
  unsigned long swap = 0;

  Object* elem = content_[n];
  Object* child1 = NULL;
  Object* child2 = NULL;

  while (true) {
    
    child2N = (n+1)*2;
    child1N = child2N - 1;

    swap = 0;

    if (child1N < size) {

      child1 = content_[child1N];

      if (compare_(elem, child1) > 0) {
        swap = child1N;
      }
    }

    if (child2N < size) {

      child2 = content_[child2N];

      if (swap == 0) {
        if (compare_(elem, child2) > 0) {
          swap = child2N;
        }
      } else {
        if (compare_(child1, child2) > 0) {
          swap = child2N;
        }
      }
    }

    if (swap == 0) break;

    content_[n] = content_[swap];
    content_[swap] = elem;
    n = swap;
  }
}