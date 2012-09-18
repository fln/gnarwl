/**
 * Out Of Memory - print error message to syslog, exit ungracefully
 */
void oom();

/**
 * Replace variables in text.
 * @param txt the text buffer
 * @param s string representing the variable
 * @param r replacement for s
 * If txt or s is NULL, this function will simply return, if r is null, s
 * will be cut out of txt (in this case r is modifyed). If s is/contains 
 * a substring of r, this function returns without doing anything.
 */
void expandVars(char** txt,char* s,char* r);

/**
 * Split a string into tokens.
 * @param str the string to split
 * @param idx stop tokenization after finding idx tokens (the rest of the
 * string will be put into the last token, no matter how many delimeters it 
 * may contain). A value of 0 means: return the complete string, while -1
 * stands for unlimited tokens.
 * @param delim char to
 * @return a NULL terminated array with substrings of str
 */
char **splitString(const char*, int idx, char delim);

/**
 * Read the contents of a file
 * @param fname - name of the file to read
 * @return a pointer do the read in content or NULL
 */
char* readFile(char* fname);

/**
 * Strip anything of a string, that is not a valid rcf822 address.
 * @param d the "dirty" emailaddress. Anything, that does not like like
 * an emaladdress is stripped of. The argument will be set to NULL, if
 * it does not contain a valid address.
 */
void cleanAddress(char** d);

/**
 * Copy a string 
 * @param dest destination buffer - will first be freed and then made
 * the proper size to hold the src string
 * @param src source string
 */
void cpyStr(char** des, const char* src);

/**
 * Convert between charsets.
 * @param txt pointer to the string to convert
 */
void translateString(char** txt);

