<?php
// adds an attribute 
// $Id: user_add_attribute.php,v 1.1 2002-12-11 15:09:23 turbo Exp $
//
require("pql.inc");
$_pql = new pql();

// forward back to users detail page
function attribute_forward($msg){
	global $domain, $user;

	$msg = urlencode($msg);
	$url = "user_detail.php?domain=$domain&user=" . urlencode($user) . "&msg=$msg";
    header("Location: " . PQL_URI . "$url");
}

// Get default domain name for this domain
$defaultdomain = pql_get_domain_value($_pql->ldap_linkid, $domain, "defaultdomain");
 
// select which attribute have to be included
switch($attrib){
	case "mailalternateaddress":
		$include = "attrib.mailalternateaddress.inc";
		break;
	case "mailforwardingaddress":
		$include = "attrib.mailforwardingaddress.inc";
		break;
    default:
		die("unknown attribute");
}

include($include);
?>

<html>
<head>
	<title>phpQL</title>
	<link rel="stylesheet" href="normal.css" type="text/css">
</head>

<body bgcolor="#e7e7e7" background="images/bkg.png">
<span class="title1"><?php echo PQL_USER_EDIT; ?></span>
<br>
<br>
<?php
	// select what to do
	if($submit == 1){
		if(attribute_check("add")){
			attribute_save("add");
		} else {
			attribute_print_form();
   		}
  	} else {
		attribute_print_form();
  	}
?>
</body>
</html>
