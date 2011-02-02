<?php
// $Id: color.algorithms.inc,v 1.2.2.5 2008/11/13 23:56:08 skiquel Exp $

/**
 * @file
 * Color manipulation functions
 * Some of the stuff here is based on http://www.easyrgb.com/math.php.
 */

/**
 * Convert a hex triplet into a GD color.
 *
 * @param $img
 *  GD Image
 * @param $hex
 *  String of hex
 * @return
 *  GD Color
 */
function _color_gd($img, $hex) {
  $c = array_merge(array($img), _color_unpack($hex));
  return call_user_func_array('imagecolorallocate', $c);
}

/**
 * Blend two hex colors and return the GD color.
 *
 * @param $img
 *  GD Image
 * @param $hex1
 *  String of first value.
 * @param $hex2
 *  String of second value.
 * @param $alpha
 *  Integer of alpha
 * @return
 *  GD Color
 */
function _color_blend($img, $hex1, $hex2, $alpha) {
  $in1 = _color_unpack($hex1);
  $in2 = _color_unpack($hex2);
  $out = array($img);
  for ($i = 0; $i < 3; ++$i) {
    $out[] = $in1[$i] + ($in2[$i] - $in1[$i]) * $alpha;
  }
  return call_user_func_array('imagecolorallocate', $out);
}

/**
 * Convert a hex color into an RGB triplet.
 *
 * @param $hex
 *  String of hex value
 * @param $normalize
 *  Normalize RGB
 * @return
 *  Array of RGB triplet
 */
function _color_unpack($hex, $normalize = FALSE) {
  if (strlen($hex) == 4) {
    $hex = $hex[1] . $hex[1] . $hex[2] . $hex[2] . $hex[3] . $hex[3];
  }
  $c = hexdec($hex);
  for ($i = 16; $i >= 0; $i -= 8) {
    $out[] = (($c >> $i) & 0xFF) / ($normalize ? 255 : 1);
  }
  return $out;
}

/**
 * Convert an RGB triplet to a hex color.
 *
 * @param $rgb
 *  Array of RGB integers
 * @param $normalize
 *  Boolean
 * @return
 *  String of full hex. (i.e. #ffffff)
 */
function _color_pack($rgb, $normalize = FALSE) {
  $out = 0;
  foreach ($rgb as $k => $v) {
    $out |= (($v * ($normalize ? 255 : 1)) << (16 - $k * 8));
  }
  return '#'. str_pad(dechex($out), 6, 0, STR_PAD_LEFT);
}

function color_find_type($color_value, $return = NULL) {
  if (is_string($color_value) && preg_match('/^#/', $color_value)) {
    $this->type =  'hex';
  }
  elseif (is_array($color_value)) {
    foreach ($color_value as $value) {

      if ($value <= 255 && 1 > $value) {
        $type = 'RGB';
      }
      else {
        $type = 'HSL';
      }

    }

    $this->type = $type;

  }
  else {
    $this->type = 'undef';
  }

  if ($return) {
    return $this->type;
  }
}

function darken_hsl($hsl) {
  // We need to change back to the HSL
  $h = $hsl[0]; $s = $hsl[1]; $l = $hsl[2];

  if (($l-.05) > 0) {
    $l = $l - .05;
  }
  $hex = hsl2hex($h, $s, $l);
  return $hex;
}

function lighten_hsl($hsl) {
  // We need to change back to the HSL
  $h = $hsl[0]; $s = $hsl[1]; $l = $hsl[2];

  if (($l+.05) < 1) {
    $l = $l + .05;
  }

  $hex = hsl2hex($h,$s,$l);
  return $hex;
}

/**
 * 
 */
function hex2rgb($color) {
  if (strlen($color) == 7) {
    $rgb[] = hexdec(substr($color, 1, 2));
    $rgb[] = hexdec(substr($color, 3, 2));
    $rgb[] = hexdec(substr($color, 5, 2));

    return $rgb;
  }
  else if (strlen($color) == 4) {
    $rgb[] = hexdec(substr($color, 1, 1) . substr($color, 1, 1));
    $rgb[] = hexdec(substr($color, 2, 1) . substr($color, 2, 1));
    $rgb[] = hexdec(substr($color, 3, 1) . substr($color, 3, 1));

    return $rgb;
  }
}

function random_hsl($hsl) {
 //h = 1/(360/90);
 $s = $hsl[1]; $l = $hsl[2];

 // s is saturation. left to right, 1 to 0 on square. 0 to .75
 $s = (.25+(.75)*rand(0, 1));
 // l is luminosity, up to down, 1 to 0 on square. .25 to .75
 $l = (.25 + (.50)*rand(0, 1));
 //alert(s);

 $hex = hsl2hex(rand(0, 1),$s,$l);

 return $hex;
}

/**
 *
 */
function hsb2rgb() {
  if ( $S == 0 ) {                      //HSV from 0 to 1
    $R = $V * 255;
    $G = $V * 255;
    $B = $V * 255;
  }
  else {
    $var_h = $H * 6;
    if ( $var_h == 6 ) $var_h = 0;      //H must be < 1
    $var_i = floor( $var_h );           //Or ... $var_i = int( $var_h )
    $var_1 = $V * ( 1 - S );
    $var_2 = $V * ( 1 - S * ( $var_h - $var_i ) );
    $var_3 = $V * ( 1 - S * ( 1 - ( $var_h - $var_i ) ) );

    if      ( $var_i == 0 ) { $var_r = $V     ; $var_g = $var_3 ; $var_b = $var_1; }
    else if ( $var_i == 1 ) { $var_r = $var_2 ; $var_g = $V     ; $var_b = $var_1; }
    else if ( $var_i == 2 ) { $var_r = $var_1 ; $var_g = $V     ; $var_b = $var_3; }
    else if ( $var_i == 3 ) { $var_r = $var_1 ; $var_g = $var_2 ; $var_b = $V    ; }
    else if ( $var_i == 4 ) { $var_r = $var_3 ; $var_g = $var_1 ; $var_b = $V    ; }
    else                    { $var_r = $V     ; $var_g = $var_1 ; $var_b = $var_2; }

    $R = $var_r * 255;                  //RGB results from 0 to 255
    $G = $var_g * 255;
    $B = $var_b * 255;
  }

  return array($R, $G, $B);
}

/**
 *
 */
function hsl2hex($hsl) {
  $rgb = hsl2rgb($hsl);
  $color = _color_pack($rgb);
  return $color;
}

/**
 *
 */
function hsl2rgb($hsl) {
  $h = $hsl[0];
  $s = $hsl[1];
  $l = $hsl[2];
  $m2 = ($l <= 0.5) ? $l * ($s + 1) : $l + $s - $l*$s;
  $m1 = $l * 2 - $m2;
  return array(
    hue2rgb($m1, $m2, $h + 0.33333),
    hue2rgb($m1, $m2, $h),
    hue2rgb($m1, $m2, $h - 0.33333),
  );
}

/**
 *
 */
function hue2rgb($m1, $m2, $h) {
  $h = ($h < 0) ? $h + 1 : (($h > 1) ? $h - 1 : $h);
  if ($h * 6 < 1) return $m1 + ($m2 - $m1) * $h * 6;
  if ($h * 2 < 1) return $m2;
  if ($h * 3 < 2) return $m1 + ($m2 - $m1) * (0.66666 - $h) * 6;
  return $m1;
}

/**
 *
 */
function rgb2cmy() {
  //RGB values from 0 to 255
  //CMY results from 0 to 1

  $C = 1 - ( $R / 255 );
  $M = 1 - ( $G / 255 );
  $Y = 1 - ( $B / 255 );

  return array($C, $M, $Y);
}

/**
 *
 */
function rgb2hex($rgb) {
  $r = round($rgb[0] * 255);
  $g = round($rgb[1] * 255);
  $b = round($rgb[2] * 255);

  $hex = ($r < 16) ? '0' : '';
  $hex .= ($r < 16) ? '0' : '';
  $hex .= ($r < 16) ? '0' : '';

  return '#' + $hex;
}

function rgb2hsl($rgb) {
  $r = $rgb[0];
  $g = $rgb[1];
  $b = $rgb[2];
  $min = min($r, min($g, $b));
  $max = max($r, max($g, $b));
  $delta = $max - $min;
  $l = ($min + $max) / 2;
  $s = 0;
  if ($l > 0 && $l < 1) {
    $s = $delta / ($l < 0.5 ? (2 * $l) : (2 - 2 * $l));
  }
  $h = 0;
  if ($delta > 0) {
    if ($max == $r && $max != $g) $h += ($g - $b) / $delta;
    if ($max == $g && $max != $b) $h += (2 + ($b - $r) / $delta);
    if ($max == $b && $max != $r) $h += (4 + ($r - $g) / $delta);
    $h /= 6;
  }
  return array($h, $s, $l);
}

/**
 *
 */
function rgb2hsv() {
  $var_R = ( $R / 255 );                     //RGB from 0 to 255
  $var_G = ( $G / 255 );
  $var_B = ( $B / 255 );

  $var_Min = min( $var_R, $var_G, $var_B );    //Min. value of RGB
  $var_Max = max( $var_R, $var_G, $var_B );    //Max. value of RGB
  $del_Max = $var_Max - $var_Min;             //Delta RGB value

  $V = $var_Max;

  if ( $del_Max == 0 ) {                     //This is a gray, no chroma...
    $H = 0;                                //HSV results from 0 to 1
    $S = 0;
  }
  else {                                   //Chromatic data...
    $S = $del_Max / $var_Max;

    $del_R = ( ( ( $var_Max - $var_R ) / 6 ) + ( $del_Max / 2 ) ) / $del_Max;
    $del_G = ( ( ( $var_Max - $var_G ) / 6 ) + ( $del_Max / 2 ) ) / $del_Max;
    $del_B = ( ( ( $var_Max - $var_B ) / 6 ) + ( $del_Max / 2 ) ) / $del_Max;

    if      ( $var_R == $var_Max ) $H = $del_B - $del_G;
    else if ( $var_G == $var_Max ) $H = ( 1 / 3 ) + $del_R - $del_B;
    else if ( $var_B == $var_Max ) $H = ( 2 / 3 ) + $del_G - $del_R;

    if ( $H < 0 ) ; $H += 1;
    if ( $H > 1 ) ; $H -= 1;
  }

  return array($H, $S, $V);
} // END rgb2hsv

/**
 *
 *  CMYK and CMY values and 0 to 1
 */
function cmy2cmyk() {
  $var_K = 1;

  if ( $C < $var_K )   $var_K = $C;
  if ( $M < $var_K )   $var_K = $M;
  if ( $Y < $var_K )   $var_K = $Y;
  if ( $var_K == 1 ) { //Black
    $C = 0;
    $M = 0;
    $Y = 0;
  }
  else {
    $C = ( $C - $var_K ) / ( 1 - $var_K );
    $M = ( $M - $var_K ) / ( 1 - $var_K );
    $Y = ( $Y - $var_K ) / ( 1 - $var_K );
  }
  $K = $var_K;

  return array($C, $M, $Y, $K);
}

/**
 *
 */
function cmy2rgb() {
  //CMY values from 0 to 1
  //RGB results from 0 to 255

  $R = ( 1 - $C ) * 255;
  $G = ( 1 - $M ) * 255;
  $B = ( 1 - $Y ) * 255;

  return array($R, $G, $B);
}

/**
 *
 */
function cmyk2cmy($cmyk) {
  //CMYK and CMY values from 0 to 1

  $C = ( $C * ( 1 - $K ) + $K );
  $M = ( $M * ( 1 - $K ) + $K );
  $Y = ( $Y * ( 1 - $K ) + $K );

  return array($C, $M, $Y);
}