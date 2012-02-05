#ifndef STATUS_H
#define STATUS_H

#define SUCCESS       0
#define EINIT_FAILURE 1
#define ENULL_NAMES   2
#define ENO_MODULES   3
#define ENULL_REGEX   4
#define EMODNEREGEX   5
#define EEXIT_FAILURE 6

#define EREGEX_ERROR  40

int moduleError(int statusCode, int errorCode);

#endif // STATUS_H
