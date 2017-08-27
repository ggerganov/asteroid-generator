/*! \file cg_io.cpp
 *  \brief Enter description here.
 *  \author Georgi Gerganov
 */

#include "cg_io.h"

#include <cstring>
#include <cstdarg>
#include <cstdlib>


FILE* CG_IO_OUTPUTS::getFILEInfo() {
  static FILE *file = 0;
  if( !file ) file = fopen(".CGINFO.log", "w");
  return file;
}

FILE* CG_IO_OUTPUTS::getFILEDBG() {
  static FILE *file = 0;
  if( !file ) file = fopen(".CGDBG.log", "w");
  return file;
}

FILE* CG_IO_OUTPUTS::getFILEFatal() {
  static FILE *file = 0;
  if( !file ) file = fopen(".CGFATAL.log", "w");
  return file;
}

#ifdef NO_VARDIADIC_MACROS

void INFO(const char *msg, ...) {
    static char *buffer = 0;
    if ( !buffer ) buffer = new char[1024];

    va_list ap; va_start(ap, msg);
    sprintf(buffer, "%s", GREEN);
    fprintf(CG_IO_OUTPUTS::getFILEInfo(), "%s", buffer);
    vfprintf(CG_IO_OUTPUTS::getFILEInfo(), msg, ap ); fflush(CG_IO_OUTPUTS::getFILEInfo());
    va_end(ap);
}

void DBG(int tag, const char *msg, ...) {
    static char *buffer = 0;
    if ( !buffer ) buffer = new char[1024];

    int line = -1;
    char f2[] = "Win32", func[] = "???";

    va_list ap; va_start(ap, msg);
    sprintf(buffer, "%sDBG(%s, %li): ", GREEN, f2, tag);
    fprintf(CG_IO_OUTPUTS::getFILEDBG(), "%-*s%s[%-*s] %s", INDENT, buffer, BLUE, INDENT, func, YELLOW);
    vfprintf(CG_IO_OUTPUTS::getFILEDBG(), msg, ap ); fflush(CG_IO_OUTPUTS::getFILEDBG());
    va_end(ap);
}


void DBGD(int tag, const char *msg, ...) {
    static char *buffer = 0;
    if ( !buffer ) buffer = new char[1024];

    int line = -1;
    char f2[] = "Win32", func[] = "???";

    va_list ap; va_start(ap, msg);
    sprintf(buffer, "%sDBGD(%s, %li): ", GREEN, f2, tag);
    fprintf(CG_IO_OUTPUTS::getFILEDBG(), "%-*s%s[%-*s] %s", INDENT, buffer, BLUE, INDENT, func, YELLOW);
    vfprintf(CG_IO_OUTPUTS::getFILEDBG(), msg, ap ); fflush(CG_IO_OUTPUTS::getFILEDBG());
    va_end(ap);
}

void FATAL(const char *msg, ...) {
    static char *buffer = 0;
    if ( !buffer ) buffer = new char[1024];

    int line = -1;
    char f2[] = "Win32", func[] = "???";

    va_list ap; va_start(ap, msg);
    sprintf(buffer, "%sFATAL(%s, %li): ", GREEN, f2, line);
    fprintf(CG_IO_OUTPUTS::getFILEFatal(), "%-*s%s[%-*s] %s", INDENT, buffer, BLUE, INDENT, func, RED);
    vfprintf(CG_IO_OUTPUTS::getFILEFatal(), msg, ap ); fflush(CG_IO_OUTPUTS::getFILEFatal());
    va_end(ap);
}

void FATALD(...) {}

#else

#  ifdef CG_OUTPUT
#    define INFO(...) cgInfo(__VA_ARGS__);
#    define DBG(T, ...) cgDebug(__FILE__, __LINE__, (T), __FUNCTION__, __VA_ARGS__);
#    define DBGD(T, ...) cgDebug(__FILE__, __LINE__, (T), __PRETTY_FUNCTION__, __VA_ARGS__);
#    define FATAL(...) cgFatal(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);
#    define FATALD(...) cgFatal(__FILE__, __LINE__, __PRETTY_FUNCTION__, __VA_ARGS__);
#  else
#    define INFO(...)
#    define DBG(...)
#    define DBGD(...)
#    define FATAL(...)
#    define FATALD(...)
#  endif

#endif

const unsigned int HOME_LEN = 0;

#ifndef CG_DUMMY

void cgInfo(const char *msg, ...) {
    static char *buffer = 0;
    if ( !buffer ) buffer = new char[1024];

    va_list ap; va_start(ap, msg);
    sprintf(buffer, "%s", GREEN);
    fprintf(stdout, "%s", buffer);
    vfprintf(stdout, msg, ap ); fflush(stdout);
    va_end(ap);
}

void cgDebug(const char *file, long line, int tag, const char *func, const char *msg, ...) {
    static char *buffer = 0;
    if ( !buffer ) buffer = new char[1024];

    char *f2;
    sprintf(buffer, "IO_DEBUG%i", tag);
    if (!getenv(buffer)) return;
    if (strlen(file) > HOME_LEN) {
        f2 = const_cast<char*>(&(file[HOME_LEN]));
    } else {
        f2 = const_cast<char*>(&(file[0]));
    }
    va_list ap; va_start(ap, msg);
    sprintf(buffer, "%sDBG(%s, %li): ", GREEN, f2, line);
    fprintf(stdout, "%-*s%s[%-*s] %s", INDENT, buffer, BLUE, INDENT, func, YELLOW);
    vfprintf(stdout, msg, ap ); fflush(stdout);
    va_end(ap);
}

void cgFatal(const char *file, long line, const char *func, const char *msg, ...) {
    static char *buffer = 0;
    if ( !buffer ) buffer = new char[1024];

    char *f2;
    sprintf(buffer, "IO_DEBUG0");
    if (!getenv(buffer)) return;
    if (strlen(file) > HOME_LEN) {
        f2 = const_cast<char*>(&(file[HOME_LEN]));
    } else {
        f2 = const_cast<char*>(&(file[0]));
    }
    va_list ap; va_start(ap, msg);
    sprintf(buffer, "%sFATAL(%s, %li): ", GREEN, f2, line);
    fprintf(stdout, "%-*s%s[%-*s] %s", INDENT, buffer, BLUE, INDENT, func, RED);
    vfprintf(stdout, msg, ap ); fflush(stdout);
    va_end(ap);
}

#else

void cgInfo(const char *msg, ...) {
    static char *buffer = 0;
    if ( !buffer ) buffer = new char[1024];

    va_list ap; va_start(ap, msg);
    sprintf(buffer, "%s", GREEN);
    fprintf(CG_IO_OUTPUTS::getFILEInfo(), "%s", buffer);
    vfprintf(CG_IO_OUTPUTS::getFILEInfo(), msg, ap ); fflush(CG_IO_OUTPUTS::getFILEInfo());
    va_end(ap);
}

void cgDebug(const char *file, long line, int tag, const char *func, const char *msg, ...) {
    static char *buffer = 0;
    if ( !buffer ) buffer = new char[1024];

    char *f2;
    if (strlen(file) > HOME_LEN) {
        f2 = const_cast<char*>(&(file[HOME_LEN]));
    } else {
        f2 = const_cast<char*>(&(file[0]));
    }
    va_list ap; va_start(ap, msg);
    sprintf(buffer, "%sDBG(%s, %li): ", GREEN, f2, line);
    fprintf(CG_IO_OUTPUTS::getFILEDBG(), "%-*s%s[%-*s] %s", INDENT, buffer, BLUE, INDENT, func, YELLOW);
    vfprintf(CG_IO_OUTPUTS::getFILEDBG(), msg, ap ); fflush(CG_IO_OUTPUTS::getFILEDBG());
    va_end(ap);
}

void cgFatal(const char *file, long line, const char *func, const char *msg, ...) {
    static char *buffer = 0;
    if ( !buffer ) buffer = new char[1024];

    char *f2;
    if (strlen(file) > HOME_LEN) {
        f2 = const_cast<char*>(&(file[HOME_LEN]));
    } else {
        f2 = const_cast<char*>(&(file[0]));
    }
    va_list ap; va_start(ap, msg);
    sprintf(buffer, "%sFATAL(%s, %li): ", GREEN, f2, line);
    fprintf(CG_IO_OUTPUTS::getFILEFatal(), "%-*s%s[%-*s] %s", INDENT, buffer, BLUE, INDENT, func, RED);
    vfprintf(CG_IO_OUTPUTS::getFILEFatal(), msg, ap ); fflush(CG_IO_OUTPUTS::getFILEFatal());
    va_end(ap);
}
#endif

