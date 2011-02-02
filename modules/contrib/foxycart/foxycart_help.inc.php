<?php

// $Id: foxycart_help.inc.php,v 1.2 2010/06/13 19:54:03 kilogauss Exp $

function foxycart_detailed_help() {
  global $base_root;
  $datafeed_link = $base_root . '/' . 'foxycart/datafeed';
  $sso_link = $base_root . '/' . 'foxycart/sso';
  $log_link = l('here','admin/reports/dblog');
  
$output = <<<EOD
<p>After installing the foxycart module, visit the FoxyCart Admin Configuration page.  Select Administer -&gt; FoxyCart -&gt; Settings.</p>

<h2>Drupal Blocks</h2>
<p>There are two blocks supplied by the FoxyCart module that display the cart status to the user.  Configuration for these can be found under Administer -&gt; Blocks.  These blocks can also be themed by overriding the foxycart-full-block and foxycart-summary-cart-bock templates.</p>

<h2>Creating Product Pages</h2>
<p>After the module is installed and configured, you can start creating links for your products so they can be added to the user's cart.  Details for creating product links can be found <a href="http://wiki.foxycart.com/getting_started:adding_links_and_forms">here on the foxycart.com wiki</a>.  These links and form inputs can be added directly to a Drupal content page using the "Full HTML" input format.</p>

<h2>foxycart.com Admin Settings</h2>
<p><strong>Datafeed</strong>: In the <a href="https://admin.foxycart.com/admin.php?ThisAction=EditAdvancedFeatures">foxycart.com admin</a> under Store -&gt; Advanced -&gt; Datafeed URL, you can provide a link to:<br /><code>{$datafeed_link}</code><br />Once enabled, you can view the logged datafed transactions {$log_link}.</p>
<p><strong>Single Sign-On</strong>: In the <a href="https://admin.foxycart.com/admin.php?ThisAction=EditAdvancedFeatures">foxycart.com admin</a> under Store -&gt; Advanced, check the "Enable Shared Authentication" box as well as in the "Shared Authentication URL" field supply the following link:<br /><code>{$sso_link}</code>.<br/>Also, be sure to copy the 'API Key' from the foxycart.com admin to the Drupal FoxyCart Module admin settings.  <br/>Select 'md5' for the 'customer password hash type'.</strong>
EOD;

  return $output;
}
?>