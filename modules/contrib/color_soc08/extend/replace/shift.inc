<?php
// $Id: shift.inc,v 1.1.2.6 2009/04/26 23:28:17 skiquel Exp $
/**
 * @file
 * Shift engine
 * Shifts colors based on 'base' color
 */
 
/**
 * Replace stylesheet's every color with blended color.
 * calculates + replaces (indivudual color + base color) => blended color!
 * @param $style: The style string
 * @param $theme: Array of theme information
 * @param $palette: Array of colors
 * @param $options: Unused, allows color extensions to enter custom params.
 *
 * @return (possibly big) string of the modified stylesheet.
 */
function _color_extend_replace_shift($style, $theme, $palette, $options) {
  module_load_include('inc', 'color', 'color.misc');
  
  $link = (array_search('link', $options) !== FALSE) ? TRUE : FALSE;
  $text = (array_search('text', $options) !== FALSE) ? TRUE : FALSE;
  
  // Prepare color conversion table
  $conversion = $palette;

  foreach ($conversion as $k => $v) {
    $conversion[$k] = drupal_strtolower($v);
  }
  // Original spot is right here, I'm moving up so i can see it in the debug
  // Bind reference hex to fields
  $reference = color_array_combine($theme['fields'],
																																			$theme['reference scheme']['hex']
																																			);
  // Find all colors in the stylesheet and the chunks in between.
  $style = preg_split('/(#[0-9a-f]{6}|#[0-9a-f]{3})/i',
																						$style,
																						-1,
																						PREG_SPLIT_DELIM_CAPTURE);
  $is_color = FALSE;
  $output = '';
  $base = 'base';
  foreach ($style as $chunk) {
    if ($is_color) {
      $chunk = drupal_strtolower($chunk);
      // Check if this is one of the colors in the default palette.
      if ($key = array_search($chunk, $reference)) {
        $chunk = $conversion[$key];
      }
      // Not a pre-set color. Extrapolate from the base.
      else {
        $chunk = _color_shift($palette[$base],
															$reference[$base],
															$chunk,
															$theme['blend target']
															);        
      }
    }
    else {
      // Determine the most suitable base color for the next color.

      // 'a' declarations. Use link.
      if ($link && preg_match('@[^a-z0-9_-](a)[^a-z0-9_-][^/{]*{[^{]+$@i',
															$chunk)
				 ) {
        $base = 'link';
      }
      // 'color:' styles. Use text.
      else if ($text && preg_match('/(?<!-)color[^{:]*:[^{#]*$/i', $chunk)) {
        $base = 'text';
      }
      // Reset back to base.
      else {
        $base = 'base';
      }
    }
    $output .= $chunk;
    $is_color = !$is_color;
  }

  $style = $output;

	return $style;
}


/**
 * Shift a given color, using a reference pair and a target blend color.
 *
 * Note: this function is significantly different from the JS version, as it
 * is written to match the blended images perfectly.
 *
 * Constraint: if (ref2 == target + (ref1 - target) * delta) for some fraction
 * 							delta then (return == target + (given - target) * delta)
 *
 * Loose constraint: Preserve relative positions in saturation and luminance
 *                   space.
 */

// $chunk = _color_shift($palette[$base], $reference[$base], $chunk, $theme['blend target']);        

function _color_shift($given, $ref1, $ref2, $target) {
  module_load_include('inc', 'color', 'color.algorithms');
								//drupal_set_message("given: {$given} | ref1: {$ref1} | ref2: {$ref2} | target: {$target} ");

  // We assume that ref2 is a blend of ref1 and target and find
  // delta based on the length of the difference vectors:

  // delta = 1 - |ref2 - ref1| / |white - ref1|
  $target = _color_unpack($target, TRUE);
  $ref1 = _color_unpack($ref1, TRUE);
  $ref2 = _color_unpack($ref2, TRUE);	




  $numerator = 0;
  $denominator = 0;
  for ($i = 0; $i < 3; ++$i) {
    $numerator += ($ref2[$i] - $ref1[$i]) * ($ref2[$i] - $ref1[$i]);
    $denominator += ($target[$i] - $ref1[$i]) * ($target[$i] - $ref1[$i]);
  }
  $delta = ($denominator > 0) ? (1 - sqrt($numerator / $denominator)) : 0;

  // Calculate the color that ref2 would be if the assumption was TRUE.
  for ($i = 0; $i < 3; ++$i) {
    $ref3[$i] = $target[$i] + ($ref1[$i] - $target[$i]) * $delta;
  }

  // If the assumption is not TRUE, there is a difference between ref2 and ref3.
  // We measure this in HSL space. Notation: x' = hsl(x).
  $ref2 = rgb2hsl($ref2);
  $ref3 = rgb2hsl($ref3);
  for ($i = 0; $i < 3; ++$i) {
    $shift[$i] = $ref2[$i] - $ref3[$i];
  }

  // Take the given color, and blend it towards the target.
  $given = _color_unpack($given, TRUE);
  for ($i = 0; $i < 3; ++$i) {
    $result[$i] = $target[$i] + ($given[$i] - $target[$i]) * $delta;
  }

  // Finally, we apply the extra shift in HSL space.
  // Note: if ref2 is a pure blend of ref1 and target, then |shift| = 0.
  $result = rgb2hsl($result);

  for ($i = 0; $i < 3; ++$i) {
    $result[$i] = min(1, max(0, $result[$i] + $shift[$i]));
  }
  $result = hsl2rgb($result);

  // Return hex color.
  return _color_pack($result, TRUE);
}