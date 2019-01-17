#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([dtmf], m4_esyscmd([ tr -d '\n' < version ]))

CXXFLAGS=""
LDFLAGS=""

AC_CONFIG_SRCDIR([src/dtmfutil.cpp])

AC_CONFIG_HEADER([include/config.h])

LT_INIT
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE([disable])

AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_FILES([Makefile
		src/Makefile])

# Checks for programs.
AC_PROG_CXX
AC_PROG_INSTALL

# Checks for header files.
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.scriptsste
AC_C_CONST
AC_TYPE_SIZE_T

# Libs list autmatically generated prom dependecy script
AC_CHECK_LIB([asound],[snd_pcm_open],[],[AC_MSG_FAILURE([could not find lib alsa])])

AC_CANONICAL_HOST
case $host_os in
  linux*)
              OPTIMIZATION="-O2 "
              if test "x$CC" = xgcc; then
                    CXXFLAGS=" -std=c++14 -g -Wall -Wextra \
                             -D_FORTIFY_SOURCE=2 ${OPTIMIZATION} -fstack-protector \
                             --param=ssp-buffer-size=4 -Wformat -Werror=format-security  "
                    LDFLAGS=" $LDFLAGS  -Wl,-z,relro -lasound "
              else
                    CXXFLAGS=" -std=c++14 -O2 -g -Wall  "
		            LDFLAGS=" $LDFLAGS -lasound  "
              fi
        ;;
    *)
              AC_MSG_ERROR([Your platform is not currently supported])
        ;;
esac


AC_OUTPUT
