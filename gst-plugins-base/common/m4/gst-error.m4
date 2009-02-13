Dnl handle various error-related things

dnl Thomas Vander Stichele <thomas@apestaart.org>
dnl Tim-Philipp Müller <tim centricular net>

dnl Last modification: 2008-02-18

dnl AG_GST_SET_ERROR_CFLAGS([ADD-WERROR])
dnl AG_GST_SET_ERROR_CXXFLAGS([ADD-WERROR])
dnl AG_GST_SET_LEVEL_DEFAULT([IS-CVS-VERSION])


dnl Sets ERROR_CFLAGS to something the compiler will accept.
dnl AC_SUBST them so they are available in Makefile

dnl -Wall is added if it is supported
dnl -Werror is added if ADD-WERROR is not "no"

dnl These flags can be overridden at make time:
dnl make ERROR_CFLAGS=
AC_DEFUN([AG_GST_SET_ERROR_CFLAGS],
[
  AC_REQUIRE([AC_PROG_CC])
  AC_REQUIRE([AS_COMPILER_FLAG])

  
  dnl if we support -Wall, set it unconditionally
  AS_COMPILER_FLAG(-Wall,
                   ERROR_CFLAGS="-Wall",
                   ERROR_CFLAGS="")
 
  dnl Warn if declarations after statements are used (C99 extension)
  AS_COMPILER_FLAG(-Wdeclaration-after-statement,
        ERROR_CFLAGS="$ERROR_CFLAGS -Wdeclaration-after-statement")

  dnl Warn if variable length arrays are used (C99 extension)
  AS_COMPILER_FLAG(-Wvla,
        ERROR_CFLAGS="$ERROR_CFLAGS -Wvla")

  dnl if asked for, add -Werror if supported
  if test "x$1" != "xno"
  then
    AS_COMPILER_FLAG(-Werror, ERROR_CFLAGS="$ERROR_CFLAGS -Werror")

    dnl if -Werror isn't suported, try -errwarn=%all (Sun Forte case)
    if test "x$ERROR_CFLAGS" == "x"
    then
      AS_COMPILER_FLAG([-errwarn=%all], [
          ERROR_CFLAGS="-errwarn=%all"
          dnl try -errwarn=%all,no%E_EMPTY_DECLARATION,
          dnl no%E_STATEMENT_NOT_REACHED,no%E_ARGUEMENT_MISMATCH,
          dnl no%E_MACRO_REDEFINED (Sun Forte case)
          dnl For Forte we need disable "empty declaration" warning produced by un-needed semicolon 
          dnl "statement not reached" disabled because there is g_assert_not_reached () in some places
          dnl "macro redefined" because of gst/gettext.h
          dnl FIXME: is it really supposed to be 'ARGUEMENT' and not 'ARGUMENT'?
          for f in 'no%E_EMPTY_DECLARATION' \
                   'no%E_STATEMENT_NOT_REACHED' \
                   'no%E_ARGUEMENT_MISMATCH' \
                   'no%E_MACRO_REDEFINED' \
                   'no%E_LOOP_NOT_ENTERED_AT_TOP'
          do
            AS_COMPILER_FLAG([-errwarn=%all,$f], [
              ERROR_CFLAGS="$ERROR_CFLAGS,$f"
            ])
          done
      ])
    else
      AS_COMPILER_FLAG(-fno-strict-aliasing,
          ERROR_CFLAGS="$ERROR_CFLAGS -fno-strict-aliasing")
    fi
  fi

  AC_SUBST(ERROR_CFLAGS)
  AC_MSG_NOTICE([set ERROR_CFLAGS to $ERROR_CFLAGS])
])

dnl Sets ERROR_CXXFLAGS to something the compiler will accept.
dnl AC_SUBST them so they are available in Makefile

dnl -Wall is added if it is supported
dnl -Werror is added if ADD-WERROR is not "no"

dnl These flags can be overridden at make time:
dnl make ERROR_CXXFLAGS=
AC_DEFUN([AG_GST_SET_ERROR_CXXFLAGS],
[
  AC_REQUIRE([AC_PROG_CXX])
  AC_REQUIRE([AS_CXX_COMPILER_FLAG])

  
  dnl if we support -Wall, set it unconditionally
  AS_CXX_COMPILER_FLAG(-Wall, [
      ERROR_CXXFLAGS="-Wall"
  ], [
      ERROR_CXXFLAGS=""
  ])
  
  dnl if asked for, add -Werror if supported
  if test "x$1" != "xno"
  then
    AS_CXX_COMPILER_FLAG(-Werror, werror_supported=yes, werror_supported=no)

    if test "x$werror_supported" = "xyes"; then
        ERROR_CXXFLAGS="$ERROR_CXXFLAGS -Werror"

        dnl add exceptions
        for f in '-Wno-non-virtual-dtor' '-fno-strict-aliasing'
        do
          AS_CXX_COMPILER_FLAG([$f], ERROR_CXXFLAGS="$ERROR_CXXFLAGS $f")
        done
    else
      dnl if -Werror isn't suported, try -errwarn=%all
      AS_CXX_COMPILER_FLAG([-errwarn=%all], errwarnall=yes, errwarnall=no)
      if test "x$errwarnall" = "xyes"; then
        ERROR_CXXFLAGS="-errwarn=%all"
        dnl try -errwarn=%all,no%E_EMPTY_DECLARATION,
        dnl no%E_STATEMENT_NOT_REACHED,no%E_ARGUEMENT_MISMATCH,
        dnl no%E_MACRO_REDEFINED (Sun Forte case)
        dnl For Forte we need disable "empty declaration" warning produced by un-needed semicolon 
        dnl "statement not reached" disabled because there is g_assert_not_reached () in some places
        dnl "macro redefined" because of gst/gettext.h
        dnl FIXME: is it really supposed to be 'ARGUEMENT' and not 'ARGUMENT'?
        dnl FIXME: do any of these work with the c++ compiler? if not, why
        dnl do we check at all?
        for f in 'no%E_EMPTY_DECLARATION' \
                 'no%E_STATEMENT_NOT_REACHED' \
                 'no%E_ARGUEMENT_MISMATCH' \
                 'no%E_MACRO_REDEFINED' \
                 'no%E_LOOP_NOT_ENTERED_AT_TOP'
        do
          AS_CXX_COMPILER_FLAG([-errwarn=%all,$f], [
            ERROR_CXXFLAGS="$ERROR_CXXFLAGS,$f"
          ])
        done
      fi
    fi
  fi

  AC_SUBST(ERROR_CXXFLAGS)
  AC_MSG_NOTICE([set ERROR_CXXFLAGS to $ERROR_CXXFLAGS])
])

dnl Sets the default error level for debugging messages
AC_DEFUN([AG_GST_SET_LEVEL_DEFAULT],
[
  dnl define correct errorlevel for debugging messages. We want to have
  dnl GST_ERROR messages printed when running cvs builds
  if test "x[$1]" = "xyes"; then
    GST_LEVEL_DEFAULT=GST_LEVEL_ERROR
  else
    GST_LEVEL_DEFAULT=GST_LEVEL_NONE
  fi
  AC_DEFINE_UNQUOTED(GST_LEVEL_DEFAULT, $GST_LEVEL_DEFAULT,
    [Default errorlevel to use])
  dnl AC_SUBST so we can use it for win32/common/config.h
  AC_SUBST(GST_LEVEL_DEFAULT)
])
