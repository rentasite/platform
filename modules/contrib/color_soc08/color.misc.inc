<?php
// $Id: color.misc.inc,v 1.5.2.15 2008/11/10 23:41:35 skiquel Exp $
/**
 * @file
 * Misc functions.
 */

function color_array_combine($keys, $vals) {
//  $keys = array_values($keys);
  //$vals = array_values($vals);

  $combined = array();
  foreach ($keys as $key) {
    $combined[$key] = array_shift($vals);
  }

  return $combined;
}

/**
 * Tests individual theme color.module support
 *
 * @param $theme
 *  String of theme name
 */
function color_compatible($theme_name) {
  static $static;

  $theme_info = unserialize(db_result(db_query("SELECT info FROM {system} WHERE type = '%s' AND name = '%s' ", 'theme', $theme_name)));
  if (!empty($theme_info['color'])) {
    $static[$theme_name] = TRUE;
  }
  else {
    $static[$theme_name] = FALSE;
  }

  return $static[$theme_name];
}

/**
 * Recursively remove directly and file contents at location by directory
 *
 * @param $directory
 *  String of directory path
 * @param $theme
 *  Optional string for theme name
 */
function color_rm_dir($directory, $theme = NULL) {
  if (is_dir($directory)) {
    $directory = (substr($directory, -1) != "/") ? $directory."/" : $directory;
    $open_dir = opendir($directory);

    while ($file = readdir($open_dir)) {
      if (!in_array($file, array(".", "..")) && (!$theme || ($theme && strstr($directory.$file, $theme)))) {
        if (!is_dir($directory.$file))
          @unlink($directory.$file);
        else
          color_rm_dir($directory.$file, $theme);
      }
    }
    closedir($open_dir);
    @rmdir($directory);
  }
}

/**
 * Normalize theme names
 *
 * @param $string
 *   String to normalize
 * @return
 *   Normalizing string.
 */
function color_normalize($string) {
  $string = preg_replace('~[^a-z0-9._]+~', '-', drupal_strtolower($string));
  return $string;
}

/**
 * Check the color.inc file for theme
 *
 * @param $info
 *   Array of theme info from color.inc.
 * @return
 *   TRUE if theme validates. FALSE if not. Also output drupal error/status messages.
 */
function color_validate($info) {
  $validated = TRUE;
  $theme = $info['theme'];

  // Is 'options' set?
  if (isset($info['engine'])) {
    // Are any of the two engine options set?
    if ((array_key_exists('shift', $info['engine']) === FALSE) && (array_key_exists('tag', $info['engine']) === FALSE)) {
      drupal_set_message("(".$theme.") info[engine]: missing 'shift' and/or 'tag' option.", 'error');
      $validated = FALSE;
    }

    // Catch common color.inc error. Not referencing 'shift' mode when base/link/text option stated.
    if (isset($info['engine']['shift']) && array_search('base', $info['engine']['shift']) !== FALSE && array_search('base-only', $info['engine']['shift']) !== FALSE) {
      drupal_set_message("(".$theme.") info[engine][shift]: missing 'base' or 'base-only' option", 'error');
    }
  }
  else {
    drupal_set_message("(".$theme.") info[engine]: missing.", 'error');
    $validated = FALSE;
  }

  if (isset($info['fields'])) {
    $fieldcount = count($info['fields']);
  }
  else {
    drupal_set_message("(".$theme.") info[fields]: missing.", 'error');
    $validated = FALSE;
  }

  if (isset($info['image'])) {
    if (isset($info['blend target'])) {
      if (strlen($info['blend target']) != 7) {
        drupal_set_message("(".$theme.") info[blend target]: (".$info['blend target'].") is not a valid full hex (i.e. #ffffff).", 'error');
        $validated = FALSE;
      }
    }
    else {
      drupal_set_message("(".$theme.") info[blend target]: missing.", 'error');
      $validated = FALSE;
    }
  }

  if (isset($info['default scheme'])) {
    if (array_search($info['default scheme'], array_keys($info['schemes'])) === FALSE) {
      $validated = FALSE;
      drupal_set_message("(".$theme.") info[default scheme] (".$info['default scheme']."): does not match a scheme name.", 'error');
    }
  }
  else {
    drupal_set_message("(".$theme.") info[default scheme]: missing.", 'error');
    $validated = FALSE;
  }

  if (isset($info['engine']['shift']) && (array_search('base', $info['engine']['shift']) !== FALSE || array_search('base-only', $info['engine']['shift']) !== FALSE)) {
    if (isset($info['reference scheme'])) {
      if (array_search($info['reference scheme'], array_keys($info['schemes'])) === FALSE) {
        $validated = FALSE;
        drupal_set_message("(".$theme.") info[reference scheme] (".$info['reference scheme']."): does not match a scheme name", 'error');
      }
    }
    else {
      drupal_set_message("(".$theme.") info[reference scheme]: missing.", 'error');
      $validated = FALSE;
    }
  }

  if (isset($info['schemes'])) {
    if (isset($info['engine']['shift']) && array_search('base-only', $info['engine']['shift']) === FALSE) {
      foreach ($info['schemes'] as $key => $v) {
        $commas = count($v);
        foreach ($v as $num => $hex) {
          if (!isset($hex) || empty($hex)) {
            drupal_set_message("(".$theme.") info[schemes][{$key}]: missing color field {$num} is missing. (Color scheme fields error)", 'error');
            drupal_set_message("(".$theme.") (Color scheme fields error): It is possible a comma may be misplaced, leaving the colors out of order.", 'warning', FALSE);
            $validated = FALSE;

          }
          elseif (strlen($hex) != 7) {
            drupal_set_message("(".$theme.") info[schemes][{$key}]: color field {$num} ({$hex}) is not a full hex (i.e. #ffffff).", 'error');
            $validated = FALSE;
          }
        }
      }
    }
  }
  elseif (!isset($info['schemes'])) {
    drupal_set_message("(".$theme.") info[schemes]: missing.", 'error');
    $validated = FALSE;
  }
  else {

  }

  if (isset($info['css'])) {
    foreach ($info['css'] as $val) {
      // Check for file
      if (!isset($val)) {
        $validated = FALSE;
        drupal_set_message("(".$theme.") info[css][{$val}]: No File provided for image!", 'error');
      }
      else {
        $path = drupal_get_path('theme', $theme) .'/';
        $file = $path . $val;
        if ($path && !file_exists($file)) {
          $validated = FALSE;
          drupal_set_message("(".$theme.") info[css][{$val}]: File ({$file}) does not exist.", 'error');
        }
      }
    }
  }

  if (isset($info['image'])) {
    // Move through $info['image'] array
    foreach ($info['image'] as $key => $val) {

      // Check for file
      if (!isset($val['file'])) {
        $validated = FALSE;
        drupal_set_message("(".$theme.") info[img][{$key}][file] - No File provided for image!", 'error');
      }
      else {
        $path = drupal_get_path('theme', $theme) .'/';
        $file = $path . $val['file'];
        if ($path && !file_exists($file)) {
            $validated = FALSE;
            drupal_set_message("(".$theme.") info[img][{$key}][file] - File does not exist.", 'error');
        }
      }

      // Fill
      if (isset($val['fill'])) {
        foreach ($val['fill'] as $fill) {
          // Possible feature: check dimensions
          if (array_search($fill[4], $info['fields']) === FALSE) {
            $validated = FALSE;
            drupal_set_message("(".$theme.") Missing fill color.", 'error');
          }
        }
      }

      // Gradient
      if (isset($val['gradient'])) {
        foreach ($val['gradient'] as $gradient) {
          // Possible feature: check dimensions
          if (array_search($gradient[4], $info['fields']) === FALSE) {
            $validated = FALSE;
            drupal_set_message("(".$theme.") Missing the top color field.", 'error');
          }
          if (array_search($gradient[5], $info['fields']) === FALSE) {
            $validated = FALSE;
            drupal_set_message("(".$theme.") Missing the bottom color field", 'error');
          }
        }
      }
    }
  }
  return $validated;
}

/**
 * Get the function info from the hook color.
 *
 * @param $theme_name
 *   String of the theme name.
 * @return
 *   Associated array with theme color info.
 */
function color_get_color_hook($theme_name) {
  $path = drupal_get_path('theme', $theme_name);
  $file = $path .'/template.php';
  if ($path && file_exists($file)) {
    include_once $file;
    if (function_exists($theme_name . '_color')) {
      $color = call_user_func($theme_name . '_color');
    }
  }
  else {
    $color = FALSE;
  }

  return $color;
}

/**
 * Function to determine if a string is a hex color
 *
 * @param $hex
 *   String to try
 * @return
 *   TRUE if hex, FALSE if not
 */
function is_hex($hex) {
	$is_hex = preg_match("(^(\#?([0-9a-fA-F]){6}|([0-9a-fA-F]){3}))", $hex) ? TRUE : FALSE;
	
	return $is_hex;
}