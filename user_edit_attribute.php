<?php
// edit an attribute of user
// $Id: user_edit_attribute.php,v 2.42 2004-10-14 07:45:20 turbo Exp $
//
// This file gets iterated through at least 2 times for any attribute (sequenced by "$submit"):
//   1) $submit is unset: Set the default value of the attribute (usually from "$oldvalue")
//      and print out the form.
//   2) $submit is 1 (or 2?): Validate the input.  The name of the input variable changes depending on
//      which attribute is being edited.
//      If the input is valid, save it, else print out the form again and return to step 2.
session_start();
require("./include/pql_config.inc");
require("./include/config_plugins.inc");

$url["domain"] = pql_format_urls($_REQUEST["domain"]);
$url["rootdn"] = pql_format_urls($_REQUEST["rootdn"]);
$url["user"]   = pql_format_urls($_REQUEST["user"]);

require_once("./dlw_porting.inc");
// These variable are "_GET" the first time, and "_POST" the other times.
if (empty($session)) {
  dlw_expect_from(__FILE__, __LINE__, '_REQUEST', array('domain', 'user', 'rootdn', 'oldvalue', 'view', 'attrib', 'PHPSESSID'));
  dlw_expect_from(__FILE__, __LINE__, '_POST', array());
} else {
  dlw_expect_from(__FILE__, __LINE__, '_POST', array('domain', 'user', 'rootdn', 'oldvalue', 'view', 'attrib', 'submit'));
}

$_pql = new pql($_SESSION["USER_HOST"], $_SESSION["USER_DN"], $_SESSION["USER_PASS"]);

if(!$_REQUEST["domain"] && $_REQUEST["user"]) {
    // We're called without branchname - try to reconstruct it
    
    $tmpdn = split(',', $_REQUEST["user"]);
    if($tmpdn[1]) {
	unset($tmpdn[0]);
	$_REQUEST["domain"] = implode(",", $tmpdn);
    } else
      $_REQUEST["domain"] = $tmpdn[count($tmpdn)-1];
}

// Get default domain name for this domain
$defaultdomain = pql_domain_get_value($_pql, $_REQUEST["domain"], pql_get_define("PQL_ATTR_DEFAULTDOMAIN"));

// Get the username. Prettier than the DN
$username = pql_get_attribute($_pql->ldap_linkid, $_REQUEST["user"], pql_get_define("PQL_ATTR_CN"));
if(!$username[0]) {
    // No common name, use uid field
    $username = pql_get_attribute($_pql->ldap_linkid, $_REQUEST["user"], pql_get_define("PQL_ATTR_UID"));
}
$username = $username[0];

// forward back to users detail page (called by attribute_save).
function attribute_forward($msg, $rlnb = false) {
    global $url;

    $link = "user_detail.php?rootdn=" . $url["rootdn"] . "&domain=" . $url["domain"]
      . "&user=" . $url["user"] . "&view=" . $_REQUEST["view"] . "&msg=".urlencode($msg);
    if ($rlnb)
      $link .= "&rlnb=2";

    header("Location: " . pql_get_define("PQL_CONF_URI") . "$link");
}

include("./header.html");

// Select which attribute have to be included
$plugin = pql_plugin_get_filename(pql_plugin_get($_REQUEST["attrib"]));
if(!$plugin) {
    die("<span class=\"error\">ERROR: No plugin file defined for attribute '<i>".$_REQUEST["attrib"]."</i>'</span>");
} elseif(!file_exists("./include/$plugin")) {
    die("<span class=\"error\">ERROR: Plugin file defined for attribute '<i>".$_REQUEST["attrib"]."</i>' does not exists!</span>");
}

include("./include/$plugin");
?>
  <span class="title1"><?php echo pql_complete_constant($LANG->_('Change user data for %user%'), array('user' => $username)); ?></span>
  <br><br>
<?php
// select what to do
// DLW: One of these days this submit==1 stuff has to be replaced with
//      human readable text such as submit="verify".
if(($_REQUEST["submit"] == 1) or ($_REQUEST["submit"] == 2)) {
    if(attribute_check("modify")){
	attribute_save("modify");
    } else {
	attribute_print_form();
    }
} elseif($_REQUEST["submit"] == 4) {
    // SAVE change directly, no need for a form
    attribute_save($action);
} elseif(($_REQUEST["submit"] == 3) and (($_REQUEST["attrib"] == 'dnmember') or
					 ($_REQUEST["attrib"] == 'dnmoderator') or
					 ($_REQUEST["attrib"] == 'dnsender'))) {
    attribute_print_form($_REQUEST["action"]);
} else {
    if(($_REQUEST["attrib"] == pql_get_define("PQL_ATTR_GROUP_CONFIRM")) or
       ($_REQUEST["attrib"] == pql_get_define("PQL_ATTR_GROUP_MEMBERS_ONLY")) or
       ($_REQUEST["attrib"] == pql_get_define("PQL_ATTR_START_ADVANCED"))) {
	// It's one of those user toggles - go save!
	attribute_save($action);
    } else {
	attribute_init();
	attribute_print_form();
    }
}
?>
</body>
</html>
