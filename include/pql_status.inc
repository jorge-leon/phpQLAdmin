<?php
// phpQLAdmin Application Programming Interface (API)
//
// Retreive LDAP connection and database status
// from the cn=Monitor backend.
//
// $Id: pql_status.inc,v 1.1 2004-01-26 05:28:04 turbo Exp $
//

// {{{ pql_get_status(linkid, dn, attribute)
function pql_get_status($linkid, $dn, $attrib) {
    $attrib = lc($attrib);

    $sr = @ldap_read($linkid, $dn, 'objectClass=*', array($attrib));
    if($sr) {
		$entry = ldap_get_entries($linkid, $sr);
		
		if($entry[0][$attrib]['count'] <= 1)
		  return($entry[0][$attrib][0]);
		else {
			// Return an array - we got more than one value
			for($i=0; $entry[0][$attrib][$i]; $i++)
			  $values[] = $entry[0][$attrib][$i];
			
			return($values);
		}
    }

	return(NULL);
}
// }}}

// {{{ pql_get_subentries(linkid, basedn, attribute, filter)
// If called without attrib and filter -> just return number
// of entries found.
function pql_get_subentries($linkid, $basedn, $attrib = 0, $filter = 0) {
	if(!$attrib and !$filter) {
		$sr = @ldap_list($linkid, $basedn, 'objectClass=*');
		if($sr) {
			$entry = ldap_get_entries($linkid, $sr);
			
			if($entry['count'])
			  return($entry['count']);
		}
	} else {
		$sr = @ldap_list($linkid, $basedn, $filter, array($attrib));
		if($sr) {
			$entry = ldap_get_entries($linkid, $sr);

			for($i=0; $i < $entry['count']; $i++) {
				$values[] = $entry[$i][$attrib][0];
			}

			return($values);
		}
	}

	return(NULL);
}
// }}}

// {{{ pql_get_status_databases(linkid, suffix)
function pql_get_status_databases($linkid, $suffix = 0) {
	$filter  = '(&(cn=Database*)(namingContexts=*))';
	$attribs = array('description', 'namingContexts', 'seeAlso');

	$sr1 = ldap_list($linkid, 'cn=Databases,cn=Monitor', $filter, $attribs);
	if($sr1) {
		$entry1 = ldap_get_entries($linkid, $sr1);

		for($i=0; $i < $entry1['count']; $i++) {
			foreach($attribs as $attrib) {
				$attrib = lc($attrib);

				if(($attrib == 'seealso') and $entry1[$i]['seealso'][0]) {
					// Follow reference to backend information
					$sr2 = ldap_read($linkid, $entry1[$i]['seealso'][0],
									 'objectClass=*', array('supportedControl'));
					if($sr2) {
						$entry2 = ldap_get_entries($linkid, $sr2);

						for($j=0; $j < $entry2[0]['supportedcontrol']['count']; $j++) {
							$values[$entry1[$i]['dn']]['supportedcontrol'][] = $entry2[0]['supportedcontrol'][$j];
						}
					}
				} else
				  $values[$entry1[$i]['dn']][$attrib] = $entry1[$i][$attrib][0];
			}
		}

		return($values);
	}

	return(NULL);
}
// }}}

/*
 * Local variables:
 * mode: php
 * mode: font-lock
 * tab-width: 4
 * End:
 */
?>