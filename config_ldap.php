<?php
// shows configuration of phpQLAdmin
// config_detail.php,v 1.3 2002/12/12 21:52:08 turbo Exp
//
session_start();
require("./include/pql_config.inc");

require("./left-head.html");
include("./header.html");

$_pql = new pql($USER_HOST, $USER_DN, $USER_PASS);

$ldap = pql_get_subschemas($_pql->ldap_linkid);
$j = 0;
?>
    <table cellspacing="0" cellpadding="3" border="0">
<?php
foreach($ldap as $x => $array) {
?>
      <th colspan="3" align="left"><?=$x?>
<?php
	if(!eregi(lc($x), 'objectclasses')) {
?>

        <tr>
          <td><?=$LANG->_('Name')?></td>
          <td><?=$LANG->_('Oid')?></td>
          <td><?=$LANG->_('Single valued')?></td>
          <td><?=$LANG->_('Description')?></td>
        </tr>

<?php    foreach($array as $value) { ?>
        <tr class="<?php table_bgcolor(); ?>">
<?php       echo "          <td>".$value['NAME']."</td>\n";
			echo "          <td>".$value['OID']."</td>\n";
			echo "          <td>".$value['SINGLE-VALUE']."</td>\n";
			echo "          <td>".$value['DESC']."</td>\n";
?>
        </tr>
<?php    } ?>
      </th>

<?php
	} else {
		// Objectclasses
		$class = table_bgcolor(0);
?>
        <br>
        <font size="1"><?=$LANG->_('Text in bold is a MUST, and non-bold is MAY')?>.</font>
        <br>
<?php	foreach($array as $value) { ?>
        <tr class="<?=$class?>">
          <td>
            <div id="el<?=$j?>Parent" class="parent">
              <a class="item" onClick="if (capable) {expandBase('el<?=$j?>', true); expandBase('el<?=$j+1?>', true); return false;}">
                <img name="imEx" src="images/plus.png" border="0" alt="+" width="9" height="9" id="el<?=$j?>Img">
              </a>
        
              <a class="item"><font color="black" class="heada"><?=$value['NAME']?></font></a>
            </div>

            <div id="el<?=$j?>Child" class="child">
<?php		for($i=0; $i < $value['MUST']['count']; $i++) { // MUST attributes ?>
              &nbsp;&nbsp;&nbsp;&nbsp;<b><?=$value['MUST'][$i]?></b><br>
<?php		} 

			for($i=0; $i < $value['MAY']['count']; $i++) {  // MAY attributes
?>
              &nbsp;&nbsp;&nbsp;&nbsp;<?=$value['MAY'][$i]?><br>
<?php		} ?>
            </div>
          </td>
<?php		$j++; ?>
          <td>
            <div id="el<?=$j?>Parent" class="parent">
              <a class="item" onClick="if (capable) {expandBase('el<?=$j-1?>', true); expandBase('el<?=$j?>', true); return false;}">
                <img name="imEx" src="images/plus.png" border="0" alt="+" width="9" height="9" id="el<?=$j?>Img">
              </a>
              <a class="item"><font color="black" class="heada"><?=$value['OID']?></font></a>
            </div>
            <div id="el<?=$j?>Child" class="child">
<?php		for($i=0; $i < $value['MUST']['count']; $i++) { // MUST attributes ?>
              &nbsp;&nbsp;&nbsp;&nbsp;<b><?php
				if($ldap['attributetypes'][lc($value['MUST'][$i])]['OID']) {
					echo $ldap['attributetypes'][lc($value['MUST'][$i])]['OID'];
				} else {
					// It's an alias
					foreach($ldap['attributetypes'] as $attrib) {
						if($attrib['ALIAS'][0])
						  if(lc($value['MUST'][$i]) == lc($attrib['ALIAS'][0]))
							echo $attrib['OID'];
					}
				} ?></b><br>

<?php       }

            for($i=0; $i < $value['MAY']['count']; $i++) { // MAY attributes ?>
              &nbsp;&nbsp;&nbsp;&nbsp;<?php
				if($ldap['attributetypes'][lc($value['MAY'][$i])]['OID']) {
					echo $ldap['attributetypes'][lc($value['MAY'][$i])]['OID'];
				} else {
					// It's an alias
					foreach($ldap['attributetypes'] as $attrib) {
						if($attrib['ALIAS'][0])
						  if(lc($value['MAY'][$i]) == lc($attrib['ALIAS'][0]))
							echo $attrib['OID'];
					}
				}?><br>

<?php       } ?>
            </div>
          </td>
          <td></td>
          <td><?=$value['DESC']?></td>
        </tr>

<?php	$class = table_bgcolor(0); $j++;
        }
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
    </table>
<?php require("./left-trailer.html"); ?>
