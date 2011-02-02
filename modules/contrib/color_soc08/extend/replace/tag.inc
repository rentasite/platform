<?php
// $Id: tag.inc,v 1.1.2.8 2009/04/26 23:28:17 skiquel Exp $
/**
 * @file
 * Tag engine
 * Finds and replaces hexes based on a tags in CSS
 * Not coincidentally, the tags replaced are the field names. :)
 */

/**
 * Replaces colors in stylesheets based up custom fields + stylesheet tags.
 * @param $style: The style string
 * @param $theme: Array of theme information
 * @param $palette: Array of colors
 * @param $options: Unused, allows color extensions to enter custom params.
 *
 * @return
 *		String of the modified stylesheet.
 */
function _color_extend_replace_tag($style, $theme, $palette, $options = NULL) {
	
		// Regex by preceding /* t:tagname */
		// Example: color: #FFFFFF /* t:tagname */
  $regex = "~(#[0-9a-f]{3,6})(?:.*;\s*)(?:/\*\s*(?:t:(?:\s*)(.*)(?:\s))\*/)~i";
		// Returns: Hex (#FFFFFF), tag name (no spaces)

		// (Addition to above) Regex to grab description
		// ~(#[0-9a-f]{3,6})(?:.*;\s*)(?:/\*\s*t:(\S*)\s*(?:d:([^/*]*)\s)?\s*\*/)~i
		// Returns: Hex (#FFFFFF), tag name (no spaces), Optionally description

  // Deprecated: Tags enclosed in commented brackets
  // Example: color: /* {{tag}} */#FFFFFF/* {{/tag}} */
  // ~(?:/\*\s*\{\{\s*([^/*]+)\s*\}\}\s*\*/)?\s*(#[0-9a-f]{3,6})\s*(?:/\*\s*\{\{\s*\/?\s*([^/*]+)\s*\}\}\s*\*/)?~is"

  _color_switch('', $palette);
  $style = preg_replace_callback($regex, "_color_switch", $style);

  return $style;
}

/**
 * Process matches in CSS, if match found is part of palette, enter
 * the new color.
 *
 * @param $matches
 *  Array of regular expression fields
 *  	First field is the whole match.
 *  	The rest are the returned matches
 * @param $static
 *  Array of field names + hexes for the scheme (palette)
 *
 * @return
 *  String of replaced hex or unaltered match (tag will remain untouched).
 */
function _color_switch($matches, $static = FALSE) {
  static $palette;
  module_load_include('inc', 'color', 'color.misc');

  if ($static != FALSE) {
    $palette = $static;
  	$palette['DT!'] = '';		
  }
  else {
    if ($match = array_intersect(array_keys($palette), array_values($matches))) {
						$matches['0'] = str_ireplace($matches['1'],
						                             $palette[$matches['2']],
						                             $matches['0']);
    }
  }
		
  return $matches[0];
}