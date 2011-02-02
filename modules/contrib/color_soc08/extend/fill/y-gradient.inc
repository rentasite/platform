<?php
// $Id: y-gradient.inc,v 1.1.2.1 2008/09/18 09:54:39 skiquel Exp $
/**
 * @file
 * Gradient fill
 * A simple, rectangular, top to bottom gradient.
 */

/**
 * Grab the scheme ID for a User ID
 * @param $target: The image being manipulated.
 * @param $palette: Keyed array of scheme's color palette. (i.e. field => hex)
 * @param $x: x-coordinate
 * @param $y: y-coordinate
 * @param $width: width of this solid color in pixels
 * @param $height: height of this solid color in pixels
 * @param $colors: array of colors, in hex. (i.e. #FFFFFF) This is 2 colors.
 *        $colors[0]: Top color
 *        $colors[1]: Bottom color
 *
 * @return the ID for the scheme the user has chosen, or 0 if none.
 */
function _color_extend_fill_y_gradient(&$target, $palette, $x, $y, $width, $height, $colors) {
  module_load_include('inc', 'color', 'color.algorithms');

  // Generates the gradient line by line. Left to right. Next line. Left to right. Done.
  for ($y_current = 0; $y_current < $height; ++$y_current) {
    $color = _color_blend($target,
                          $colors[0], // Top color
                          $colors[1], // Bottom color
                          $y_current / ($height - 1)
                          );
                          
    imagefilledrectangle($target,
                         $x,
                         $y + $y_current,
                         $x + $width,
                         $y + $y_current + 1,
                         $color
                        );
  }
}