#ifndef MONITOR_H
#define MONITOR_H

#ifdef __cplusplus
extern "C" {
#endif
  
  extern
  void annotate(const char * str);

  extern
  void start_monitor();

  extern
  void stop_monitor();

  typedef int (*error_returner_t)(long long *);
  typedef long long (*value_returner_t)();

  extern
  void register_tracker_error_returner_t(error_returner_t tracker, char const * key);

  extern
  void register_tracker_value_returner_t(value_returner_t tracker, char const * key);

#ifdef __cplusplus
}
#endif

#endif
