<?php
// $Id: color.image.inc,v 1.4.2.12 2008/11/19 12:08:49 skiquel Exp $

/**
 * @file
 * Provides image manipulation functions
 */

/**
 * Generate images
 *
 * @param $theme
 *   String with theme name
 * @param $info
 *   Array with theme info
 * @param $paths
 *   Array containing file paths to files
 * @param $palette
 *   Array of hex values. i.e. array('#ffffff', '#000000')
 * @return
 *   Array of generated image file paths
 */
function _color_render_images($theme, &$paths, $palette) {
  module_load_include('inc', 'color', 'color.algorithms');
  module_load_include('inc', 'color', 'color.database');

  foreach ($theme['images'] as $img) {
    // Prepare template image.
    $source = $paths['source'] .'/'. $img['file'];
    $source = imagecreatefrompng($source);

    imagealphablending($source, FALSE);
    imagesavealpha($source, TRUE);

    $width = imagesx($source);
    $height = imagesy($source);

    // Prepare target buffer.
    $target = imagecreatetruecolor($width, $height);
    imagealphablending($target, TRUE);

    if (isset($img['fill']) && count($img['fill']) > 0) {
      foreach ($img['fill'] as $fill) {
        $type = $fill['type'];
        
        $colors_pre = $fill['colors'];
        $colors = array();

        foreach ($colors_pre as $key) {
          if (isset($palette[$key])) {
            $colors[] = $palette[$key];
          }
        }

        module_load_include('inc', 'color', 'extend/fill/' . $type);
        $type = str_replace('-', '_', $type);
        if (function_exists('_color_extend_fill_' . $type) == TRUE) {
          call_user_func('_color_extend_fill_' . $type, $target, $palette, $fill['x'], $fill['y'], $fill['width'], $fill['height'], $colors);
        }
      }
    }

    // Blend over template.
    imagecopy($target, $source, 0, 0, 0, 0, $width, $height);

    // Clean up template image.
    imagedestroy($source);

    // Cut out slices.
    foreach ($img['slices'] as $file => $coord) {
      $x = $coord['x'];
      $y = $coord['y'];
      $width = $coord['width'];
      $height = $coord['height'];
      $base = basename($file);
      $image = $paths['target'] . $base;

      // Cut out slice.
      if ($file == 'screenshot.png') {
        $slice = imagecreatetruecolor(150, 90);
        imagecopyresampled($slice, $target, 0, 0, $x, $y, 150, 90, $width, $height);
        $scheme['screenshot'] = $image;
        //variable_set('color_'. $theme['name'] .'_screenshot', $image);
      }
      else {
        $slice = imagecreatetruecolor($width, $height);

        imagealphablending($target, TRUE);
        imagecopy($slice, $target, 0, 0, $x, $y, $width, $height);
      }

      // Save image.
      if (isset($img['transparent color']) && !empty($img['transparent color'])) {
        imagecolortransparent($slice, $black);
      }
      imagepng($slice, $image);
      imagedestroy($slice);
      $paths['images'][] = $image;

      // Set standard file permissions for webserver-generated files
      @chmod(realpath($image), 0664);

      // Build before/after map of image paths.
      $paths['map'][$file] = $base;
    }

    // Clean up target buffer.
    imagedestroy($target);
  }

//  $paths += $paths;
}