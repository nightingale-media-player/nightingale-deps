AC_DEFUN([AG_GST_BISON_CHECK],
[
  dnl FIXME: check if AC_PROG_YACC is suitable here
  dnl FIXME: make precious
  AC_PATH_PROG(BISON_PATH, bison, no)
  if test x$BISON_PATH = xno; then
    AC_MSG_ERROR(Could not find bison)
  fi

  dnl check bison version
  dnl FIXME 0.11: we need version >= 1.875 for the reentrancy support
  dnl in the parser. If an older version is installed pre-generated
  dnl sources are used. This should become a hard dependency for 0.11!
  bison_min_version=1.875
  bison_version=`$BISON_PATH --version | head -n 1 | sed 's/^.*) //' | sed 's/[[a-zA-Z]]*$//' | cut -d' ' -f1`
  AC_MSG_CHECKING([bison version $bison_version >= $bison_min_version])

  if perl -w <<EOF
    exit ($bison_version < $bison_min_version) ? 0 : 1;
EOF 
  then 
    AC_MSG_RESULT([yes])
    AM_CONDITIONAL(GENERATE_PARSER, test -z $GENERATE_PARSER_TRUE)
  else 
    AC_MSG_RESULT([no, using pre-generated parser sources])
    AM_CONDITIONAL(GENERATE_PARSER, false)
  fi
])

AC_DEFUN([AG_GST_FLEX_CHECK],
[
  dnl we require flex for building the parser
  AC_PATH_PROG(FLEX_PATH, flex, no)
  if test x$FLEX_PATH = xno; then
    AC_MSG_ERROR(Could not find flex)
  fi
  
  dnl check flex version
  dnl FIXME 0.11: we need version >= 2.5.31 for the reentrancy support
  dnl in the parser. If an older version is installed pre-generated
  dnl sources are used. This should become a hard dependency for 0.11!
  flex_min_version=2.5.31
  flex_version=`$FLEX_PATH --version | head -n 1 | sed 's/^.* //' | sed 's/[[a-zA-Z]]*$//' | cut -d' ' -f1`
  AC_MSG_CHECKING([flex version $flex_version >= $flex_min_version])
  if perl -w <<EOF
    (\$min_version_major, \$min_version_minor, \$min_version_micro ) = "$flex_min_version" =~ /(\d+)\.(\d+)\.(\d+)/;
    (\$flex_version_major, \$flex_version_minor, \$flex_version_micro ) = "$flex_version" =~ /(\d+)\.(\d+)\.(\d+)/;
    exit (((\$flex_version_major > \$min_version_major) ||
     ((\$flex_version_major == \$min_version_major) &&
      (\$flex_version_minor > \$min_version_minor)) ||
     ((\$flex_version_major == \$min_version_major) &&
      (\$flex_version_minor == \$min_version_minor) &&
      (\$flex_version_micro >= \$min_version_micro)))
     ? 0 : 1);
EOF
  then
    AC_MSG_RESULT(yes)
    AM_CONDITIONAL(GENERATE_PARSER, test -z $GENERATE_PARSER_TRUE)
  else
    AC_MSG_RESULT([no, using pre-generated parser sources])
    AM_CONDITIONAL(GENERATE_PARSER, false)
  fi
])
