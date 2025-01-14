<?php
// add a user
// $Id: user_add.php,v 2.146 2008-03-18 21:56:30 turbo Exp $
//
// --------------- Pre-setup etc.

// {{{ Setup session etc
require("./include/pql_session.inc");
require($_SESSION["path"]."/include/pql_config.inc");
require($_SESSION["path"]."/include/pql_control.inc");
require($_SESSION["path"]."/include/pql_templates.inc");

$url["domain"]		= pql_format_urls($_REQUEST["domain"]);
$url["rootdn"]		= pql_format_urls($_REQUEST["rootdn"]);
$url["subbranch"]	= pql_format_urls($_REQUEST["subbranch"]);
$url["user"]		= pql_format_urls($_REQUEST["user"]);
// }}}

// {{{ Get the organization name, or the DN if it's unset
$orgname = $_pql->get_attribute($_REQUEST["domain"], pql_get_define("PQL_ATTR_O"));
if(!$orgname) {
	$orgname = urldecode($_REQUEST["domain"]);
} elseif(is_array($orgname)) {
	$orgname = $orgname[0];
}
$_REQUEST["orgname"] = $orgname;
// }}}

// {{{ Check if domain exist
if(!$_pql->get_dn($_REQUEST["domain"], '(objectclass=*)', 'BASE')) {
	echo "Domain &quot;".$_REQUEST["domain"]."&quot; does not exists";
	exit();
}
// }}}

// {{{ Get default domain values for this domain
$additionaldomainname = $_pql->get_attribute($_REQUEST["domain"], pql_get_define("PQL_ATTR_ADDITIONAL_DOMAINNAME"));
if($additionaldomainname and !is_array($additionaldomainname)) {
	$additionaldomainname = array($additionaldomainname);
}

$attribs = array("defaultdomain"		=> pql_get_define("PQL_ATTR_DEFAULTDOMAIN"),
				 "maxusers"				=> pql_get_define("PQL_ATTR_MAXIMUM_DOMAIN_USERS"),
				 "basehomedir"			=> pql_get_define("PQL_ATTR_BASEHOMEDIR"),
				 "basemaildir"			=> pql_get_define("PQL_ATTR_BASEMAILDIR"),
				 "defaultaccounttype"	=> pql_get_define("PQL_ATTR_DEFAULT_ACCOUNTTYPE"),
				 "lockusername"			=> pql_get_define("PQL_ATTR_LOCK_USERNAME"),
				 "lockemailaddress"		=> pql_get_define("PQL_ATTR_LOCK_EMAILADDRESS"),
				 "lockdomainaddress"	=> pql_get_define("PQL_ATTR_LOCK_DOMAINADDRESS"),
				 "lockpassword"			=> pql_get_define("PQL_ATTR_LOCK_PASSWORD"),
				 "lockaccounttype"		=> pql_get_define("PQL_ATTR_LOCK_ACCOUNTTYPE"));
foreach($attribs as $key => $attrib) {
	// Get default value
	$value = $_pql->get_attribute($_REQUEST["domain"], $attrib);
	if(is_array($value))
	  $value = $value[0];

	if(($key == 'lockusername') or ($key == 'lockemailaddress') or
	   ($key == 'lockdomainaddress') or ($key == 'lockpassword') or
	   ($key == 'lockaccounttype'))
	{
	  // A toggle value
	  if(!$value)
		// No value
		$value = 0;
	  else
		// Got a value
		$value = pql_format_bool($value);
	}

	$$key = $value;
}

// leg20170512
// some fallbacks to rootdn values
if(empty($basehomedir)) $basehomedir = $_pql->get_attribute($_REQUEST["rootdn"],$attribs["basehomedir"]);
if(empty($basemaildir)) $basemaildir = $_pql->get_attribute($_REQUEST["rootdn"],$attribs["basemaildir"]);
// }}}



if(empty($_REQUEST["page_curr"])) {
  $_REQUEST["page_curr"] = '';
}
if(empty($_REQUEST["page_next"])) {
  if($defaultaccounttype && $lockaccounttype) {
	$_REQUEST["template"] = $defaultaccounttype;
	$_REQUEST["page_next"] = 'one';
  } else
	$_REQUEST["page_next"] = '';
}

// {{{ Retreive the template definition
if(!empty($_REQUEST["template"]) and !@is_array($template)) {
  $template = pql_get_template($_REQUEST["template"]);
  $template_attribs = pql_templates_attributes($template["userobjectclass"]);
} else {
  $templates = pql_get_templates();
}
// }}}

// {{{ Retreive password encryption schemes
function pql_user_add_retreive_encryption_schemes($template, $rootdn) {
  global $template_attribs;

  if(pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_PASSWD"))) {
	if(is_array($template["passwordscheme"])) {
	  // There's schemes specified in the template we're using - use them!
	  $schemes = $template["passwordscheme"];
	} else {
	  $tmp = pql_get_define("PQL_CONF_PASSWORD_SCHEMES", $rootdn);
	  if(preg_match('/,/', $tmp)) {
		// We got more than one password scheme...
		$schemes = explode(",", $tmp);
	  } else {
		$schemes = array($tmp);
	  }
	  
	  unset($tmp); // We're done with the temp variable...
	}
  }

  if(empty($schemes))
	return false;
  else {
	if($_REQUEST["defaultpasswordscheme"]) {
	  // Add the default password scheme from the domain to the complete list of
	  // schemes to choose from...
	  $default_password_scheme_exist_in_schemes = 0;
	  for($i=0; $schemes[$i]; $i++) {
		if($schemes[$i] == $_REQUEST["defaultpasswordscheme"]) {
		  $default_password_scheme_exist_in_schemes = 1;
		} else
		  $tmp[] = $schemes[$i];
	  }
	  if(!$default_password_scheme_exist_in_schemes)
		$tmp[] = $_REQUEST["defaultpasswordscheme"];

	  if(is_array($tmp))
		$schemes = $tmp;
	}

	return($schemes);
  }
}
// }}}

// --------------- Verification and action(s).

// {{{ Verify the input from the current page.  Autogen input for the next page.
if(pql_get_define("PQL_CONF_DEBUG_ME")) {
  echo "page_curr: '".$_REQUEST["page_curr"]."'<br>";
  echo "page_next: '".$_REQUEST["page_next"]."'<br>";
}

// {{{ Retreive some stuff for autogeneration
// Needed in both page_curr '' and 'one' (if password is wrong type)
$attribs = array("autocreatemailaddress" => pql_get_define("PQL_ATTR_AUTOCREATE_MAILADDRESS"),
				 "autocreateusername"	 => pql_get_define("PQL_ATTR_AUTOCREATE_USERNAME"),
				 "autocreatepassword"	 => pql_get_define("PQL_ATTR_AUTOCREATE_PASSWORD"));
foreach($attribs as $key => $attrib) {
  // Get default value
  $value = $_pql->get_attribute($_REQUEST["domain"], $attrib);
  $$key  = pql_format_bool($value);
}
// }}}

// Check the input
$error = false; $error_text = array();
switch($_REQUEST["page_curr"]) {
  // {{{ '':    Make sure a new user can be added OR that we autogenerate stuff for the very first page)
  case "":
	if($_REQUEST["page_next"] == "one") {

		// {{{ Verify/Create uid
		// But only if:
		// 1. We haven't specified this ourself (Hmmm, how could we!? We have not been given a choice before this! TODO)
		// 2. We have set autoCreateUserName to TRUE for this branch/domain
		// 3. The function 'user_generate_uid()' is defined in include/config.inc.
		// 4. At least one of the objects we've choosen to use when creating users MAY or MUST the 'uid' attribute..
		if(empty($_REQUEST["uid"]) and pql_templates_check_attribute($template_attribs, 'uid')) {
		  if(function_exists('user_generate_uid') and empty($_REQUEST["uid"]) and
			 (($autocreateusername and pql_templates_check_attribute($template_attribs, 'uid', 'MUST'))
			  or
			  (pql_get_define("PQL_CONF_REFERENCE_USERS_WITH", $_REQUEST["rootdn"]) == 'uid')))
		  {
			// Generate the username
			$_REQUEST["uid"] = strtolower(user_generate_uid((empty($_REQUEST["surname"]) ? '' : $_REQUEST["surname"]),
															(empty($_REQUEST["name"]) ? '' : $_REQUEST["name"]),
															(empty($email) ? '' : $email),
															(empty($_REQUEST["domain"]) ? '' : $_REQUEST["domain"]),
															(empty($_REQUEST["template"]) ? '' : $_REQUEST["template"])));
			
			// Check again. There should be a user name, either from the input
			// form OR from the user_generate_uid() function above...
			if(empty($_REQUEST["uid"])) {
				$error = true;

				if(pql_templates_check_attribute($template_attribs, 'uid', 'MUST'))
				  $error_text["uid"] = "&nbsp;&nbsp;".$LANG->_("Can't autogenerate, but this value is required.");
				else
				  $error_text["uid"] = $LANG->_("Can't autogenerate.");
			} else {
				if(preg_match("/[^a-z0-9\.@%_-]/i", $_REQUEST["uid"])) {
					$error = true;
					$error_text["uid"] = $LANG->_('Invalid');
				}
			}
		  }
		}
		// }}}

		// {{{ Verify/Create email address
		if(empty($_REQUEST["mail"]) and $autocreatemailaddress and function_exists('user_generate_email') and
		   pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_MAIL")))
		{
			// It's not supplied - generate one
			$_REQUEST["mail"] = strtolower(user_generate_email($_REQUEST["uid"], "", "",
									   $_REQUEST["domain"], $_REQUEST["template"]));			
			if(preg_match("/ /", $_REQUEST["mail"]))
			  // Replace spaces with underscore
			  $_REQUEST["mail"] = preg_replace(" ", "_", $_REQUEST["mail"], -1);
		}
		// }}}

		// {{{ Get the default password scheme for branch and from template
		if(!$_REQUEST["pwscheme"]) {
		  $_REQUEST["defaultpasswordscheme"] = $_pql->get_attribute($_REQUEST["domain"],
																	pql_get_define("PQL_ATTR_DEFAULT_PASSWORDSCHEME"));
		  $schemes = pql_user_add_retreive_encryption_schemes($template, $_REQUEST["rootdn"]);
		}
		// }}}

		// {{{ Generate a password
		$auto_generated_kerberos_pw = 0;
		if(($_REQUEST["template"] != "internal_group") and empty($_REQUEST["password"]) and	// Not internal group and not already availible
		   $autocreatepassword and															// SHOULD generate a password
		   function_exists('pql_generate_password') and										// CAN generate a password
		   pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_PASSWD")) and	// ALLOWS password
		   !$_REQUEST["pwscheme"])
		{
		  // If we only have one password scheme, and it's a Kerberos V scheme, then we generate a Kerberos V
		  // principal and generate a password which we put in the clear_text_password value.
		  // Also, since we only have one scheme which is allowed, put this as a default for the rest of the
		  // user add session.
		  //
		  // We MUST autogenerate a password HERE, even if it's not specified. This because othervise
		  // the create user script will create a randomized password, which we have no idea what it is..
		  if(in_array('KERBEROS', $schemes)) {
			$_REQUEST["password"]            = $_REQUEST["uid"]."@".pql_get_define("PQL_CONF_KRB5_REALM");
			$_REQUEST["pwscheme"]            = 'KERBEROS';

			if($autocreatepassword) {
			  $_REQUEST["clear_text_password"] = pql_generate_password();
			  $_REQUEST["autogenerated_password"] = 1;
			}

			// Just so that we'll get the opportunity to add a password in tables/user_add-details.inc which
			// is/should be next...
			$auto_generated_kerberos_pw      = 1;
			$_REQUEST["autogenerated_password"] = 1;
		  } else {
			$_REQUEST["password"] = pql_generate_password();
			$_REQUEST["autogenerated_password"] = 1;
		  }
		}
		// }}}

		// {{{ If this is a samba template, make sure that 'mkntpw' exists
		if($template) {
		  $mkntpw = pql_get_define("PQL_CONF_MKNTPW_PATH");
		  if(in_array('sambasamaccount', $template["userobjectclass"]) and !empty($mkntpw) and !is_file($mkntpw)) {
			$error = true;
			$error_text["mkntpw"] = $LANG->_("mkntpw binary missing, can't generate password!");
		  }
		}
		// }}}
	} else {
	  // {{{ Step 1: Make sure we have at least one user template
	  if(!is_array($templates)) {
		include($_SESSION["path"]."/header.html");
?>
  <span class="title1"><?php echo $LANG->_('No user templates')?></span>
  <br><br>
  <?php echo $LANG->_('You do not have any user templates defined. As of phpQLAdmin version 2.1.4, this is the new way of doing things. No more hard coded stuff! Please go to <a href="config_detail.php?view=template">the template editor</a> and define some...')?>
<?php
		die();
	  }
	  // }}}

	  // {{{ Step 2: Make sure that the attribute we're using for user reference(s) is availible
	  //             in at least ONE object class we've choosen to use when creating users...
	  $attrib_is_availible = 0;
	  for($i=0; $i < count($templates); $i++) {
		// NOTE/BUG (?): Orig: $templates[$i]. New: $template_attribs

		if(!isset($template_attribs)) {
		  $template_attribs =  pql_templates_attributes($templates[$i]["userobjectclass"]);
		}
		if(pql_templates_check_attribute($template_attribs,
										 pql_get_define("PQL_CONF_REFERENCE_USERS_WITH",
														$_REQUEST["rootdn"])))
		{
		  $attrib_is_availible = 1;
		  break;
		}
	  }

	  if(!$attrib_is_availible) {
		include("./header.html");
?>
  <span class="title1"><?php echo pql_complete_constant($LANG->_('Attribute %attrib% is not availible.'), array("attrib" => pql_get_define("PQL_CONF_REFERENCE_USERS_WITH", $_REQUEST["rootdn"])))?></span>
  <br><br>
  Sorry, but the attribute you're choosen to use to reference users 
  (<b><?php echo pql_get_define("PQL_CONF_REFERENCE_USERS_WITH", $_REQUEST["rootdn"])?></b>)
  with isn't availible as a <i>MUST</i> nor a <i>MAY</i> in any of the defined
  user templates.
  <p>
  There''s little point in continuing from here, please go to the
  <a href="config_detail.php?view=template">configuration</a>
  and setup a correct user template...
<?php
		die();
	  }
	  // }}}
	  
	  // {{{ Step 3: Selected account type (see how many users there can be)
	  if($maxusers and !$_SESSION["ALLOW_BRANCH_CREATE"]) {
		// Create a user search filter
		if(pql_get_define("PQL_CONF_USER_FILTER", $_REQUEST["rootdn"]))
		  $filter = pql_get_define("PQL_CONF_USER_FILTER", $_REQUEST["rootdn"]);
		else
		  $filter  = pql_get_define("PQL_CONF_REFERENCE_USERS_WITH", $_REQUEST["rootdn"])."=*";

		// Retreive all users in this branch/domain.
	    $users   = $_pql->get_dn($_REQUEST["domain"], $filter);
		if(count($users) >= $maxusers) {
			// We have reached the maximum amount of users.
			include($_SESSION["path"]."/header.html");
?>
  <span class="title1"><?php echo pql_complete_constant($LANG->_('Maximum amount (%max%) of users reached'), array("max" => $maxusers))?></span>
  <br><br>
  Sorry, but the maximum amount of users have been reached in this domain. You are not allowed
  to create more. Please talk to your administrator if you think this is wrong.
<?php
			die();
		}
	  }
	  // }}}
	}
	break;
	// }}}

  // {{{ 'one': Validate all user details specified in the very first page
  // Values such as: surname, name, email, template, account_status etc
  case "one":
	if($_REQUEST["template"] == "alias") {
		// {{{ Verify the 'alias' account type
		if(empty($_REQUEST["source"])) {
			$error = true;
			$error_text["source"] = $LANG->_('Missing');
		}

		if(empty($_REQUEST["destination"])) {
			$error = true;
			$error_text["destination"] = $LANG->_('Missing');
		}

		if(!$error) {
			// Construct the user/alias RDN
			$_REQUEST["user"]  = pql_get_define("PQL_CONF_REFERENCE_USERS_WITH", $_REQUEST["rootdn"]) . "=" . $_REQUEST["source"];
			$_REQUEST["user"] .= "," . $_REQUEST["subbranch"];

			// Now when we have the RDN, double check that it doesn't already exists!
			if($_pql->get_dn($_REQUEST["user"], '(objectclass=*)', 'BASE')) {
				$error = true;
				$error_text["source"] = pql_complete_constant($LANG->_('User %user% already exists'), array("user" => $_REQUEST["user"])) . "<br>";
			} else {
				// The 'tables/user_add-save.inc' file isn't suitable for creating an alias. Do it here directly
				$entry[pql_get_define("PQL_ATTR_OBJECTCLASS")][] = 'referral';
				$entry[pql_get_define("PQL_ATTR_OBJECTCLASS")][] = 'extensibleObject';
				$entry[pql_get_define("PQL_CONF_REFERENCE_USERS_WITH", $_REQUEST["rootdn"])] = $_REQUEST["source"];
				
				// Use _this_ host reference in the referral value
				$host = explode(';', $_SESSION["USER_HOST"]);
				$entry["ref"] = $host[0]."/".urlencode($_REQUEST["destination"]);
				
				// Add the object to the database
				if(!$_pql->add($_REQUEST["user"], $entry, 'alias', 'user_add.php')) {
					// failed to add user
					pql_format_error(1);
					die($LDIF);
				}
				
				if(pql_get_define("PQL_CONF_DEBUG_ME"))
				  die("Object added: <b>".$_REQUEST["user"].'</b>');

				$msg = urlencode($LANG->_('Successfully created the new user'));
				$link  = "user_detail.php?rootdn=".$url["rootdn"]."&";
				$link .= "domain=".$url["domain"]."&user=".$_REQUEST["destination"]."&msg=$msg&rlnb=2";
				pql_header($link);
			}
		}
		// }}}
	} else {
		// {{{ Verify all account types EXEPT 'alias'
		// ------------------------

		// {{{ Fix 'uid' attribute (no international characters)
		// If 'uid' is set and we're using that to reference users - Don't allow international characters!
		if($_REQUEST["uid"] and pql_get_define("PQL_CONF_REFERENCE_USERS_WITH", $_SESSION["BASE_DN"][0]) == 'uid') {
		  $new = pql_format_international(utf8_decode($_REQUEST["uid"]));
		  if($_REQUEST["uid"] != $new)
			$_REQUEST["uid"] = $new;
		}
		// }}}

		// {{{ Verify surname
		if(($_REQUEST["dosave"] != 'yes') and empty($_REQUEST["surname"]) and pql_templates_check_attribute($template_attribs, 'sn', 'MUST')) {
			$error = true;
			$error_text["surname"] = $LANG->_('Missing');
		}
		$user = $_REQUEST["surname"];
		// }}}

		// {{{ Verify lastname
		if(($_REQUEST["dosave"] != 'yes') and empty($_REQUEST["name"]) and pql_templates_check_attribute($template_attribs, 'sn', 'MUST')) {
			$error = true;
			$error_text["name"] = $LANG->_('Missing');
		}
		$user .= " " . $_REQUEST["name"];
		// }}}

		// {{{ Verify username
		unset($filter);
		if(pql_templates_check_attribute($template_attribs, 'sn') and
		   (pql_get_define("PQL_CONF_REFERENCE_USERS_WITH", $_REQUEST["rootdn"]) == sprintf("%s", pql_get_define("PQL_ATTR_CN"))))
		{
			// 1. Template allows attribute 'sn'
			// 2. 'sn' is used to reference users
			$filter = "(&(objectclass=*)(".pql_get_define("PQL_CONF_REFERENCE_USERS_WITH", $_REQUEST["rootdn"])."=$user))";
			$error_user = $user;
		} elseif(pql_templates_check_attribute($template_attribs, 'uid') and
		   (pql_get_define("PQL_CONF_REFERENCE_USERS_WITH", $_REQUEST["rootdn"]) == sprintf("%s", pql_get_define("PQL_ATTR_UID"))))
		{
			// 1. Template allows attribute 'uid'
			// 2. 'uid' is used to reference users
			$filter = "(&(objectclass=*)(".pql_get_define("PQL_CONF_REFERENCE_USERS_WITH", $_REQUEST["rootdn"])."=".$_REQUEST['uid']."))";
			$error_user = $_REQUEST['uid'];
		}

		if($_pql->get_dn($_REQUEST["domain"], $filter)) {
			$error = true;
			$error_text["uid"] = pql_complete_constant($LANG->_('User %user% already exists'), array("user" => $error_user));
			unset($_REQUEST['uid']);
		}
		// }}}

		// {{{ Verify (and/or generate) the mail address
		// If email is set and allowed.
		// or:
		// If email isn't set but is required.
		if(($_REQUEST["dosave"] != 'yes') and
		   (($_REQUEST["mail"] and
			 pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_MAIL")))
			or
			(empty($_REQUEST["mail"]) and
			 pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_MAIL"), 'MUST'))))
		{
		    // leg20170511: this part was missing.  Should be
		    // merged better do avoid additional stripping and
		    // adding of the domain, but...
		    if(empty($_REQUEST["mail"])) {
			$domain = $lockdomainaddress ? $defaultdomain : $_REQUEST["domain"];
			$_REQUEST["mail"] = strtolower(user_generate_email($_REQUEST["surname"], $_REQUEST["name"], $_REQUEST["subbranch"],
									   $domain, $_REQUEST["template"]));
		    }
		    if(!preg_match("/@/", $_REQUEST["mail"])) {
			if($_REQUEST["email_domain"])
			    $_REQUEST["mail"] .= "@" . $_REQUEST["email_domain"];
			else
			    $_REQUEST["mail"] .= "@" . $defaultdomain;
			
			if(!pql_check_email($_REQUEST["mail"])) {
			    $error = true;
			    $error_text["mail"] = $LANG->_('Invalid');
			}
		    } elseif($lockdomainaddress) {
			// There's an @ in the emailaddress, but we've specifically said that this isn't allowed. Remove
			// the domain part and replace it with the default domain.
			$tmp = preg_replace('/@.*/', '', $_REQUEST["mail"]);
			$_REQUEST["mail"] = $tmp . "@" . $_REQUEST["email_domain"];
		    }
		
		    // It exists, it's valid. Does it already exists in the database?
		    if(empty($error_text["mail"]) and pql_email_exists($_REQUEST["mail"])) {
			$error = true;
			$error_text["mail"] = pql_complete_constant($LANG->_('Mail address %address% already exists'),
								    array("address" => '<i>'.$_REQUEST["mail"].'</i>'));
			unset($_REQUEST["mail"]);
		    } elseif(!$lockdomainaddress) {
			// The mail address is perfectly ok, but if user specify an exact email address (which most
			// likley don't match the email_domain we must change the email_domain request value for
			// the rest of the checks to work correctly.
			$tmp = explode('@', $_REQUEST["mail"]);
			$_REQUEST["email_domain"] = $tmp[1];
		    }
		}
		// }}}

		// {{{ Verify (and/or generate) the password
		// If password is set and allowed.
		// or:
		// If password isn't set but required.
		// or:
		// If password isn't set, we have been asked to autogenerate one AND it's allowed.
		if(($_REQUEST["password"] and
			pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_PASSWD")))
		   or
		   (empty($_REQUEST["password"]) and
            pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_PASSWD"), 'MUST'))
		   or
		   (empty($_REQUEST["password"]) and ($_REQUEST["autogenerate"] == "on") and
			pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_PASSWD"))))
		{
			// Only forward and group accounts is ok without password
			if(empty($_REQUEST["password"])) {
				// {{{ No previous value
				if(empty($_REQUEST["autogenerate"])) {
					$error = true;
					$error_text["password"] = $LANG->_('Missing');
				} elseif(function_exists('pql_generate_password')) {
					// Autogenerate password
					$_REQUEST["password"] = pql_generate_password();
					$_REQUEST["autogenerated_password"] = 1;
				}
				// }}}
			} else {
			  // Got a previous value
			  if(($_REQUEST["dosave"] == 'yes') and $autocreatepassword) {
				// {{{ Change the password
				// This might happen when we change password encryption type
				// Example: Initially KERBEROS was selected and the 'password'
				//          (i.e. a kerberos principal) was generated.
				//          If we now change to MD5 for example, the password
				//          must change.
				// We only care about going FROM Kerberos or TO Kerberos...
				if($_REQUEST["clear_text_password"]) {
				  // If we come from kerberos scheme, this will be set...
				  $_REQUEST["password"] = $_REQUEST["clear_text_password"];

				  // Since we no longer use KERBEROS, unset the clear text password value
				  unset($_REQUEST["clear_text_password"]);
				} elseif(preg_match('/KERBEROS/i', $_REQUEST["pwscheme"])) {
				  // Going (back?) to Kerberos scheme...
				  $_REQUEST["clear_text_password"] = $_REQUEST["password"];
				  $_REQUEST["password"] = $_REQUEST["uid"]."@".pql_get_define("PQL_CONF_KRB5_REALM");
				}
				// }}}
			  } elseif(preg_match("/KERBEROS/i", $_REQUEST["pwscheme"])) {
				// {{{ Preselected kerberos password
				// Should be in the form: userid@DOMAIN.LTD
				// TODO: Is this regexp correct!?
				if(!pql_get_define("PQL_CONF_ALLOW_ALL_CHARS", $_REQUEST["rootdn"]) and
				   !preg_match("/^[a-zA-Z0-9]+[\._-a-z0-9]*[a-zA-Z0-9]+@[A-Z0-9][-A-Z0-9]+(\.[-A-Z0-9]+)+$/", $_REQUEST["password"]))
				{
				  $error = true;
				  $error_text["password"] = $LANG->_('Invalid');
				  
				  // Since this is a Kerberos V 'password', we must remember the _real_ password.
				  $_REQUEST["clear_text_password"] = $_REQUEST["password"];
				  
				  // Try generating a new value. We SHOULD know all we need
				  $_REQUEST["password"] = $_REQUEST["uid"]."@".pql_get_define("PQL_CONF_KRB5_REALM");
				}
				// }}}
			  } elseif($_REQUEST["pwscheme"] and empty($_REQUEST["crypted"])) {
				// {{{ A password in cleartext, NOT already encrypted
				if(!pql_get_define("PQL_CONF_ALLOW_ALL_CHARS", $_REQUEST["rootdn"]) and
				   preg_match("/[^a-z0-9]/i", $_REQUEST["password"]))
				  {
					$error = true;
					$error_text["password"] = $LANG->_('Invalid');
				  }
				// }}}
			  }
			}
			
			// Need to get this just in case we changed encryption schemes
			// and password got changed (we still want to give the user a
			// chance to change his/her mind again ... they usually do :)
			$_REQUEST["defaultpasswordscheme"] = $_pql->get_attribute($_REQUEST["domain"],
																	  pql_get_define("PQL_ATTR_DEFAULT_PASSWORDSCHEME"));
			$schemes = pql_user_add_retreive_encryption_schemes($template, $_REQUEST["rootdn"]);
		}
		// }}}

		// {{{ Verify mail forwarding address
		// If it's a forwarding accounts (allowing 'mailForwardingAddress') make sure the forwarding mail address is ok
		if(($_REQUEST["dosave"] != 'yes') and pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_FORWARDS"), "MUST")) {
			if(!pql_check_email($_REQUEST["forwardingaddress"])) {
				$error = true;
				$error_text["forwardingaddress"] = $LANG->_('Invalid');
			}
			
			if(empty($_REQUEST["forwardingaddress"])) {
				$error = true;
				$error_text["forwardingaddress"] = $LANG->_('Missing');
			}
		}
		// }}}

		// {{{ Generate the mailHost attribute/value
		if(($_REQUEST["dosave"] != 'yes') and $_REQUEST["mail"] and
		   pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_MAILHOST"))) {
		    // leg20170511: added the option to specify a default mailHost attribute per branch
		    // this prevails the heuristic mx lookup
		    $default_mailhost = $_pql->get_attribute($_REQUEST["domain"], pql_get_define("PQL_ATTR_DEFAULT_MAILHOST"));
		    if(empty($default_mailhost)) {
		    	$default_mailhost = $_pql->get_attribute($_REQUEST["rootdn"], pql_get_define("PQL_ATTR_DEFAULT_MAILHOST"));
		    }
		    if(!empty($default_mailhost)) {
			$_REQUEST["host"] = $default_mailhost;
		    } elseif($_pql->get_attribute($_REQUEST["rootdn"], pql_get_define("PQL_ATTR_CONTROL_USE")))  {
			// Find MX (or QmailLDAP/Controls with locals=$email_domain)
			$mx = pql_get_mx($_REQUEST["email_domain"]);
			if(!$mx) {
			    if(!$error)
				// Only set this to 'mx' if it's not already set.
				// This so that the switch at the bottom won't break.
				$error = "mx";

			    $error_text["mx"] = pql_complete_constant($LANG->_('Sorry, I can\'t find any MX or any QmailLDAP/Controls object that listens to the domain <u>%domain%</u>.<br>You will have to specify one manually.'),
			    						    array('domain' => pql_maybe_idna_decode($_REQUEST["email_domain"])));
			} else
			    // We got a MX or QmailLDAP/Controls object. Use it.
			    $_REQUEST["mx"] = $mx;
		    }
		}
		// }}}

		// {{{ Generate the mail directory value
		// If email set and mailMessageStore is allowed
		// or
		// If email set and mailMessageStore is required
		if(
		   ($_REQUEST["dosave"] != 'yes') and $_REQUEST["mail"] and
		   (pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_MAILSTORE"))
			or
			pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_MAILSTORE"), 'MUST')))
		{
		  if(!empty($basemaildir)) {
			if(function_exists("user_generate_mailstore")) {
			  // {{{ Function user_generate_mailstore() exists
			  if((pql_get_define("PQL_CONF_REFERENCE_USERS_WITH", $_REQUEST["rootdn"]) == sprintf("%s", pql_get_define("PQL_ATTR_UID")))
				 and $_REQUEST["uid"])
				$reference = $_REQUEST["uid"];
			  elseif($_REQUEST["surname"] and $_REQUEST["name"])
				$reference = $_REQUEST["surname"]." ".$_REQUEST["name"];
			  elseif($_REQUEST["surname"])
				$reference = $_REQUEST["surname"];
			  elseif($_REQUEST["name"])
				$reference = $_REQUEST["name"];
			  elseif($_REQUEST["mail"]) {
				$reference = explode('@', $_REQUEST["mail"]);
				$reference = $reference[0];
			  }
			  
			  $_REQUEST["maildirectory"] = user_generate_mailstore($_REQUEST["mail"], $_REQUEST["domain"],
																   array(pql_get_define("PQL_ATTR_UID") => $reference),
																   'user');
			  // }}}
			} else {
			  // {{{ Function user_generate_mailstore() doesn't exists
			  //     We do however have a base mail directory.
			  //     Try creating the mail directory manually, using the username.
			  
			  $ref = pql_get_define("PQL_CONF_REFERENCE_USERS_WITH", $_REQUEST["rootdn"]);
			  if($_REQUEST[$ref]) {
				if(pql_get_define("PQL_CONF_ALLOW_ABSOLUTE_PATH", $_REQUEST["rootdn"])
				   or
				   (!pql_get_define("PQL_CONF_ALLOW_ABSOLUTE_PATH", $_REQUEST["rootdn"]) and !preg_match('/^\//', $basemaildir)))
				{
				  // Absolute path is ok
				  // or
				  // Absolute path is not ok, but the baseMailDir doesn't start with a slash
				  //
				  // - create 'baseMailDir/username'
				  $_REQUEST["maildirectory"] = $basemaildir.$_REQUEST[$ref];
				} else {
				  // We're not allowing an absolute path which starts with a slash
				  // - don't use the baseMailDir.
				  $_REQUEST["maildirectory"] = $_REQUEST[$ref];
				}
			  }
			  // }}}
			}
			
			if($_REQUEST["maildirectory"]) {
			  if(!pql_get_define("PQL_CONF_CREATE_MBOX"))
				// We're not putting mails in a MBox - add a slash at the end to make it a Maildir.
				$_REQUEST["maildirectory"] .= '/';
			  $_REQUEST["maildirectory"] = pql_fix_path($_REQUEST["maildirectory"]);
			}
		  } else {
			// Can't autogenerate!
			$error_text["maildirectory"] = pql_complete_constant($LANG->_('Attribute <u>%what%</u> is missing. Can\'t autogenerate %type%.'),
																 array('what' => pql_get_define("PQL_ATTR_BASEMAILDIR"), 
																	   'type' => 'Path to mailbox'));
		  }
		}
		// }}}

		// {{{ Generate the home directory value
		// leg20170512: I need the homedir in my setup, but don't want to change the schema...
		//   so: if I define the function, it means I want to create it.
		if(($_REQUEST["dosave"] != 'yes') and pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_HOMEDIR"), 'MUST')
		   or function_exists("user_generate_homedir")) {
		    
		    if(!empty($basehomedir)) {
			if(function_exists("user_generate_homedir")) {
			    if((pql_get_define("PQL_CONF_REFERENCE_USERS_WITH", $_REQUEST["rootdn"]) == sprintf("%s", pql_get_define("PQL_ATTR_UID")))
			       and $_REQUEST["uid"])
				$reference = $_REQUEST["uid"];
			    elseif($_REQUEST["surname"] and $_REQUEST["name"])
				$reference = $_REQUEST["surname"]." ".$_REQUEST["name"];
			    elseif($_REQUEST["surname"])
				$reference = $_REQUEST["surname"];
			    elseif($_REQUEST["name"])
				$reference = $_REQUEST["name"];
			    elseif($_REQUEST["mail"]) {
				$reference = explode('@', $_REQUEST["mail"]);
				$reference = $reference[0];
			    }
			    
			    $_REQUEST["homedirectory"] = user_generate_homedir(preg_replace('/@.*/', '', $_REQUEST["mail"]), $_REQUEST["domain"],
									       array(pql_get_define("PQL_ATTR_UID") => $reference),
									       'user');
			} else {
			    // Function user_generate_homedir() doesn't exists but we have a base home directory.
			    // Try creating the home directory manually, using the username.
			  
			    if(pql_get_define("PQL_CONF_ALLOW_ABSOLUTE_PATH", $_REQUEST["rootdn"])) {
				// Absolute path is ok - create 'baseHomeDir/username/'
				$ref = pql_get_define("PQL_CONF_REFERENCE_USERS_WITH", $_REQUEST["rootdn"]);
				if($_REQUEST[$ref]) {
				    $_REQUEST["homedirectory"] = $basehomedir;
				    if(!preg_match('/\/$/', $basehomedir))
					$_REQUEST["homedirectory"] .= '/';
				    $_REQUEST["homedirectory"] .= $_REQUEST[$ref]."/";
				}
			    } else
				// We're not allowing an absolute path - don't use the baseHomeDir.
				$_REQUEST["homedirectory"] = $_REQUEST["uid"]."/";
			}
			
			if($_REQUEST["homedirectory"]) {
			    // Replace space(s) with underscore(s)
			    $_REQUEST["homedirectory"] = preg_replace('/ /', '_', $_REQUEST["homedirectory"], -1);

			    // Replace double slashes with ONE slash...
			    $_REQUEST["homedirectory"] = preg_replace('/\/\//', '/', $_REQUEST["homedirectory"], -1);
			}
		    } else {
			// Can't autogenerate!
			$error_text["homedirectory"] = pql_complete_constant($LANG->_('Attribute <u>%what%</u> is missing. Can\'t autogenerate %type%'),
									     array('what' => pql_get_define("PQL_ATTR_BASEHOMEDIR"), 
										   'type' => 'Path to homedirectory'));
		    }
		}
		// }}}

		// {{{ If we have an error, what page should come after this?
		if($_REQUEST["dosave"] == 'yes') {
		  // Redisplay the current page
		  $_REQUEST["page_next"] = 'one';
		} else {
		  if($error and !empty($error_text)) {
			if(preg_match("/mx/i", $error)) {
			  // This is the only case where we really SHOULD go to the next page - MX problems.
			  $_REQUEST["page_next"] = 'two';
			} else {
			  // Redisplay the current page
			  $_REQUEST["page_next"] = 'one';
			}
		  }
		}
		// }}}
		// }}}
	}
	break;
	// }}}

  // {{{ 'two': Tripplecheck the mail host value
  case "two":
      if($_REQUEST["mail"] and ($_REQUEST["page_next"] == 'save')
	 and !(isset($_REQUEST["host"]) or isset($_REQUEST_["mx"]))
	 and pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_MAILHOST")))
	{
	  $error = true;
	  $error_text["host"] = $LANG->_('Missing');
	  $_REQUEST["page_next"] = 'two';
	}
	break;
	// }}}
}
// }}}

// {{{ Process the next page ($page_next).
include($_SESSION["path"]."/header.html");
?>
  <span class="title1">
    <?php echo pql_complete_constant($LANG->_('Create account in domain %domain%'),
									 array("domain" => $_REQUEST["orgname"])); ?>
<?php
if(!empty($_SESSION["ADVANCED_MODE"]) and @is_array($template)) {
  echo ': '.$template["short"];
} elseif(!empty($template) and !@is_array($template)) {
  // Retreive template we're using
  $template = pql_get_template($_REQUEST["template"]);
  echo ': '.$template["short"];
} else {
  echo '.';
}
?>
  </span>

  <br><font color=red>*</font> = Required value<br>

  <br><br>

<?php
if($error and !empty($error_text["mkntpw"])) {
  echo pql_format_error_span("WARNING:".$error_text["mkntpw"]);
}

// ------------------------------------------------
// Select next form to display using 'page_next'.
// This will be set correctly above if there's
// an error.
if(pql_get_define("PQL_CONF_DEBUG_ME")) {
  echo "<pre>";
  echo "Request array: "; printr($_REQUEST);
  echo "Error array: "; printr($error_text);
  echo "</pre>";
}

if(is_array($error_text)) {
	// Add a HTML newline to (all) the error text(s)
	foreach($error_text as $key => $msg)
	  $error_text[$key] = "&nbsp;&nbsp;".$error_text[$key]."<br>";
}

switch($_REQUEST["page_next"]) {
  case "":
	// Step 1 - Choose account properties (type of account)
	$include = "./tables/user_add-properties.inc";
	break;
	
  case "one":
	// Step 2 - Choose user details (name, password etc)
	if($_REQUEST["template"] == "alias")
	  $include = "./tables/user_add-alias.inc";
	else
	  $include = "./tables/user_add-details.inc";
	break;
	
  case "two":
	// Step 3 - Choose additional information (mailhost, maildir etc)
	$include = "./tables/user_add-additional.inc";
	break;
	
  case "save":
	// Step 4 - Save the user into DB
	if(!empty($_REQUEST["page_curr"]) and !empty($_REQUEST["autogenerate"]) and empty($_REQUEST["password_shown"]))
	  $include = "./tables/user_add-show_password.inc";
	else
	  $include = "./tables/user_add-save.inc";
	break;
}

if(pql_get_define("PQL_CONF_DEBUG_ME")) {
  echo "Including file '$include'<br>\n";
}
include($include);

// }}}
?>
  </body>
</html>
<?php
pql_flush();

/*
 * Local variables:
 * mode: php
 * mode: font-lock
 * tab-width: 4
 * End:
 */
?>
