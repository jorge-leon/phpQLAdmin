<?php
// Edit and set configuration values in the LDAP database
// $Id: config_edit_attribute.php,v 1.12 2004-11-05 10:42:26 turbo Exp $
//
session_start();

require("./include/pql_config.inc");
$_pql = new pql($_SESSION["USER_HOST"], $_SESSION["USER_DN"], $_SESSION["USER_PASS"]);

include("./include/attrib.config.inc");
include("./header.html");

// forward back to configuration detail page
function attribute_forward($msg, $rlnb = false) {
    $attrib = $_REQUEST["attrib"];

    $msg = urlencode($msg);
	if(lc($attrib) == 'controlsadministrator') {
		if($_REQUEST[$attrib])
		  $userdn = urlencode($_REQUEST[$attrib]);
		elseif($_REQUEST[$attrib])
		  $userdn = urlencode($_REQUEST[$attrib]);

		$url = "user_detail.php?rootdn=" . $_REQUEST["rootdn"]
		  . "&domain=" . $_REQUEST["domain"] . "&user=$userdn&view=" . $_REQUEST["view"] . "&msg=$msg";
	} else {
		if($_REQUEST["rootdn"])
		  $url = "config_detail.php?branch=" . $_REQUEST["rootdn"] . "&view=branch&msg=$msg";
		else
		  $url = "config_detail.php?msg=$msg";
	}

	if($rlnb and pql_get_define("PQL_CONF_AUTO_RELOAD"))
	  $url .= "&rlnb=1";

    header("Location: " . pql_get_define("PQL_CONF_URI") . "$url");
}

// select what to do
if(@$_REQUEST["submit"] == 1) {
    if(attribute_check()) {
		attribute_save();
    } else {
		attribute_print_form();
    }
} elseif(@$_REQUEST["submit"] == 2) {
	attribute_save();
} elseif(!empty($_REQUEST[$attrib]) or !empty($_REQUEST["toggle"])) {
	attribute_save();
} else {
    attribute_print_form();
}

/*
 * Local variables:
 * mode: php
 * mode: font-lock
 * tab-width: 4
 * End:
 */
?>
</body>
</html>
