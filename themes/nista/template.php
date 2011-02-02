<?php
// $Id: template.php,v 1.2.2.8.2.5 2009/05/12 12:13:36 skiquel Exp $

/**
 * @file
 * Contains theme override functions and preprocess functions for the theme.
 *
 * ABOUT THE TEMPLATE.PHP FILE
 *
 *   The template.php file is one of the most useful files when creating or
 *   modifying Drupal themes. You can add new regions for block content, modify
 *   or override Drupal's theme functions, intercept or make additional
 *   variables available to your theme, and create custom PHP logic. For more
 *   information, please visit the Theme Developer's Guide on Drupal.org:
 *   http://drupal.org/theme-guide
 *
 * OVERRIDING THEME FUNCTIONS
 *
 *   The Drupal theme system uses special theme functions to generate HTML
 *   output automatically. Often we wish to customize this HTML output. To do
 *   this, we have to override the theme function. You have to first find the
 *   theme function that generates the output, and then "catch" it and modify it
 *   here. The easiest way to do it is to copy the original function in its
 *   entirety and paste it here, changing the prefix from theme_ to nista_.
 *   For example:
 *
 *     original: theme_breadcrumb()
 *     theme override: nista_breadcrumb()
 *
 *   where nista is the name of your sub-theme. For example, the
 *   zen_classic theme would define a zen_classic_breadcrumb() function.
 *
 *   If you would like to override any of the theme functions used in Zen core,
 *   you should first look at how Zen core implements those functions:
 *     theme_breadcrumbs()      in zen/template.php
 *     theme_menu_item_link()   in zen/template.php
 *     theme_menu_local_tasks() in zen/template.php
 *
 *   For more information, please visit the Theme Developer's Guide on
 *   Drupal.org: http://drupal.org/node/173880
 *
 * CREATE OR MODIFY VARIABLES FOR YOUR THEME
 *
 *   Each tpl.php template file has several variables which hold various pieces
 *   of content. You can modify those variables (or add new ones) before they
 *   are used in the template files by using preprocess functions.
 *
 *   This makes THEME_preprocess_HOOK() functions the most powerful functions
 *   available to themers.
 *
 *   It works by having one preprocess function for each template file or its
 *   derivatives (called template suggestions). For example:
 *     THEME_preprocess_page    alters the variables for page.tpl.php
 *     THEME_preprocess_node    alters the variables for node.tpl.php or
 *                              for node-forum.tpl.php
 *     THEME_preprocess_comment alters the variables for comment.tpl.php
 *     THEME_preprocess_block   alters the variables for block.tpl.php
 *
 *   For more information on preprocess functions and template suggestions,
 *   please visit the Theme Developer's Guide on Drupal.org:
 *   http://drupal.org/node/223440
 *   and http://drupal.org/node/190815#template-suggestions
 */


/*
 * Add any conditional stylesheets you will need for this sub-theme.
 *
 * To add stylesheets that ALWAYS need to be included, you should add them to
 * your .info file instead. Only use this section if you are including
 * stylesheets based on certain conditions.
 */
/* -- Delete this line if you want to use and modify this code
// Example: optionally add a fixed width CSS file.
if (theme_get_setting('nista_fixed')) {
  drupal_add_css(path_to_theme() . '/layout-fixed.css', 'theme', 'all');
}
// */


/**
 * Implementation of HOOK_theme().
 */
function nista_theme(&$existing, $type, $theme, $path) {
  $hooks = zen_theme($existing, $type, $theme, $path);
  // Add your theme hooks like this:
  /*
  $hooks['hook_name_here'] = array( // Details go here );
  */
  // @TODO: Needs detailed comments. Patches welcome!
  return $hooks;
}

/**
 * Override or insert variables into all templates.
 *
 * @param $vars
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered (name of the .tpl.php file.)
 */
/* -- Delete this line if you want to use this function
function nista_preprocess(&$vars, $hook) {
  $vars['sample_variable'] = t('Lorem ipsum.');
}
// */

/**
 * Override or insert variables into the page templates.
 *
 * @param $vars
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("page" in this case.)
 */
/* -- Delete this line if you want to use this function
function nista_preprocess_page(&$vars, $hook) {
  $vars['sample_variable'] = t('Lorem ipsum.');
}
// */

/**
 * Override or insert variables into the node templates.
 *
 * @param $vars
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("node" in this case.)
 */
/* -- Delete this line if you want to use this function
function nista_preprocess_node(&$vars, $hook) {
  $vars['sample_variable'] = t('Lorem ipsum.');
}
// */

/**
 * Override or insert variables into the comment templates.
 *
 * @param $vars
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("comment" in this case.)
 */
/* -- Delete this line if you want to use this function
function nista_preprocess_comment(&$vars, $hook) {
  $vars['sample_variable'] = t('Lorem ipsum.');
}
// */

/**
 * Override or insert variables into the block templates.
 *
 * @param $vars
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("block" in this case.)
 */
/* -- Delete this line if you want to use this function
function nista_preprocess_block(&$vars, $hook) {
  $vars['sample_variable'] = t('Lorem ipsum.');
}
// */

function nista_comment_submitted($comment) {
  return t('by <strong>!username</strong> | !datetime',
    array(
      '!username' => theme('username', $comment),
      '!datetime' => format_date($comment->timestamp)
    ));
}

function nista_node_submitted($node) {
  return t('!datetime | by <strong>!username</strong>',
    array(
      '!username' => theme('username', $node),
      '!datetime' => format_date($node->created),
    ));
}

if ($GLOBALS['theme'] == 'nista') { // If we're in the main theme
  if (theme_get_setting('nista_layout') == 'border-politics-fixed') {
    drupal_add_css(drupal_get_path('theme', 'nista') . '/layout-fixed.css', 'theme', 'all');
  }
  else {
    drupal_add_css(drupal_get_path('theme', 'nista') . '/layout-liquid.css', 'theme', 'all');
  }
}

/**
 *  Implements hook_color()
 *  Color replacement methods: 'shift' (base, link, text), 'tag' (navbar, navbarlink)
 *                            custom tag replacement
 *  Image generation: Yes (header)
 */
function nista_color() {
	/**
	 *  $color = array() (Required)
	 *  For essential scheme information.
	 */
	$color = array(
	  /**
	   *  'replacement methods'
	   *  Declare replacement replacement methods in the 'replacement methods' array,
	   *  in the order you want them to work.
	   *    They are comprised different algorithms to change your colors.
	   *  replacement methods like 'shift' can accept different options and behave differently.
	   *  To change the order of the replacement methods color replacement, put them in order.
	   */
	  'replacement methods' => array(
	    /**
	     *  'shift' method
	     *  Requires 'base-only', or 'base', and/or'link' and/or 'text'
	     */
	    'shift' => array('base', 'link', 'text'),
	    /**
	     *  'tag' method
	     *  Replaces custom tags in stylesheet based on fieldname.
	     */
	    'tag' => TRUE,
	  ),

	  /**
	   *  'fields' (Required)
	   *  The machine-readable names of color fields. The name will help you
	   *  and your theme users identify colors.
	   */
	  'fields' => array('base', 'link', 'top', 'bottom', 'text', 'navbar', 'navbarlink'),

	  /**
	   *  'premade schemes' (Required)
	   *  A list of pre-made schemes that will be available for install by default.
	   *  The scheme name, human-readable, is followed by hex values separated by
	   *  commas (no spaces). The colors are to be ordered respectable to 'fields'.
	   */
	  'premade schemes' => array(
	    'Aquamarine' => array('base' => '#55c0e2', 'link' => '#000000', 'top' => '#085360', 'bottom' => '#007e94', 'text' => '#696969', 'navbar' => '#000000', 'navbarlink' => '#666666'),
	    'Ash' => array('base' => '#464849', 'link' => '#2f416f', 'top' => '#b3b7b8', 'bottom' => '#262b32', 'text' => '#494949', 'navbar' => '#000000', 'navbarlink' => '#dbdbdb'),
	    'Belgian Chocolate' => array('base' => '#d5b048', 'link' => '#3e2608', 'top' => '#331900', 'bottom' => '#971702', 'text' => '#494949', 'navbar' => '#000000', 'navbarlink' => '#666666'),
	    'Blue Lagoon' => array('base' => '#0072b9', 'link' => '#027ac6', 'top' => '#2385c2', 'bottom' => '#5ab5ee', 'text' => '#494949', 'navbar' => '#00436e', 'navbarlink' => '#07a0ff'),
	    'Bluemarine' => array('base' => '#3f3f3f', 'link' => '#336699', 'top' => '#6598cb', 'bottom' => '#6598cb', 'text' => '#000000', 'navbar' => '#000000', 'navbarlink' => '#666666'),
	    'Citrus Blast' => array('base' => '#e08800', 'link' => '#914d03', 'top' => '#f99d01', 'bottom' => '#fbd028', 'text' => '#494949', 'navbar' => '#f99d01', 'navbarlink' => '#4c4c4c'),
	    'Cold Day' => array('base' => '#0f005c', 'link' => '#434f8c', 'top' => '#4d91ff', 'bottom' => '#1a1575', 'text' => '#000000', 'navbar' => '#1a1575', 'navbarlink' => '#666666'),
	    'Greenbeam' => array('base' => '#a8c997', 'link' => '#0c7a00', 'top' => '#03961e', 'bottom' => '#7be000', 'text' => '#494949', 'navbar' => '#0c7a00', 'navbarlink' => '#b4b4b4'),
	    'Majestic' => array('base' => '#5c00b9', 'link' => '#5c00b9', 'top' => '#f028ab', 'bottom' => '#5f009e', 'text' => '#494949', 'navbar' => '#420087', 'navbarlink' => '#666666'),
	    'Mediterrano' => array('base' => '#ffe23d', 'link' => '#a9290a', 'top' => '#fc6d1d', 'bottom' => '#a30f1d', 'text' => '#494949', 'navbar' => '#000000', 'navbarlink' => '#666666'),
	    'Mercury' => array('base' => '#4a5461', 'link' => '#242424', 'top' => '#a9adbc', 'bottom' => '#d4d4d4', 'text' => '#707070', 'navbar' => '#d4d4d4', 'navbarlink' => '#666666'),
	    'Nocturnal' => array('base' => '#5b5fa9', 'link' => '#5b5faa', 'top' => '#0a2352', 'bottom' => '#9fa8d5', 'text' => '#494949', 'navbar' => '#0a2352', 'navbarlink' => '#666666'),
	    'Olivia' => array('base' => '#7db323', 'link' => '#4b6d0f', 'top' => '#b5d52a', 'bottom' => '#7db323', 'text' => '#191a19', 'navbar' => '#000000', 'navbarlink' => '#666666'),
	    'Pink Plastic' => array('base' => '#f41063', 'link' => '#651b54', 'top' => '#f391c6', 'bottom' => '#f41063', 'text' => '#898080', 'navbar' => '#000000', 'navbarlink' => '#666666'),
	    'Shiny Tomato' => array('base' => '#ff1a29', 'link' => '#c70000', 'top' => '#a1443a', 'bottom' => '#f21107', 'text' => '#515d52', 'navbar' => '#c70000', 'navbarlink' => '#9a9a9a'),
	    'SR-71' => array('base' => '#595959', 'link' => '#9a4c00', 'top' => '#333333', 'bottom' => '#9e9e9e', 'text' => '#696969', 'navbar' => '#000000', 'navbarlink' => '#666666'),
	    'Teal Top' => array('base' => '#228059', 'link' => '#0f3726', 'top' => '#34775a', 'bottom' => '#52bf90', 'text' => '#2d2d2d', 'navbar' => '#18583d', 'navbarlink' => '#8d8d8d'),
	  ),

	  /**
	   *  'reference scheme' (Required)
	   *  A scheme name from one in your 'premade schemes' list.
	   *  On schemes which use the 'base', 'link' or 'text' flag, always mention this.
	   *  Other schemes will be shifted against the fields of the reference one.
	   */
	  'reference scheme' => 'SR-71',

	  /**
	   *  'default scheme' (Required)
	   *  A scheme name from one in your 'premade schemes' list.
	   */
	  'default scheme' => 'SR-71',

	  /**
	   *  'stylesheets' (Required)
	   *  CSS file relative to the root of your theme.
	   *  Can mention an array of stylesheets.
	   */
	  'stylesheets' => array(
	    'nista.css',
	  ),

	  /**
	   *  'blend target' (Required)
	   *  Used as a target color for image and shifting colors to and fro.
	   *  Usually #ffffff, #000000, or #666666 can do.
	   */
	  'blend target' => '#666666',
	);

	/**
	 *  $img['images'][imgname] (Required)
	 *  Transparent images you wish to fill the BG's of.
	 *  And/or slice. List as many as you want, keep the imgname unique.
	 */
	$color['images']['header'] = array(
	  /**
	   *  'file' (Required)
	   *  Path to your source image in terms of theme's root.
	   */
	  'file' => 'images/color/bg-header.png',

	  /**
	   *  'fill' (Optional)
	   *  Solid: x-coordinate, y-coordinate, width, height, fill color
	   *  Gradient: x-coordinate, y-coordinate, width, height, top color,
	   *  bottom color
	   *  You can list as many solid/gradient fills as you want
	   */
	  'fill' => array(
	    array('type' => 'y-gradient',
                  'x' => 0,
                  'y' => 0,
                  'width' => 1,
                  'height' => 180,
                  'colors' => array('top', 'bottom'),
                  ),
	  ),

	  /**
	   *  'slices' (Required)
	   *  Slice your image up to file destination.
	   *  If your slice output file matches a CSS image BG it will be
	   *  replaced
	   *  automatically for you per scheme. :)
	   *  file-path according theme root  => x-coordinate, y-coordinate,
	   *  width, height
	   */
	  'slices' => array(
	    'images/background-header.png' => array(
                                                    'x' => 0,
                                                    'y' => 0,
                                                    'width' => 1,
                                                    'height' => 180
                                                    ),
	  )
	);

	return $color;
}
