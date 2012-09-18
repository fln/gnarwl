<?php
/*
This is a little hack to add support for ldap/gnarwl based vacation in horde.
You have to adjust your vacation/config/conf.php and add these values:

$conf['server']['driver'] = 'gnarwl';
$conf['server']['params'] = array();
$conf['server']['params']['ldap_server'] = 'localhost';
$conf['server']['params']['ldap_base'] = 'ou=Users,dc=example,dc=de';
 
Of ocurse you have to adjust your LDAP server and base settings.
Copy this file in vacation/lib/Driver/gnarwl.php.


Horde can be found under http://www.horde.org.
Gnarwl can be found under http://www.oss.billiton.de/software.shtml

This script by C. Schwede <alt-f4@cschwede.de>, 2003.
*/


class Vacation_Driver_gnarwl extends Vacation_Driver {
    var $params;


function Vacation_Driver_gnarwl($params = array())
{
    $this->params = $params;
}


   

function check_config() {
    if (empty($this->params['ldap_server']) ||
        empty($this->params['ldap_base']) ) {
            $this->err_str = _("The module is not properly configured!");
            return false;
    }
    return true;
}


/*
 * @param string    $user       The username to enable vacation for.
 * @param string    $realm      The realm of the user - isn't used with gnarwl.
 * @param string    $pass       The password for the user.
 * @param string    $message    The message to install.
  
*/

function set_vacation($user, $realm, $pass, $message) 
        { if (!$this->check_config()) {return false;}
        $connect=ldap_connect($this->params['ldap_server']) or die($this->err_str = ("Can't connect to LDAP Server") );
        $bind=ldap_bind($connect, "uid=$user,".$this->params['ldap_base'], $pass);
                if ($bind) 
                        {
                                $info["vacationInfo"]=utf8_encode($message);
                                $info["vacationActive"]="TRUE";
                                $r=ldap_modify($connect,"uid=$user," . $this->params['ldap_base'], $info);
                                if (!$r) {$this->err_str = _(ldap_error($connect));return false;}
                        }
                                else {$this->err_str = _("Couldn't bind to LDAP server");return false;} 
                return true;
                
                ldap_close($connect);
        }


function unset_vacation($user, $realm, $pass, $message) 
        { if (!$this->check_config()) {return false;}
        $connect=ldap_connect($this->params['ldap_server']) or die($this->err_str = ("Can't connect to LDAP Server") );
        $bind=ldap_bind($connect, "uid=$user,".$this->params['ldap_base'], $pass);
                if ($bind) 
                        {
                                $info["vacationActive"]="FALSE";
                                $r=ldap_modify($connect,"uid=$user," . $this->params['ldap_base'], $info);
                                if (!$r) {$this->err_str = _(ldap_error($connect));return false;}
                        }
                                else {$this->err_str = _("Couldn't bind to LDAP server");return false;} 
                return true;
                
                ldap_close($connect);
        }



}
?>
