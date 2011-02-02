<?php
// $Id: color.stylesheet.inc,v 1.5.2.10 2009/04/26 23:28:16 skiquel Exp $

/**
 * @file
 * Stylesheet modification functions
 */

/**
 * Rewrite the stylesheet to match the colors in the palette.
 *
 * @param $theme
 *  String of theme name
 * @param &$info
 *   Reference array of theme info
 * @param &$paths
 *   Reference array of file paths for CSS/image slices.
 * @param $palette
 *  Array of hexes
 * @param $style
 *  String of the stylesheet contents
 */
function _color_rewrite_stylesheet($theme, &$paths, $palette, $style) {
  // Split off the "Don't touch" section of the stylesheet.
  $split = "Color Module: Don't touch";
  if (strpos($style, $split) !== FALSE) {
    list($style, $fixed) = explode($split, $style);
  }

  foreach (array_keys($theme['replacement methods']) as $engine) {
    module_load_include('inc', 'color', 'extend/replace/' . $engine);
    if (function_exists('_color_extend_replace_' . $engine) == TRUE) {
      $style = call_user_func('_color_extend_replace_' . $engine, $style, $theme, $palette, $theme['replacement methods'][$engine]);
    }
  }

  // Append fixed colors segment.
  if (isset($fixed)) {
    $style .= $fixed;
  }

  // Replace paths to images.
  foreach ($paths['map'] as $before => $after) {
    $before = base_path() . $paths['source'] . $before;
    $before = preg_replace('`(^|/)(?!../)([^/]+)/../`', '$1', $before);
    $style = str_replace($before, $after, $style);
  }

  return $style;
}

/**
 * Save the rewritten stylesheet to disk.
 */
function _color_save_stylesheet($file, $style, &$paths) {
  // Write new stylesheet.
  file_save_data($style, $file, FILE_EXISTS_REPLACE);
  $paths['files'][] = $file;

  // Set standard file permissions for webserver-generated files.
  @chmod($file, 0664);
}