/*
* Libavl is a library to manage AVL structure to store and organize
* everykind of data. You just need to implement function to compare,
* to desallocate and to print your structure.
*
* DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
* Version 2, December 2004
*
* Copyright (C) 2013 Adrien Oliva <adrien.oliva@yapbreak.fr>
*
* Everyone is permitted to copy and distribute verbatim or modified
* copies of this license document, and changing it is allowed as long
* as the name is changed.
*
* DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
* TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
*
* 0. You just DO WHAT THE FUCK YOU WANT TO.
*/

#ifndef __SYSLOG_H__
#define __SYSLOG_H__

#if LOGLEVEL != 0
# include <stdio.h>
#endif

/** \def ELOG(fmt, args...)
* \brief Macro to log error.
*
* \param fmt Format of message logged.
* \param args Argument(s) needed by format message.
*
* If \c LOGLEVEL macro is set to a value higher than 0, the \c ELOG
* macro will be expanded to a message on \c stdout, with flag \c [E],
* the name of function and current line, and your message. The \c fmt
* and \c args... arguments use the \c printf format.
*/
#if LOGLEVEL > 0
# define ELOG(fmt, args...) printf("[E] %s:%u "fmt"\n", __func__,\
__LINE__,\
##args)
#else
# define ELOG(fmt, args...)
#endif

/** \def WLOG(fmt, args...)
* \brief Macro to log warning.
*
* \param fmt Format of message logged.
* \param args Argument(s) needed by format message.
*
* If \c LOGLEVEL macro is set to a value higher than 1, the \c WLOG
* macro will be expanded to a message on \c stdout, with flag \c [W],
* the name of function and current line, and your message. The \c fmt
* and \c args... arguments use the \c printf format.
*/
#if LOGLEVEL > 1
# define WLOG(fmt, args...) printf("[W] %s:%u "fmt"\n", __func__,\
__LINE__,\
##args)
#else
# define WLOG(fmt, args...)
#endif

/** \def ILOG(fmt, args...)
* \brief Macro to log information.
*
* \param fmt Format of message logged.
* \param args Argument(s) needed by format message.
*
* If \c LOGLEVEL macro is set to a value higher than 2, the \c ILOG
* macro will be expanded to a message on \c stdout, with flag \c [I],
* the name of function and current line, and your message. The \c fmt
* and \c args... arguments use the \c printf format.
*/
#if LOGLEVEL > 2
# define ILOG(fmt, args...) printf("[I] %s:%u "fmt"\n", __func__,\
__LINE__,\
##args)
#else
# define ILOG(fmt, args...)
#endif

/** \def DLOG(fmt, args...)
* \brief Macro to log debug info.
*
* \param fmt Format of message logged.
* \param args Argument(s) needed by format message.
*
* If \c LOGLEVEL macro is set to a value higher than 3, the \c DLOG
* macro will be expanded to a message on \c stdout, with flag \c [D],
* the name of function and current line, and your message. The \c fmt
* and \c args... arguments use the \c printf format.
*/
#if LOGLEVEL > 3
# define DLOG(fmt, args...) printf("[D] %s:%u "fmt"\n", __func__,\
__LINE__,\
##args)
#else
# define DLOG(fmt, args...)
#endif

/** \def ENTER_FN()
* \brief Macro to log start of function.
*
* If \c LOGLEVEL macro is set to a value higher than 4, the \c ENTER_FN
* macro will be expanded to a message on \c stdout, with flag \c [F],
* the name of function and current line.
*/
#if LOGLEVEL > 4
# define ENTER_FN() printf("[F] -----> %s (%u)\n", __func__, __LINE__)
#else
# define ENTER_FN()
#endif

#endif
