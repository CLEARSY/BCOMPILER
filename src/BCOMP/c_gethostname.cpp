#ifdef WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#endif
#include "c_gethostname.h"

int bcomp_gethostname(char* name, int len) {
    return gethostname(name, len);
}
