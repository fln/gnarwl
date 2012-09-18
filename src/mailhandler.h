/**
 * Mail is ok
 */
#define MAIL_NOTSPECIAL 0

/**
 * Mail contains at least one blacklisted recepient
 */ 
#define MAIL_BLADDR 1

/**
 * Mail has predefined sender address
 */
#define MAIL_PREDEF_SENDER 2

/**
 * Mail has a predefined recepient address
 */
#define MAIL_PREDEF_RECEIVER 4

/**
 * Mail has a "reply-to:" header
 */
#define MAIL_HAS_REPLYTO 8

/**
 * Mail contains a forbidden header line
 */
#define MAIL_BADHEADER 16

/**
 * Mail has too many headers
 */
#define MAIL_TOOBIG 32

/**
 * Mail specifies to many recepients
 */
#define MAIL_TOOMANY 64

/**
 * Mail lacks important information
 */
#define MAIL_LACK 128

/**
 * Mail has a "Sender:" header
 */
#define MAIL_HAS_SENDER 256

/**
 * Status of the mail (calculated by ORing the above macros)
 */
int mail_status;

/**
 * Message ID
 */
char* messageid;

/**
 * sender of incomming mail (filled by receiveMail() )
 */
char* sender;

/**
 * subject of incomming mail (filled by receiveMail() )
 */
char* subject;

/**
 * receivers of incomming mail (filled by receiveMail() )
 */
char** receivers;

/**
 * Parse a headerline
 * @param hl pointer to the header to inspect
 */
void parseHeader(const char* hl);

/**
 * Does the actual reading of the mail(header) from stdin.
 * @return number of lines read from stdin or -1 if the header contains
 * lines, that are no allowed
 */
void readFromSTDIN(void);

/**
 * Takes care of retreiving and parsing mail.
 * @param recv Use these receivers, instead of searching the mail
 * @param sndr Use this sender, instead of searching the mail
 * @return TRUE the Mail contains all the data required for replying to
 */
int receiveMail(char** recv, const char* sndr);

/**
 * Put a new mail address in the list of recepient mails
 * @param adr the a copy of adr will be put into the list
 */
void addAddr(const char* adr);

/**
 * Send out mail
 * @param addr who is sending the mail
 * @param body the mail itself
 */
void sendMail(char* addr, char* body);
