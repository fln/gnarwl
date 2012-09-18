#include <gdbm.h>
#include <ldap.h>

/**
 * Reads the contents of cfg.mailheader and cfg.mailfooter. The data
 * is put in the global vars "header" and "footer". If the files are
 * not accessable, an empty string will be stored in the variables.
 * Subsequent calls to this method do nothing. 
 */
void dbCacheHF(void);

/**
 * Check whether or not it is ok to send a message
 * @param she the address to send to
 * @param me the address to act for
 * @return TRUE if gnarwl is allowed to send a mail for the she/me combo
 */
int dbCheck(char* she, char* me);

/**
 * Lock a sender/receiver combo in the database
 * @param he remote address
 * @param me local address
 * @return TRUE entry was successfully entered into the database
 */
void dbLock( char* he, char* me);

/**
 * Convinience method for opening gdbm files
 * @param fname name of the file to open. If NULL is passed, this function
 * simply returns;
 * @param mode, directly passed through to gdbm_open
 * @return a pointer to the opened file or NULL, if file could not be opened.
 */
GDBM_FILE dbOpen( char* fname,  int mode);

/**
 * Convinience method for closing gdbm files.
 * param dbf_ptr filepointer to close (may be NULL). After closing, the file,
 * dbf_ptr is set to NULL.
 */
void dbClose(GDBM_FILE dbf_ptr);

/**
 * Convinience method, for checking, whether a string is stored as key
 * in a database.
 * @param str_ptr string to look for
 * @param dbf_ptr database to search
 * @return TRUE str_ptr is found as key in dbf_ptr. FALSE otherweise, or if
 * NULL was passed as a parameter.
 */
int dbContains(const char* str_ptr, GDBM_FILE dbf_ptr);

/**
 * Connect to the LDAP database
 */
void dbConnect();

/**
 * Disconnect from LDAP database and clean up
 */
void dbDisconnect();

/**
 * Do a query on the LDAP database
 * @param mail emailaddress to query with
 * @return an NULL terminated array of emails, that belong to "mail".
 */
char** dbQuery(const char* mail);

