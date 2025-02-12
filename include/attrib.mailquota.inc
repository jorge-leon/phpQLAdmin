<?php
// attribute plugin for
// mail (standard mail address)
// $Id: attrib.mailquota.inc,v 2.39 2007-09-29 21:15:13 turbo Exp $

// {{{ attribute_check(type)
function attribute_check($type) {
    global $error, $LANG;

	$failed = false;

	// Just to save some typing.
	$options  = (!empty($_REQUEST["options"]) ? $_REQUEST["options"] : "");
	$maxsize  = (!empty($_REQUEST["maxsize"]) ? $_REQUEST["maxsize"] : "");
	$maxmails = (!empty($_REQUEST["maxmails"]) ? $_REQUEST["maxmails"] : "");
	
	// the checks only make sense if we're doing user defined
	// mailquotas, not if we're setting this user to the standard
	// in controls/ldapdefaultquota
	
	// empty
	if($maxsize == "" and $options == "user") {
		$error["maxsize"] = $LANG->_('Missing');
		$failed = true;
	}
	
	// valid (numeric)
	if($maxsize != "" and preg_match("/[^0-9]/", $maxsize) and $options == "user") {
		$error["maxsize"] = $LANG->_('Invalid');
		$failed = true;
	}
	
	// bigger than 0
	if($error["maxsize"] == "" and $maxsize <= 0 and $options == "user") {
		$error["maxsize"] = $LANG->_('Must be above zero');
		$failed = true;
	}
	
	// empty
	if($maxmails == ""  and $options == "user") {
		$error["maxmails"] = $LANG->_('Missing');
		$failed = true;
	}
	
	// valid (numeric)
	if($maxmails != "" and preg_match("/[^0-9]/", $maxmails)  and $options == "user") {
		$error["maxmails"] = $LANG->_('Invalid');
		$failed = true;
	}
	
	// bigger than 0
	if($error["maxmails"] == "" and $maxmails <= 0 and $options == "user"){
		$error["maxmails"] = $LANG->_('Must be above zero');
		$failed = true;
	}
	
	if($failed) {
		return false;
	} else {
		return true;
  }
}
// }}}

// {{{ attribute_init(void)
function attribute_init() {
    global $_pql;

	// Retreive old values
	if($_REQUEST["attrib"] == 'basequota') {
	  // Get default quota from domain
	  $quota = $_pql->get_attribute($_REQUEST["domain"], pql_get_define("PQL_ATTR_BASEQUOTA"));
	  if($quota) {
		//$tmp = explode(',', $quota);
		//$_REQUEST["maxsize"] = $tmp[0];
		//$_REQUEST["maxmails"] = $tmp[1];
		$quota = pql_parse_quota($quota);
		$_REQUEST["maxsize"] = $quota["maxsize"];
		$_REQUEST["maxmails"] = $quota["maxmails"];

		$_REQUEST["options"]  = "user";
	  } else
		$_REQUEST["options"]    = "default";
	} elseif($_REQUEST["attrib"] == 'mailquota') {
	  // Get quota value(s) from user
	  $quota = pql_user_get_quota($_REQUEST["user"]);
	  if(is_array($quota)) {
		if($quota["maxmails"])
		  $_REQUEST["maxmails"] = $quota["maxmails"];

		if($quota["maxsize"])
		  $_REQUEST["maxsize"]  = $quota["maxsize"];

		$_REQUEST["options"]  = "user";
	  } else
		$_REQUEST["options"]    = "default";
	}
}
// }}}

// {{{ attribute_print_form(type)
function attribute_print_form($type = "modify") {
    global $error, $LANG;
?>
  <form action="<?php echo $_SERVER["PHP_SELF"]?>" method="post">
    <table cellspacing="0" cellpadding="3" border="0">
      <th colspan="3" align="left"><?php echo $LANG->_('Change quota')?></th>
<?php if(!empty($_SESSION["USE_CONTROLS"])) { ?>
        <tr class="<?php pql_format_table(); ?>">
          <td class="title"><?php echo $LANG->_('Options')?></td>
          <td>
            <input type="radio" name="options" value="default" <?php if($_REQUEST["options"] == "default") { echo "checked"; } ?>>
            <?php echo $LANG->_('Standard')?>
            <input type="radio" name="options" value="user" <?php if($_REQUEST["options"] == "user") { echo "checked"; } ?>>
            <?php echo $LANG->_('User defined')?>
          </td>
        </tr>
<?php } else { ?>
        <input type="hidden" name="options" value="user">
<?php } ?>

        <tr class="<?php pql_format_table(); ?>">
          <td class="title"><?php echo $LANG->_('Maximum size of mailbox')?></td>
          <td><?php echo pql_format_error_span($error["maxsize"]); ?><input type="text" name="maxsize" value="<?php echo $_REQUEST["maxsize"]/1024; ?>"> KB</td>
        </tr>

        <tr class="<?php pql_format_table(); ?>">
          <td class="title"><?php echo $LANG->_('Maximum number of mails')?></td>
          <td><?php echo pql_format_error_span($error["maxmails"]); ?><input type="text" name="maxmails" value="<?php echo $_REQUEST["maxmails"]; ?>"> <?php echo $LANG->_('Mails')?></td>
        </tr>

<?php if(!empty($_SESSION["USE_CONTROLS"])) { ?>
        <tr class="subtitle">
          <table>
            <tr>
              <td>
                <?php echo pql_format_error_span($error["selected"]); ?>
                <img src="images/info.png" width="16" height="16" alt="" border="0">
              </td>
              <td>
                &nbsp;<?php echo $LANG->_('With \'Standard\', QmailLDAP will get quotas from its configuration file (~controls/ldapdefaultquota) or corresponding value in QmailLDAP/Controls. See "q-l mailbox" below your QmailLDAP/Controls object(s).')?>
              </td>
            </tr>
          </table>
        </tr>
<?php } ?>
      </th>
    </table>

    <input type="hidden" name="submit" value="1">
    <input type="hidden" name="attrib" value="<?php echo $_REQUEST["attrib"]?>">
    <input type="hidden" name="domain" value="<?php echo urlencode($_REQUEST["domain"])?>">
    <input type="hidden" name="rootdn" value="<?php echo urlencode($_REQUEST["rootdn"])?>">
<?php if($_REQUEST["attrib"] == 'mailquota') { ?>
    <input type="hidden" name="user" value="<?php echo urlencode($_REQUEST["user"])?>">
<?php } ?>
    <input type="hidden" name="view" value="<?php echo $_REQUEST["view"]?>">
    <br>
    <input type="submit" value="<?php echo $LANG->_('Save')?>">
  </form>

<?php
}
// }}}

// {{{ attribute_save(save)
function attribute_save($type) {
    global $_pql, $LANG;

	// Just incase user 'forgets' to tick in the 'User defined' field...
	if((($_REQUEST["options"] == 'default') or !$_REQUEST["options"]) and (!empty($_REQUEST["maxsize"]) or !empty($_REQUEST["maxmails"])))
	  // Default specified, but we have value(s) for quota! Change options to 'user'
	  $_REQUEST["options"] = "user";

	switch($_REQUEST["options"]) {
	  case "user":
		// It's possible to support both old AND new style quota at the same time!
		// Hence no 'elseif()' below!

		if($_REQUEST["attrib"] != 'basequota') {
		  // {{{ Old style quota
		  if($_SESSION["OLD_STYLE_QUOTA"]) {
			$mailquota[pql_get_define("PQL_ATTR_QUOTA_VALUE")] = '';
			if(!empty($_REQUEST["maxsize"])) {
			  $_REQUEST["maxsize"] = $_REQUEST["maxsize"] * 1024;
			  $mailquota[pql_get_define("PQL_ATTR_QUOTA_VALUE")] .= $_REQUEST["maxsize"] . "S";
			}
			
			if(!empty($_REQUEST["maxmails"]))
			  $mailquota[pql_get_define("PQL_ATTR_QUOTA_VALUE")] .= ',';
			
			if($_REQUEST["maxmails"] != "")
			  $mailquota[pql_get_define("PQL_ATTR_QUOTA_VALUE")] .= $_REQUEST["maxmails"] . "C";
		  }
		  // }}}
		  
		  // {{{ New style quota
		  if($_SESSION["NEW_STYLE_QUOTA"]) {
			// Set the new style quota values
			if($_REQUEST["maxsize"])
			  $mailquota[pql_get_define("PQL_ATTR_QUOTA_SIZE")]  = $_REQUEST["maxsize"] * 1024;
			
			if($_REQUEST["maxmails"] != "")
			  $mailquota[pql_get_define("PQL_ATTR_QUOTA_COUNT")] = $_REQUEST["maxmails"];
		  }
		  // }}}
		}

		$msg = pql_complete_constant($LANG->_('Successfully changed %what% to %new%'),
									 array('what' => $LANG->_('quota'),
										   'new'  => pql_ldap_mailquota(array('maxsize'  => $_REQUEST["maxsize"] * 1024,
																			  'maxmails' => $_REQUEST["maxmails"]), 0)));
		break;

	  case "default":
		if(!empty($_SESSION["USE_CONTROLS"])) {
			unset($mailquota); // need to be unset initialy in check later

			// Instead of doing this again, we include the file where this is done.
			global $quota, $_pql_control;
			include($_SESSION["path"]."/include/pql_control.inc");
			include($_SESSION["path"]."/include/attrib.control.ldapdefaultquota.inc");

			// Initialize a QmailLDAP/Controls session
			$_pql_control = new pql_control($_SESSION["USER_HOST"], $_SESSION["USER_DN"], $_SESSION["USER_PASS"]);

			// Get all QmailLDAP/Control hosts.
			$result = $_pql_control->get_dn($_SESSION["USER_SEARCH_DN_CTR"], '(&(cn=*)(objectclass=qmailControl))');
			for($i=0; $i < count($result); $i++)
			  $hosts[] = $_pql_control->get_attribute($result[$i], pql_get_define("PQL_ATTR_CN"));

			if(is_array($hosts)) {
				foreach($hosts as $host) {
					if(empty($mailquota)) {
						// Retreive default quota for this QmailLDAP/Controls object.
						$quota = ldapdefaultquota_init($host);
						if(is_array($quota)) {
							if($_SESSION["OLD_STYLE_QUOTA"]) {
							  $mailquota[pql_get_define("PQL_ATTR_QUOTA_VALUE")] = '';
							  if(!empty($quota[0]))
								$mailquota[pql_get_define("PQL_ATTR_QUOTA_VALUE")] = $quota[0]."S";

							  if(!empty($quota[1])) {
								$mailquota[pql_get_define("PQL_ATTR_QUOTA_VALUE")] .= ','.$quota[1]."C";
							  }
							}

							if($_SESSION["NEW_STYLE_QUOTA"]) {
							  if(!empty($quota[0]))
								$mailquota[pql_get_define("PQL_ATTR_QUOTA_SIZE")]  = $quota[0];
							  else
								$mailquota[pql_get_define("PQL_ATTR_QUOTA_SIZE")]  = array();

							  if(!empty($quota[1]))
								$mailquota[pql_get_define("PQL_ATTR_QUOTA_COUNT")] = $quota[1];
							  else
								$mailquota[pql_get_define("PQL_ATTR_QUOTA_COUNT")] = array();
							}
						}
					}
				}
			}
		}
// Eh?
//		else
//		  $mailquota = "";

		$msg = pql_complete_constant($LANG->_('Successfully changed %what% to %new%'),
									 array('what' => $LANG->_('quota'),
										   'new'  => $LANG->_('Standard')." ($mailquota)"));
	}

	switch($type) {
	  case "add":
		// no add operation possible with mail
		break;
		
	  case "fulldomain":
		// Create a user search filter
		if(pql_get_define("PQL_CONF_USER_FILTER", $_REQUEST["rootdn"]))
		  $filter = pql_get_define("PQL_CONF_USER_FILTER", $_REQUEST["rootdn"]);
		else
		  $filter  = pql_get_define("PQL_CONF_REFERENCE_USERS_WITH", $_REQUEST["rootdn"])."=*";

		// Retreive all users in this branch/domain.
	    $users   = $_pql->get_dn($_REQUEST["domain"], $filter);
		if(is_array($users)) {
			foreach($users as $user)
			  $return[] = pql_modify_attribute($user, '', '', $mailquota);

			if(in_array(false, $return)) 
			  $msg = pql_complete_constant($LANG->_('Failed to change %what%'),
										   array('what' => $LANG->_('quota'))) . ": " . pql_format_error(0);
			
			attribute_forward($msg);
		}
		
	  case "modify":
		// remove attribute if it is set to ""

		if($_REQUEST["attrib"] == sprintf("%s", pql_get_define("PQL_ATTR_BASEQUOTA"))) {
			$domaindn = urldecode($_REQUEST["domain"]);

			// Setup the array to send to LDAP server
			$entry[pql_get_define("PQL_ATTR_BASEQUOTA")]      = '';
			if($_REQUEST["maxsize"])
			  $entry[pql_get_define("PQL_ATTR_BASEQUOTA")]   .= $_REQUEST["maxsize"] * 1024 ."S";
			if($_REQUEST["maxmails"])
			  $entry[pql_get_define("PQL_ATTR_BASEQUOTA")]   .= ','.$_REQUEST["maxmails"]."C";

			$entry[pql_get_define("PQL_ATTR_OBJECTCLASS")]    = pql_setup_branch_objectclasses(0, $domaindn, $_pql->ldap_linkid);
			// if already present we get an ldap error here because of duplicate attribute.
			//$entry[pql_get_define("PQL_ATTR_OBJECTCLASS")][]  = 'phpQLAdminConfig';

			// We're setting default quota for domain, not user.
			if(!pql_modify_attribute($_REQUEST["domain"], '', 1, $entry))
			  // Failed
			  $msg = pql_complete_constant($LANG->_('Failed to change %what% for domain %domain%'),
										   array('what'   => $LANG->_('default quota'),
												 'domain' => $domaindn)) . ": " . pql_ldap_error(0);
			// The success msg is set above (in the switch($type) code).
		} else {
			if(!pql_modify_attribute($_REQUEST["user"], '', '', $mailquota))
			  $msg = pql_complete_constant($LANG->_('Failed to change %what%'),
										   array('what' => $LANG->_('quota'))) . ": " . pql_format_error(0);
		}
		
		attribute_forward($msg);
		break;

	  default:
		die(pql_complete_constant($LANG->_('Unknown save type %type% in file %file%, function save()'),
								  array('type' => $type, 'file' => __FILE__)));
	}
}
// }}}

// Local variables:
// mode: php
// mode: font-lock
// tab-width: 4
// End:
?>
