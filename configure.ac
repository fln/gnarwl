# Process this file with autoconf to produce a configure script.
AC_INIT()
AC_CONFIG_HEADERS(conf.h)
AH_TOP([#include "static.h"])
AC_CONFIG_FILES(Makefile)

# Checks for programs.
AC_PROG_CC

AC_ARG_WITH(homedir,AC_HELP_STRING([--with-homedir=DIR],[Homedir for the gnarwl user]),[homedir="$withval"],[homedir="\${prefix}/var/lib/gnarwl"])
AC_ARG_WITH(docdir,AC_HELP_STRING([--with-docdir=DIR],[Where to install the docs]),docdir="$withval",docdir="\${prefix}/share/doc/packages/gnarwl")
AC_ARG_WITH(useradd_prog,AC_HELP_STRING([--with-useradd_prog=DIR],[Programm for adding users]),useradd_prog="$withval",useradd_prog="/usr/sbin/useradd")
AC_ARG_WITH(useradd_args,AC_HELP_STRING([--with-useradd_args=ARG],[Arguments for the useradd programm]),useradd_args="$withval",useradd_args="-r -s /bin/false -c \"Email autoreply agent\" -d \$(HOMEDIR) \$(BIN)")
AC_ARG_WITH(mta,AC_HELP_STRING([--with-mta=prog],[MTA to use]),mta="\"$withval\"",mta="\"/usr/sbin/sendmail\"")

AC_SUBST(homedir)
AC_SUBST(docdir)
AC_SUBST(useradd_prog)
AC_SUBST(useradd_args)

AC_ARG_WITH(permmask,AC_HELP_STRING([--with-permmask=MASK],[File creation mask for database files]),permmask="$withval",permmask="0600")
AC_ARG_WITH(maxline,AC_HELP_STRING([--with-maxline=NUMBER],[Maximum input buffer line length]),maxline="$withval",maxline="2100")
AC_SUBST(permmask)
AC_SUBST(maxline)


AC_CHECK_LIB([gdbm], [gdbm_open])

AC_CHECK_LIB([ldap],[ldap_init])
AC_CHECK_FUNCS([ldap_set_option])

AC_CHECK_FUNCS([iconv])

AC_DEFINE_UNQUOTED(UMASK,[$permmask], [File creation mask for database files])
AC_DEFINE_UNQUOTED(MAXLINE,[$maxline], [Maximum input buffer line length])
AC_DEFINE_UNQUOTED(DEFAULT_MTA,[$mta], [MTA to use])

AC_OUTPUT
