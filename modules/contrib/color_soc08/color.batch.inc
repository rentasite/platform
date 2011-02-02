<?php
// $Id: color.batch.inc,v 1.1.2.8 2009/04/26 23:28:16 skiquel Exp $

/**
 * @file
 * Batch stuff
 */

/**
 * Batch progress to generate scheme info
 *
 * The $batch can include the following values. Only 'operations'
 * and 'finished' are required, all others will be set to default values.
 *
 * @param operations
 *   An array of callbacks and arguments for the callbacks.
 *   There can be one callback called one time, one callback
 *   called repeatedly with different arguments, different
 *   callbacks with the same arguments, one callback with no
 *   arguments, etc.
 *
 * @param finished
 *   A callback to be used when the batch finishes.
 *
 * @param title
 *   A title to be displayed to the end user when the batch starts.
 *
 * @param init_message
 *   An initial message to be displayed to the end user when the batch starts.
 *
 * @param progress_message
 *   A progress message for the end user. Placeholders are available.
 *   Placeholders note the progression by operation, i.e. if there are
 *   2 operations, the message will look like:
 *    'Processed 1 out of 2.'
 *    'Processed 2 out of 2.'
 *   Placeholders include:
 *     @current, @remaining, @total and @percent
 *
 * @param error_message
 *   The error message that will be displayed to the end user if the batch
 *   fails.
 *
 */
function color_batch_generate($theme) {
  $batch = array(
    'operations' => array(
      array('color_batch_generate_schemes', array($theme)),
      ),
    'finished' => 'color_generate_finished',
    'title' => t('Generating schemes'),
    'init_message' => t('Color scheme generater is initializing.'),
    'progress_message' => t('Processed @current out of @total.'),
    'error_message' => t('An error has occured while processing the color schemes.'),
    'file' => './'. drupal_get_path('module', 'color') .'/color.batch.inc',

  );
  batch_set($batch);

  // If this function was called from a form submit handler, stop here,
  // FAPI will handle calling batch_process().

  // If not called from a submit handler, add the following,
  // noting the url the user should be sent to once the batch
  // is finished.
  batch_process('admin/build/themes/settings/' . $theme);
}

/**
 * Add theme info into {color}
 *
 * @param $theme: Theme name
 */
function color_install_theme(&$theme) {
  if (!isset($context['sandbox']['progress'])) {
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['max'] = 1;
  }

  module_load_include('inc', 'color', 'color.misc');

  $theme += color_get_color_hook($theme['name']);
  /*
  foreach ($theme['premade schemes'] as $name => $hex) {
    $theme['premade schemes'][$name]  = explode(',', $hex);
  }
  */

  // Add fields to db
  db_query('INSERT INTO {color}
           (`name`, `type`, `fields`, `replacement methods`) VALUES
           (\'%s\', \'%s\', \'%s\', \'%s\')',
           $theme['name'], 'theme', serialize($theme['fields']),
           serialize($theme['replacement methods']));

  $theme['id'] = db_last_insert_id('color', 'id');

  // Add other info to db
  $columns = array('blend target', 'images', 'stylesheets', 'premade schemes');
  foreach ($columns as $column) {
    if (isset($theme[$column])) {
      $column_data = in_array($column, array('images',
                                             'stylesheets',
                                             'premade schemes'))
                      ? serialize($theme[$column]) : $theme[$column];
      db_query('UPDATE {color} SET `%s` = \'%s\' WHERE id = \'%s\'',
               $column,
               $column_data,
               $theme['id']);
    }
  }

  $context['sandbox']['progress']++;
  if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] /
                            $context['sandbox']['max'];
  }
}

/**
 * Populate the scheme database with info from the color.inc file.
 * @param $theme: Theme name
 */
function color_batch_populate_schemes(&$theme) {

  if (!isset($context['sandbox']['progress'])) {
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['max'] = 1;
  }

  module_load_include('inc', 'color', 'color.database');

  foreach ($theme['premade schemes'] as $name => $hex) {
    // Add colors to database
    db_query('INSERT INTO {color_schemes} (name, theme, hex, status)
             VALUES (\'%s\', \'%s\', \'%s\', \'%s\')',
             $name, $theme['id'], serialize($hex), '0');
    $last_id = db_last_insert_id('color_schemes', 'id');

    if (strstr($theme['default scheme'], $name)) {
      db_query("UPDATE {color} SET `default scheme` = '%s' WHERE id = '%s'",
               $last_id, $theme['id']);
      $default['id'] = $last_id;
      $theme['default scheme'] = color_get_scheme($reference, TRUE);
    }

    if (strstr($theme['reference scheme'], $name)) {
      db_query("UPDATE {color} SET `reference scheme` = '%s' WHERE id = '%s'",
               $last_id, $theme['id']);
      $reference['id'] = $last_id;
      $theme['reference scheme'] = color_get_scheme($reference, TRUE);
    }
  }

  $context['sandbox']['progress']++;

  if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] /
                            $context['sandbox']['max'];
  }
}

function color_batch_generate_schemes($theme_name, &$context) {
  module_load_include('inc', 'color', 'color.database');

  $theme['name'] = $theme_name;
  if (!isset($context['sandbox']['progress'])) {
    color_install_theme($theme);
    color_batch_populate_schemes($theme);
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['scheme_id'] = 0;
    $context['sandbox']['theme'] = serialize($theme);
    $context['sandbox']['max'] = db_result(db_query("SELECT COUNT(id)
                                                    FROM {color_schemes}
                                                    WHERE theme = '%s'",
                                                    $theme['id'])
                                           );
  }
  else {
    $theme = unserialize($context['sandbox']['theme']);
  }

  if ($query = db_query("SELECT id, name, theme, hex FROM {color_schemes}
                        WHERE theme = '%s' AND id > %d ORDER BY id ASC LIMIT 1",
                        $theme['id'], $context['sandbox']['scheme_id'])
      ) {
//  if ($query = db_query_range("SELECT id, name, theme, hex FROM {color_schemes} WHERE theme = '%s' AND id > %d ORDER BY id ASC", $theme['id'], $context['sandbox']['scheme_id'], 1)) {

    $scheme = db_fetch_array($query);
    $context['sandbox']['scheme_id'] = $scheme['id'];
    $scheme['hex'] = unserialize($scheme['hex']);
    color_set_scheme($scheme, $theme, 'update');
  }

  $context['sandbox']['progress']++;
  $context['message'] = t('Now generating %scheme',
                          array('%scheme' => $scheme['name'])
                          );

  // Inform the batch engine that we are not finished,
  // and provide an estimation of the completion level we reached.
  if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] /
                            $context['sandbox']['max'];
  }
  else {
    variable_set('theme_color_'. $theme['name'] .'_generated', 1);
    variable_set('theme_color_'. $theme['name'], 1);
  }
}

/**
* Batch 'finished' callback
*/
function color_generate_finished($success, $results, $operations) {
  if ($success) {
    // Here we do something meaningful with the results.
    //  $message = count($results) .' processed.';
    //  $message .= theme('item_list', $results);
  }
  else {
    // An error occurred.
    // $operations contains the operations that remained unprocessed.
    $error_operation = reset($operations);
    $message = 'An error occurred while processing '. $error_operation[0] .
                ' with arguments :'. print_r($error_operation[0], TRUE);
  }
  drupal_set_message($message);
}