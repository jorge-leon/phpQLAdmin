<?php
// delete a domain and all users within
// domain_del.php,v 1.3 2002/12/12 21:52:08 turbo Exp
//
session_start();
require("./include/pql_config.inc");
require("./include/pql_control.inc");

include("./header.html");
?>
  <span class="title1"><?php echo pql_complete_constant($LANG->_('Remove domain %domain%'),
														array("domain" => maybe_decode($domain)))?></span>
<?php
if(isset($ok) || !pql_get_define("PQL_CONF_VERIFY_DELETE", $rootdn)) {
	$_pql = new pql($USER_HOST, $USER_DN, $USER_PASS);
	$_pql_control = new pql_control($USER_HOST, $USER_DN, $USER_PASS);
	
	$delete_forwards = (isset($delete_forwards) || pql_get_define("PQL_CONF_VERIFY_DELETE", $rootdn)) ? true : false;
	
	// Make sure we can have a ' in branch
	$domain = eregi_replace("\\\'", "'", $domain);

	// Before we delete the domain/branch, we need to get the defaultDomain, additionalDomainName
	// and smtpRoutes value(s) so that we can remove it from the QmailLDAP/Controls object(s)
	$domainname  = pql_get_domain_value($_pql, $domain, pql_get_define("PQL_GLOB_ATTR_DEFAULTDOMAIN"));
	$additionals = pql_get_domain_value($_pql, $domain, pql_get_define("PQL_GLOB_ATTR_ADDITIONALDOMAINNAME"));
	$routes		 = pql_get_domain_value($_pql, $domain, pql_get_define("PQL_GLOB_ATTR_SMTPROUTES"));

	// delete the domain
	if(pql_domain_del($_pql, $domain, $delete_forwards)) {
	    // Deletion of the branch was successfull - Update the QmailLDAP/Controls object(s)

		// Remove the domain name
		if($domainname)
		  pql_control_update_domains($_pql, $USER_SEARCH_DN_CTR, '*', array($domainname, ''));

		// Remove the additional domain name(s)
		if(is_array($additionals)) {
			foreach($additionals as $additional)
			  pql_control_update_domains($_pql, $USER_SEARCH_DN_CTR, '*', array($additional, ''));
		}
	    
		// Remove the SMTP route(s)
		if(is_array($routes)) {
			foreach($routes as $route)
			  pql_control_update_domains($_pql, $USER_SEARCH_DN_CTR, '*', array($route, ''));
		}
	    
	    $msg = $LANG->_('Successfully removed the domain');
		
	    // redirect to home page
	    $msg = urlencode($msg);
	    header("Location: " . pql_get_define("PQL_GLOB_URI") . "home.php?msg=$msg&rlnb=1");
	} else {
	    $msg = $LANG->_('Failed to remove the domain') . ":&nbsp;" . ldap_error($_pql->ldap_linkid);
		
	    // redirect to domain detail page
	    $msg = urlencode($msg);
	    header("Location: " . pql_get_define("PQL_GLOB_URI") . "domain_detail.php?domain=$domain&msg=$msg");
	}
} else {
?>
<br>
<br>
<img src="images/info.png" width="16" height="16" border="0">
<?php echo $LANG->_('Attention: If you deleted a domain, all users within this domain will be deleted too'); ?>!
<br>
<br>
<?php echo $LANG->_('Are you really sure'); ?>
<br>
<form action="<?php echo $PHP_SELF; ?>" method="GET">
	<input type="hidden" name="domain" value="<?php echo $domain; ?>">
	
	<input type="checkbox" name="delete_forwards" checked> <?php echo $LANG->_('Also delete forwards to users in this domain'); ?><br><br>
	<input type="submit" name="ok" value="<?php echo $LANG->_('Yes'); ?>">
	<input type="button" name="back" value="<?php echo $LANG->_('No'); ?>" onClick="history.back();">
</form>
<br>
<?php
}
?>
</body>
</html>

<?php
/*
 * Local variables:
 * mode: php
 * mode: font-lock
 * tab-width: 4
 * End:
 */
?>
