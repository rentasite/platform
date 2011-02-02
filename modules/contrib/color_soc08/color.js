// $Id: color.js,v 1.2.2.10 2009/04/26 16:39:19 skiquel Exp $

Drupal.behaviors.color = function(context) {
  // Constructor
  // - Initialize one particular color selector
  // - Store data related to that selector in 'this'
  // - Create markup required for the color picker
  // - Do other setup tasks
  var inputs = [];
  var hooks = [];
  var locks = [];
  var focused = null;
  var form = $('#color-scheme-wrapper .color-form');
  this.farb = [];
  this.form = form;
  this.context = context;
  this.inputs = inputs;
  this.hooks = [];
  this.locks = [];
  this.lock = [];
  this.hook = [];
  this.focused = focused;
  this.reference = [];
  this.hslColor = [];
  this.hexColor = [];
  this.hex = [];

  var colorPicker = this;

  //var self = this;
  var focus=function () {
    var input = this;

    // Remove old bindings.
    colorPicker.focused && $(colorPicker.focused).unbind('keyup', colorPicker.farb.updateValue)
        .unbind('keyup', resetScheme)
        .parent().removeClass('item-selected');

    // Add new bindings.
    colorPicker.focused = input;
    colorPicker.farb.linkTo(function (color) { callback(input, color, true, false); });
    colorPicker.farb.setColor(input.value);
      colorPicker.farb.updateCompatibleColors();
    $(this).keyup(colorPicker.farb.updateValue).keyup(resetScheme)
      .parent().addClass('item-selected');
  };

  var colorSchemeSelect = function () {
    var colors = this.options[this.selectedIndex].value;
    //alert(colors);
    if (colors != '') {
      colors = colors.split(',');
      for (i in colors) {
        callback(colorPicker.inputs[i], colors[i], false, true);
      }
    }
  };

  var schemeSelector = function () {
    // Extract palette field name
    this.key = this.id.substring(13);

    // Link to color picker temporarily to initialize.
    colorPicker.farb.linkTo(function () {}).setColor('#000').linkTo(this);

    var i = colorPicker.inputs.length;

    // Add lock
    if (colorPicker.inputs.length) {
      colorPicker.lock = $('<div class="lock"></div>').toggle(
        function () {
          $(this).addClass('unlocked');
          $(colorPicker.hooks[i - 1]).attr('class',
            colorPicker.locks[i - 2] && $(colorPicker.locks[i - 2]).is(':not(.unlocked)') ? 'hook up' : 'hook'
          );
          $(colorPicker.hooks[i]).attr('class',
            colorPicker.locks[i] && $(colorPicker.locks[i]).is(':not(.unlocked)') ? 'hook down' : 'hook'
          );
        },
        function () {
          $(this).removeClass('unlocked');
          $(colorPicker.hooks[i - 1]).attr('class',
            colorPicker.locks[i - 2] && $(colorPicker.locks[i - 2]).is(':not(.unlocked)') ? 'hook both' : 'hook down'
          );
          $(colorPicker.hooks[i]).attr('class',
            colorPicker.locks[i] && $(colorPicker.locks[i]).is(':not(.unlocked)') ? 'hook both' : 'hook up'
          );
        }
      );

      var la = $(this);
      $('#palette:not(.locks-processed)').each(function() {
        // alert('1');
        $(la).after(colorPicker.lock);
        colorPicker.locks.push(colorPicker.lock);

      });

    };



    var hook = $('<div class="hook"></div>');
    var la = $(this);
    $('#palette:not(.hooks-processed)').each(function() {
      $(la).after(hook);
    });

    $(la).parent().find('.lock').click();

    colorPicker.hooks.push(hook);
    colorPicker.hook = hook;

    this.i = i;
    colorPicker.inputs.push(this);
  };

  var getLength = function (variable) {
    return variable.length;
  };

  var callback = function (input, color, propagate, colorscheme) {
    if (color === "") {
      $(input).css("background-color", 'gray').css("color", "black").attr('disabled',true).val('None');
    }
    else {
      // Set background/foreground color
      $(input).css({
        backgroundColor: color,
        'color': colorPicker.farb.RGBToHSL(colorPicker.farb.unpack(color))[2] > 0.5 ? '#000' : '#fff'
      }).attr('disabled',false);


      // Change input value
      if (input.value && input.value != color) {
        input.value = color;
        // Update locked values
        if (propagate) {
          var i = input.i;
          for (j = i + 1; ; ++j) {
            if (!colorPicker.locks[j - 1] || $(colorPicker.locks[j - 1]).is('.unlocked')) break;
            var matched = shift_color(color, colorPicker.reference[input.key], colorPicker.reference[colorPicker.inputs[j].key]);
            callback(colorPicker.inputs[j], matched, false);
          }
          for (j = i - 1; ; --j) {
            if (!colorPicker.locks[j] || $(colorPicker.locks[j]).is('.unlocked')) break;
            var matched = shift_color(color, colorPicker.reference[input.key], colorPicker.reference[colorPicker.inputs[j].key]);

            callback(colorPicker.inputs[j], matched, false);
          }

          // Update color theory tools
          colorPicker.farb.updateCompatibleColors();
          colorPicker.farb.addPrevColor(input.value);
          colorPicker.farb.listPrevColors();
        }

        // Reset colorscheme selector
        if (!colorscheme) {
          resetScheme;
        }
      }
    }
  };

  var resetScheme = function(form) {
    $('#edit-scheme', colorPicker.form).each(function () {
      colorPicker.selectedIndex = colorPicker.options.length - 1;
    });
  };

 /**
   * Shift a given color, using a reference pair (ref in HSL).
   *
   * This algorithm ensures relative ordering on the saturation and luminance
   * axes is preserved, and performs a simple hue shift.
   *
   * It is also symmetrical. If: shift_color(c, a, b) == d,
   *                        then shift_color(d, b, a) == c.
   */
   var shift_color = function (given, ref1, ref2) {
    // Convert to HSL
    given = colorPicker.farb.RGBToHSL(colorPicker.farb.unpack(given));

    // Hue: apply delta
    given[0] += ref2[0] - ref1[0];

    // Saturation: interpolate
    if (ref1[1] == 0 || ref2[1] == 0) {
      given[1] = ref2[1];
    }
    else {
      var d = ref1[1] / ref2[1];
      if (d > 1) {
        given[1] /= d;
      }
      else {
        given[1] = 1 - (1 - given[1]) * d;
      }
    }

    // Luminance: interpolate
    if (ref1[2] == 0 || ref2[2] == 0) {
      given[2] = ref2[2];
    }
    else {
      var d = ref1[2] / ref2[2];
      if (d > 1) {
        given[2] /= d;
      }
      else {
        given[2] = 1 - (1 - given[2]) * d;
      }
    }

    return colorPicker.farb.pack(colorPicker.farb.HSLToRGB(given));
  };


  $('#color-scheme-wrapper .color-form:not(.color-processed)').each(function () {
    $(colorPicker.form).prepend('<div id="placeholder"></div>');
    colorPicker.farb = $.farbtastic('#placeholder');

    // Decode reference colors to HSL
    colorPicker.reference = Drupal.settings.color.reference;
    for (i in colorPicker.reference) {
      colorPicker.reference[i] = colorPicker.farb.RGBToHSL(colorPicker.farb.unpack(colorPicker.reference[i]));
    }
    Drupal.settings.color.reference = colorPicker.reference;

  });

  $('#color-scheme-wrapper .color-form:not(.color-processed)').addClass('color-processed').each(function() {
    $('#palette input.form-text', colorPicker.form)
    .each(schemeSelector).focus(focus);
    $('#palette:not(.locks-processed)').addClass('locks-processed');
    $('#palette:not(.hooks-processed)').addClass('hooks-processed');

    $('#palette label', colorPicker.form).focus;

    focus.call(colorPicker.inputs[0]);
  });

  if (colorPicker.farb == 0){
    colorPicker.farb = $.farbtastic('#placeholder');
    $('#palette.color-processed').removeClass('color-processed');
    $('#palette input.form-text', colorPicker.form)
    .each(schemeSelector).focus(focus);
    $('#palette label', form).focus;
    focus.call(colorPicker.inputs[0]);
  }

  $('#edit-edit-scheme:not(.color-processed)').addClass('color-processed').each(function() {
    $('#edit-edit-scheme').change(colorSchemeSelect);
  });

  var hslColor, hexColor, hex;

  $('#selectsuggestedcolors').change(function() {
    colorPicker.farb.updateCompatibleColors();

  });

  $('#darken').click(function(){
    colorPicker.hslColor = colorPicker.farb.hsl;
    var h = colorPicker.hslColor[0], s = colorPicker.hslColor[1], l = colorPicker.hslColor[2];
    if ((l-.05) > 0) {
      l = l - .05;
    }
    colorPicker.hex = colorPicker.farb.HSLToHex([h,s,l]);
    colorPicker.farb.setColor(colorPicker.hex);
    return false;

  });


  $('#lighten').click(function(){
    colorPicker.hslColor = colorPicker.farb.hsl;
    var h = colorPicker.hslColor[0], s = colorPicker.hslColor[1], l = colorPicker.hslColor[2];
    if ((l+.05) < 1) {
      l = l + .05;
    }
    colorPicker.hex = colorPicker.farb.HSLToHex([h,s,l]);
    colorPicker.farb.setColor(colorPicker.hex);
    return false;

  });

  $('#randomize').click(function(){
    colorPicker.hslColor = colorPicker.farb.hsl;
    var h = colorPicker.hslColor[0], s = colorPicker.hslColor[1], l = colorPicker.hslColor[2];
    //h = 1/(360/90);
    // s is saturation. left to right, 1 to 0 on square. 0 to .75
    s = (.25+(.75)*Math.random());
    // l is luminosity, up to down, 1 to 0 on square. .25 to .75
    l = (.25 + (.50)*Math.random());
    //alert(s);

    colorPicker.hex = colorPicker.farb.HSLToHex([Math.random(),s,l]);
    colorPicker.farb.setColor(colorPicker.hex);
    return false;

  });

  $('#reset').click(function(){
    colorPicker.reference = Drupal.settings.color.reference;


    for (i in colorPicker.reference) {
      var resetInputs = colorPicker.inputs;
      resetInputs.pop(input);
      $(input, colorPicker.form).each(schemeSelector);
    }
  });
  /*
  $('#edit-selectsuggestedcolors').change(function(){
    colorPicker.farb.updateCompatibleColors();
  });*/
};