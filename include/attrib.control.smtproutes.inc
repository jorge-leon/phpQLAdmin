<?php
// control attribute plugin for
// smtproutes (max number of bytes in message)
//
// $Id: attrib.control.smtproutes.inc,v 2.33 2007-02-15 12:08:05 turbo Exp $

// {{{ smtproutes_check(type)

function smtproutes_check($type) {
	global $error, $LANG;

	// check all domains
	if(@is_array($_REQUEST["domainname"])) {
		foreach($_REQUEST["domainname"] as $key => $dom) {
			if(!empty($dom)) {
				if(preg_match("/[^0-9a-z-\.]+/i",$dom)) {
					$error["domain_$key"] = $LANG->_('Invalid');
					$is_error = true;
				}
			}
		}
	}
	
	// relay hosts
	if(@is_array($_REQUEST["relay"])) {
		foreach($_REQUEST["relay"] as $key => $rel) {
			if(!empty($rel) and !pql_check_hostaddress($rel, true)) {
				$error["relay_$key"] = $LANG->_('Invalid');
				$is_error = true;
			}
		}
	}
	
	// ports - check only if we're not deleting a specific domain
	if(@is_array($_REQUEST["port"]) and ($_REQUEST["type"] != 'del')) {
		foreach($_REQUEST["port"] as $key => $prt) {
			if(preg_match("/[^0-9]+/i", $prt)) {
				$error["port_$key"] = $LANG->_('Invalid');
				$is_error = true;
			}
		}
	}

	if($is_error)
	  return false;
	
	return true;
}

// }}}

// {{{ smtproutes_init(host)
// init values
function smtproutes_init($host) {
	global $_pql_control;

	if(!empty($host)) {
		// fetch data from ldap server
		if($host == "Global")
		  $value = $_pql_control->get_attribute($_SESSION["USER_SEARCH_DN_CTR"],
									 pql_get_define("PQL_ATTR_SMTPROUTES"), 0, pql_get_define("PQL_ATTR_CN")."=*");
		else
		  $value = $_pql_control->get_attribute($host, pql_get_define("PQL_ATTR_SMTPROUTES"));

		if(empty($value))
		  return true;
		elseif(is_array($value)) {
			foreach($value as $route)
			  $_REQUEST["smtproutes"][] = explode(":", $route);
		} else
		  $_REQUEST["smtproutes"][] = explode(":", $value);
		
		foreach($_REQUEST["smtproutes"] as $route) {
			$_REQUEST["domain"][] = $route[0];
			$_REQUEST["relay"][]  = $route[1];
			$_REQUEST["port"][]   = $route[2];
		}
	} else
	  // We're called from the domain details page,
	  // get SMTP routes later (in the print_form() func).
	  return true;
}
// }}}


// {{{ smtproutes_print_view(host)
function smtproutes_print_view($host) {
	global $_pql_control, $LANG;

	// init data
	smtproutes_init($host);

	$routes = array();
	if(@is_array($_REQUEST["smtproutes"])) {
		foreach($_REQUEST["smtproutes"] as $route) {
			// domain
			if(@empty($route[0]))
			  $route[0] = "&lt; ".$LANG->_('All', 4)." &gt;";
			
			// relay host
			if(@empty($route[1]))
			  $route[1] = $LANG->_('MX record');
			
			// port
			if(@empty($route[2]))
			  $route[2] = "25";
			
			$routes[] = $route;
		}
	}
?>
  <table cellspacing="0" cellpadding="3" border="0">
    <th colspan="3" align="left">smtproutes (<?php echo $LANG->_('Defined SMTP routes')?>)</th>
      <tr class="title">
        <td><?php echo $LANG->_('Source domain')?></td>
        <td>&nbsp;</td>
        <td><?php echo $LANG->_('Relay host')?></td>
        <td>&nbsp;</td>
        <td><?php echo $LANG->_('Port')?></td>
        <td></td>
      </tr>
<?php
	if(!is_array($routes)) {
?>
      <tr class="<?php pql_format_table(); ?>">
        <td colspan="6"><?php echo $LANG->_('No routes defined')?></td>
      </tr>
<?php
	} else {
		foreach($routes as $route) {
?>
      <tr class="<?php pql_format_table(); ?>">
        <td><?php echo pql_maybe_idna_decode($route[0])?></td>
        <td><b>=&gt;</b></td>
        <td><?php echo pql_maybe_idna_decode($route[1])?></td>
        <td><b>:</b></td>
        <td><?php echo pql_maybe_idna_decode($route[2])?></td>
        <td>
          <a href="control_edit_attribute.php?mxhost=<?php echo $host?>&attrib=smtproutes&type=modify&set=<?php echo $route[0].":".$route[1].":".$route[2]?>"><img src="images/edit.png" width="12" height="12" border="0" alt="<?php echo $LANG->_('Change options')?>"></a>
          <a href="control_edit_attribute.php?mxhost=<?php echo $host?>&attrib=smtproutes&type=del&submit=1&set=<?php echo $route[0].":".$route[1].":".$route[2]?>"><img src="images/del.png"  width="12" height="12" border="0" alt="<?php echo $LANG->_('Delete options')?>"></a>
        </td>
      </tr>

<?php
		} // end foreach
	} // end if is_array
?>
      <tr class="subtitle">
        <td><a href="control_edit_attribute.php?mxhost=<?php echo $host?>&attrib=smtproutes&type=add"><img src="images/edit.png" width="12" height="12" border="0"><?php echo $LANG->_('Add SMTP route')?></a></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
        <td></td>
      </tr>
    </th>
  </table>

<?php
}
// }}}

// {{{ smtproutes_print_form(void)

function smtproutes_print_form() {
	global $_pql_control, $error, $LANG;

	if(@$_REQUEST["attrib"] && @$_REQUEST["rootdn"] && @$_REQUEST["set"])
	  // We're called from the domain details page -> the domain name is in $set
	  $_REQUEST["domainname"][0] = $_REQUEST["set"];
	elseif(@$_REQUEST["attrib"] && @$_REQUEST["set"] && @$_REQUEST["mxhost"]) {
		// We're called from the "QmailLDAP/Controls -> qmail-remote" page  - change value
		$tmp = explode(":", $_REQUEST["set"]); unset($_REQUEST["set"]);

		$ch_domainname = $tmp[0];
		$ch_relay      = $tmp[1];
		$ch_port       = $tmp[2];
	} elseif(@$_REQUEST["type"] && @$_REQUEST["mxhost"])
	  // We're called from the "QmailLDAP/Controls -> qmail-remote" page  - add value
	  unset($_REQUEST["domainname"]);
	else
	  // We're called from the "QmailLDAP/Controls -> qmail-remote" page  - the domain name is in $domain
	  $_REQUEST["domainname"] = $_REQUEST["domain"];

	if(($_REQUEST["routecount"] < count($_REQUEST["domainname"]) - 1) and (count($_REQUEST["domainname"]) != 0))
	  $routecount = count($_REQUEST["domainname"]) - 1;

	// --------------------------------------------------------------------
	// There's acctually TWO 'completly' different forms here...
	//
	// One is when we're called from the "Domain details -> QmailLDAP/Controls Options".
	// The intention with this is that we want to route the domain to <empty> mailserver.
	// We know the domain name, and we can find out the mailserver by getting all
	// QmailLDAP/Controls object from the USER_SEARCH_DN_CTR DN. Show a select menu
	// and fill in a default port (25).
	//
	// The other form is when we're called from "QmailLDAP/Controls -> qmail-remote".
	// The intention with this is that we want to route <empty> domain to the known
	// mailserver. We know the mailserver and we fill in a default port (25). All we
	// need is a source domain (the domain to route).
	// --------------------------------------------------------------------

	// Default port...
	if(@empty($port[0]))
	  $port[0] = 25;

	if($_REQUEST["type"] != 'del') {
		if(@empty($_REQUEST["mxhost"])) {
			// Get all QmailLDAP/Control hosts.
			$result = $_pql_control->get_dn($_SESSION["USER_SEARCH_DN_CTR"],
								 '(&(cn=*)(objectclass=qmailControl))');
			for($i=0; $i < count($result); $i++)
			  $hosts[] = $_pql_control->get_attribute($result[$i], pql_get_define("PQL_ATTR_CN"));
		}
	} else {
		// Get all hosts which have this domain in 'attrib'
		$hosts = $_pql_control->get_dn($_SESSION["USER_SEARCH_DN_CTR"],
							"(&(cn=*)(objectclass=qmailControl)(".$_REQUEST["attrib"]."=".$_REQUEST["set"]."*))");
		// Convert the DN into a host list
		for($i=0; $i < count($hosts); $i++) {
			$host = explode(',', $hosts[$i]);
			$host = preg_replace(pql_get_define("PQL_ATTR_CN") . "=", "", $host[0]);
			$newhosts[] = $host;
		}

		$hosts = $newhosts;
	}
?>

    <form action="<?php echo $_SERVER["PHP_SELF"]?>" method="post">
<?php if(($ch_domainname and $ch_relay and $ch_port)
		 or
		 ($_REQUEST["type"] == 'add' and is_array($_REQUEST["smtproutes"]))) {
		// Called from the domain details page -> adding ONE value, but we want to remember the previous

		for($i=0; $i < count($_REQUEST["domain"]); $i++) {
			if($_REQUEST["domain"][$i] != $ch_domainname) {
?>
    <!-- Relay #<?php echo $i?>: <?php echo $_REQUEST["domain"][$i]?> -> <?php echo $_REQUEST["relay"][$i]?>:<?php echo $_REQUEST["port"][$i]?> -->
    <input type="hidden" name="domainname[<?php echo $i?>]" value="<?php echo $_REQUEST["domain"][$i]?>">
    <input type="hidden" name="relay[<?php echo $i?>]"      value="<?php echo $_REQUEST["relay"][$i]?>">
    <input type="hidden" name="port[<?php echo $i?>]"       value="<?php echo $_REQUEST["port"][$i]?>">

<?php		}
		}

		if($ch_domainname && $ch_relay && $ch_port) {
			// Remember what value to change
			for($i=0; $i < count($_REQUEST["domain"]); $i++) {
				if(preg_match("/$ch_domainname/i", $_REQUEST["domain"][$i]))
				  $change = $i;
			}
		}

		$counter = $i;
      } else
		$counter = 0;

	if(isset($change)) {
		$objnr = $change;
		$_REQUEST["domainname"][$objnr] = $ch_domainname;
	} else {
		$objnr = $counter;
		unset($_REQUEST["relay"]); unset($_REQUEST["port"]);
	}
	
	if($ch_domainname && $ch_relay && $ch_port) {
?>

    <!-- This is the number of the relay (see above) we are changing -->
    <input type="hidden" name="change"        value="<?php echo $change?>">

<?php } ?>
    <table cellspacing="0" cellpadding="0" border="0">
      <th colspan="3" align="left">smtproutes (<?php echo $LANG->_('SMTP routes')?>)</th>
        <!-- A --------------------------------- -->
        <tr class="title">
          <td><?php echo $LANG->_('Source domain')?></td>
<?php if(@$_REQUEST["attrib"] && @$_REQUEST["rootdn"] && @$_REQUEST["set"]) { ?>
          <td>QmailLDAP/Controls</td>
<?php } else { ?>
          <td><?php echo $LANG->_('Relay host')?></td>
<?php } ?>
<?php if($_REQUEST["type"] != 'del') { ?>
          <td><?php echo $LANG->_('Port')?></td>
<?php } ?>
        </tr>

        <!-- B --------------------------------- -->
        <tr class="<?php pql_format_table(); ?>">
          <td><?php echo pql_format_error_span($error["domainname_$counter"]); ?><input type="text" name="domainname[<?php echo $counter?>]" value="<?php echo pql_maybe_idna_decode($_REQUEST["domainname"][$objnr])?>" <?php if(!@empty($_REQUEST["domainname"][$objnr])) { echo "READONLY"; } ?>>&nbsp;&nbsp;<b><?php if($type != 'del') { ?>=><?php } ?></b></td>
          <td>
<?php if(@$_REQUEST["attrib"] && @$_REQUEST["rootdn"] && @$_REQUEST["set"]) { ?>
            <select name="relay[]" size="5" multiple>
<?php	for($i=0; $i < count($hosts); $i++) { ?>
              <option value="<?php echo $hosts[$i]?>"><?php echo pql_maybe_idna_decode($hosts[$i])?></option>
<?php	} ?>
            </select>
<?php } else { ?>
            <?php echo pql_format_error_span($error["relay_$counter"]); ?><input type="text" name="relay[<?php echo $counter?>]" value="<?php echo $_REQUEST["relay"][$objnr]?>">&nbsp;&nbsp;<b>:</b></td>
<?php } ?>
          </td>
<?php if($_REQUEST["type"] != 'del') { ?>
          <td><?php echo pql_format_error_span($error["port_$counter"]); ?><input type="text" size="4" maxlength="4" name="port[<?php echo $objnr?>]" value="<?php if($ch_port) { echo $ch_port; } else { echo $port[$objnr]; } ?>"></td>
<?php } ?>
        </tr>

<?php for($i=1, $j=$counter; $i <= $_REQUEST["routecount"]; $i++, $counter++) { ?>
        <!-- C --------------------------------- -->
        <tr class="<?php pql_format_table(); ?>">
          <td><?php echo pql_format_error_span($error["domainname_$i"]); ?><input type="text" name="domainname[<?php echo $i?>]" value="<?php echo $_REQUEST["domainname"][$i]?>">&nbsp;&nbsp;<b>=></b></td>
          <td><?php echo pql_format_error_span($error["relay_$i"]); ?><input type="text" name="relay[<?php echo $i?>]" value="<?php echo $_REQUEST["relay"][$i]?>">&nbsp;&nbsp;<b>:</b></td>
          <td><?php echo pql_format_error_span($error["port_$i"]); ?><input type="text" size="4" maxlength="4" name="port[<?php echo $i?>]" value="<?php echo $_REQUEST["port"][$i]?>"></td>
        </tr>

<?php } ?>
<?php if(@empty($_REQUEST["attrib"])) { ?>
        <tr class="subtitle">
          <td colspan="3"><a href="<?php echo $_SERVER["PHP_SELF"]?>?attrib=<?php echo $_REQUEST["attrib"]?>&routecount=<?php echo ($_REQUEST["routecount"]+1)?>&mxhost=<?php echo $_REQUEST["mxhost"]?>">add additional route</a>(<?php echo $LANG->_('Please save first, changes will be lost', 4)?>)</td>
        </tr>

<?php } 

	  if($_REQUEST["type"] != 'del') {
?>
        <!-- D --------------------------------- -->
        <tr class="subtitle">
          <td colspan="3"><img src="images/info.png" width="16" height="16" border="0">If the source domain is empty, the route affects to all hosts. If relay host is empty, qmail will take the MX record of the domain to relay. Use port if the smtp port of the relay host is not 25.</td>
        </tr>
<?php } else { ?>
        <tr class="subtitle">
          <td colspan="3"><img src="images/info.png" width="16" height="16" border="0">You have selected to remove the domain <b><?php echo $_REQUEST["domainname"][0]?></b> from <i>smtproutes</i>. Please select host(s) to remove it from. All other entries in the object(s) will be preserved</td>
        </tr>
<?php } ?>
      </th>
    </table>

    <input type="hidden" name="submit"        value="1">
    <input type="hidden" name="attrib"        value="<?php echo $_REQUEST["attrib"]?>">
<?php if(@$_REQUEST["mxhost"]) { ?>
    <input type="hidden" name="mxhost"        value="<?php echo $_REQUEST["mxhost"]?>">
<?php } ?>
<?php if(@$_REQUEST["rootdn"]) { ?>
    <input type="hidden" name="rootdn"        value="<?php echo urlencode($_REQUEST["rootdn"])?>">
<?php } ?>
<?php if(!@is_array($_REQUEST["domain"])) { ?>
    <input type="hidden" name="domain"        value="<?php echo urlencode($_REQUEST["domain"])?>">
<?php } ?>
    <input type="hidden" name="type"          value="<?php echo $_REQUEST["type"]?>">
    <input type="hidden" name="view"          value="<?php echo $_REQUEST["view"]?>">
    <br>
    <input type="submit"                      value="<?php echo $LANG->_('Save')?>">
  </form>
<?php
}

// }}}

// {{{ smtproutes_save(type, host)
function smtproutes_save($type, $host) {
	global $_pql_control, $LANG;
	$success = 0;

	switch($type) {
	  case "modify":
		// Fall trough. Modify and Add is basically the same thing (at lease it uses the same functions).
	  case "add":
		// {{{ save entries - combine attributes
		if(!empty($host)) {
			// We're called from the control details page -> we know what object to update

			if($_REQUEST["change"] >= 0) {
				// We're replacing a value with another - Delete the value in $change
				// and rearrange the array.
				unset($_REQUEST["domainname"][$change]);
				unset($_REQUEST["relay"][$change]);
				unset($_REQUEST["port"][$change]);

				foreach($_REQUEST["domainname"] as $d)
				  $tmp[] = $d;
				$_REQUEST["domainname"] = $tmp; unset($tmp);

				foreach($_REQUEST["relay"] as $r)
				  $tmp[] = $r;
				$_REQUEST["relay"] = $tmp; unset($tmp);

				foreach($_REQUEST["port"] as $p)
				  $tmp[] = $p;
				$_REQUEST["port"] = $tmp; unset($tmp);
			}

			for($i=0; $i < count($_REQUEST["domainname"]); $i++) {
				// if empty, delete route
				if(($_REQUEST["domainname"][$i] == "") and ($_REQUEST["relay"][$i] == ""))
				  continue;
				
				// if port or relay is not defined, leave out
				if(@empty($_REQUEST["port"][$i]) or @empty($_REQUEST["relay"][$i]))
				  $tmp = array($_REQUEST["domainname"][$i], $_REQUEST["relay"][$i]);
				else
				  $tmp = array($_REQUEST["domainname"][$i], $_REQUEST["relay"][$i], $_REQUEST["port"][$i]);
				
				$route = join(":", $tmp);
				$smtproutes[] = $route;
			}

			// Update the object
			if($host == "Global") {
			  // Get all QLC host
			  $qlcs = $_pql_control->get_dn($_SESSION["USER_SEARCH_DN_CTR"],
								 '(&(cn=*)(objectclass=qmailControl))');

			  $success = 1;
			  foreach($qlcs as $host) {
				if(!pql_modify_attribute($host,
										 pql_get_define("PQL_ATTR_SMTPROUTES"), '', $smtproutes))
				  $success = 0;
			  }
			} else {
			  if(pql_modify_attribute($host,
									  pql_get_define("PQL_ATTR_SMTPROUTES"), '', $smtproutes))
				$success = 1;
			}
		} else {
			// We're called from the domain details page -> we must find out what object to update

			if(@$_REQUEST["relay"]) {
				// Go through each QmailLDAP/Controls object, fetching old values
				for($i=0; $i < count($_REQUEST["relay"]); $i++) {
					if(@$_REQUEST["relay"][$i]) {
						// fetch data from ldap server
						$relay  = $_REQUEST["relay"][$i];
						$values = $_pql_control->get_attribute(
													pql_get_define("PQL_ATTR_CN")."=$relay,".$_SESSION["USER_SEARCH_DN_CTR"],
													pql_get_define("PQL_ATTR_SMTPROUTES"));
						if($values and !is_array($values))
						  $values = array($values);

						// Add the new value
						$values[] = $_REQUEST["domainname"][0].":".$_REQUEST["relay"][$i].":".$_REQUEST["port"][0];
						
						// Update the object
						if(pql_modify_attribute(pql_get_define("PQL_ATTR_CN") . "=" . $_REQUEST["relay"][$i] . "," . $_SESSION["USER_SEARCH_DN_CTR"],
												pql_get_define("PQL_ATTR_SMTPROUTES"), '', $values))
						  $success = 1;
					}
				}
			}
		}

		if($success)
		  $msg = pql_complete_constant($LANG->_('Successfully changed %what%'),
									   array('what' => $LANG->_('smtproutes options')));
		else
		  $msg = pql_complete_constant($LANG->_('Failed to change %what%'),
									   array('what' => $LANG->_('smtproutes options'))) . ": " . pql_format_error(0);

		attribute_forward($msg);
		break;
		// }}}

	  case "del":
		// {{{ We'd like to remove a domain from one (or more) QmailLDAP/Controls objects
		// Go through each QmailLDAP/Controls object, fetching old values
		if(@$_REQUEST["relay"]) {
			// We're called from the control details page -> we know what object to update

			for($i=0; $i < count($_REQUEST["relay"]); $i++) {
				if(@$_REQUEST["relay"][$i]) {
					// fetch data from ldap server
					$relay  = $_REQUEST["relay"][$i];
					$values = $_pql_control->get_attribute(
												pql_get_define("PQL_ATTR_CN")."=$relay,".$_SESSION["USER_SEARCH_DN_CTR"],
												pql_get_define("PQL_ATTR_SMTPROUTES"));
					// Go through the values, removing the specific domain
					for($j=0; $j < count($values); $j++) {
						if(!preg_match("/".$_REQUEST["domainname"][0].'/i', $values[$j]))
						  $newvals[] = $values[$j];
					}
					
					// Update the object
					if(pql_modify_attribute(pql_get_define("PQL_ATTR_CN") . "=" . $_REQUEST["relay"][$i] . "," . $_SESSION["USER_SEARCH_DN_CTR"],
											pql_get_define("PQL_ATTR_SMTPROUTES"), '', $newvals))
					  $success = 1;
				}
			}
		} elseif(@$_REQUEST["domain"] && @$_REQUEST["rootdn"] && @$_REQUEST["attrib"] && $host && @$_REQUEST["set"]) {
			// We're deleting ONE value. $set contains the whole string, as it looks like in the object
			
			// Find the object(s) which contain this string
			$ctrls = $_pql_control->get_dn($_SESSION["USER_SEARCH_DN_CTR"],
								"(&(cn=*)(objectclass=qmailControl)(".pql_get_define("PQL_ATTR_SMTPROUTES")."=".$_REQUEST["set"]."))");
			// If it isn't an array, but it IS set -> make it an array!
			if($ctrls && !is_array($ctrls)) {
				$tmp = $ctrls; unset($ctrls);
				$ctrls[] = $tmp;
			}
			
			foreach($ctrls as $ctrl) {
				// Retreive this QmailLDAP/Controls object current value(s)
				$routes = $_pql_control->get_attribute($ctrl, pql_get_define("PQL_ATTR_SMTPROUTES"));
				if(is_array($routes)) {
					// Go through the SMTP routes, remove the one in $relay
					foreach($routes as $route)
					  if(!preg_match('/'.$_REQUEST["set"].'/i', $route))
						$newroutes[] = $route;
				}
				
				// Save this new array to the object
				if(pql_modify_attribute($ctrl, pql_get_define("PQL_ATTR_SMTPROUTES"), '', $newroutes))
				  $success = 1;
			}
		} elseif(@$_REQUEST["domain"] && @$_REQUEST["set"] && @$_REQUEST["rootdn"] && @$_REQUEST["attrib"]) {
			// We're called from the domain details page -> we must find out what object to update

			// Find QmailLDAP/Controls object(s) which contain this domain in SMTPRoutes
			$ctrls = $_pql_control->get_dn($_SESSION["USER_SEARCH_DN_CTR"],
								"(&(cn=*)(objectclass=qmailControl)(".pql_set_define("PQL_ATTR_SMTPROUTES")."=".$_REQUEST["set"]."*))");
			// If it isn't an array, but it IS set -> make it an array!
			if($ctrls && !is_array($ctrls)) {
				$tmp = $ctrls; unset($ctrls);
				$ctrls[] = $tmp;
			}

			foreach($ctrls as $ctrl) {
				// Retreive this QmailLDAP/Controls object current value(s)
				$routes = $_pql_control->get_attribute($ctrl, pql_get_define("PQL_ATTR_SMTPROUTES"));
				if(is_array($routes)) {
					// Go through the SMTP routes, remove the one in $set
					foreach($routes as $route)
					  if(!preg_match('/'.$_REQUEST["set"].'/i', $route))
						$newroutes[] = $route;
				}

				// Save this new array to the object
				if(pql_modify_attribute($ctrl, pql_get_define("PQL_ATTR_SMTPROUTES"), '', $newroutes))
				  $success = 1;
			}
		}

		if($success)
		  $msg = pql_complete_constant($LANG->_('Successfully changed %what%'),
									   array('what' => $LANG->_('smtproutes options')));
		else
		  $msg = pql_complete_constant($LANG->_('Failed to change %what%'),
									   array('what' => $LANG->_('smtproutes options'))) . ": " . pql_format_error(0);

		attribute_forward($msg);
		break;
		// }}}

   	  default:
		die(pql_complete_constant($LANG->_('Unknown save type %type% in file %file%, function save()'),
								  array('type' => $type, 'file' => __FILE__)));
	}
}

// }}}


// {{{ smtproutes_help(void)
function smtproutes_help() {
    global $LANG;
?>
	Artificial SMTP routes.  Each route has the form
	domain:relay, without any extra spaces.  If domain
	matches host, qmail-remote will connect to relay, as if
	host had relay as its only MX.  (It will also avoid
	doing any CNAME lookups on recip.)  host may include a
	port number to use instead of the normal SMTP port, 25:
    <pre>
            Source:      inside.af.mil
            Destination: firewall.af.mil
            Port:        26
    </pre>
	relay may be empty; this tells qmail-remote to look up
	MX records as usual.  smtproutes may include wildcards:
    <pre>
            Source:      .af.mil
            Destination: heaven.af.mil
    </pre>
	Here any address ending with .af.mil (but not af.mil
	itself) is routed by its MX records; any other address
	is artificially routed to heaven.af.mil.
	<br><br>
	The qmail system does not protect you if you create an
	artificial mail loop between machines.  However, you
	are always safe using smtproutes if you do not accept
	mail from the network.
    <p>
    If you leave the <i>source domain</i> entry empty, <u>all</u>
    domains will be affected. This can be used to send all mails
    to another mailserver for (further) processing. This is called
    a <a href="http://cr.yp.to/qmail/faq/outgoing.html#null-client" target="_new">null host</a>.
<?php
}
// }}}

// {{{ smtproutes_help_cr(void)
function smtproutes_help_cr() {
	global $LANG;
	echo $LANG->_('Help text taken from qmail man pages');
}
// }}}

// Local variables:
// mode: php
// mode: font-lock
// tab-width: 4
// End:
?>
