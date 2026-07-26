dnl
dnl AC_CHECK_CC_M(<compiler>,<extension>,<flag>,<result>)
dnl
dnl test if the compilers can detect the dependencies alone
dnl

AC_DEFUN(AC_CHECK_CC_M,[
AC_MSG_CHECKING([whether $1 accepts $3])
AC_CACHE_VAL(ac_cv_$4,[

# compile the sample program
echo 'foo(){}' > conftest.$2

# check if the dependency was detected
if test "x`$1 $3 conftest.$2 2>/dev/null | tr -d ' '`" = "xconftest.o:conftest.$2"; then
  $4=YES
else
  $4=NO
fi
rm -f conftest*
ac_cv_$4=$$4
])
$4=$ac_cv_$4
lower=`echo $$4 | tr A-Z a-z`
AC_MSG_RESULT($lower)
])

dnl
dnl AC_CHECK_GLD(<compiler>, <flag>)
dnl
dnl test if the compiler uses the GNU linker
dnl

AC_DEFUN(AC_CHECK_GLD,[
AC_MSG_CHECKING([whether $1 uses GNU ld])
AC_CACHE_VAL(ac_cv_$2,[
echo "main(){return 0;}" > conftest.c
if GNUTARGET=no_such_target $CC $CFLAGS -o conftest conftest.c 2>/dev/null >/dev/null; then
  ac_cv_$2=NO
else
  ac_cv_$2=YES
fi
rm conftest*
])
$2=$ac_cv_$2
lower=`echo $$2 | tr A-Z a-z`
AC_MSG_RESULT($lower)
AC_SUBST($2)
])

dnl
dnl AC_CHECK_EXCEPTIONS()
dnl
dnl test if exception-handling works
dnl

AC_DEFUN(AC_CHECK_EXCEPTIONS,[
AC_MSG_CHECKING([whether $CXX handles exceptions])
AC_CACHE_VAL(ac_cv_have_exceptions,[
# write a sample program
cat > conftest.cc << EOF
#include <iostream.h>
main(){ try{throw 7;}catch(int n){cout<<n;} return 0;}
EOF
# check if the magic number was thrown
ac_cv_have_exceptions=NO
if $CXX $CXXFLAGS -o conftest conftest.cc 2>/dev/null >/dev/null; then
  if test x7 = x`./conftest`; then
    ac_cv_have_exceptions=YES
  fi
fi
rm conftest*
])
HAVE_EXCEPTIONS=$ac_cv_have_exceptions
lower=`echo $HAVE_EXCEPTIONS | tr A-Z a-z`
AC_MSG_RESULT($lower)
AC_SUBST(HAVE_EXCEPTIONS)
])

dnl
dnl AC_FIND_LIB(<upper>,<lower>,<header>,<lib>,<path>,<spaces>)
dnl
dnl search a library and the accompanying header file
dnl

AC_DEFUN(AC_FIND_LIB,[
AC_MSG_CHECKING([searching $2])
AC_CACHE_VAL(ac_cv_$2_home,[

# check the command-line at first
$1_HOME=""
AC_ARG_WITH($2,[  --with-$2=DIR$6 $2 library is in DIR],[$1_HOME="${with_$2}"])

# search the library in a search path
if test x${$1_HOME} = x; then
  for DIR in / /usr /usr/local \
    `echo $LD_LIBRARY_PATH: | sed -e "s@/lib:@ @g" -e "s@:@ @g"` $5; do
    if test -f $DIR/include/$3.h; then
      for EXT in a so sa dylib; do
        if test -f "$DIR/lib/lib$4.$EXT"; then
          $1_HOME=$DIR
          break 2
        fi
      done
    fi
  done
fi

# check if the library really exists
if test x${$1_HOME} != x; then
  if test ! -f ${$1_HOME}/include/$3.h; then
    $1_HOME=""
  fi
  MISSES=""
  for EXT in a so sa dylib; do
    if test ! -f ${$1_HOME}/lib/lib$4.$EXT; then
      MISSES="$MISSES+"
    fi
  done
  if test x+++ = x$MISSES; then
    $1_HOME=""
  fi
fi

# check if it was found at all
if test x${$1_HOME} != x; then
  HAVE_$1=YES
else
  HAVE_$1=NO
  $1_HOME=no
fi
ac_cv_$2_home=${$1_HOME}
ac_cv_have_$2=${HAVE_$1}
])
$1_HOME=$ac_cv_$2_home
HAVE_$1=$ac_cv_have_$2
AC_SUBST(HAVE_$1)
AC_SUBST($1_HOME)
AC_MSG_RESULT([${$1_HOME}])
])

AC_MSG_RESULT($lower)
AC_SUBST(HAVE_EXCEPTIONS)

dnl
dnl AC_FIND_APP(<upper>,<lower>,<file>,<path>,<spaces>)
dnl
dnl search a program
dnl

AC_DEFUN(AC_FIND_APP,[
AC_MSG_CHECKING([searching $2])
AC_CACHE_VAL(ac_cv_$2_home,[

# check the command-line at first
$1_HOME=""
AC_ARG_WITH($2,[  --with-$2=DIR$5 $2 is in DIR],[$1_HOME="${with_$2}"])

# search the program in a search path
if test x${$1_HOME} = x; then
  for DIR in /bin /sbin /usr/bin /usr/sbin /usr/local/bin \
    `echo $PATH | sed -e "s@:@ @g"` $4; do
    if test -f $DIR/$3; then
      $1_HOME=$DIR
      break 1
    fi
  done
fi

# check if the program really exists
if test x${$1_HOME} != x; then
  if test ! -f ${$1_HOME}/$3; then
    $1_HOME=""
  fi
fi

# check if it was found at all
if test x${$1_HOME} != x; then
  HAVE_$1=YES
else
  HAVE_$1=NO
  $1_HOME=no
fi
ac_cv_$2_home=${$1_HOME}
ac_cv_have_$2=${HAVE_$1}
])
$1_HOME=$ac_cv_$2_home
HAVE_$1=$ac_cv_have_$2
AC_SUBST(HAVE_$1)
AC_SUBST($1_HOME)
AC_MSG_RESULT([${$1_HOME}])
])

dnl
dnl AC_FIND_X11(<upper>,<lower>,<file>,<path>,<spaces>)
dnl
dnl find the x-windows libraries and headers
dnl

AC_DEFUN(AC_FIND_X11,[
AC_CACHE_VAL(ac_cv_x11_home,[
AC_PATH_XTRA
if test x${no_x} = xyes; then
  HAVE_X11=NO
  X11_HOME=no
else
  HAVE_X11=YES
  if test x = x${x_includes}; then
    if test x = x${x_libraries}; then
      X11_HOME=/usr
    else
      X11_HOME=`dirname ${x_libraries}`
    fi
  else
    X11_HOME=`dirname ${x_includes}`
  fi
fi
ac_cv_x11_home=$X11_HOME
ac_cv_have_x11=$HAVE_X11
])
X11_HOME=$ac_cv_x11_home
HAVE_X11=$ac_cv_have_x11
AC_SUBST(HAVE_X11)
AC_SUBST(X11_HOME)
AC_SUBST(X11_EXTRA_FLAGS)
])

dnl
dnl AC_FIND_NET()
dnl
dnl find the networking libraries
dnl

AC_DEFUN(AC_FIND_NET,[
NET_LIBS=""
if test "$ISC" = yes; then
  NET_LIBS="$NET_LIBS -lnsl_s -linet"
else
  AC_CHECK_LIB(dnet, dnet_ntoa, [NET_LIBS="$NET_LIBS -ldnet"])
  if test x$ac_cv_lib_dnet = xno; then
    AC_CHECK_LIB(dnet_stub, dnet_ntoa,
      [NET_LIBS="$NET_LIBS -ldnet_stub"])
  fi

  AC_CHECK_LIB(nsl, t_accept, [NET_LIBS="$NET_LIBS -lnsl"])

  if test "`(uname) 2>/dev/null`" != IRIX; then
    AC_CHECK_LIB(socket, socket, [NET_LIBS="$NET_LIBS -lsocket"])
  fi
fi

NET_HOME=/usr
AC_SUBST(NET_LIBS)
AC_SUBST(NET_HOME)
])

AC_DEFUN(AC_CHECK_NET,[
# check if the network works at all
AC_MSG_CHECKING([whether network works])
AC_CACHE_VAL(ac_cv_have_net,[
ac_save_LDFLAGS="$LDFLAGS"
LDFLAGS="$LDFLAGS $NET_LIBS"
AC_TRY_LINK([#include <sys/types.h>
#include <sys/socket.h>],[socket (AF_INET,SOCK_STREAM,0);],
ac_cv_have_net=YES,ac_cv_have_net=NO)])
LDFLAGS="$ac_save_LDFLAGS"
HAVE_NET=$ac_cv_have_net
lower=`echo $HAVE_NET | tr A-Z a-z`
AC_MSG_RESULT($lower)
AC_SUBST(HAVE_NET)
])

dnl
dnl AC_NET_FROM_X11
dnl
dnl use the X_EXTRA_LIBS as NET_LIBS
dnl

AC_DEFUN(AC_NET_FROM_X11,[
HAVE_NET=YES
NET_LIBS=$X_EXTRA_LIBS
NET_HOME=/usr
AC_SUBST(HAVE_NET)
AC_SUBST(NET_LIBS)
AC_SUBST(NET_HOME)
])

dnl
dnl AC_CLIB_CHECK(<upper>,<header>,<lib>,<func>)
dnl
dnl check if a library is in the C or an external library
dnl

AC_DEFUN(AC_C_LIB_CHECK,[
$1_LIBS=""

# search the header
AC_CHECK_HEADER($2,HAVE_$1=YES,HAVE_$1=NO)
if test ${HAVE_$1} = YES; then

  # search the library in the C library
  AC_CHECK_FUNC($4,,HAVE_$1=NO)
  if test ${HAVE_$1} = NO; then

    # search the library in the external C library
    AC_CHECK_LIB($3,$4,HAVE_$1=YES)
    $1_LIBS=-l$3
  fi
fi

# export the results
$1_HOME=/usr
AC_SUBST(HAVE_$1)
AC_SUBST($1_HOME)
AC_SUBST($1_LIBS)
])
