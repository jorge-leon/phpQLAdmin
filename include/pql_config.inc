<?php
// Load basic configuration
//
// $Id: pql_config.inc,v 1.101 2007-09-13 18:09:11 turbo Exp $
//

// leg20170515: helper functions to shorten some code
function isSessionSetAndTrue($index) {
  return (isset($_SESSION[$index]) and $_SESSION[$index]);
 }
function is_setAndTrue(&$var, $index = null) {
  if(is_array($var))
    return (isset($arr[$index]) and $arr[$index]);
  else
    return (isset($var) and $var);
 }


// {{{ Include the API files
require($_SESSION["path"]."/include/pql.inc");					// Main API file
require($_SESSION["path"]."/include/pql_formating.inc");		// Output formating API
require($_SESSION["path"]."/include/pql_cache.inc");			// Cache API
require($_SESSION["path"]."/include/pql_search.inc");			// Search API
require($_SESSION["path"]."/include/pql_write.inc");			// Write API
require($_SESSION["path"]."/include/pql_units.inc");			// Sub branch API
// }}}

// {{{ Start profiling - what time is it now?
if(file_exists($_SESSION["path"]."/.DEBUG_PROFILING")) {
  $now = pql_format_return_unixtime();
  if(!preg_match('/index2\.php$/i', $_SERVER["SCRIPT_FILENAME"]))
	echo "Now: <b>$now</b><br>";
  $start = $now;
}
// }}}

// {{{ pql_set_define(define, value, case_insensitive, dn)
// We need this instead of define(), so that we can get rid
// of 'Constant ... already defined' if running with
//	error_reporting=E_ALL
// in PHP...
function pql_set_define($define, $value, $case_insensitive = false, $dn = NULL) {
	global $pql_defines, $pql_cache;

	if(!isset($value))
	  return;

	pql_format_bool($value);

	if(is_array($value)) {
	  $tmp = '';
	  
	  for($i=0; $i < count($value); $i++) {
		$tmp .= $value[$i];
		if(!empty($value[$i+1]))
		  $tmp .= ',';
	  }
	  
	  $value = $tmp;
	}

	//error_log("--- define:$define=$value\n", 3, $_SESSION["path"]."/phpQLadmin.log");

	if($dn)
	  // Called with a DN specific define
	  $define .= ":".$dn;

	// Cache this...
	if(preg_match('/^PQL_CONF_/', $define))
	  // Only use the defines cache for config values
	  $pql_defines->add($define, $value);
	else
	  $pql_cache->add($define, $value);
}
// }}}

// {{{ pql_get_define(define, dn)
function pql_get_define($define, $dn = NULL) {
	global $pql_defines, $pql_cache;

	if($dn) {
	  $dn = pql_format_normalize_dn($dn);
	  if(preg_match('/^PQL_CONF_/', $define))
		$value = $pql_defines->get($define.":".$dn);
	  else
		$value = $pql_cache->get($define.":".$dn);
	} else {
	  // Not called with a DN
	  if(preg_match('/^PQL_CONF_/', $define))
		$value = $pql_defines->get($define);
	  else
		$value = $pql_cache->get($define);
	}

	return($value);
}
// }}}

// {{{ Do any debugging?
if(file_exists($_SESSION["path"]."/.DEBUG_ME")) {
  pql_set_define("PQL_CONF_DEBUG_ME", true);
}

if(file_exists($_SESSION["path"]."/.DEBUG_CACHE") and !preg_match('/index2\.php$/i', $_SERVER["SCRIPT_FILENAME"])) {
  echo "<b>*</b> Loading base config<br>\n";
}
// }}}

// ----------------------------

// 1. Load site specific configuration
require($_SESSION["path"]."/include/config.inc");

// 2. Include the attribute configuration
require($_SESSION["path"]."/include/config_attributes.inc");

// {{{ Get the LDAP host and port
if(empty($_SESSION["USER_HOST"]) or ($_SESSION["USER_HOST"] == ';;') or ($_SESSION["USER_HOST"] == '0;;')) {
  if(! preg_match('/\+/i', pql_get_define("PQL_CONF_HOST")))
	$host = explode(';', pql_get_define("PQL_CONF_HOST"));
  else {
	$tmp  = explode('\+', pql_get_define("PQL_CONF_HOST"));
	$host = explode(';', $tmp[0]);

	unset($tmp);
  }

  $_SESSION["USER_HOST"] = implode(";", $host);
  // was $host[0] . ";" . $host[1] . ";" . $host[2];
  unset($host);
}
// }}}

// {{{ (Possibly) open a connection to the LDAP server
if(empty($_pql)) {
  $binddn = (!empty($_SESSION["USER_DN"]))   ? $_SESSION["USER_DN"]   : '';
  $bindpw = (!empty($_SESSION["USER_PASS"])) ? $_SESSION["USER_PASS"] : '';

  $_pql = new pql($_SESSION["USER_HOST"], $binddn, $bindpw);
  if($_pql->ldap_error) {
	unset($_SESSION["USER_ID"]);
	unset($_SESSION["USER_PASS"]);
	unset($_SESSION["USER_DN"]);
	
	die("$_pql->ldap_error<br><a href=\"index.php\" target=\"_top\">".$LANG->_('Login again')."</a>");
  }
}
// }}}

// {{{ Initalize some session variable - either with the old OR with an empty value
$_SESSION["ALLOW_BRANCH_CREATE"]	= (!empty($_SESSION["ALLOW_BRANCH_CREATE"]))	? $_SESSION["ALLOW_BRANCH_CREATE"]	: '';
$_SESSION["ALLOW_CONTROL_CREATE"]	= (!empty($_SESSION["ALLOW_CONTROL_CREATE"]))	? $_SESSION["ALLOW_CONTROL_CREATE"]	: '';
$_SESSION["ALLOW_EZMLM_CREATE"]		= (!empty($_SESSION["ALLOW_EZMLM_CREATE"]))	? $_SESSION["ALLOW_EZMLM_CREATE"]	: '';
$_SESSION["ADVANCED_MODE"]		= (!empty($_SESSION["ADVANCED_MODE"]))		? $_SESSION["ADVANCED_MODE"]		: '';
$_SESSION["SINGLE_USER"]		= (!empty($_SESSION["SINGLE_USER"]))		? $_SESSION["SINGLE_USER"]		: '';
// }}}

// {{{ Load configuration from the database...
foreach($_pql->ldap_basedn as $dn)  {
  $dn = pql_format_normalize_dn($dn);
  unset($info);
  
  // 3a. Load 'resonable defaults' for this top DN
  $_SESSION["loading_resonable_defaults"] = 1;
  if(file_exists($_SESSION["path"]."/.DEBUG_CACHE") and !preg_match('/index2\.php$/i', $_SERVER["SCRIPT_FILENAME"]))
	echo "<br>\n<b>*</b> Loading 'resonable defaults'<br>\n";
  require($_SESSION["path"]."/include/defaults.inc");
  if(file_exists($_SESSION["path"]."/.DEBUG_CACHE") and !preg_match('/index2\.php$/i', $_SERVER["SCRIPT_FILENAME"]))
	echo "</b>";
  unset($_SESSION["loading_resonable_defaults"]);

  if(file_exists($_SESSION["path"]."/.DEBUG_CACHE") and !preg_match('/index2\.php$/i', $_SERVER["SCRIPT_FILENAME"]))
	echo "<br>\n<b>*</b> Go through DN: '$dn'<br>\n";

  if(@(is_array($_SESSION["BASE_DN"]) and !in_array($dn, $_SESSION["BASE_DN"])) or !@$_SESSION["BASE_DN"])
	// Remember the base DN(s)
	$_SESSION["BASE_DN"][] = $dn;
  
  // 3b. Load configurations from LDAP base DN, override defaults
  $sr = @ldap_read($_pql->ldap_linkid, $dn, pql_get_define("PQL_ATTR_OBJECTCLASS").'=*');
  if($sr) {
	$info = ldap_get_entries($_pql->ldap_linkid, $sr)
	  or pql_format_error(1);
	
	// Go through the PQL_ATTRIBUTE defined in include/config_attributes.inc
	// once for each level (GLOBAL, SUFFIX, USER), looking for the attribute.
	// If it have a value (usually TRUE/FALSE), then set the appropriate define.
	for($i=0; $i < $info["count"]; $i++) {
	  foreach($PQL_ATTRIBUTE as $level => $level_array) {
		if($level != 'USER') {
		  foreach($level_array as $define_name => $define_attribute) {
			$define_attribute = strtolower($define_attribute);
			$pql_config_private_array_which_irritates_me = NULL;
			
			if(!empty($info[$i][$define_attribute])) {
			  // leg2017053
			  // We think, that there as a bug iterating over $info
			  // since we get a warning with not existing ["count"] index
			  // so we skip these
			  if(is_numeric($define_attribute)) {
				continue;
		          }
			  for($k=0; $k < $info[$i][$define_attribute]["count"]; $k++) {
				if($pql_config_private_array_which_irritates_me)
				  $pql_config_private_array_which_irritates_me .= ",".$info[$i][$define_attribute][$k];
				else
				  $pql_config_private_array_which_irritates_me = $info[$i][$define_attribute][$k];
			  }
			  
			  if($level == 'GLOBAL')
				pql_set_define($define_name, $pql_config_private_array_which_irritates_me, true);
			  else
				pql_set_define($define_name, $pql_config_private_array_which_irritates_me, true, $dn);
			  
			  unset($pql_config_private_array_which_irritates_me);
			}
		  }
		}
	  }
	}
	
	// 4a. Verify if the user is administrator on the topmost DN (i.e. 'super admin') -> Allow creation of domain/branches.
	if(!empty($_SESSION["USER_DN"]) and empty($_SESSION["ALLOW_BRANCH_CREATE"])) {
	  $_SESSION["ALLOW_BRANCH_CREATE"] = pql_validate_administrator($dn, pql_get_define("PQL_ATTR_ADMINISTRATOR"),
																	$_SESSION["USER_DN"]);
	  
	  // User is super administrator - allow for administrating QmailLDAP/Control objects and ezmlm lists as well.
	  if($_SESSION["ALLOW_BRANCH_CREATE"]) {
		$_SESSION["ALLOW_CONTROL_CREATE"] = 1;
		$_SESSION["ALLOW_EZMLM_CREATE"]   = 1;
	  }
	}
	
	// 4b. Verify if the user is controlsAdministrator in the topmost DN -> Allow administration of QmailLDAP/Control objects.
	if(!empty($_SESSION["USER_DN"]) and empty($_SESSION["ALLOW_CONTROL_CREATE"]))
	  $_SESSION["ALLOW_CONTROL_CREATE"] = pql_validate_administrator($dn, pql_get_define("PQL_ATTR_ADMINISTRATOR_CONTROLS"),
																	 $_SESSION["USER_DN"]);
	
	// 4c. Verify if the user is ezmlmAdministrator in the topmost DN -> Allow administration of Ezmlm lists.
	if(!empty($_SESSION["USER_DN"]) and empty($_SESSION["ALLOW_EZMLM_CREATE"]))
	  $_SESSION["ALLOW_EZMLM_CREATE"] = $_pql->get_dn($dn, pql_get_define("PQL_ATTR_ADMINISTRATOR_EZMLM").'='.$_SESSION["USER_DN"]);
	
	// 4d. Find out which attribute to use to reference users with in this root DN
	$ref = $_pql->get_attribute($dn, pql_get_define("PQL_ATTR_REFERENCE_USERS_WITH"));
	pql_set_define("PQL_CONF_REFERENCE_USERS_WITH", $ref, false, $dn);
	
	// 4e. Find out which attribute to use to reference domains with in this root DN
	$ref = $_pql->get_attribute($dn, pql_get_define("PQL_ATTR_REFERENCE_DOMAINS_WITH"));
	pql_set_define("PQL_CONF_REFERENCE_DOMAINS_WITH", $ref, false, $dn);
	
	// 4f. Find the computer DN
	if(empty($_SESSION["USER_SEARCH_DN_CTR"])) {
	  $filter = '(&('.pql_get_define("PQL_ATTR_OU").'=Computers)('.pql_get_define("PQL_ATTR_OBJECTCLASS").'=organizationalUnit))';
	  $tmp = $_pql->get_dn($dn, $filter, 'ONELEVEL');
	  
	  if(@$tmp[0])
		$_SESSION["USER_SEARCH_DN_CTR"] = $tmp[0];
	  
	  unset($filter); unset($tmp);
	}
	
	// ----------------------------
	if(!pql_get_define("PQL_CONF_OBJECTCLASS_DOMAIN", $dn)) {
	  if(pql_get_define("PQL_CONF_REFERENCE_DOMAINS_WITH", $dn) == "dc") {
		$objectclass_domain = "domain dcOrganizationNameForm";
	  } elseif(pql_get_define("PQL_CONF_REFERENCE_DOMAINS_WITH", $dn) == "ou") {
	  $objectclass_domain = "organizationalUnit";
	  } elseif(pql_get_define("PQL_CONF_REFERENCE_DOMAINS_WITH", $dn) == "o") {
		$objectclass_domain = "organization";
	  }
	  pql_set_define("PQL_CONF_OBJECTCLASS_DOMAIN", $objectclass_domain . " phpQLAdminBranch", false, $dn);
	}

	if(!empty($_SESSION["USER_DN"]) and empty($_SESSION["ALLOW_GLOBAL_CONFIG_SAVE"])) {
	  // 4g. Verify that the user have access to write the global phpQLAdmin config
	  $_SESSION["ALLOW_GLOBAL_CONFIG_SAVE"] = pql_validate_administrator($dn,
																		 pql_get_define("PQL_ATTR_ADMINISTRATOR"),
																		 $_SESSION["USER_DN"]);
	}
  } elseif(count($_pql->ldap_basedn) == 1) {
	// We only have one Base DN -> failure here is grave!
	pql_format_error(1);
  }
}
unset($dn);
// }}}

// 5. Check the Control DN if the user is there -> Allow management of Controls information
if(!empty($_SESSION["USER_DN"]) and empty($_SESSION["ALLOW_CONTROL_CREATE"])
   and isset($_SESSION["USER_SEARCH_DN_CTR"])) {
  $_SESSION["ALLOW_CONTROL_CREATE"] = pql_validate_administrator($_SESSION["USER_SEARCH_DN_CTR"],
																 pql_get_define("PQL_ATTR_ADMINISTRATOR_CONTROLS"),
																 $_SESSION["USER_DN"]);
}

// 6. Get user configuration for phpQLAdmin
$user_object = $_pql->search($_SESSION["USER_DN"], "(objectClass=*)", 'BASE');
foreach($PQL_ATTRIBUTE["USER"] as $define_name => $attrib) {
  if(!empty($user_object["dn"]) and isset($user_object[$attrib])) {
    // skip rootdn and not existing indices

    pql_set_define($define_name, $user_object[$attrib], true, $_SESSION["USER_DN"]);
  }
}
// 7. Get language translation
require($_SESSION["path"]."/include/pql_lang.inc");
$LANG = new pql_lang($_pql);

// 8. Include the plugin API
require($_SESSION["path"]."/include/pql_plugins.inc");

if((empty($_SESSION["ACI_SUPPORT_ENABLED"]) and function_exists("user_generate_aci")) or empty($_SESSION["NEW_STYLE_QUOTA"])) {
  $ocs = pql_get_subschema('attributeTypes');
  
  // {{{ 9a. Check ifthe attribute OpenLDAPaci exists - ACI support is enabled in the LDAP server.
  //     OR: We've specified (manually) to (en)force the use of ACI's via the baseDN attribute
  //         'useACI'.
  //     -> Just make sure that useACI isn't equal to 'FALSE' which means the user/admin have
  //        elected to NOT use ACI's!
  if(empty($_SESSION["ACI_SUPPORT_ENABLED"]) and function_exists("user_generate_aci") and
	 (!empty($ocs[pql_get_define("PQL_ATTR_LDAPACI")]["OID"]) or pql_get_define("PQL_CONF_ACI_USE")) and
	 (pql_get_define("PQL_CONF_ACI_USE") != 'FALSE'))
	$_SESSION["ACI_SUPPORT_ENABLED"] = 1;
// }}}
  
  // {{{ 9b. Check to see if the attributes mailQuotaSize and mailQuotaCount exists - new style quota.
  $_SESSION["NEW_STYLE_QUOTA"] = 0;
  if(!empty($ocs[pql_get_define("PQL_ATTR_QUOTA_SIZE")]["OID"]) and
     !empty($ocs[pql_get_define("PQL_ATTR_QUOTA_COUNT")]["OID"]))
      $_SESSION["NEW_STYLE_QUOTA"] = 1;
// }}}

  // {{{ 9c. Do we support old style quota, even if new style is enabled?
  if(empty($ocs[pql_get_define("PQL_ATTR_QUOTA_VALUE")]["OID"])) {
	// The old style quota attribute isn't defined - don't support it
	$_SESSION["OLD_STYLE_QUOTA"] = 0;
  } else {
	// It's defined so support old style quota
	$_SESSION["OLD_STYLE_QUOTA"] = 1;
  }
// }}}
}

if(@$_SESSION["ACI_SUPPORT_ENABLED"]) {
  require($_SESSION["path"]."/include/pql_aci.inc");
}

// {{{ Remove some variables so we don't clutter the global variable name space!
unset($host); unset($servers); unset($i);
unset($define_attribute); unset($sr); unset($info);
unset($v); unset($nkey); unset($k); unset($ref);
// }}}

// See bug #203...
// This created an empty '$_REQUEST["domain"]' value sometimes...
//@(($_REQUEST["domain"] = $_GET["domain"]) || ($_REQUEST["domain"] = $_POST["domain"]));
if(!@$_REQUEST["domain"]) {
  if(@$_GET["domain"])
	$_REQUEST["domain"] = $_GET["domain"];
  elseif(@$_POST["domain"])
	$_REQUEST["domain"] = $_POST["domain"];
}

/*
 * Local variables:
 * mode: php
 * mode: font-lock
 * tab-width: 4
 * End:
 */
?>
