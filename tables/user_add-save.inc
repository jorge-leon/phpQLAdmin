<?php
// $Id: user_add-save.inc,v 1.55 2008-03-18 23:14:14 turbo Exp $
require($_SESSION["path"]."/include/pql_krbafs.inc");

// {{{ URL Decode the surname and lastname values which was/is encoded in tables/user_add-additional.inc.
if(!empty($_REQUEST["surname"])) {
  $_REQUEST["surname"] = urldecode($_REQUEST["surname"]);
}
if(!empty($_REQUEST["name"])) {
  $_REQUEST["name"]    = urldecode($_REQUEST["name"]);
}
// }}}

// ============================================================================================
// ============== PREPARE THE USERS ATTRIBUTE(S)

// {{{ Convert uid to lowercase
if(!empty($_REQUEST["uid"])) {
	$_REQUEST["uid"] = strtolower($_REQUEST["uid"]);
	$entry[pql_get_define("PQL_ATTR_UID")] = $_REQUEST["uid"];
}
// }}}

// {{{ Add Firstname
if((!empty($_REQUEST["name"])
	or ($_REQUEST["uid"]
		and
		pql_get_define("PQL_CONF_REFERENCE_USERS_WITH", $_REQUEST["rootdn"]) == sprintf("%s", pql_get_define("PQL_ATTR_UID"))))
   and
   pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_GIVENNAME")))
{
  if(!empty($_REQUEST["name"])) 
	$entry[pql_get_define("PQL_ATTR_GIVENNAME")]	= $_REQUEST["name"];
  elseif(pql_get_define("PQL_CONF_REFERENCE_USERS_WITH", $_REQUEST["rootdn"]) == sprintf("%s", pql_get_define("PQL_ATTR_UID")))
	$entry[pql_get_define("PQL_ATTR_GIVENNAME")]	= $_REQUEST["uid"];
}
// }}}

// {{{ Add Lastname
if((!empty($_REQUEST["surname"])
	or ($_REQUEST["uid"]
		and
		pql_get_define("PQL_CONF_REFERENCE_USERS_WITH", $_REQUEST["rootdn"]) == sprintf("%s", pql_get_define("PQL_ATTR_UID"))))
   and
   pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_SN")))
{
  if(!empty($_REQUEST["surname"]))
	$entry[pql_get_define("PQL_ATTR_SN")]			= $_REQUEST["surname"];
  elseif(pql_get_define("PQL_CONF_REFERENCE_USERS_WITH", $_REQUEST["rootdn"]) == sprintf("%s", pql_get_define("PQL_ATTR_UID")))
	$entry[pql_get_define("PQL_ATTR_SN")]			= $_REQUEST["uid"];
}
// }}}
	
// {{{ Add full (common) name
if(((((!empty($_REQUEST["surname"]) and !empty($_REQUEST["name"]))
	  or ($_REQUEST["uid"] and
		  pql_get_define("PQL_CONF_REFERENCE_USERS_WITH", $_REQUEST["rootdn"]) == sprintf("%s", pql_get_define("PQL_ATTR_UID")))))
	and
	pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_SN")))
   or
   pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_SN")))
{
  if(!empty($_REQUEST["surname"]) and !empty($_REQUEST["name"]))
	$entry[pql_get_define("PQL_ATTR_CN")] = trim($_REQUEST["name"]) . " " . trim($_REQUEST["surname"]);
  elseif(pql_get_define("PQL_CONF_REFERENCE_USERS_WITH", $_REQUEST["rootdn"]) == sprintf("%s", pql_get_define("PQL_ATTR_UID")))
	$entry[pql_get_define("PQL_ATTR_CN")] = $_REQUEST["uid"];
}
// }}}

// {{{ Add mail and/or mail aliases
if(!empty($_REQUEST["mail"]) and
   (pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_MAIL")) or
	pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_ISACTIVE")) or
	pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_MAILALTERNATE"))))
{
	// {{{ Add the account status attribute
	if(!empty($_REQUEST["account_status"]) and 
	   pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_ISACTIVE")))
	  $entry[pql_get_define("PQL_ATTR_ISACTIVE")]	= $_REQUEST["account_status"];
	// }}}

	// {{{ Add the primary mail address to the entry array
	if(!empty($_REQUEST["mail"]) and
	   pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_MAIL")))
	  $entry[pql_get_define("PQL_ATTR_MAIL")] = $_REQUEST["mail"];
	// }}}

	// {{{ Add the additional email addresses as well
	$email_temp = explode('\@', $_REQUEST["mail"]); $email_temp = $email_temp[0];
	
	if($_REQUEST["template"] == "internal_group") {
		// For group objects, we need a 'catchall' mail alias as well.
		if(!empty($_REQUEST["email_domain"]))
		  $entry[pql_get_define("PQL_ATTR_MAILALTERNATE")][] = $email_temp . "-catchall@" . $_REQUEST["email_domain"];
		elseif($defaultdomain)
		  $entry[pql_get_define("PQL_ATTR_MAILALTERNATE")][] = $email_temp . "-catchall@" . $defaultdomain;
	}
	
	if(($_REQUEST["include_additional"] == 'on') and is_array($additionaldomainname) and
	   pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_MAILALTERNATE")))
	{
		foreach($additionaldomainname as $additional) {
		  if(!preg_match("/$additional/i", $_REQUEST["mail"]))
		  $entry[pql_get_define("PQL_ATTR_MAILALTERNATE")][] = strtolower($email_temp . "@" . $additional);
	}
	}
	// }}}
}
// }}}

if(!empty($_REQUEST["forwardingaddress"]) and
   pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_FORWARDS")))
{
  // {{{ Setup mail forwarding
  $entry[pql_get_define("PQL_ATTR_FORWARDS")]	= strtolower($_REQUEST["forwardingaddress"]);
  
  // Setup the delivery modes
  if(empty($_SESSION["NEW_STYLE_QUOTA"])) {
	// The new QmailLDAP have a changed 'deliveryMode' setup.
	// Don't add this automaticly.
	if($_REQUEST["forwardingtype"] == 'moveto')
	  $entry[pql_get_define("PQL_ATTR_MODE")][]	= "nombox";
	$entry[pql_get_define("PQL_ATTR_MODE")][]	= "forwardonly";
  } else {
	if($_REQUEST["forwardingtype"] == 'moveto')
	  $entry[pql_get_define("PQL_ATTR_MODE")][]	= "nolocal";
	$entry[pql_get_define("PQL_ATTR_MODE")][]	= "noprogram";
  }
  // }}}
}

// {{{ Add [ug]id number, gecos etc to the entry array
if(pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_QMAILUID"))
   and $template[pql_get_define("PQL_ATTR_AUTOCREATE_UGIDNUMBER")])
{
  // Get a free UserID number
  $uidnr = pql_get_next_ugidnumber('uid', $_REQUEST["rootdn"]);
  if($uidnr > 0)
	$entry[pql_get_define("PQL_ATTR_QMAILUID")] = $uidnr;
  
  // Can we reuse the same uidNumber as gidNumber - i.e., is there a group with this number?
  foreach($_SESSION["BASE_DN"] as $dn) {
	$results = $_pql->search($dn, pql_get_define("PQL_ATTR_QMAILGID")."=$uidnr");
	if(is_array($results)) {
	  // Exists - get a free GroupID number
	  $gidnr = pql_get_next_ugidnumber('gid', $_REQUEST["rootdn"]);
	  if($gidnr > 0) {
		$entry[pql_get_define("PQL_ATTR_QMAILGID")] = $gidnr;
		break;
	  }
	}
  }
  
  // Gecos is needed to do PAM/NSS LDAP login 
  if(!empty($_REQUEST["surname"]) and !empty($_REQUEST["name"]) and
	 pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_GECOS")) and
	 (pql_get_define("PQL_CONF_REFERENCE_USERS_WITH", $_REQUEST["rootdn"]) == sprintf("%s", pql_get_define("PQL_ATTR_UID"))))
	{
	  // We have first and last name, 'gecos' is allowed AND we're referencing users with 'uid'.
	  
	  // Gecos is one attribute that can't be internationalized!
	  // Decode in two steps: UTF8 decoding and then translate international chars to non-us chars
	  $tmp1 = pql_maybe_decode($_REQUEST["surname"]); $tmp1 = pql_format_international($tmp1);
	  $tmp2 = pql_maybe_decode($_REQUEST["name"]);    $tmp2 = pql_format_international($tmp2);
	  
	  $entry[pql_get_define("PQL_ATTR_GECOS")]  = $tmp1 . " " . $tmp2;
	}
}
// }}}

// {{{ Modify the password scheme in a way the LDAP server will understand - within {SCHEME}.
if($_REQUEST["pwscheme"]) {
  if(! preg_match('/\{/', $_REQUEST["pwscheme"]))
	$_REQUEST["pwscheme"] = '{'.$_REQUEST["pwscheme"];
  
  if(! preg_match('/\}/', $_REQUEST["pwscheme"]))
	$_REQUEST["pwscheme"] .= '}';
}
// }}}

// {{{ Add the password to the entry array
if(pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_PASSWD"))) {
  if(preg_match('/KERBEROS/i', $_REQUEST["pwscheme"])) {
	// {{{ We're using the {KERBEROS} password scheme. Special circumstances.
	// The userPassword and krb5PrincipalName needs to be set. This is 
	// automagicly created with the help of the username (uid) value and
	// the REALM name.
	if(preg_match('/@/', $_REQUEST["password"]))
	  // User know what he/she's doing. We specified the full principal
	  // name directly in the password field! Use that as userPassword
	  // and krb5PrincipalName.
	  $entry[pql_get_define("PQL_ATTR_KRB5PRINCIPALNAME")] = $_REQUEST["password"];
	elseif(pql_get_define("PQL_CONF_REFERENCE_USERS_WITH", $_REQUEST["rootdn"]) == sprintf("%s", pql_get_define("PQL_ATTR_UID")))
	  // The password really IS a password!
	  $entry[pql_get_define("PQL_ATTR_KRB5PRINCIPALNAME")] = $_REQUEST["uid"] . "@" . pql_get_define("PQL_CONF_KRB5_REALM");
	
	// Encrypt and create the hash using the krb5PrincipalName attribute
	$entry[pql_get_define("PQL_ATTR_PASSWD")] = pql_password_hash($entry[pql_get_define("PQL_ATTR_KRB5PRINCIPALNAME")],
																  $_REQUEST["pwscheme"]);
	// }}}
  } else {
	if(!empty($_REQUEST["crypted"]) and empty($_REQUEST["autogenerate"]))
	  // Password is already encrypted, prefix with choosen password scheme
	  $entry[pql_get_define("PQL_ATTR_PASSWD")] = $_REQUEST["pwscheme"] . $_REQUEST["password"];
	elseif(!empty($_REQUEST["password"]) and !empty($_REQUEST["pwscheme"]))
	  // Password isn't already encrypted, create the hash using the password value
	  $entry[pql_get_define("PQL_ATTR_PASSWD")] = pql_password_hash($_REQUEST["password"], $_REQUEST["pwscheme"]);
  }

  // We're allowing PQL_ATTR_PASSWD. Do we also allow the 'clearTextPassword'?
  if(pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_PASSWD_CLEAR"))) {
	if(!empty($_REQUEST["clear_text_password"])) {
	  // Probably a Kerberos V scheme, use the cleartext password value
	  $entry[pql_get_define("PQL_ATTR_PASSWD_CLEAR")] = $_REQUEST["clear_text_password"];
	} elseif(!empty($_REQUEST["password"])) {
	  // Add the unencrypted password
	  $entry[pql_get_define("PQL_ATTR_PASSWD_CLEAR")] = $_REQUEST["password"];
	}
  }
}
// }}}

// {{{ Add login shell
if(!empty($_REQUEST["loginshell"]) and
   pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_LOGINSHELL")))
{
	$entry[pql_get_define("PQL_ATTR_LOGINSHELL")] = $_REQUEST["loginshell"];
}
// }}}

// {{{ Add mail directory
if(!empty($_REQUEST["mail"]) and !empty($_REQUEST["maildirectory"]) and
   pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_MAILSTORE")))
{
  // No auto generation - that's done in 'user_add.php'.
  $entry[pql_get_define("PQL_ATTR_MAILSTORE")] = $_REQUEST["maildirectory"];
}
// }}}

// {{{ Add mail host
// 'host' can be zero (0) hence isset().
if(!empty($_REQUEST["mail"]) and isset($_REQUEST["host"]) and
   pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_MAILHOST")))
{
	if(!empty($_REQUEST["host"]) or !empty($_REQUEST["text_host"])) {
		// useControls style or defaultMailHost style?
		if($_pql->get_attribute($_REQUEST["rootdn"], pql_get_define("PQL_ATTR_CONTROL_USE"))) {
			// useControls give a list of hosts, which are selected by number
			// Is the manual input field set/used?
			$nr = $_REQUEST["text_host"];
			if(!empty($_REQUEST["host_".$nr]))
				$entry[pql_get_define("PQL_ATTR_MAILHOST")]       = $_REQUEST["host_".$nr];
			else {
				// ... no it isn't. Use the one that is.
				$nr = $_REQUEST["host"];
				$entry[pql_get_define("PQL_ATTR_MAILHOST")]       = $_REQUEST["host_".$nr];
			}
		} else {
			$which = empty($_REQUEST["host"]) ? "text_host" : "host";
			$entry[pql_get_define("PQL_ATTR_MAILHOST")]       = $_REQUEST[$which];
		}
	}
	if(empty($_SESSION["NEW_STYLE_QUOTA"]) and
	   pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_MODE")))
	  // The new QmailLDAP have a changed 'deliveryMode' setup.
	  // Don't add this automaticly.
	  $entry[pql_get_define("PQL_ATTR_MODE")] = "localdelivery";
}
// }}}

// {{{ Add home directory to the entry array
if(!empty($_REQUEST["homedirectory"]) and
   pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_HOMEDIR")))
{
  // No auto generation - that's done in 'user_add.php'.
  $entry[pql_get_define("PQL_ATTR_HOMEDIR")] = $_REQUEST["homedirectory"];
}
// }}}

// {{{ Add the mail quota
if(!empty($_REQUEST["mail"]) and
   (pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_QUOTA_SIZE")) or
	pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_QUOTA_COUNT"))))
{
  $default_quota = $_pql->get_attribute($_REQUEST["domain"], pql_get_define("PQL_ATTR_BASEQUOTA"));
  if(!empty($default_quota)) {
	if(!empty($_SESSION["NEW_STYLE_QUOTA"])) {
	  $tmp = pql_parse_quota($default_quota);
	  
	  $entry[pql_get_define("PQL_ATTR_QUOTA_SIZE")]  = $tmp["maxsize"];
	  $entry[pql_get_define("PQL_ATTR_QUOTA_COUNT")] = $tmp["maxmails"];
	} else
	  $entry[pql_get_define("PQL_ATTR_QUOTA_VALUE")]   = $default_quota;
  }
}
// }}}

// {{{ Setup the objectclass(es)
$entry[pql_get_define("PQL_ATTR_OBJECTCLASS")] = $template["userobjectclass"];
// }}}

// {{{ Check if this is a samba user/account
if(in_array(pql_get_define("PQL_ATTR_SAMBAOBJECTCLASS"), $template["userobjectclass"])) {
  // {{{ Set account flags
  $days = pql_get_define("PQL_CONF_DAYSUNTILEXPIRE");
  if(!empty($days)) {
	$acctflags = '[U ]';
  } else {
	$acctflags = '[UX ]';
  }
  $sid = pql_get_define("PQL_CONF_SAMBASID", $_REQUEST["branch"]);
  if(!$sid)
	die("Branch Security ID is not set!");
  // }}}
  
  // {{{ Convert uid to lowercase
  if(!empty($_REQUEST["uid"])) {
	$entry[pql_get_define("PQL_ATTR_SAMBASID")] = $sid . '-' . (3 * $entry[pql_get_define("PQL_ATTR_QMAILUID")]);
	$entry[pql_get_define("PQL_ATTR_SAMBALMPASSWORD")] = pql_password_hash($_REQUEST["password"], '{lmpasswd}');
	$entry[pql_get_define("PQL_ATTR_SAMBANTPASSWORD")] = pql_password_hash($_REQUEST["password"], '{ntpasswd}');
	$entry[pql_get_define("PQL_ATTR_SAMBAPWDLASTSET")] = time();
	$entry[pql_get_define("PQL_ATTR_SAMBAPWDMUSTCHANGE")] = time() + ($days * 24 * 60 * 60);
	$entry[pql_get_define("PQL_ATTR_SAMBAACCTFLAGS")] = $acctflags;
  }
  // }}}
}
// }}}

// -------------------------------
// User object setup done here!

if(pql_get_define("PQL_CONF_DEBUG_ME")) {
  echo "Entry array: "; printr($entry);
}

// {{{ Create the RDN for the user
if(!empty($entry[pql_get_define("PQL_CONF_REFERENCE_USERS_WITH", $_REQUEST["rootdn"])]))
{
  $userdn = pql_get_define("PQL_CONF_REFERENCE_USERS_WITH", $_REQUEST["rootdn"]) . "=" .
	$entry[pql_get_define("PQL_CONF_REFERENCE_USERS_WITH", $_REQUEST["rootdn"])] . ",";
} elseif(!empty($entry[pql_get_define("PQL_ATTR_UID")])) {
  $userdn = pql_get_define("PQL_ATTR_UID")."=".$entry[pql_get_define("PQL_ATTR_UID")].",";
} else {
  echo "I can't figure out how to name the RDN!<br>";
  echo "Both the user reference value (".pql_get_define("PQL_CONF_REFERENCE_USERS_WITH", $_REQUEST["rootdn"]);
  echo ") and the backup reference (".pql_get_define("PQL_ATTR_UID").") is empty...<br>";
  die("I have no idea how to continue from here...");
}

if(pql_get_define("PQL_CONF_SUBTREE_USERS") and (!empty($_REQUEST["subbranch"]) and !preg_match('/^ou=/i', $_REQUEST["subbranch"]))) {
  $userdn .= pql_get_define("PQL_CONF_SUBTREE_USERS").",";
}

if(!empty($_REQUEST["subbranch"])) {
	if(preg_match('/=/', $_REQUEST["subbranch"]))
	  $userdn .= urldecode($_REQUEST["subbranch"]);
	else
	  $userdn .= 'ou='.urldecode($_REQUEST["subbranch"].",".$_REQUEST["domain"]);
} else { 
	$userdn .= urldecode($_REQUEST["domain"]);
}
// }}}

// {{{ Add the user to the database
// pql_user_add() will first URL encode the value, THEN UTF8 encode it
// Hence, we must reverse this here ($_REQUEST["subbranch"] is already
// URL decoded).
if(!preg_match('/=/', $_REQUEST["subbranch"])) {
  // When there's multiple subbranches
  $branch = 'ou='.pql_maybe_decode($_REQUEST["subbranch"]).','.$_REQUEST["domain"];
} else {
  // No subbranch
  $branch = pql_maybe_decode($_REQUEST["subbranch"]);
}
$DNs = pql_user_add($_REQUEST["domain"], $userdn, $entry, 'user', $branch);
if(!empty($DNs[0])) {
  // TODO: DNs[1] (the group object) might still be empty -> failed to add it.

  if(pql_get_define("PQL_CONF_DEBUG_ME")) {
	echo "DNs added: ";
	printr($DNs);
	die();
  }

  // Add the user principal (if a Kerberos V 'password')
  if(pql_get_define("PQL_ATTR_KRB5PRINCIPALNAME") &&  $entry["krb5principalname"])
	kadmin_ank($entry["krb5principalname"], $entry["cleartextpassword"]);

  // {{{ Now it's time to run the special adduser script if defined...
  if(pql_get_define("PQL_CONF_SCRIPT_CREATE_USER", $_REQUEST["rootdn"])) {
	// Setup the environment with the user details
	putenv("PQL_CONF_DOMAIN=$domain");
	putenv("PQL_CONF_WEBUSER=".posix_getuid());
	foreach($entry as $key => $e) {
	  $key = "PQL_" . strtoupper($key);
	  if($key != 'PQL_CONF_OBJECTCLASS')
		putenv("$key=$e");
	}
	
	if(pql_get_define("PQL_CONF_KRB5_ADMIN_COMMAND_PATH") and 
	   pql_get_define("PQL_CONF_KRB5_REALM") and
	   pql_get_define("PQL_CONF_KRB5_ADMIN_PRINCIPAL") and
	   pql_get_define("PQL_CONF_KRB5_ADMIN_SERVER") and 
	   pql_get_define("PQL_CONF_KRB5_ADMIN_KEYTAB")) {
	  putenv("PQL_KADMIN_CMD=".pql_get_define("PQL_CONF_KRB5_ADMIN_COMMAND_PATH")."/kadmin");
	  putenv("PQL_KADMIN_REALM=".pql_get_define("PQL_CONF_KRB5_REALM"));
	  putenv("PQL_KADMIN_PRINC=".pql_get_define("PQL_CONF_KRB5_ADMIN_PRINCIPAL"));
	  putenv("PQL_KADMIN_SERVR=".pql_get_define("PQL_CONF_KRB5_ADMIN_SERVER"));
	  putenv("PQL_KADMIN_KEYTB=".pql_get_define("PQL_CONF_KRB5_ADMIN_KEYTAB"));

	  if(!empty($_REQUEST["clear_text_password"]))
		putenv("PQL_CLEARTEXT_PASSWORD=".$_REQUEST["clear_text_password"]);
	}
	
	// Execute the user add script (0 => show output)
	if(pql_execute(pql_get_define("PQL_CONF_SCRIPT_CREATE_USER", $_REQUEST["rootdn"]), 0)) {
	  echo pql_complete_constant($LANG->_('The %what% add script failed'),
								 array('what' => $LANG->_('user'))) . "!<br>";
	  $msg = urlencode(pql_complete_constant($LANG->_('The %what% add script failed'),
											 array('what' => $LANG->_('user'))) ."!") . ".&nbsp;<br>";
	} else {
	  echo "<b>" . pql_complete_constant($LANG->_('Successfully executed the %what% add script'),
										 array('what' => $LANG->_('user'))) . "</b><br>";
	  $msg = urlencode(pql_complete_constant($LANG->_('Successfully executed the %what% add script'),
											 array('what' => $LANG->_('user')))) . ".&nbsp;<br>";
	}
  }
// }}}
	
  $msg .= urlencode($LANG->_('Successfully created the new user'));
	
  // {{{ Send a test mail or redirect to user details page
  if(pql_get_define("PQL_CONF_TESTMAIL_AUTOSEND", $_REQUEST["rootdn"])) {
	$link  = "user_sendmail.php?email=" . urlencode($_REQUEST["mail"]) . "&";
	$link .= "rootdn=".$url["rootdn"]."&domain=".$url["domain"]."&user=" . urlencode($DNs[0]) . "&rlnb=2&msg=$msg";
  } else {
	$link  = "user_detail.php?rootdn=".$url["rootdn"]."&"."domain=".$url["domain"];
	$link .= "&user=".urlencode(pql_maybe_decode($DNs[0]))."&view=personal&rlnb=2&msg=$msg";
  }
// }}}
	
  if(pql_get_define("PQL_CONF_SCRIPT_CREATE_USER", $_REQUEST["rootdn"])) {
?>
    <form action="<?php echo $link?>" method="post">
      <input type="submit" value="Continue">
    </form>
<?php
	   die();
  } else {
	pql_header($link);
  }
} else {
  $msg = urlencode($LANG->_('Failed to create the new user') . ":&nbsp;" . ldap_error($_pql->ldap_linkid));
  $link = "domain_detail.php?rootdn=".$url["rootdn"]."&domain=".$url["domain"]."&msg=$msg";
  pql_header($link);
}
// }}}

/*
 * Local variables:
 * mode: php
 * mode: font-lock
 * tab-width: 4
 * End:
 */
