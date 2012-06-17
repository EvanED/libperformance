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
  void register_tracker_error_returner(error_returner_t tracker, char const * key);

  extern
  void register_tracker_value_returner(value_returner_t tracker, char const * key);


  /// You may not store or free the value of the argument. It is managed by the system.
  typedef void (*monitor_callback_t)(char const *);

  extern
  void register_montior_callback(monitor_callback_t callback);

#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
#include <functional>
#include <string>

extern
void register_tracker(std::function<long long ()>, std::string const & str);

extern
void register_monitor_callback(std::function<void (char const *)>);

#endif


#endif
