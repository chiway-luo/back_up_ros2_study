#ifndef PLANNER_CLEAN__VISIBILITY_CONTROL_H_
#define PLANNER_CLEAN__VISIBILITY_CONTROL_H_

// This logic was borrowed (then namespaced) from the examples on the gcc wiki:
//     https://gcc.gnu.org/wiki/Visibility

#if defined _WIN32 || defined __CYGWIN__
  #ifdef __GNUC__
    #define PLANNER_CLEAN_EXPORT __attribute__ ((dllexport))
    #define PLANNER_CLEAN_IMPORT __attribute__ ((dllimport))
  #else
    #define PLANNER_CLEAN_EXPORT __declspec(dllexport)
    #define PLANNER_CLEAN_IMPORT __declspec(dllimport)
  #endif
  #ifdef PLANNER_CLEAN_BUILDING_LIBRARY
    #define PLANNER_CLEAN_PUBLIC PLANNER_CLEAN_EXPORT
  #else
    #define PLANNER_CLEAN_PUBLIC PLANNER_CLEAN_IMPORT
  #endif
  #define PLANNER_CLEAN_PUBLIC_TYPE PLANNER_CLEAN_PUBLIC
  #define PLANNER_CLEAN_LOCAL
#else
  #define PLANNER_CLEAN_EXPORT __attribute__ ((visibility("default")))
  #define PLANNER_CLEAN_IMPORT
  #if __GNUC__ >= 4
    #define PLANNER_CLEAN_PUBLIC __attribute__ ((visibility("default")))
    #define PLANNER_CLEAN_LOCAL  __attribute__ ((visibility("hidden")))
  #else
    #define PLANNER_CLEAN_PUBLIC
    #define PLANNER_CLEAN_LOCAL
  #endif
  #define PLANNER_CLEAN_PUBLIC_TYPE
#endif

#endif  // PLANNER_CLEAN__VISIBILITY_CONTROL_H_
