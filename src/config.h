/**
 * Holds the data of the config file
 */
struct conf {
  char *uri;		// LDAP uri
  char *ca_cert;	// LDAP TLS_CACERTFILE
  int starttls;     //
  char *base;		// LDAP base
  char *uid;		// LDAP bind dn
  char *pwd;		// LDAP bind password
  char *server;		// LDAP server
  char *qfilter;	// LDAP query filter
  char *result;		// LDAP attribute containing the mail body
  int scope;		// LDAP search scope
  int port;		// LDAP server port
  int protver;		// LDAP protocol version
  char *charset;	// Locale charset for character conversion
  char *mfilter;	// Path to dbfile, containing mail deny patterns
  char *dbdir;		// Path where the blockfiles are stored
  char *mta;		// Path to the MTA
  char *mta_opts;	// Optional arguments for the MTA
  char *blist;		// Path to dbfile, containing address deny patterns
  char *mailheader;	// Path to the txtfile, containing standard header
  char *mailfooter;	// Path to the txtfile, containing standard footer
  char *map_sender;	// Macroname for "From:" header
  char *map_receiver;	// Macroname for "To:" and "Cc:" header
  char **recv_header;	// Which headers hold recepient addresses
  char *map_subject;	// Macroname for "Subject:" header
  char **macro_attr;	// List of additional LDAP attributes
  char **macro_name;	// List of macronames for the attributes above
  int dbexp;		// How long to block emailaddresses
  int maxmail;		// max number of recepients allowed
  int maxheader;	// max number of header lines allowed in mail
  int umask;		// file creation mask for db files
  int deref;		// When to follow LDAP aliases
};

/**
 * Enter a key/value pair into the config structure
 * @param key the keyword from the configfile
 * @param val the value of the keyword from the configfile
 */
void putEntry(char*, char*);

/**
 * Fill the configstructure with default values
 */
void setDefaults(void);

/**
 * Fill the configstructure with values from a configfile
 * @param fname name of the configfile
 */
void readConf(char*);
