<?php
// $Id: mailchimp.drush.inc,v 1.1.2.1.2.1 2010/07/14 20:02:15 loubabe Exp $

/**
 * @file
 * Mailchimp module Drush integration
 */

/**
 * Implements hook_drush_command().
 *
 * @return
 *   An associative array of commands.
 */
function mailchimp_drush_command() {
  $items = array();

  $items['mailchimp-cron'] = array(
    'callback' => '_mailchimp_cron_batch',
    'description' => "Trigger Mailchimp cron task",
    'examples' => array(
      'drush mailchimp-cron 5000' => 'Run Mailchimp cron with a batch size of 5000.',
      'drush mailchimp-cron' => 'Run a Mailchimp cron task.',
    ),
  );

  return $items;
}

function _mailchimp_cron_batch($temp_batchsize = NULL) {
  if ( !is_null($temp_batchsize) ) {
    $stored_batchsize = variable_get('mailchimp_batch_limit', 100);
    variable_set('mailchimp_batch_limit', $temp_batchsize);
  }
  mailchimp_cron();
  if ( !is_null($batchsize) ) {
    variable_set('mailchimp_batch_limit', $stored_batchsize);
  }
}
