dnl $Id$
dnl config.m4 for extension nbphp

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(nbphp, for nbphp support,
dnl Make sure that the comment is aligned:
dnl [  --with-nbphp             Include nbphp support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(nbphp, whether to enable nbphp support,
Make sure that the comment is aligned:
[  --enable-nbphp           Enable nbphp support])

if test "$PHP_NBPHP" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-nbphp -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/nbphp.h"  # you most likely want to change this
  dnl if test -r $PHP_NBPHP/$SEARCH_FOR; then # path given as parameter
  dnl   NBPHP_DIR=$PHP_NBPHP
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for nbphp files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       NBPHP_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$NBPHP_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the nbphp distribution])
  dnl fi

  dnl # --with-nbphp -> add include path
  dnl PHP_ADD_INCLUDE($NBPHP_DIR/include)

  dnl # --with-nbphp -> check for lib and symbol presence
  dnl LIBNAME=nbphp # you may want to change this
  dnl LIBSYMBOL=nbphp # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $NBPHP_DIR/lib, NBPHP_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_NBPHPLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong nbphp lib version or lib not found])
  dnl ],[
  dnl   -L$NBPHP_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(NBPHP_SHARED_LIBADD)

  PHP_NEW_EXTENSION(nbphp, nbphp.c, $ext_shared)
fi
