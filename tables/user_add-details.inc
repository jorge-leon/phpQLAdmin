<?php /* $Id: user_add-details.inc,v 1.49 2008-03-18 23:11:29 turbo Exp $ */ ?>
  <form action="<?php echo $_SERVER["PHP_SELF"]?>" method="post" accept-charset="UTF-8" name="user_add">
    <table cellspacing="0" cellpadding="3" border="0">
      <th colspan="3" align="left"><?php echo $LANG->_('User data')?></th>
<?php
	// {{{ Does KERBEROS exists in $schemes
	$krb_scheme = 0;
	if($schemes and in_array('KERBEROS', $schemes)) {
	  $krb_scheme = 1;
	}
// }}}

	// {{{ Username
	if($mark = pql_templates_check_attribute($template_attribs, 'uid')) {
	  if((!pql_get_define("PQL_CONF_AUTOCREATE_USERNAME", $_REQUEST["rootdn"]) and $_SESSION["ADVANCED_MODE"]) or
		 empty($_REQUEST["uid"]))
	  {
?>
        <!-- Username (UID) -->
        <tr class="<?php pql_format_table(); ?>">
          <td class="title"><?php echo $LANG->_('Username')?></td>
          <td>
            <?php if(!empty($error_text["uid"])) { echo pql_format_error_span($error_text["uid"]); } ?>
<?php		if($lockusername) { ?>
            <input type="hidden" name="uid" value="<?php echo $_REQUEST["uid"]?>">
            &nbsp;&nbsp;&nbsp;<b><?php echo $_REQUEST["uid"]?></b>
<?php		} else { ?>
            <?php echo pql_templates_mark_importance($mark); ?>
            <input type="text" name="uid" value="<?php echo $_REQUEST["uid"]?>">
<?php		} ?>
          </td>
        </tr>

<?php		if(!$lockusername) { ?>
        <tr class="<?php pql_format_table(); ?>">
          <td><img src="images/info.png" width="16" height="16" alt="" border="0" align="right"></td>
          <td><?php echo $LANG->_('Numbers, letters and the following special chars: @, %, . (dot), _, -.\nIf left out, a username will be created automatically')?>.</td>
        </tr>

<?php		}
		} else { ?>
        <tr class="<?php pql_format_table(); ?>">
          <td><img src="images/info.png" width="16" height="16" alt="" border="0" align="right"></td>
          <td><?php echo pql_complete_constant($LANG->_('Automatically generated %what%'), array('what' => $LANG->_('username'))); ?> => <b><?php echo $_REQUEST["uid"]?></b>.</td>
        </tr>
        <input type="hidden" name="uid" value="<?php echo $_REQUEST["uid"]?>">

<?php 	}
	  }
// }}}

	// {{{ Firstname
	if($mark = pql_templates_check_attribute($template_attribs, 'sn')) {
?>
        <!-- Firstname -->
        <tr class="<?php pql_format_table(); ?>">
          <td class="title"><?php echo $LANG->_('Surname')?></td>
          <td>
            <?php if(!empty($error_text["surname"])) { echo pql_format_error_span($error_text["surname"]); }?>
            <?php echo pql_templates_mark_importance($mark); ?>
            <input type="text" name="surname" value="<?php if(!empty($_REQUEST["surname"])) { echo $_REQUEST["surname"]; }?>">
          </td>
        </tr>
<?php }
// }}}

	// {{{ Lastname
	if($mark = pql_templates_check_attribute($template_attribs, 'cn')) {
?>

        <!-- Lastname -->
        <tr class="<?php pql_format_table(); ?>">
          <td class="title"><?php echo $LANG->_('Given name')?></td>
          <td>
            <?php if(!empty($error_text["name"])) { echo pql_format_error_span($error_text["name"]); }?>
            <?php echo pql_templates_mark_importance($mark); ?>
            <input type="text" name="name" value="<?php if(!empty($_REQUEST["name"])) { echo $_REQUEST["name"]; }?>">
          </td>
        </tr>
<?php }
// }}}

	if($mark = pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_PASSWD"))) {
	  // {{{ Password encryption scheme
	  if((!$_REQUEST["defaultpasswordscheme"] or $_SESSION["ADVANCED_MODE"]) and !$lockpassword and 
		 pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_PASSWD")))
	  {
		// We have NO default password scheme (or we're running in advanced mode) and password is allowed.
		// -> Allow password to be entered...
?>

        <!-- Password scheme -->
        <tr class="<?php pql_format_table(); ?>">
          <td class="title"><?php echo $LANG->_('Password encryption scheme')?></td>
          <td>
            <?php echo pql_templates_mark_importance($mark); ?>

<?php	if(!empty($schemes[1])) {
			// We got more than one password scheme...
	  
			// Show each of the schemes as radio buttons
			foreach($schemes as $scheme) {
			  if(!(($_REQUEST["template"] == "mail") and ($scheme == "KERBEROS"))) {
				// Do not show the KERBEROS scheme if it's a mail account! We don't have
				// uidNumber and/or gidNumber for that account type, so we can't mapp the
				// user correctly if using KERBEROS!
				if(!$_REQUEST["pwscheme"]) 
				  $_REQUEST["pwscheme"] = $scheme;
?>
            <input type="radio" name="pwscheme" value="<?php echo $scheme?>"<?php
			   if((!$_REQUEST["pwscheme"] and ($scheme == $_REQUEST["defaultpasswordscheme"]))
				  or ($_REQUEST["pwscheme"] and ($scheme == $_REQUEST["pwscheme"])))
			   {
				 // 1. We don't have a password scheme (pre)choosen
				 //    a. this is the default password scheme from branch
				 // 2. We HAVE password scheme (pre)choosen
				 //    b. this is the (pre)choosen password scheme
				 echo " CHECKED";
			   }
?> onChange="document.user_add.dosave.value='yes'; submit()"><?php echo $scheme?>
																																																	
<?php		  }
			}
		} elseif($_REQUEST["defaultpasswordscheme"]) {
		  if(!@$_REQUEST["pwscheme"])
			$_REQUEST["pwscheme"] = $schemes[0];
		  else {
			$schemes[0] = preg_replace('/\{/', '', $_REQUEST["pwscheme"]);
			$schemes[0] = preg_replace('/\}/', '', $schemes[0]);
		  }
?>
            <input type="radio" name="pwscheme" value="<?php echo $schemes[0]?>"><?php echo $schemes[0]?>
            <input type="radio" name="pwscheme" value="<?php echo $_REQUEST["defaultpasswordscheme"]?>" CHECKED><?php echo $_REQUEST["defaultpasswordscheme"]?>
<?php
		} else {
		  if(!@$_REQUEST["pwscheme"])
			$_REQUEST["pwscheme"] = $schemes[0];
		  else {
			$schemes[0] = preg_replace('/\{/', '', $_REQUEST["pwscheme"]);
			$schemes[0] = preg_replace('/\}/', '', $schemes[0]);
		  }
?>
            Scheme: <b>{<?php echo $schemes[0]?>}</b>
            <input type="hidden" name="pwscheme" value="<?php echo $schemes[0]?>"<?php if($_REQUEST["defaultpasswordscheme"] == $scheme) { echo " CHECKED"; }?>>
<?php	} ?>
          </td>
        </tr>
<?php } elseif($_REQUEST["template"] != "internal_group") {
		// We got a default password scheme OR we don't allow changing of password (then we can't allow
		// changing the default password scheme either).
		if(!$_REQUEST["pwscheme"])
		  $_REQUEST["pwscheme"] = $_REQUEST["defaultpasswordscheme"];
?>
        <input type="hidden" name="pwscheme" value="<?php echo $_REQUEST["pwscheme"]?>">
<?php	if($lockpassword) { ?>
        <!-- Password scheme -->
        <tr class="<?php pql_format_table(); ?>">
          <td class="title"><?php echo $LANG->_('Password encryption scheme')?></td>
          <td>&nbsp;&nbsp;&nbsp;<b><?php echo $_REQUEST["pwscheme"]?></b></td>
        </tr>

<?php	} ?>
<?php }

	  if($krb_scheme and !$lockpassword and
		 in_array('KERBEROS', $schemes) and
		 !(($_REQUEST["template"] == "mail") and ($scheme == "KERBEROS")) and
		 (!$_REQUEST["defaultpasswordscheme"] or $_SESSION["ADVANCED_MODE"]))
	  {
?>
        <tr class="<?php pql_format_table(); ?>">
          <td><img src="images/info.png" width="16" height="16" alt="" border="0" align="right"></td>
          <td><?php echo $LANG->_('If using {KERBEROS} as password scheme, make sure you include the correct REALM (principal@REALM.TLD)')?></td>
        </tr>
<?php }
// }}}

	  // {{{ Password
?>

        <!-- Password -->
<?php if($krb_scheme or $_SESSION["ADVANCED_MODE"]) {
			$class = pql_format_table(0);
?>
        <tr class="<?php echo $class?>">
<?php	if(!preg_match("/KERBEROS/i", $_REQUEST["pwscheme"])) { ?>
          <td class="title"><?php echo $LANG->_('Password')?></td>
<?php	} else { ?>
          <td class="title">
            <?php echo $LANG->_('Kerberos V Principal')?>
<?php		if(!empty($schemes[1])) { // We got more than one password scheme... ?>
            <br>(or)<br>
            <?php echo $LANG->_('Password')?>
<?php		} ?>
          </td>
<?php	}

		if(!$lockpassword) {
?>
          <td>
<?php		if(!preg_match('/KERBEROS/i', $_REQUEST["pwscheme"]) and !$_REQUEST["autogenerated_password"]) { ?>
            &nbsp;&nbsp;<input type="checkbox" name="autogenerate">
            <?php echo $LANG->_('Autogenerate password')?><br>
<?php		} ?>

<?php	} ?>
          <!-- Crude hackery. Using type=password won't be so good if we're using {KERBEROS} -->
            <?php if(!empty($error_text["password"])) { echo pql_format_error_span($error_text["password"]); }?>
            <?php echo pql_templates_mark_importance($mark); ?>
<?php	if($lockpassword) { ?>
            <input type="hidden" name="password" value="<?php echo $_REQUEST["password"]?>">
            <b><?php echo $_REQUEST["password"]?></b>
<?php	} else { ?>
            <input type="text" name="password" value="<?php echo $_REQUEST["password"]?>"<?php if($krb_scheme) { ?> size="30"<?php } ?>>
<?php	}

		if(!$lockpassword and !preg_match('/KERBEROS/i', $_REQUEST["pwscheme"])) {
?>
            <input type="checkbox" name="crypted"><?php echo $LANG->_('Password is already encrypted')?>
            <?php echo pql_format_error_span($error["pwscheme"]); ?>
<?php	} ?>
          </td>
        </tr>

<?php	if(!empty($_REQUEST["clear_text_password"])) { ?>
        <tr class="<?php echo $class?>">
          <td class="title"><?php echo $LANG->_('Password that will be set')?></td>
          <td>
            <?php echo pql_templates_mark_importance($mark); ?>
            <b><?php echo $_REQUEST["clear_text_password"]?></b>
          </td>
        </tr>
        <input type="hidden" name="clear_text_password" value="<?php echo $_REQUEST["clear_text_password"]?>">

<?php	} elseif($auto_generated_kerberos_pw) { ?>
        <tr class="<?php echo $class?>">
          <td class="title"><?php echo $LANG->_('Password')?></td>
          <td>
            <?php echo pql_templates_mark_importance($mark); ?>
            <input type="text" name="clear_text_password" value="<?php echo $_REQUEST["clear_text_password"]?>">
          </td>
        </tr>

<?php	} ?>

<?php	if(!$lockpassword) { ?>
        <tr class="<?php pql_format_table(); ?>">
          <td><img src="images/info.png" width="16" height="16" alt="" border="0" align="right"></td>
          <td><?php echo $LANG->_('If you enter an already encrypted password, you must make sure that the password scheme you\'ve choosen is the correct one. Also, choose the checkbox \uPassword is already encrypted\U')?></td>
        </tr>
<?php	}
	  } else { ?>
        <tr class="<?php pql_format_table(); ?>">
          <td class="title"><?php echo $LANG->_('Password')?></td>
          <td><?php echo $LANG->_('Will be auto generated')?></td>
        </tr>
        <input type="hidden" name="autogenerate" value="on">
<?php }
// }}}
	}
?>
      </th>
    </table>

    <br>

    <table cellspacing="0" cellpadding="3" border="0">
<?php if($_SESSION["ADVANCED_MODE"] and !$_REQUEST["subbranch"]) {
		// Retreive sub branches
		$branches = pql_unit_get($_REQUEST["domain"]);
	  }

	  if(pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_LOGINSHELL")) or
		 pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_MAIL"))       or
		 pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_ISACTIVE"))   or
		 (pql_get_define("PQL_CONF_REFERENCE_USERS_WITH", $_REQUEST["rootdn"]) == sprintf("%s", pql_get_define("PQL_ATTR_MAIL")))       or
		 ($_SESSION["ADVANCED_MODE"] and !empty($branches[1]))                                                           or
		 ($_SESSION["ADVANCED_MODE"] and ($_SESSION["NEW_STYLE_QUOTA"] and ($_REQUEST["template"] == 'forward')))        or
		 ($_REQUEST["template"] == "forward")) {
		// Weee! Just because it would look strange with a table header, but nothing in it!
?>
      <th colspan="3" align="left"><?php echo $LANG->_('Account properties')?></th>
<?php }

	  // {{{ Login shell
	  if($mark = pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_LOGINSHELL"))) {
		if($_SESSION["ADVANCED_MODE"]) {
		  $shells = pql_get_valid_shells();
?>

        <!-- Loginshell -->
        <tr class="<?php pql_format_table(); ?>">
          <td class="title"><?php echo $LANG->_('Login shell')?></td>
          <td>
            <?php echo pql_format_error_span($error["loginshell"]); ?>
            &nbsp;&nbsp;
            <select name="loginshell">
              <option value="/bin/false"<?php if(empty($_REQUEST["loginshell"])) { ?> SELECTED<?php } ?>>/bin/false</option>
<?php	  foreach($shells as $shell) { ?>
              <option value="<?php echo $shell?>"<?php if(@($_REQUEST["loginshell"] == $shell)) { ?> SELECTED<?php } ?>><?php echo $shell?></option>
<?php	  } ?>
            </select>
          </td>
        </tr>
<?php  } // end if ADVANCED mode
	  } // end if login shell allowed
// }}}

	  // {{{ Mail address
	  $generate_email = function_exists('user_generate_email');
	  if($mark = pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_MAIL"))) {
		if(!pql_get_define("PQL_CONF_CREATE_ADDRESS", $_REQUEST["domain"]) or
		   (pql_get_define("PQL_CONF_CREATE_ADDRESS", $_REQUEST["domain"]) and !$generate_email) or
		   $_SESSION["ADVANCED_MODE"] or !empty($error_text["mail"])) {
?>

        <!-- Email address -->
        <tr class="<?php pql_format_table(); ?>">
          <td class="title"><?php echo $LANG->_('Main address')?></td>
          <td>
            <?php if(!empty($error_text["mail"])) { echo pql_format_error_span($error_text["mail"]); }?>
<?php		if($lockemailaddress) { ?>
            &nbsp;&nbsp;&nbsp;<b><?php echo $_REQUEST["mail"]?></b>
            <input type="hidden" name="mail" value="<?php echo $_REQUEST["mail"]?>">
<?php		} else {
		if(!$generate_email)
		    echo pql_templates_mark_importance($mark); ?>
<?php		} ?>
            <input type="text" name="mail" value="<?php echo $_REQUEST["mail"]?>">
<?php 		if(is_array($additionaldomainname)) { ?>
            <b>@ <select name="email_domain"></b>
              <option value="<?php echo $defaultdomain?>"><?php echo pql_maybe_idna_decode($defaultdomain)?></option>
<?php			foreach($additionaldomainname as $additional) { ?>
              <option value="<?php echo $additional?>"><?php echo pql_maybe_idna_decode($additional)?></option>
<?php   		} ?>
            </select>
<?php 		} elseif(@$defaultdomain) { ?>
            <b>@<?php echo pql_maybe_idna_decode($defaultdomain)?></b>
            <input type="hidden" name="email_domain" value="<?php echo $defaultdomain?>">
<?php 		} ?>
<?php           if($generate_email) ?>
		<br>
<?php               echo $LANG->_('If left out, the email address will be generated automatically')?>
          </td>
        </tr>

<?php 		if(is_array($additionaldomainname)) { ?>
        <tr class="<?php pql_format_table(); ?>">
          <td class="title"><?php echo $LANG->_('Alias')?></td>
          <td>
            <table>
              <td colspan="2"><input type="checkbox" name="include_additional" checked></td>
              <td>
                <?php echo $LANG->_('Include username in additional domains as alias')?>
              </td>
            </table>
          </td>
        </tr>
<?php			if(!$lockemailaddress) { ?>

        <tr class="subtitle">
          <td><img src="images/info.png" width="16" height="16" alt="" border="0" align="right"></td>
          <td><?php echo $LANG->_('The email address and the username will be automatically converted to lowercase')?></td>
        </tr>
<?php			} ?>

        <tr></tr>
<?php 		}
		} else {
?>
        <tr class="<?php pql_format_table(); ?>">
          <td><img src="images/info.png" width="16" height="16" alt="" border="0" align="right"></td>
          <td><?php echo pql_complete_constant($LANG->_('Automatically generated %what%'), array('what' => $LANG->_('email address'))); ?>.</td>
        </tr>
<?php	}
	  } // end if mail address allowed
// }}}
?>

        <!-- sub branch -->
<?php if($_SESSION["ADVANCED_MODE"]) {
		if(!$_REQUEST["subbranch"]) {
			if(!empty($branches[1])) {
				// {{{ More than one subbranch - show a select menu
?>
        <tr class="<?php pql_format_table(); ?>">
          <td class="title"><?php echo $LANG->_('Put user in subbranch')?></td>
          <td>
            &nbsp;&nbsp;
            <select name="subbranch">
              <option value="<?php echo pql_format_urls($_REQUEST["domain"])?>"><?php echo $_REQUEST["domain"]?></option>
<?php
				for($i=0; $i < count($branches); $i++) {
?>
              <option value="<?php echo pql_format_urls($branches[$i])?>"><?php echo $branches[$i]?></option>
<?php
				}
?>
            </select>
          </td>
        </tr>
<?php
// }}}
			} else {
				// {{{ Only one subbranch
?>
        <input type="hidden" name="subbranch" value="<?php echo $url["domain"]?>">
<?php
// }}}
			}
		} else {
			// We've already specified a subbranch in where to put the user
?>
        <input type="hidden" name="subbranch" value="<?php echo pql_format_urls($_REQUEST["subbranch"])?>">
<?php	}
	  } else {
		// We're NOT running in advanced mode
		if(pql_get_define("PQL_CONF_SUBTREE_USERS")) {
			$branch = pql_get_define("PQL_CONF_SUBTREE_USERS").",".$_REQUEST["domain"];
			$branch = pql_format_urls($branch);
			
			// Just to make sure - does this subbranch exists?!
			$xyz = $_pql->get_dn($branch, 'objectclass=*', 'BASE');
			if(is_array($xyz)) {
?>
        <input type="hidden" name="subbranch" value="<?php echo $branch?>">
<?php
			} else {
?>
        <input type="hidden" name="subbranch" value="<?php echo $url["domain"]?>">
<?php		}
		} else {
?>
        <input type="hidden" name="subbranch" value="<?php echo $url["domain"]?>">
<?php	}
	  } // end if ADVANCED mode

	  // {{{ Forwarding address
	  if($_REQUEST["template"] == "forward") {
?>

        <!-- Forwarding address -->
        <tr class="<?php pql_format_table(); ?>">
          <td class="title"><?php echo $LANG->_('Forwarding address')?></td>
          <td>
            <?php if(!empty($error_text["forwardingaddress"])) { echo pql_format_error_span($error_text["forwardingaddress"]); }?>
            <?php echo pql_templates_mark_importance($mark); ?>
            <input type="text" name="forwardingaddress" value="<?php echo $_REQUEST["forwardingaddress"]?>"><?php echo $LANG->_('Email')?>
          </td>
        </tr>

        <tr class="subtitle">
          <td colspan="2"><img src="images/info.png" width="16" height="16" alt="" border="0">&nbsp;<?php echo $LANG->_('You can add more forwarding address in the user details page')?></td>
        </tr>
<?php } // template == forward
// }}}
  
	  // {{{ Account status
      if($mark = pql_templates_check_attribute($template_attribs, pql_get_define("PQL_ATTR_ISACTIVE"))) {
?>

        <!-- Account status -->
        <tr class="<?php pql_format_table(); ?>">
          <td class="title"><?php echo $LANG->_('Status')?></td>
          <td>
            <?php echo pql_templates_mark_importance($mark); ?>
            <select name="account_status">
              <option value="active" SELECTED><?php echo $LANG->_('Active')?></option>
<?php if(!$_SESSION["NEW_STYLE_QUOTA"])  { ?>
              <option value="nopop"><?php echo $LANG->_('POP locked')?></option>
<?php } else { ?>
              <option value="noaccess"><?php echo $LANG->_('Account locked')?></option>
<?php } ?>
              <option value="disabled"><?php echo $LANG->_('Disabled')?></option>
            </select>
          </td>
        </tr>
<?php } elseif($_REQUEST["template"] != "internal_group") { ?>
    <input type="hidden" name="account_status" value="">
<?php }
// }}}

	  // {{{ Delivery mode
	  if($_SESSION["NEW_STYLE_QUOTA"] and ($_REQUEST["template"] == 'forward')) {
		  if($_SESSION["ADVANCED_MODE"]) {
			  // The new QmailLDAP have a changed 'deliveryMode' setup.
			  // The only thing that we need to know NOW is:
			  // Should we delivery into local mailbox as well as forward mail!?
?>
        <!-- deliveryMode -->
        <tr class="<?php pql_format_table(); ?>">
          <td class="title"></td>
          <td><input type="checkbox" name="nolocal" CHECKED><?php echo $LANG->_('No local delivery allowed')?></td>
        </tr>
<?php 	  } else { ?>
          <input type="hidden" name="nolocal" value="1">
<?php	  }
	  }
// }}}
?>
      </th>
    </table>

<?php
	  // {{{ Hidden values
?>

    <input type="hidden" name="page_curr"              value="one">
<?php if(($_SESSION["ADVANCED_MODE"] == 0) or ($_REQUEST["template"] == "forward")) {
		// Go to save, no next form...
		if($_SESSION["ADVANCED_MODE"] == 0) {
			// Autocreate some values by using 'safe' (?) defaults
?>
    <input type="hidden" name="loginshell"             value="/bin/false">
    <input type="hidden" name="homedirectory"          value="">
    <input type="hidden" name="maildirectory"          value="">
<?php	} ?>
    <input type="hidden" name="page_next"              value="save">
<?php } else {
		if(($_REQUEST["template"] != 'shell') and ($_REQUEST["template"] != "internal_group")) {
?>
    <input type="hidden" name="page_next"              value="two">
<?php	} else {
?>
    <input type="hidden" name="page_next"              value="save">
<?php	}
	  } // template == forward
?>
    <input type="hidden" name="dosave"                 id="dosave">
    <input type="hidden" name="rootdn"                 value="<?php echo $url["rootdn"]?>">
    <input type="hidden" name="domain"                 value="<?php echo $url["domain"]?>">
    <input type="hidden" name="template"               value="<?php echo $_REQUEST["template"]?>">
    <input type="hidden" name="autogenerated_password" value="<?php echo $_REQUEST["autogenerated_password"]?>">

<?php
// }}}
?>
    <br>

<?php if($_SESSION["ADVANCED_MODE"] == 1) { ?>
    <input type="submit" value="--&gt;&gt;">
<?php } else { ?>
    <input type="submit" value="<?php echo $LANG->_('Save')?>">
<?php } ?>
  </form>

  <script language="JavaScript">
    <!--
    document.user_add.uid.focus();
    // -->
  </script>
<?php
/*
 * Local variables:
 * mode: php
 * mode: font-lock
 * tab-width: 4
 * End:
 */
?>
