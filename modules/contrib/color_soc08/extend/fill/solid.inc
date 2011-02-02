<?php
// $Id: solid.inc,v 1.1.2.1 2008/09/18 09:54:39 skiquel Exp $
/**
 * @file
 * Solid fill
 * A rectangular, solid fill.
 */

/**
 * Grab the scheme ID for a User ID
 * @param $target: The image being manipulated.
 * @param $palette: Keyed array of scheme's color palette. (i.e. field => hex)
 * @param $x: x-coordinate
 * @param $y: y-coordinate
 * @param $width: width of this solid color in pixels
 * @param $height: height of this solid color in pixels
 * @param $colors: array of colors, in hex. (i.e. #FFFFFF) Only one color for this.
 *        $colors[0]: Hex of the solid color to fill
 *
 * @return the ID for the scheme the user has chosen, or 0 if none.
 */
function _color_extend_fill_solid(&$target, $palette, $x, $y, $width, $height, $colors) {
  module_load_include('inc', 'color', 'color.algorithms');

  imagefilledrectangle($target, 
                      $x,
                      $y,
                      $x + $width,
                      $y + $height,
                      _color_gd($target, $colors[0])
                      );
}
