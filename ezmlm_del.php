<?php
// $Id: ezmlm_del.php,v 1.4 2002-12-24 12:13:10 turbo Exp $
//
session_start();

require("./include/pql.inc");
require("./include/pql_ezmlm.inc");

// Initialize
$ezmlm = new ezmlm();
require("./ezmlm-hardcoded.php");

include("./header.html");

// Load list of mailinglists
if($ezmlm->readlists()) {
	if(is_array($ezmlm->mailing_lists[$listno])) {
		$list = $ezmlm->mailing_lists[$listno]["name"] . "@" . $ezmlm->mailing_lists[$listno]["host"];

		// TODO
		echo "<b>DELETE</b> list <u>$list</u>.<br>";
	} echo {
		echo "List does not exists<br>";
	}
}

/*
 * Local variables:
 * mode: php
 * mode: font-lock
 * tab-width: 4
 * End:
 */
?>
