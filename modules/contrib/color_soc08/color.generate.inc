<?php
// $Id: color.generate.inc,v 1.1.2.7 2009/04/26 17:19:57 skiquel Exp $

/**
 * @file
 * Function to generate schemes
 */

/**
 * Generate a new scheme, including stylesheet(s) and image(s).
 * Also updates {color_scheme} row generated earlier (immediately after
 * fresh theme added, or modifying new scheme). If fresh, will populate
 * the scheme_id's 'extra_attr' column, if modifying current, will update.
 * Will change the name of current scheme.
 *
 * @param $theme
 *   String of theme name.
 * @param $hex
 *   Array of hex codes for scheme to be added. array('#ffffff', '#000000')
 * @param $scheme_id
 *   Optional integer of scheme id
 * @param $schemename
 *   Optional string, for modifying scheme name.
 */
function color_generate_scheme_files(&$scheme, $theme) {
  module_load_include('inc', 'color', 'color.database');
  module_load_include('inc', 'color', 'color.misc');
  module_load_include('inc', 'color', 'color.stylesheet');

  $reference = $theme['reference scheme'];
  $palette = $scheme['hex'];

  // Prepare target locations for generated files.
  $id = $theme['name'] .'-';
  $id .= (isset($theme['set_preview']) && $theme['set_preview'] === TRUE) ? 'preview': $scheme['name'];
  $id .= '-'. substr(md5(serialize($palette) . microtime()), 0, 8);
  // Normalize $id
  $id = color_normalize($id);

  $paths['color'] = file_directory_path() .'/color';
  $paths['target'] = $paths['color'] .'/'. $id;

  foreach ($paths as $path) {
    file_check_directory($path, FILE_CREATE_DIRECTORY);
  }
  $paths['target'] = $paths['target'] .'/';
  $paths['id'] = $id;
  $paths['source'] = drupal_get_path('theme', $theme['name']) .'/';
  $paths['files'] = $paths['map'] = array();

  // Copy over neutral images.
  if (isset($theme['copy']) && is_array($theme['copy'])) {
    foreach ($theme['copy'] as $file) {
      $base = basename($file);
      $source = $paths['source'] . $file;

      file_copy($source, $paths['target'] . $base);
      $paths['map'][$file] = $base;
      $paths['files'][] = $paths['target'] . $base;
    }
  }

  // Render new images, if image provided.
  if (isset($theme['images']) && is_array($theme['images'])) {
    module_load_include('inc', 'color', 'color.image');
    _color_render_images($theme, $paths, $palette);
  }

  // Rewrite theme stylesheets.
  foreach ($theme['stylesheets'] as $stylesheet) {
    // Build a temporary array with LTR and RTL files.
    $files = array();
    if (file_exists($paths['source'] . $stylesheet)) {
      $files[] = $stylesheet;

      $rtl_file = str_replace('.css', '-rtl.css', $stylesheet);
      if (file_exists($paths['source'] . $rtl_file)) {
        $files[] = $rtl_file;
      }
    }

    foreach ($files as $file) {
      // Aggregate @imports recursively for each configured top level CSS file
      // without optimization. Aggregation and optimization will be
      // handled by drupal_build_css_cache() only.
      $style = drupal_load_stylesheet($paths['source'] . $file, FALSE);

      // Return the path to where this CSS file originated from, stripping
      // off the name of the file at the end of the path.
      // Also strip off the color/ dir. if it's there.
      $file = str_replace("color/", NULL, $file);
      $base = base_path() . dirname($paths['source'] . $file) .'/';
      _drupal_build_css_path(NULL, $base);

      // Prefix all paths within this CSS file, ignoring absolute paths.
      $style = preg_replace_callback('/url\([\'"]?(?![a-z]+:|\/+)([^\'")]+)[\'"]?\)/i', '_drupal_build_css_path', $style);

      // Rewrite stylesheet with new colors.
      module_load_include('inc', 'color', 'color.stylesheet');
      $style = _color_rewrite_stylesheet($theme, $paths, $palette, $style);
      $base_file = basename($file);
      $css[] = $paths['target'] . $base_file;
      _color_save_stylesheet($paths['target'] . $base_file, $style, $paths);
    }
  }

  // Find screenshot.png slice/copy
  foreach ($paths['map'] as $img => $val) {
    if (strpos('screenshot.png', $val) !== FALSE) {
      $screenshot = $val;
      break;
    }
  }

  foreach ($paths['map'] as $img => $val) {
    if (strpos('logo.png', $val) !== FALSE) {
      $logo = $val;
      break;
    }
  }

  // Find logo.png slice/copy
  if (isset($logo)) {
    $scheme['logo'] = $paths['target'] . $logo;
  }
  else {
    $scheme['logo'] = $paths['source'] . "logo.png";
  }

  $scheme['stylesheets'] = $css;
  $scheme['images'] = $paths['images'];
  $scheme['path'] = $paths['target'];

}

/**
 *  For PHP 4 compatibility, make a dir recursively
 *  PHP5's mkdir has a recusive option.
 *
 */
function mkdir_recursive($pathname, $mode)
{
    is_dir(dirname($pathname)) || mkdir_recursive(dirname($pathname), $mode);
    return is_dir($pathname) || @mkdir($pathname, $mode);
}