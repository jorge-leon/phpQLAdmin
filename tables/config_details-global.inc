  <table cellspacing="0" cellpadding="3" border="0">
    <th colspan="3" align="left">Global configuration
      <tr>
        <td class="title">LDAP host</td>
<?php $class=table_bgcolor(0); ?>
        <td class="<?=$class?>"></td>
        <td class="<?=$class?>"><?=$USER_HOST?>&nbsp;</td>
      </tr>

      <tr>
        <td class="title">Language</td>
<?php $class=table_bgcolor(0);
      if($ALLOW_GLOBAL_CONFIG_SAVE and 0) {
	  // TODO: Sorry, phpQLAdmin isn't translated propperly to anything else than English!
?>
        <td class="<?=$class?>">
          <a href="config_edit_attribute.php?attrib=<?=$PQL_ATTRIBUTE["PQL_CONF_LANG"]?>">
            <img src="images/edit.png" width="12" height="12" border="0" alt="Edit attrib <?=$PQL_ATTRIBUTE["PQL_CONF_LANG"]?>">
          </a>
        </td>
<?php } else { ?>
        <td class="<?=$class?>"></td>
<?php } ?>
        <td class="<?=$class?>"><?=PQL_LANG_LANG?>&nbsp;</td>
      </tr>

      <tr>
        <td class="title">Who are we</td>
<?php $class=table_bgcolor(0); ?>
        <td class="<?=$class?>"><a href="config_edit_attribute.php?attrib=<?=$PQL_ATTRIBUTE["PQL_GLOB_WHOAREWE"]?>"><img src="images/edit.png" width="12" height="12" border="0" alt="Edit attrib <?=$PQL_ATTRIBUTE["PQL_GLOB_WHOAREWE"]?>"></a></td>
        <td class="<?=$class?>"><?=$config["PQL_GLOB_WHOAREWE"]?>&nbsp;</td>
      </tr>

      <tr>
        <td class="title">Hostmaster</td>
<?php $class=table_bgcolor(0);
      if($ALLOW_GLOBAL_CONFIG_SAVE) {
?>
        <td class="<?=$class?>"><a href="config_edit_attribute.php?attrib=<?=$PQL_ATTRIBUTE["PQL_GLOB_HOSTMASTER"]?>"><img src="images/edit.png" width="12" height="12" border="0" alt="Edit attrib <?=$PQL_ATTRIBUTE["PQL_GLOB_HOSTMASTER"]?>"></a></td>
<?php } else { ?>
        <td class="<?=$class?>"></td>
<?php } ?>
        <td class="<?=$class?>"><?=$config["PQL_GLOB_HOSTMASTER"]?>&nbsp;</td>
      </tr>

<?php if($config["PQL_GLOB_CONTROL_USE"]) { ?>
      <tr>
        <td class="title">LDAP control base dn</td>
<?php     $class=table_bgcolor(0);
          if($ALLOW_GLOBAL_CONFIG_SAVE) {
?>
        <td class="<?=$class?>"><a href="config_edit_attribute.php?attrib=<?=$PQL_ATTRIBUTE["PQL_GLOB_CONTROL_DN"]?>"><img src="images/edit.png" width="12" height="12" border="0" alt="Set/Modify <?=$PQL_ATTRIBUTE["PQL_GLOB_CONTROL_DN"]?>"></a></td>
<?php     } else { ?>
        <td class="<?=$class?>"></td>
<?php     } ?>
        <td class="<?=$class?>"><?=$config["PQL_GLOB_CONTROL_DN"]?></td>
      </tr>
<?php } ?>

      <tr></tr>

      <tr>
        <td class="title">Manage Controls DB</td>
<?php $class=table_bgcolor(0);
      if($ALLOW_GLOBAL_CONFIG_SAVE) {
?>
        <td class="<?=$class?>"><a href="config_edit_attribute.php?toggle=1&attrib=<?=$PQL_ATTRIBUTE["PQL_GLOB_CONTROL_USE"]?>"><img src="images/edit.png" width="12" height="12" border="0" alt="Toggle <?=$PQL_ATTRIBUTE["PQL_GLOB_CONTROL_USE"]?>"></a></td>
<?php } else { ?>
        <td class="<?=$class?>"></td>
<?php } ?>
        <td class="<?=$class?>"><?php if($config["PQL_GLOB_CONTROL_USE"]) {echo 'Yes';} else {echo 'No';}?>&nbsp;</td>
      </tr>

      <tr>
        <td class="title">Manage EZMLM mailinglists</td>
<?php $class=table_bgcolor(0);
      if($ALLOW_GLOBAL_CONFIG_SAVE) {
?>
        <td class="<?=$class?>"><a href="config_edit_attribute.php?toggle=1&attrib=<?=$PQL_ATTRIBUTE["PQL_GLOB_EZMLM_USE"]?>"><img src="images/edit.png" width="12" height="12" border="0" alt="Toggle <?=$PQL_ATTRIBUTE["PQL_GLOB_EZMLM_USE"]?>"></a></td>
<?php } else { ?>
        <td class="<?=$class?>"></td>
<?php } ?>
        <td class="<?=$class?>"><?php if($config["PQL_GLOB_EZMLM_USE"]) {echo 'Yes';}else{echo 'No';}?>&nbsp;</td>
      </tr>

<?php if($ADVANCED_MODE and 0) { // TODO: Currently disabled. Not even started. ?>
      <tr>
        <td class="title">Manage Bind 9 DNSSEC domains</td>
<?php $class=table_bgcolor(0);
      if($ALLOW_GLOBAL_CONFIG_SAVE) {
?>
        <td class="<?=$class?>"><a href="config_edit_attribute.php?toggle=1&attrib=<?=$PQL_ATTRIBUTE["PQL_GLOB_BIND9_USE"]?>"><img src="images/edit.png" width="12" height="12" border="0" alt="Toggle <?=$PQL_ATTRIBUTE["PQL_GLOB_BIND9_USE"]?>"></a></td>
<?php } else { ?>
        <td class="<?=$class?>"></td>
<?php } ?>
        <td class="<?=$class?>"><?php if($config["PQL_GLOB_BIND9_USE"]) {echo 'Yes';}else{echo 'No';}?>&nbsp;</td>
      </tr>
<?php } ?>

      <tr>
        <td class="title">Automatic reload of navigation bar</td>
<?php $class=table_bgcolor(0);
      if($ALLOW_GLOBAL_CONFIG_SAVE) {
?>
        <td class="<?=$class?>"><a href="config_edit_attribute.php?toggle=1&attrib=<?=$PQL_ATTRIBUTE["PQL_GLOB_AUTO_RELOAD"]?>"><img src="images/edit.png" width="12" height="12" border="0" alt="Toggle <?=$PQL_ATTRIBUTE["PQL_GLOB_AUTO_RELOAD"]?>"></a></td>
<?php } else { ?>
        <td class="<?=$class?>"></td>
<?php } ?>
        <td class="<?=$class?>"><?php if($config["PQL_GLOB_AUTO_RELOAD"]) {echo 'Yes';}else{echo 'No';}?>&nbsp;</td>
      </tr>

      <tr>
        <td class="title">Automatically replicate domains to locals</td>
<?php $class=table_bgcolor(0);
      if($ALLOW_GLOBAL_CONFIG_SAVE) {
?>
        <td class="<?=$class?>"><a href="config_edit_attribute.php?toggle=1&attrib=<?=$PQL_ATTRIBUTE["PQL_GLOB_CONTROL_AUTOADDLOCALS"]?>"><img src="images/edit.png" width="12" height="12" border="0" alt="Toggle <?=$PQL_ATTRIBUTE["PQL_CONF_CONTROL_AUTOADDLOCALS"]?>"></a></td>
<?php } else { ?>
        <td class="<?=$class?>"></td>
<?php } ?>
        <td class="<?=$class?>"><?php if($config["PQL_GLOB_CONTROL_AUTOADDLOCALS"]) {echo 'Yes';}else{echo 'No';}?>&nbsp;</td>
      </tr>

      <tr>
        <td class="title">Automatically replicate domains to rcpthosts</td>
<?php $class=table_bgcolor(0);
      if($ALLOW_GLOBAL_CONFIG_SAVE) {
?>
        <td class="<?=$class?>"><a href="config_edit_attribute.php?toggle=1&attrib=<?=$PQL_ATTRIBUTE["PQL_GLOB_CONTROL_AUTOADDRCPTHOSTS"]?>"><img src="images/edit.png" width="12" height="12" border="0" alt="Toggle <?=$PQL_ATTRIBUTE["PQL_CONF_CONTROL_AUTOADDRCPTHOSTS"]?>"></a></td>
<?php } else { ?>
        <td class="<?=$class?>"></td>
<?php } ?>
        <td class="<?=$class?>"><?php if($config["PQL_GLOB_CONTROL_AUTOADDRCPTHOSTS"]) {echo 'Yes';}else{echo 'No';}?>&nbsp;</td>
      </tr>

      <tr>
        <td class="title">Automatically add hostname to domain for replication</td>
<?php $class=table_bgcolor(0);
      if($ALLOW_GLOBAL_CONFIG_SAVE) {
?>
        <td class="<?=$class?>"><a href="config_edit_attribute.php?toggle=1&attrib=<?=$PQL_ATTRIBUTE["PQL_GLOB_CONTROL_AUTOADDHOSTNAME"]?>"><img src="images/edit.png" width="12" height="12" border="0" alt="Toggle <?=$PQL_ATTRIBUTE["PQL_CONF_CONTROL_AUTOADDHOSTNAME"]?>"></a></td>
<?php } else { ?>
        <td class="<?=$class?>"></td>
<?php } ?>
        <td class="<?=$class?>"><?php if($config["PQL_GLOB_CONTROL_AUTOADDHOSTNAME"]) {echo 'Yes';}else{echo 'No';}?>&nbsp;</td>
      </tr>

      <tr>
        <td class="title">Allow change of LDAP server</td>
<?php $class=table_bgcolor(0);
      if($ALLOW_GLOBAL_CONFIG_SAVE) {
?>
        <td class="<?=$class?>"><a href="config_edit_attribute.php?toggle=1&attrib=<?=$PQL_ATTRIBUTE["PQL_GLOB_CHANGE_SERVER"]?>"><img src="images/edit.png" width="12" height="12" border="0" alt="Toggle <?=$PQL_ATTRIBUTE["PQL_CONF_CHANGE_SERVER"]?>"></a></td>
<?php } else { ?>
        <td class="<?=$class?>"></td>
<?php } ?>
        <td class="<?=$class?>"><?php if($config["PQL_GLOB_CHANGE_SERVER"]) {echo 'Yes';}else{echo 'No';}?>&nbsp;</td>
      </tr>
<?php if($show_kerberos_info) { ?>

      <tr></tr>

      <!-- -------------------- --!>
      <tr>
        <td class="title">Kerberos realm</td>
        <td class="<?=$class?>"><a href="config_edit_attribute.php?attrib=<?=$PQL_ATTRIBUTE["PQL_GLOB_KRB5_REALM"]?>"><img src="images/edit.png" width="12" height="12" border="0" alt="Edit attrib <?=$PQL_ATTRIBUTE["PQL_GLOB_KRB5_REALM"]?>"></a></td>
        <td class="<?=$class?>"><?=$config["PQL_GLOB_KRB5_REALM"]?>&nbsp;</td>
      </tr>

      <tr>
        <td class="title">Kerberos admin server</td>
        <td class="<?=$class?>"><a href="config_edit_attribute.php?attrib=<?=$PQL_ATTRIBUTE["PQL_GLOB_KRB5_ADMIN_SERVER"]?>"><img src="images/edit.png" width="12" height="12" border="0" alt="Edit attrib <?=$PQL_ATTRIBUTE["PQL_GLOB_KRB5_ADMIN_SERVER"]?>"></a></td>
        <td class="<?=$class?>"><?=$config["PQL_GLOB_KRB5_ADMIN_SERVER"]?>&nbsp;</td>
      </tr>

      <tr>
        <td class="title">Kerberos admin principal</td>
        <td class="<?=$class?>"><a href="config_edit_attribute.php?attrib=<?=$PQL_ATTRIBUTE["PQL_GLOB_KRB5_ADMIN_PRINCIPAL"]?>"><img src="images/edit.png" width="12" height="12" border="0" alt="Edit attrib <?=$PQL_ATTRIBUTE["PQL_GLOB_KRB5_ADMIN_PRINCIPAL"]?>"></a></td>
        <td class="<?=$class?>"><?=$config["PQL_GLOB_KRB5_ADMIN_PRINCIPAL"]?>&nbsp;</td>
      </tr>

      <tr>
        <td class="title">Kerberos admin keytab</td>
        <td class="<?=$class?>"><a href="config_edit_attribute.php?attrib=<?=$PQL_ATTRIBUTE["PQL_GLOB_KRB5_ADMIN_KEYTAB"]?>"><img src="images/edit.png" width="12" height="12" border="0" alt="Edit attrib <?=$PQL_ATTRIBUTE["PQL_GLOB_KRB5_ADMIN_KEYTAB"]?>"></a></td>
        <td class="<?=$class?>"><?=$config["PQL_GLOB_KRB5_ADMIN_KEYTAB"]?>&nbsp;</td>
      </tr>

      <tr>
        <td class="title">Path to Kerberos <u>kadmin</u> command</td>
        <td class="<?=$class?>"><a href="config_edit_attribute.php?attrib=<?=$PQL_ATTRIBUTE["PQL_GLOB_KRB5_ADMIN_COMMAND_PATH"]?>"><img src="images/edit.png" width="12" height="12" border="0" alt="Edit attrib <?=$PQL_ATTRIBUTE["PQL_GLOB_KRB5_ADMIN_COMMAND_PATH"]?>"></a></td>
        <td class="<?=$class?>"><?=$config["PQL_GLOB_KRB5_ADMIN_COMMAND_PATH"]?>&nbsp;</td>
      </tr>
<?php } ?>
    </th>
  </table>

  <br><br>

<?php
/*
 * Local variables:
 * mode: php
 * mode: font-lock
 * tab-width: 4
 * End:
 */
?>