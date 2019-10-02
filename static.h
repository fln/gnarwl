/* These #define have been taken from conf.h.in which is supposed to
 * be autocreated by autoheader. The settings below are never defined
 * anywhere and were probably added manually to conf.h.in, which made
 * the build break any time autoreconf was called
 */

#define TIMEFACTOR 3600
#define LDAPQUERY_MAXWAIT 10
#define DEFAULT_SERVER "localhost"
#define DEFAULT_QFILTER "(&(mail=$recepient)(vacationActive=TRUE)"
#define DEFAULT_MES "vacationInfo"
#define DEFAULT_BASE ""
#define DEFAULT_EXPIRE 48
#define DEFAULT_MAIL_LIMIT 256
#define DEFAULT_MAP_SENDER "$sender"
#define DEFAULT_MAP_RECEIVER "$recepient"
#define DEFAULT_MAP_SUBJECT "$subject"
#define LDAP_PROTOCOL_DETECT 0
#define LDAP_CHARSET "UTF-8"
#define LVL_CRIT 0
#define LVL_WARN 1
#define LVL_INFO 2
#define LVL_DEBUG 3

#define TRUE 1
#define FALSE 0

#ifndef BLOCKDIR
#define BLOCKDIR "/tmp"
#endif

#ifndef CFGFILE
#define CFGFILE "/etc/gnarwl.cfg"
#endif
#ifndef VERSION
#define VERSION " <unknown>"
#endif

