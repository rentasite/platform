<?php
// $Id: color.database.inc,v 1.6.2.25 2008/11/13 23:56:09 skiquel Exp $
/**
 * @file
 * Database magician functions
 */

/**
 * Special version of drupal's core list_themes()
 * Filter through enabled and disabled, colorable and
 * non-colorable.
 *
 * @param $refresh
 *   TRUE or FALSE, whether or not to refresh the theme DB.
 * @param $enabled
 *   1 for enabled. 0 for disabled. NULL for both.
 * @param $colorable
 *   1 for colorable. 0 for not colorable. NULL for both.
 * @return
 *   Array of themes
 */
function color_list_themes($refresh = FALSE, $enabled = NULL, $colorable = NULL) {
  static $list = array();

  if ($refresh) {
    $list = array();
  }

  if (empty($list)) {
    $list = array();
    $themes = array();
    // Extract from the database only when it is available.
    // Also check that the site is not in the middle of an install or update.
    if (db_is_active() && !defined('MAINTENANCE_MODE')) {
      if (isset($enabled)) {
        $result = db_query("SELECT * FROM {system} WHERE type = '%s' AND status = '%s'", 'theme', $enabled);
      }
      else {
        $result = db_query("SELECT * FROM {system} WHERE type = '%s'", 'theme');
      }

      while ($theme = db_fetch_object($result)) {
        if (file_exists($theme->filename)) {
          $theme->info = unserialize($theme->info);
          if (!isset($colorable)) {
            $themes[] = $theme;
          }
          elseif ($colorable && $theme->info['color'] === $colorable) {
            $themes[] = $theme;
          }
        }
      }
    }
    else {
      // Scan the installation when the database should not be read.
      $themes = _system_theme_data();
    }

    foreach ($themes as $theme) {
      foreach ($theme->info['stylesheets'] as $media => $stylesheets) {
        foreach ($stylesheets as $stylesheet => $path) {
          $theme->stylesheets[$media][$stylesheet] = $path;
        }
      }
      foreach ($theme->info['scripts'] as $script => $path) {
        if (file_exists($path)) {
          $theme->scripts[$script] = $path;
        }
      }
      if (isset($theme->info['engine'])) {
        $theme->engine = $theme->info['engine'];
      }
      if (isset($theme->info['base theme'])) {
        $theme->base_theme = $theme->info['base theme'];
      }
      // Status is normally retrieved from the database. Add zero values when
      // read from the installation directory to prevent notices.
      if (!isset($theme->status)) {
        $theme->status = 0;
      }
      $list[$theme->name] = $theme;
    }
  }

  return $list;
}

// Takes $scheme, including $scheme['id']
function color_get_scheme(&$scheme, $return = FALSE) {
  static $static;

  if (is_string($scheme)) {
    $scheme_id = $scheme;
    $scheme = array();
    $scheme['id'] = $scheme_id;
  }

  if (isset($static[$scheme['id']])) {
    if ($return) {
      return $static[$scheme['id']];
    }
    else {
      $scheme = $static[$scheme['id']];

      return;
    }
  }

  // Check for scheme['id'].
  if (isset($scheme['id'])) {
    $query = db_query("SELECT * FROM {color_schemes} WHERE id = '%s'", $scheme['id']);
    $rows = db_fetch_array($query);
  }

  $unserialize = array('hex', 'images', 'stylesheets');
  foreach ($unserialize as $rowname) {
    if (isset($rows[$rowname])) {
      $rows[$rowname] = unserialize($rows[$rowname]);
    }
  }

  if ($return) {
    $static[$scheme['id']] = $rows;

    return $rows;
  }
  else {
    $scheme = $rows;
    $static[$scheme['id']] = $rows;

  }

}

/**
 *  This function operates essentially the same as color_get_scheme,
 *  it is returning in the same format, but extracting it in a different way,
 *  from a special location (the `preview` column in the {color} table, instead of
 *  a row in {color_schemes}.
 *
 *  @param $theme
 *    Reference array of theme stuff!
 *  @param $scheme
 *    Reference array of scheme info. This will be updated with the info pulled
 *    for the preview scheme.
 */
function color_get_preview(&$theme, &$scheme) {
  $result = db_result(db_query("SELECT preview FROM {color} ".
  "WHERE name = '%s'", $theme['name']));

  $row = unserialize($result);

  $scheme = $row;
}

/**
 * Set/modify site schemes.
 * @param $scheme
 *   Reference array of scheme info.
 * @param $method
 *   Optional array. Scheme info to update or add.
 */
function color_set_scheme($scheme = NULL, $theme = NULL, $method) {
  module_load_include('inc', 'color', 'color.generate');

  // See if the $vars include the status of the theme.
  if ($method == 'populate') {
    db_query('INSERT INTO {color_schemes} (name, theme, hex, status) VALUES (\'%s\', \'%s\', \'%s\', \'%s\')', $scheme['name'], $scheme['theme'], serialize($scheme['hex']), $scheme['status']);
    $last_id = db_last_insert_id('color_schemes', 'id');
    drupal_set_message("Scheme <em>".$scheme['name']."</em> added.", "status", FALSE);
    return $last_id;
  }
  elseif ($method == 'create') {
    color_generate_scheme_files($scheme, $theme);

    db_query('INSERT INTO {color_schemes} (name, theme, hex, images, stylesheets, status) VALUES (\'%s\', \'%s\', \'%s\', \'%s\', \'%s\', \'%s\')', $scheme['name'], $scheme['theme'], serialize($scheme['hex']), serialize($scheme['images']), serialize($scheme['stylesheets']), $scheme['status']);

    $last_id = db_last_insert_id('color_schemes', 'id');
    if (isset($scheme['logo'])) {
      $extras[] = " `logo` = '" . $scheme['logo'] . "'";
    }
    if (isset($scheme['screenshot'])) {
      $extras[] = " `screenshot` = '" . $scheme['screenshot'] . "' ";
    }
    if (isset($extras)) {
      $extras = implode(',', $extras);
      db_query('UPDATE {color_schemes} SET '. $extras . ' WHERE id = \'%s\'', $last_id);
    }

    return $last_id;
  }
  elseif ($method == 'update') {
    color_generate_scheme_files($scheme, $theme);

    if (isset($scheme['logo'])) {
      $extras[] = " `logo` = '" . $scheme['logo'] . "'";
    }
    if (isset($scheme['screenshot'])) {
      $extras[] = " `screenshot` = '" . $scheme['screenshot'] . "' ";
    }
    if (isset($extras)) {
      $extras = implode(',', $extras);
      $extras = "," . $extras;
    }
    else {
      $extras = '';
    }

    db_query('UPDATE {color_schemes} SET `path` = \'%s\', `stylesheets` = \'%s\', `images` = \'%s\', `hex` = \'%s\''. $extras . ' WHERE id = \'%s\'', $scheme['path'], serialize($scheme['stylesheets']), serialize($scheme['images']), serialize($scheme['hex']), $scheme['id']);

    if (isset($scheme['name'])) {
      db_query('UPDATE {color_schemes} SET `name` = \'%s\' WHERE id = \'%s\'', $scheme['name']);
    }
  }
  elseif ($method == 'delete' && isset($scheme['id'])) {
    db_query('DELETE FROM {color_schemes} WHERE id = \'%s\'', $scheme['id']);
    drupal_set_message('Scheme removed.', "status", FALSE);
    return "deleted";
  }
}

/**
 *  This function operates similar to color_set_scheme
 *  except it's placed into the `preview` column of the theme
 *  inside {color} as a serialized array.
 *
 *  @param $scheme
 *    Referenace array of scheme values, will be changed and updated
 *    with any generated images/stylesheets that are returned.
 *  @param $theme
 *    Array of theme info to pass down to color_generate_scheme_files()
 */
function color_set_preview(&$scheme, $theme) {
  module_load_include('inc', 'color', 'color.generate');

  // Because this is a color_set_preview (A preview), we're
  // Going to put a special boolean in the $scheme
  $theme['set_preview'] = TRUE;

  if (isset($scheme['id'])) {
    color_generate_scheme_files($scheme, $theme);
    db_query('UPDATE {color} SET preview = \'%s\' WHERE id = \'%s\'', serialize($scheme), $theme['id']);
  }
}

/**
 * Check if scheme_id matches up with a real scheme.
 *  This function will automatically match against name and ID, so $theme be
 *  it's ID in {color} or the name itself.
 * @param $scheme_id: the scheme ID
 * @param $theme: the theme name or ID
 */
function color_scheme_id_valid($scheme_id, $theme = NULL) {
  $query = db_query("SELECT name, id FROM {color} WHERE id = (SELECT theme FROM {color_schemes} WHERE id = '%s')", $scheme_id);
  $row = db_fetch_array($query);

  if ($theme == $row['name'] || $theme == $row['id']) {
    $valid = TRUE;
  }
  else {
    $valid = FALSE;
  }

  return $valid;
}

/**
 * Retrieve the color info for a theme.
 *
 * @param $theme
 *   A string for the theme name
 * @param $source
 *   An optional string. 'sql' or 'inc'.
 *   Typically 'inc' is used for generated scheme before DB is populated for schemes
 *   Typically 'sql' is for current, generated schemes
 * @param $columns
 *   An optional array or string of what columns to return. If none, will return all rows.
 *   Currently not fully implemented, will default to all rows.
 * @return
 *   Array of theme info. Referred to in other functions as $info.
 */
function color_get_theme(&$theme) {
  static $static;

  if (is_string($theme)) {
    $theme_name = $theme;
    $theme = array();
    $theme['name'] = $theme_name;
  }
  if (!isset($theme['id'])) {
    $theme['id'] = db_result(db_query("SELECT id FROM {color} WHERE name = '%s'", $theme['name']));
  }

  if (isset($static[$theme['name']])) {
    $theme = $static[$theme['name']];
    return;
  }

  module_load_include('inc', 'color', 'color.misc');

  // Get information regarding theme
  if ($query = db_query("SELECT * FROM {color} WHERE id = '%s'", $theme['id'])) {
    $theme = db_fetch_array($query);

    if (isset($theme['default scheme'])) {
      $default['id'] = $theme['default scheme'];
      $theme['default scheme'] = color_get_scheme($default, TRUE);
    }
    if (isset($theme['reference scheme'])) {
      $reference['id'] = $theme['reference scheme'];
      $theme['reference scheme'] = color_get_scheme($reference, TRUE);
    }
  }

  $unserialize = array('fields', 'replacement methods', 'premade schemes', 'images', 'stylesheets');
  foreach ($unserialize as $rowname) {
    if (isset($theme[$rowname])) {
      $theme[$rowname] = unserialize($theme[$rowname]);
    }
  }

  $static[$theme['name']] = $theme;
}

function color_get_theme_extras(&$theme) {
  static $static;

  if (isset($static[$theme['name']]) && count($theme['schemes']) > 1) {
    $theme = $static[$theme['name']];
    return;
  }

  if ($result = db_query("SELECT name, id, hex FROM {color_schemes} WHERE theme = '%s'", $theme['id'])) {
    while ($row = db_fetch_array($result)) {
      $theme['schemes']['hex'][$row['name']] = unserialize($row['hex']);
      $theme['schemes']['id'][$row['name']] = $row['id'];
    }

    if (isset($theme['schemes']['id'])) {
      ksort($theme['schemes']['id']);
    }
  }

  $static[$theme['name']] = $theme;
}

/**
 * Delete theme schemes, in DB and in scheme files.
 *
 * @param $theme
 *   String of theme name
 */
function color_scheme_reset($theme_name) {
  module_load_include('inc', 'color', 'color.misc');

  if (variable_get('color_hook_validator', 0) == 1 && !color_validate($info)) {
    return;
  }
  $theme['name'] = $theme_name;
  $theme['id'] = db_result(db_query("SELECT id FROM {color} WHERE name = '%s'",
                                    $theme['name'])
                           );

  db_query("DELETE FROM {color} WHERE name = '%s'", $theme['name']);
  db_query("DELETE FROM {color_schemes} WHERE theme = '%s'", $theme['id']);
  db_query("DELETE FROM {variable} WHERE name LIKE 'color_%s%'",
           $theme['name']
           );

  color_rm_dir(file_directory_path() .'/color', $theme['name']);
  variable_set("theme_color_" . $theme['name'] . "_generated", 0);
}
