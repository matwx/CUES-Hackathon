#ifndef __YOTTA_BUILD_INFO_H__
#define __YOTTA_BUILD_INFO_H__
// yotta build info, #include YOTTA_BUILD_INFO_HEADER to access
#define YOTTA_BUILD_YEAR 2015 // UTC year
#define YOTTA_BUILD_MONTH 10 // UTC month 1-12
#define YOTTA_BUILD_DAY 24 // UTC day 1-31
#define YOTTA_BUILD_HOUR 23 // UTC hour 0-24
#define YOTTA_BUILD_MINUTE 11 // UTC minute 0-59
#define YOTTA_BUILD_SECOND 4 // UTC second 0-61
#define YOTTA_BUILD_UUID b3a840db-933c-44b2-8141-67fecd5afb0d // unique random UUID for each build
#define YOTTA_BUILD_VCS_ID 07ce6b69a02863980629fec757cb8d8f2c69955f // git or mercurial hash
#define YOTTA_BUILD_VCS_CLEAN 0 // evaluates true if the version control system was clean, otherwise false
#endif // ndef __YOTTA_BUILD_INFO_H__
