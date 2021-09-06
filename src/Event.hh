#ifndef __EVENT_HH__
#define __EVENT_HH__

#include "Object.hh"
#include "Time.hh"

class Module;

class Event : public Object {

 private:

 protected:

  unsigned int type_;

  Module *owner_;

  Module *from_;

  Module *to_;

  Time et_;

  bool scheduled_;

 public:

  Event();

  Event(Module *owner, unsigned int type = 0);

  Event(double t);

  ~Event();

  unsigned int getType() { return type_; }

  Time* getTime() { return &et_; }

  Module* getOwner() { return owner_; }

  Module* getFrom() { return from_; }

  Module* getTo() { return to_; }

  bool isScheduled() { return scheduled_; }

  void setType(unsigned int type) { type_ = type; }

  void setTime(const Time &t) { et_ = t; }

  void setFrom(Module *from) { from_ = from; }

  void setTo(Module *to) { to_ = to; }

  void setScheduled(bool sched) { scheduled_ = sched; }

  static int32_t compare(Object *a, Object *b) {
    Time *t1 = ((Event *)a)->getTime();
    Time *t2 = ((Event *)b)->getTime();
    if (*t1 > *t2) return 1;
    if (*t1 < *t2) return -1;

    // Event times are equal below this line
    unsigned int ty1 = ((Event *)a)->getType();
    unsigned int ty2 = ((Event *)b)->getType();
    if (ty1 > ty2) return 1;
    if (ty1 < ty2) return -1;

    // Events have same time and type
    return 0;

    //return *t1 > *t2 ? 1 : (*t1 < *t2 ? -1 : 0);
  }
};

#endif
