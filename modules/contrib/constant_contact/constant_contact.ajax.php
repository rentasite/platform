<?php
// $Id: constant_contact.ajax.php,v 1.11 2010/03/28 12:02:30 justphp Exp $

/**
 * @file
 */

/**
 * Authenticates with the API
 * This is used for the admin settins page to check the details they entered
 */
function constant_contact_auth($username, $password)
{
	require_once dirname(__FILE__) . '/class.cc.php';
	$cc = new cc($username, $password);

	if(is_object($cc) && $cc->get_service_description()):
		// we have successfully connected
		return $cc;
	elseif($cc->http_response_code):
		// oops, problem occured and we have an error code
		return $cc->http_get_response_code_error($cc->http_response_code);
	else:
		return "Please enter your username and password";
	endif;
}

$username = ($_GET['username']) ? strip_tags($_GET['username']) : '';
$password = ($_GET['password']) ? strip_tags($_GET['password']) : '';

$result = constant_contact_auth($username, $password);

if(is_object($result)):
	echo '<div class="cc_auth_res" style="color:green;">Authenticated OK, please click the "Save configuration" button below</div>';
else:
	echo "<div class='cc_auth_res' style='color:red;'>$result</div>";
endif;