// $Id: farbtastic.js,v 1.1.2.2 2008/10/24 09:34:36 skiquel Exp $
// Farbtastic 1.2

jQuery.fn.farbtastic = function (callback) {
  $.farbtastic(this, callback);
  return this;
};

jQuery.farbtastic = function (container, callback) {
  var container = $(container).get(0);
  return container.farbtastic || (container.farbtastic = new jQuery._farbtastic(container, callback));
};

jQuery._farbtastic = function (container, callback) {
  // Store farbtastic object
  var fb = this;

  // Store latest colors array
  var latestColors = [];

  // Insert markup
  $(container).html('<div class="farbtastic"><div class="color"></div><div class="wheel"></div><div class="overlay"></div><div class="h-marker marker"></div><div class="sl-marker marker"></div></div>');
  var e = $('.farbtastic', container);
  fb.wheel = $('.wheel', container).get(0);
  // Dimensions
  fb.radius = 84;
  fb.square = 100;
  fb.width = 194;

  // Fix background PNGs in IE6
  if (navigator.appVersion.match(/MSIE [0-6]\./)) {
    $('*', e).each(function () {
      if (this.currentStyle.backgroundImage != 'none') {
        var image = this.currentStyle.backgroundImage;
        image = this.currentStyle.backgroundImage.substring(5, image.length - 2);
        $(this).css({
          'backgroundImage': 'none',
          'filter': "progid:DXImageTransform.Microsoft.AlphaImageLoader(enabled=true, sizingMethod=crop, src='" + image + "')"
        });
      }
    });
  }

  /*
   * NEW: latestColors
   */
  fb.addPrevColor = function (color) {

    if (latestColors.length >= 6) {
      latestColors = latestColors.splice(1,5);
    }
    latestColors.push(color);
  }

  fb.listPrevColors = function () {
    //alert(latestColors);
    var comboboxes = "", i, element, hsl, hex, color;

    for (i = 0; i<latestColors.length; i++) {
      element = "prev"+i;
      hex = latestColors[i];
      hsl = fb.unpack(latestColors[i]);
      color = hsl[2] > 0.5 ? '#000' : '#fff';
      comboboxes = comboboxes + "<input type=\"text\" id=\"" + element + "\" name=\"" + element + "\" value=\"" + hex + "\" style=\"background-color: "+hex+"; color: " + color + "\" class=\"prevbox\"/>";
    }

    $('#prevcolors').html(comboboxes);

    $('.prevbox').focus(function(){
      var hex = $(this).val();
      $.farbtastic('#picker').setColor(hex);
    });

  }

  fb.undoColor = function () {
    fb.setColor(latestColors[latestColors.length]);
  }

  /*
   * NEW: updateCompatibleColors
   */
  fb.updateCompatibleColors = function () {
    var hsl, type, element,comboboxes = "", hex;

    type = $('#selectsuggestedcolors').val();

    hsl = fb.hsl;
    colors = fb.findColors(hsl, type);

    for (i = 0; i<colors.length; i++) {
      element = "combo"+i;
      hex = fb.HSLToHex(colors[i]);
      color = colors[i][2] > 0.5 ? '#000' : '#fff';
      comboboxes = comboboxes + "<input type=\"text\" class=\"suggestedbox\" id=\"" + element + "\" name=\"" + element + "\" value=\"" + hex + "\" style=\"background-color: "+hex+"; color: " + color + "\" />\n";
    }
    //comboboxes = comboboxes + '<input id="combo3" class="suggestedbox" type="text" style="background-color: rgb(0, 18, 52); color: rgb(255, 255, 255);" value="#000000" name="combo3"/>';

    $('#suggestedcolors').html(comboboxes);

    $('input[@class=suggestedbox]').focus(function(){
      var hex = $(this).val();
      $.farbtastic('#picker').setColor(hex);
    });

  }

  /*
   * NEW: Update debug
   */
  fb.updateDebug = function () {
    hsl = fb.unpack(fb.color);
    h = hsl[0];
    s = hsl[1];
    l = hsl[2];

    $('#debugstuff').html(
      "<b>h</b>ue:" + h + "<br />" +
      "<b>s</b>aturation:" + s + "<br />" +
      "<b>l</b>ightness:" + l + "<br />"
      );

  }

  /**
   * Find colors
   */
  fb.findColors = function(hsl, kind) {
    //var angle = hsl[0] * 6.28;
    var baseangle = hsl[0] * 360;
    var angles;
    var colors = [];
    //alert ("angle: "+angle);

    if (kind == "analogous") angles = [0, 30, 330];
    else if (kind == "complementary") angles = [0,180];
    else if (kind == "split-complementary") angles = [0,150,210];
    else if (kind == "triadic") angles = [0,120,24];
    else if (kind == "tetradic") angles = [0,90,180,270];
    else if (kind == "neutral") angles = [0,15,345];
    else if (kind == "clash") angles = [0,90,180];
    else if (kind == "four-tone") angles = [0,60,180,240];
    else if (kind == "five-tone") angles = [0,120,160,210,240];
    else if (kind == "six-tone") angles = [0,30,120,150,240,270];

    var currentPos;
    currentPos = baseangle;
    for (i = 0; i<angles.length; i++) {

      colors[i] = currentPos + angles[i];
      //currentPos = colors[i];

      if (colors[i] > 360) {
        colors[i] = colors[i] - 360;
      }
      colors[i] = 1/(360/colors[i]);
      colors[i] = [colors[i],hsl[1],hsl[2]];
    }

    /* for (i = 0; i<angles.length; i++) {
      colors[i] = baseangle + angles[i];
      if (colors[i] > 360) {
        colors[i] = colors[i] - 360;
      }
      colors[i] = 1/(360/colors[i]);
      colors[i] = [colors[i],hsl[1],hsl[2]];
    } */
    return colors;

    /*
    color1 = hsl;

    color2 = angle + 30;
    if (color2 > 360) {
      color2 = color2 - 360;
    }
    color2 = 1/(360/color2);
    color2 = [color2, hsl[1], hsl[2]];

    color3 = angle + 330;
    if (color3 > 360) {
      color3 = color3 - 360;
    }
    color3 = 1/(360/color3);
    color3 = [color3, hsl[1], hsl[2]];

  }

    return [color1, color2, color3] */
  }

  /**
   * Link to the given element(s) or callback.
   */
  fb.linkTo = function (callback) {
    // Unbind previous nodes
    if (typeof fb.callback == 'object') {
      $(fb.callback).unbind('keyup', fb.updateValue);
    }

    // Reset color
    fb.color = null;

    // Bind callback or elements
    if (typeof callback == 'function') {
      fb.callback = callback;
    }
    else if (typeof callback == 'object' || typeof callback == 'string') {
      fb.callback = $(callback);
      fb.callback.bind('keyup', fb.updateValue);
      if (fb.callback.get(0).value) {
        fb.setColor(fb.callback.get(0).value);
      }
    }
    return this;
  };
  fb.updateValue = function (event) {
    if (this.value && this.value != fb.color) {
      fb.setColor(this.value);
    }
  };

  /**
   * Change color with HTML syntax #123456
   */
  fb.setColor = function (color) {
    var unpack = fb.unpack(color);
    if (fb.color != color && unpack) {
      fb.color = color;
      fb.rgb = unpack;
      fb.hsl = fb.RGBToHSL(fb.rgb);
      fb.updateDisplay();
    }
    return this;
  };

  /**
   * Change color with HSL triplet [0..1, 0..1, 0..1]
   */
  fb.setHSL = function (hsl) {
    fb.hsl = hsl;
    fb.rgb = fb.HSLToRGB(hsl);
    fb.color = fb.pack(fb.rgb);
    fb.updateDisplay();
    return this;
  };

  /////////////////////////////////////////////////////

  /**
   * Retrieve the coordinates of the given event relative to the center
   * of the widget.
   */
  fb.widgetCoords = function (event) {
    var x, y;
    var el = event.target || event.srcElement;
    var reference = fb.wheel;

    // If the offset from the relative element is undefined calculate it.
    if ( typeof event.offsetX == 'undefined' && typeof event.offsetY == 'undefined' ) {
      var offset = $(event.target).offset(false);
      event.offsetX = event.pageX - offset.left;
      event.offsetY = event.pageY - offset.top;
    }

    // Use offset coordinates and find common offsetParent
    var pos = { x: event.offsetX, y: event.offsetY };

    // Send the coordinates upwards through the offsetParent chain.
    var e = el;
    while (e) {
      e.mouseX = pos.x;
      e.mouseY = pos.y;
      pos.x += e.offsetLeft;
      pos.y += e.offsetTop;
      e = e.offsetParent;
    }

    // Look for the coordinates starting from the wheel widget.
    var e = reference;
    var offset = { x: 0, y: 0 };
    while (e) {
      if (typeof e.mouseX != 'undefined') {
        x = e.mouseX - offset.x;
        y = e.mouseY - offset.y;
        break;
      }
      offset.x += e.offsetLeft;
      offset.y += e.offsetTop;
      e = e.offsetParent;
    }

    // Reset stored coordinates
    e = el;
    while (e) {
      e.mouseX = undefined;
      e.mouseY = undefined;
      e = e.offsetParent;
    }

    // Subtract distance to middle
    return { x: x - fb.width / 2, y: y - fb.width / 2 };
  };

  /**
   * Mousedown handler
   */
  fb.mousedown = function (event) {
    // Capture mouse
    if (!document.dragging) {
      $(document).bind('mousemove', fb.mousemove).bind('mouseup', fb.mouseup);
      document.dragging = true;
    }

    // Check which area is being dragged
    var pos = fb.widgetCoords(event);
    fb.circleDrag = Math.max(Math.abs(pos.x), Math.abs(pos.y)) * 2 > fb.square;

    // Process
    fb.mousemove(event);
    return false;
  };

  /**
   * Mousemove handler
   */
  fb.mousemove = function (event) {
    // Get coordinates relative to color picker center
    var pos = fb.widgetCoords(event);

    // Set new HSL parameters
    if (fb.circleDrag) {
      var hue = Math.atan2(pos.x, -pos.y) / 6.28;
      if (hue < 0) hue += 1;
      fb.setHSL([hue, fb.hsl[1], fb.hsl[2]]);
    }
    else {
      var sat = Math.max(0, Math.min(1, -(pos.x / fb.square) + .5));
      var lum = Math.max(0, Math.min(1, -(pos.y / fb.square) + .5));
      fb.setHSL([fb.hsl[0], sat, lum]);
    }
    return false;
  };

  /**
   * Mouseup handler
   */
  fb.mouseup = function () {
    // Uncapture mouse
    $(document).unbind('mousemove', fb.mousemove);
    $(document).unbind('mouseup', fb.mouseup);
    document.dragging = false;
  };

  /**
   * Update the markers and styles
   */
  fb.updateDisplay = function () {
    // Markers
    var angle = fb.hsl[0] * 6.28;
    $('.h-marker', e).css({
      left: Math.round(Math.sin(angle) * fb.radius + fb.width / 2) + 'px',
      top: Math.round(-Math.cos(angle) * fb.radius + fb.width / 2) + 'px'
    });

    $('.sl-marker', e).css({
      left: Math.round(fb.square * (.5 - fb.hsl[1]) + fb.width / 2) + 'px',
      top: Math.round(fb.square * (.5 - fb.hsl[2]) + fb.width / 2) + 'px'
    });

    // Saturation/Luminance gradient
    $('.color', e).css('backgroundColor', fb.pack(fb.HSLToRGB([fb.hsl[0], 1, 0.5])));

    // Linked elements or callback
    if (typeof fb.callback == 'object') {
      // Set background/foreground color
      $(fb.callback).css({
        backgroundColor: fb.color,
        color: fb.hsl[2] > 0.5 ? '#000' : '#fff'
      });

      // Change linked value
      $(fb.callback).each(function() {
        if (this.value && this.value != fb.color) {
          this.value = fb.color;
        }
      });
    }
    else if (typeof fb.callback == 'function') {
      fb.callback.call(fb, fb.color);
    }
  };

  /* Various color utility functions */
  fb.pack = function (rgb) {
    var r = Math.round(rgb[0] * 255);
    var g = Math.round(rgb[1] * 255);
    var b = Math.round(rgb[2] * 255);
    return '#' + (r < 16 ? '0' : '') + r.toString(16) +
           (g < 16 ? '0' : '') + g.toString(16) +
           (b < 16 ? '0' : '') + b.toString(16);
  };

  fb.unpack = function (color) {
    if (color.length == 7) {
      return [parseInt('0x' + color.substring(1, 3)) / 255,
        parseInt('0x' + color.substring(3, 5)) / 255,
        parseInt('0x' + color.substring(5, 7)) / 255];
    }
    else if (color.length == 4) {
      return [parseInt('0x' + color.substring(1, 2)) / 15,
        parseInt('0x' + color.substring(2, 3)) / 15,
        parseInt('0x' + color.substring(3, 4)) / 15];
    }
  };

  /*
   * NEW: latestColors
   */
  fb.addPrevColor = function (color) {

    if (latestColors.length >= 6) {
      latestColors = latestColors.splice(1,5);
    }
    latestColors.push(color);
  }

  fb.listPrevColors = function () {
    //alert(latestColors);
    var comboboxes = "", i, element, hsl, hex, color;

    for (i = 0; i<latestColors.length; i++) {
      element = "prev"+i;
      hex = latestColors[i];
      hsl = fb.unpack(latestColors[i]);
      color = hsl[2] > 0.5 ? '#000' : '#fff';
      comboboxes = comboboxes + "<input type=\"text\" id=\"" + element + "\" name=\"" + element + "\" value=\"" + hex + "\" style=\"background-color: "+hex+"; color: " + color + "\" class=\"prevbox\"/>";
    }

    $('#prevcolors').html(comboboxes);

    $('.prevbox').focus(function(){
      var hex = $(this).val();
      $.farbtastic('#placeholder').setColor(hex);
    });

  }

  fb.undoColor = function () {
    fb.setColor(latestColors[latestColors.length]);
  }

  /*
   * NEW: updateCompatibleColors
   */
  fb.updateCompatibleColors = function () {
    var hsl, type, element,comboboxes = "", hex;

    type = $('#selectsuggestedcolors').val();

    hsl = fb.hsl;
    colors = fb.findColors(hsl, type);

    for (i = 0; i<colors.length; i++) {
      element = "combo"+i;
      hex = fb.HSLToHex(colors[i]);
      color = colors[i][2] > 0.5 ? '#000' : '#fff';
      comboboxes = comboboxes + "<input type=\"text\" class=\"suggestedbox\" id=\"" + element + "\" name=\"" + element + "\" value=\"" + hex + "\" style=\"background-color: "+hex+"; color: " + color + ";\" />";
    }
    //comboboxes = comboboxes + '<input id="combo3" class="suggestedbox" type="text" style="background-color: rgb(0, 18, 52); color: rgb(255, 255, 255);" value="#000000" name="combo3"/>';

    $('#suggestedcolors').html(comboboxes);

    $('input[@class=suggestedbox]').focus(function(){
      var hex = $(this).val();
      $.farbtastic('#placeholder').setColor(hex);
    });

  }


  fb.roundToNearestFifth = function (number) {
    var nearestFifth, decimal, thevar;

    // first we need to see how many .20s are in the number
    // .8 / .2 = 4; .88 / .2 = 4.4
    nearestFifth = number / .20 ;
    nearestFifth = Math.floor(nearestFifth);
    //alert ("nearest fifth: " + nearestFifth);


    //find remainder after divided by .20
    thevar = number % .20 ;
    //alert ("remainer after %: " + thevar);
    // we're rounding out of .20, so multiply by 5 so we
    // can get a decimal out under 1 to round
    thevar = thevar * 5;
    //alert("between 20: " + thevar);

    thevar = Math.round(thevar);

    decimal = .20 * nearestFifth;
    if (thevar == 1) {
      decimal += .20;
    }

    return decimal;
  }

  fb.HSLToRGB = function (hsl) {
    var m1, m2, r, g, b;
    var h = hsl[0], s = hsl[1], l = hsl[2];
    m2 = (l <= 0.5) ? l * (s + 1) : l + s - l*s;
    m1 = l * 2 - m2;
    return [this.hueToRGB(m1, m2, h+0.33333),
        this.hueToRGB(m1, m2, h),
        this.hueToRGB(m1, m2, h-0.33333)];
  };

  // new
  fb.HSLToHex = function (hsl) {
    fb.rgb = fb.HSLToRGB(hsl);
    color = fb.pack(fb.rgb);
    return color;
  }

  fb.hueToRGB = function (m1, m2, h) {
    h = (h < 0) ? h + 1 : ((h > 1) ? h - 1 : h);
    if (h * 6 < 1) return m1 + (m2 - m1) * h * 6;
    if (h * 2 < 1) return m2;
    if (h * 3 < 2) return m1 + (m2 - m1) * (0.66666 - h) * 6;
    return m1;
  };

  fb.RGBToHSL = function (rgb) {
    var min, max, delta, h, s, l;
    var r = rgb[0], g = rgb[1], b = rgb[2];
    min = Math.min(r, Math.min(g, b));
    max = Math.max(r, Math.max(g, b));
    delta = max - min;
    l = (min + max) / 2;
    s = 0;
    if (l > 0 && l < 1) {
      s = delta / (l < 0.5 ? (2 * l) : (2 - 2 * l));
    }
    h = 0;
    if (delta > 0) {
      if (max == r && max != g) h += (g - b) / delta;
      if (max == g && max != b) h += (2 + (b - r) / delta);
      if (max == b && max != r) h += (4 + (r - g) / delta);
      h /= 6;
    }
    return [h, s, l];
  };

  // Install mousedown handler (the others are set on the document on-demand)
  $('*', e).mousedown(fb.mousedown);

    // Init color
  fb.setColor('#000000');
//  fb.updateCompatibleColors();
//  fb.listPrevColors();

  // Set linked elements/callback
  if (callback) {
    fb.linkTo(callback);
  }
};