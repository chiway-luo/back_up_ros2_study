#ifndef PLANNER_SAMPLE__VISIBILITY_CONTROL_H_
#define PLANNER_SAMPLE__VISIBILITY_CONTROL_H_

// This logic was borrowed (then namespaced) from the examples on the gcc wiki:
//     https://gcc.gnu.org/wiki/Visibility

#if defined _WIN32 || defined __CYGWIN__
  #ifdef __GNUC__
    #define PLANNER_SAMPLE_EXPORT __attribute__ ((dllexport))
    #define PLANNER_SAMPLE_IMPORT __attribute__ ((dllimport))
  #else
    #define PLANNER_SAMPLE_EXPORT __declspec(dllexport)
    #define PLANNER_SAMPLE_IMPORT __declspec(dllimport)
  #endif
  #ifdef PLANNER_SAMPLE_BUILDING_LIBRARY
    #define PLANNER_SAMPLE_PUBLIC PLANNER_SAMPLE_EXPORT
  #else
    #define PLANNER_SAMPLE_PUBLIC PLANNER_SAMPLE_IMPORT
  #endif
  #define PLANNER_SAMPLE_PUBLIC_TYPE PLANNER_SAMPLE_PUBLIC
  #define PLANNER_SAMPLE_LOCAL
#else
  #define PLANNER_SAMPLE_EXPORT __attribute__ ((visibility("default")))
  #define PLANNER_SAMPLE_IMPORT
  #if __GNUC__ >= 4
    #define PLANNER_SAMPLE_PUBLIC __attribute__ ((visibility("default")))
    #define PLANNER_SAMPLE_LOCAL  __attribute__ ((visibility("hidden")))
  #else
    #define PLANNER_SAMPLE_PUBLIC
    #define PLANNER_SAMPLE_LOCAL
  #endif
  #define PLANNER_SAMPLE_PUBLIC_TYPE
#endif

#endif  // PLANNER_SAMPLE__VISIBILITY_CONTROL_H_
