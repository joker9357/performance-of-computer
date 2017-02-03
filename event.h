#ifndef EV_H
#define EV_H


struct Job {
  int priority;
  Job(int p){
    priority = p;
  }
};

struct Event {
  double time;            // Time at which Event takes place
  int type;               // Type of Event
  
  Job* cst;           // Customer in the Event
  bool lbd_source;        // Generate from Lambda
  
  Event* next;            // Points to next event in list
  Event(double t, int i, Job* cus, bool source) {
    time = t;
    type = i;
    cst = cus;
    lbd_source = source;
    next = 0;
  }
};

class EventList {
  Event* head;           // Points to first Event in EventList
  int event_count;       // Total number of Events in EventList
public:
  ~EventList() { clear();}
  EventList() { event_count = 0; head = 0;}
  void insert(double time, int type, Job* cus, bool source);  // Insert new event into EventList
  Event* get();                        // Returns first Event in EventList
  void clear();                        // Removes all Events from EventList
  Event* remove(int type);             // Returns first Event of given type 
};

#endif
