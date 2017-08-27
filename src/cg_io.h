/*! \file cg_io.h
 *  \brief Implementation of some standard input and output routines.
 *  \author Georgi Gerganov
*/

#ifndef __CG_IO_H__
#define __CG_IO_H__

#ifdef COLOR_OUTPUT
#define RED "\033[01;31m"
#define YELLOW "\033[01;33m"
#define GREEN "\033[01;32m"
#define BLUE "\033[01;34m"
#else
#define RED ""
#define YELLOW ""
#define GREEN ""
#define BLUE ""
#endif

//! \brief Number of whitespaces used in the io functions.
#define INDENT 38
//! \brief Macro for tagging debug messages.

#include <cstdio>

class CG_IO_OUTPUTS {
  public:
    static FILE* getFILEInfo();
    static FILE* getFILEDBG();
    static FILE* getFILEFatal();
};

#ifdef NO_VARDIADIC_MACROS
// Microsoft's compiler does not support Macros with variable number of arguments.
// Therefore, we get rid of the __FILE__ and __LINE__ information and simply print
// the debug messages.

void INFO(const char *msg, ...);
void DBG(int tag, const char *msg, ...);
void DBGD(int tag, const char *msg, ...);
void FATAL(const char *msg, ...);
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

#ifndef CG_DUMMY

/*! \brief Output any information for the user to the standard output with this function
 */
void cgInfo(const char *msg, ...);

/*! \brief Output any debug information with this function.

    A useful macro \link DBG() DBG(T, ...) \endlink is implemented. The macro takes as arguments
    a tag vrber \a T and printf-style message together with its arguments. The
    \a T macro is used to tag debug messages, so that it is possible to turn the
    output of specific debug information using the environment variables defined
    in the \c Makefile.

    For example adding "export IO_DEBUG5" in the \c Makefile will turn on all
    debug messages tagged with the vrber \c 5 .
 */
void cgDebug(const char *file, long line, int tag, const char *func, const char *msg, ...);

/*! \brief Output any critical error messages with this function.

    Usually after such messages the execution of the program has to be
    terminated.
 */
void cgFatal(const char *file, long line, const char *func, const char *msg, ...);

#else

void cgInfo(const char *msg, ...);
void cgDebug(const char *file, long line, int tag, const char *func, const char *msg, ...);
void cgFatal(const char *file, long line, const char *func, const char *msg, ...);

#endif

#endif
