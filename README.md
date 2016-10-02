GNARWL - LDAP based email autoresponder
=======================================

This is a fork of GNARWL originally written by Patrick Ahlbrecht 
([Source](http://www.onyxbits.de/gnarwl)).

Gnarwl is an autoresponder software, written to replace the legacy vacation(1) 
tool, which is included in most unix server distributions. With the 
traditional vacation(1) tool, users had to be given full shell accounts on the 
email server and trained in using them. This had serious implications on 
usability, working time and network security policy.

In essence, from a network management point of view, users should never be 
allowed or forced to access systems, they can easily break or compromise (both 
accidentally or on purpose). They should especially not have to use arcane 
tools, that are not part of their daily routine. As, according to experience, 
this only results in an additional amount of work for the network support team.

Since modern email server setups typically include an LDAP database server as 
a backend for storing actual user objects, it only makes sense to move 
vacation specific information from flat files to LDAP as well, in order to 
ease network management. That is, the user object in the database should also 
contain the users' out of office settings. That way, the same administrative 
interface, that is already in place for account management (changing 
passwords, etc.) can simply be extended to include absentee notification 
settings, sparing training and security issues.

Gnarwl makes the process of generating out of office notifications easy by 
plugging into the email server and inspecting every piece of incoming mail, 
checking it against the LDAP database and sending an automatic reply when 
necessary.

Gnarwl features:

*   Fully compatible with the old vacation(1) tool. Gnarwl can be used as a 
    drop in replacement.
*   Careful about not creating mail loops by answering to mailing lists or 
    other automated user agents.
*   Configurable blacklist of corporate email addresses for which to never 
    send out vacation notifications, even if an associated user is out of 
    office (e.g. webmaster@...).
*   Works with all major mail transport agents (postfix, sendmail and qmail).
*   Highly configurable output generation. Gnarwl allows the administrator to 
    force headers and footers, users may reference fields of their own LDAP 
    object in the outgoing mail.
*   Unicode support.
*   GPL software.

Improvements
------------


*   LDAP protocol version 3 is used by default.
*   New config option *server_uri* allows using more than one LDAP server for 
    failover.
*   New config option *ca_cert* allow specifying CA certificate when ldaps or 
    StartTLS is used.
*   New config option *starttls*.
*   New config option *deref* allows control of LDAP alias dereferencing.
