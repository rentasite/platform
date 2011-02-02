<?php
// $Id: theme-settings.php,v 1.1.2.1.2.1 2009/04/27 06:03:32 skiquel Exp $

// Include the definition of zen_settings() and zen_theme_get_default_settings().
include_once './' . drupal_get_path('theme', 'zen') . '/theme-settings.php';


/**
 * Implementation of THEMEHOOK_settings() function.
 *
 * @param $saved_settings
 *   An array of saved settings for this theme.
 * @return
 *   A form array.
 */
function nista_settings($saved_settings) {

  // Get the default values from the .info file.
  $defaults = zen_theme_get_default_settings('nista');

  // Merge the saved variables and their default values.
  $settings = array_merge($defaults, $saved_settings);

  /*
   * Create the form using Forms API: http://api.drupal.org/api/6
   */
  $form = array();
  /* -- Delete this line if you want to use this setting
  $form['nista_example'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Use this sample setting'),
    '#default_value' => $settings['nista_example'],
    '#description'   => t("This option doesn't do anything; it's just an example."),
  );
  // */

  $form['themedev']['nista_layout'] = array(
    '#type'          => 'radios',
    '#title'         => t('Layout method'),
    '#options'       => array(
                          'border-politics-liquid' => t('Liquid layout') . ' <small>(layout-liquid.css)</small>',
                          'border-politics-fixed' => t('Fixed layout') . ' <small>(layout-fixed.css)</small>',
                        ),
    '#default_value' => $settings['nista_layout'],
  );
  
  // Add the base theme's settings.
  $form += zen_settings($saved_settings, $defaults);

  // Remove some of the base theme's settings.
  unset($form['themedev']['zen_layout']); // We don't need to select the base stylesheet.

  // Return the form
  return $form;
}
