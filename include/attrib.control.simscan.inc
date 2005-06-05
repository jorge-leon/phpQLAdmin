<?php
// control attribute plugin for
// SimScan
//
// $Id: attrib.control.simscan.inc,v 2.1 2005-05-26 15:53:18 turbo Exp $

// {{{ simscan_check(type)
function simscan_check($type) {
	return true;
}
// }}}

// {{{ simscan_init(host)
// init values
function simscan_init($host) {
	global $_pql_control, $_pql, $LANG;
	
	// We do this here, instead of in the very similar domain_detail.php.
	// This because I don't want to have an almost exact file that can/is
	// called from 'QmailLDAP/Controls->[mxhost]'...
	$attribs = array("simscanspamassassin"		=> pql_get_define("PQL_ATTR_SIMSCAN_SPAM"),
					 "simscanclamantivirus"		=> pql_get_define("PQL_ATTR_SIMSCAN_CLAM"),
					 "simscantrophie"			=> pql_get_define("PQL_ATTR_SIMSCAN_TROPHIE"),
					 "simscanspamassassinhits"	=> pql_get_define("PQL_ATTR_SIMSCAN_SPAM_HITS"),
					 "simscanattachmentsuffix"	=> pql_get_define("PQL_ATTR_SIMSCAN_ATTACHMENTS"));
	
	foreach($attribs as $key => $attrib) {
	  // Get default value
	  if($_REQUEST["mxhost"]) {
		$cn = pql_get_define("PQL_ATTR_CN") . "=" . $_REQUEST["mxhost"] . "," . $_SESSION["USER_SEARCH_DN_CTR"];
		$value = pql_get_attribute($_pql_control->ldap_linkid, $cn, $attrib);
	  } else
		$value = pql_get_attribute($_pql->ldap_linkid, $_REQUEST["domain"], $attrib);
	  
	  $submit = ''; $set = '';
	  if(($attrib == "simscanspamassassin") or ($attrib == "simscanclamantivirus") or ($attrib == simscantrophie)) {
		// We have a value and it's a toggle - Convert the boolean value to an integer
		$value = pql_format_bool($value);
		
		if($_REQUEST["mxhost"]) {
		  if($value)
			$set = '&set=0';
		  else
			$set = '&set=1';
		  
		  $submit = "&submit=1";
		}
	  } elseif(($attrib == 'simscanattachmentsuffix') and !is_array($value))
		$value = array($value);
	  
	  $_REQUEST[$key] = $value;
	  
	  // Setup edit links. If it's a dcOrganizationNameForm attribute, then
	  // we add a delete link as well.
	  $_REQUEST["links"][$key] = $key . "_link";
	  if($_REQUEST["mxhost"]) {
		// Called from control_details.ph
		$alt1 = pql_complete_constant($LANG->_('Modify attribute %attribute% for %host%'),
									  array('attribute' => $attrib, 'host' => $_REQUEST["mxhost"]));
		$alt2 = pql_complete_constant($LANG->_('Delete attribute %attribute% for %host%'),
									  array('attribute' => $attrib, 'host' => $_REQUEST["mxhost"]));
		
		$_REQUEST["links"][$key]  = '<a href="control_edit_attribute.php?mxhost='.$_REQUEST["mxhost"].'&attrib='.$attrib.$submit.'&type=modify'.$set;
		$_REQUEST["links"][$key] .= '"><img src="images/edit.png" width="12" height="12" border="0" alt="'.$alt1.'"></a>';
	  } else {
		// Called from domain_details.ph
		global $url; // Defined in domain_details.php

		$alt1 = pql_complete_constant($LANG->_('Modify attribute %attribute% for %domainname%'),
									  array('attribute' => $attrib, 'domainname' => $domainname));
		$alt2 = pql_complete_constant($LANG->_('Delete attribute %attribute% for %domainname%'),
									  array('attribute' => $attrib, 'domainname' => $domainname));
		
		$_REQUEST["links"][$key] = "<a href=\"domain_edit_attributes.php?type=modify&attrib=$attrib&rootdn="
		  . $url["rootdn"] . "&domain=" . $url["domain"] . "&$attrib=". urlencode($value)
		  . "&view=" . $_REQUEST["view"] . "\"><img src=\"images/edit.png\" width=\"12\" height=\"12\""
		  . "border=\"0\" alt=\"$alt1\"></a>";
	  }
	}
}
// }}}


// {{{ simscan_print_view(host)
function simscan_print_view($host) {
	global $LANG;
	
	// init data
	simscan_init($host);
?>
  <table cellspacing="0" cellpadding="3" border="0">
    <th colspan="3" align="left"><?=$LANG->_('SimScan configuration')?></th>
      <tr class="<?php pql_format_table(); ?>">
        <td class="title"><?=$LANG->_('Run SpamAssassin')?></td>
        <td><?php if($_REQUEST["simscanspamassassin"]) { echo $LANG->_('Yes');}else{echo $LANG->_('No');} ?></td>
        <td align="right"><?=$_REQUEST["links"]["simscanspamassassin"]?></td>
      </tr>

      <tr class="<?php pql_format_table(); ?>">
        <td class="title"><?=$LANG->_('Run Clam Anti Virus')?></td>
        <td><?php if($_REQUEST["simscanclamantivirus"]) { echo $LANG->_('Yes');}else{echo $LANG->_('No');} ?></td>
        <td align="right"><?=$_REQUEST["links"]["simscanclamantivirus"]?></td>
      </tr>

      <tr class="<?php pql_format_table(); ?>">
        <td class="title"><?=$LANG->_('Run Trophie')?></td>
        <td><?php if($_REQUEST["simscantrophie"]) { echo $LANG->_('Yes');}else{echo $LANG->_('No');} ?></td>
        <td align="right"><?=$_REQUEST["links"]["simscantrophie"]?></td>
      </tr>

      <tr class="<?php pql_format_table(); ?>">
        <td class="title"><?=$LANG->_('Max spam hits')?></td>
        <td><?php if($_REQUEST["simscanspamassassinhits"]) { echo $_REQUEST["simscanspamassassinhits"]; } else { echo "<i>Not set</i>"; }?>&nbsp;</td>
        <td align="right"><?=$_REQUEST["links"]["simscanspamassassinhits"]?></td>
      </tr>

<?php	// Attachments to check
		$class = pql_format_table(0);
		if($_REQUEST["mxhost"]) {
		  $url_tmp  = "control_edit_attribute.php?mxhost=".$_REQUEST["mxhost"];
		  $url_tmp .= "&attrib=".pql_get_define("PQL_ATTR_SIMSCAN_ATTACHMENTS");
		} else {
		  $url_tmp  = "domain_edit_attributes.php?rootdn=".$url["rootdn"]."&domain=".$url["domain"]."&view=".$_REQUEST["view"];
		  $url_tmp .= "&attrib=".pql_get_define("PQL_ATTR_SIMSCAN_ATTACHMENTS");
		}

		$tbl_txt = array('tit' => $LANG->_('Attachment checks'),
						 'add' => $LANG->_('Add attachment check for domain '),
						 'del' => $LANG->_('Remove attachment check'));

		if($_REQUEST["mxhost"]) {
		  $tbl_url = array('add' => $url_tmp."&type=add",
						   'del' => $url_tmp."&submit=1&type=delete");
		} else {
		  $tbl_url = array('add' => $url_tmp."&submit=3&action=add",
						   'del' => $url_tmp."&submit=4&action=delete");
		}

		pql_format_table_single($_pql->ldap_linkid, $tbl_txt, $tbl_url, $_REQUEST["simscanattachmentsuffix"],
								pql_get_define("PQL_ATTR_SIMSCAN_ATTACHMENTS"), $class);
?>
    </th>
  </table>
<?php
}

// }}}

// {{{ simscan_print_form(host)
function simscan_print_form($host) {
	global $LANG;
	$attrib = $_REQUEST["attrib"];
?>
  <form action="<?=$_SERVER["PHP_SELF"]?>" method="post">
    <table cellspacing="0" cellpadding="3" border="0">
<?php if($attrib == "simscanspamassassinhits") { ?>
      <th colspan="3" align="left"><?php echo pql_complete_constant($LANG->_('How many SA hits before mail is considered spam on server %mxhost%'), array('mxhost' => $host)); ?></th>
<?php } else { ?>
      <th colspan="3" align="left"><?php echo pql_complete_constant($LANG->_('What attachment suffix to check for virus/spam on server %mxhost%'), array('mxhost' => $host)); ?></th>
<?php } ?>
        <tr class="<?php pql_format_table(); ?>">
<?php if($attrib == "simscanspamassassinhits") { ?>
          <td class="title"><?=$LANG->_('Number of hits')?></td>
<?php } else { ?>
          <td class="title"><?=$LANG->_('Attachment suffix')?></td>
<?php } ?>
          <td>
            <?php echo pql_format_error_span($error[$attrib]); ?>
            <input type="text" name="set" size="10">
          </td>
        </tr>
      </th>
    </table>

    <input type="hidden" name="submit" value="1">
    <input type="hidden" name="attrib" value="<?=$attrib?>">
    <input type="hidden" name="mxhost" value="<?=$host?>">
    <br>
    <input type="submit" value="<?=$LANG->_('Save')?>">
  </form>
<?php
}
// }}}

// {{{ simscan_save(type, host)
function simscan_save($type, $host) {
	global $_pql_control, $LANG;
	echo "simscan_save($type, $host)<br>";

	$attrib = $_REQUEST["attrib"];
	switch($type) {
		case "modify":
		case "add":
			// {{{ Add or Modify an entry

			if($attrib == 'simscanattachmentsuffix')
			  $replace = '';
			else
			  $replace = 1;

			// -------------------------
			// Make sure the QmailLDAP/Controls object contain the objectclass 'phpQLAdminMXHostAddition'.
			$ocs = pql_get_attribute($_pql_control->ldap_linkid, pql_get_define("PQL_ATTR_CN")."=$host,".$_SESSION["USER_SEARCH_DN_CTR"],
									 pql_get_define("PQL_ATTR_OBJECTCLASS"));
			if((!is_array($ocs) and !eregi('phpQLAdminMXHostAddition', $ocs)) or
			   (is_array($ocs) and !in_array('phpQLAdminMXHostAddition', $ocs)))
			  $add_oc = 1;

			if($add_oc) {
			  if(!is_array($ocs))
				$ocs = array($ocs, 'phpQLAdminMXHostAddition');
			  else
				$ocs[] = 'phpQLAdminMXHostAddition';

			  if(!pql_modify_attribute($_pql_control->ldap_linkid,
									  pql_get_define("PQL_ATTR_CN")."=$host,".$_SESSION["USER_SEARCH_DN_CTR"],
									  pql_get_define("PQL_ATTR_OBJECTCLASS"), $replace, $ocs))
			  {
				if(file_exists($_SESSION["path"]."/.DEBUG_ME"))
				  echo "<font color=\"red\">Can't add object class 'phpQLAdminMXHostAddition'</font>";
				else
				  die("Can't add object class 'phpQLAdminMXHostAddition'");
			  }
			}

			// -------------------------
			// Modify attribute
			if(($attrib == "simscanspamassassin") or ($attrib == "simscanclamantivirus") or ($attrib == simscantrophie))
			  // We have a value and it's a toggle - Convert the boolean value to an integer
			  $_REQUEST["set"] = pql_format_bool($_REQUEST["set"]);

			if(pql_modify_attribute($_pql_control->ldap_linkid,
									pql_get_define("PQL_ATTR_CN")."=$host,".$_SESSION["USER_SEARCH_DN_CTR"],
									$attrib, $replace, $_REQUEST["set"]))
			  $msg = pql_complete_constant($LANG->_('Successfully changed %what%'),
										   array('what' => $LANG->_('SimScan option')));
			else
			  $msg = pql_complete_constant($LANG->_('Failed to change %what%'),
										   array('what' => $LANG->_('SimScan option')))
				. ": " . pql_format_error(0);
			break;

			// }}}

		case "delete":
			// {{{ Remove an entry
			$old = pql_get_attribute($_pql_control->ldap_linkid, pql_get_define("PQL_ATTR_CN")."=$host,".$_SESSION["USER_SEARCH_DN_CTR"], $attrib);
			if(!is_array($old))
			  $old = array($old);

			for($i=0; $old[$i]; $i++) {
			  if(lc($old[$i]) != lc($_REQUEST[$attrib]))
				$OLD[] = $old[$i];
			}

			if(pql_modify_attribute($_pql_control->ldap_linkid,	pql_get_define("PQL_ATTR_CN")."=$host,".$_SESSION["USER_SEARCH_DN_CTR"],
									$attrib, $replace, $OLD))
			  $msg = pql_complete_constant($LANG->_('Successfully changed %what%'),
										   array('what' => $LANG->_('SimScan option')));
			else
			  $msg = pql_complete_constant($LANG->_('Failed to change %what%'),
										   array('what' => $LANG->_('SimScan option')))
				. ": " . pql_format_error(0);
			break;
		  // }}}

	  default:
		die(pql_complete_constant($LANG->_('Unknown save type %type% in file %file%, function save()'),
								  array('type' => $type, 'file' => __FILE__)));
	}

	attribute_forward($msg);
}
// }}}

// Local variables:
// mode: php
// mode: font-lock
// tab-width: 4
// End:
?>