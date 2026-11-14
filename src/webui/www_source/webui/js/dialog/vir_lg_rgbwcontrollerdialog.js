/**
 * vir_lg_rgbwcontrollerdialog.js
 **/

/**
 * Dialogbox mit den Schaltflächen "Ja" und "Neine"
 * Normalerweise wird als content Text übergeben,
 * wenn contentType 'html' gesetzt ist, kann auch HTML übergeben werden.
 * Die Höhe des Dialoges sollte sich dynamisch der Contentgröße anpassen.
 **/
VIR_LG_RGBWControllerDialog = Class.create({

  initialize: function (title, content, param, curValue, callback, contentType) {
    var _this_ = this;

    this.sliderInfoElm;
    this.sliderElm;
    this.param = param;
    this.curValue = curValue;
    this.arCurValue = curValue.split(",");

    this.configString = "not initialized";
    this.whiteLevel = 255;


    this.m_contentType = contentType;
    this.m_callback = callback;
    this.m_layer = document.createElement("div");
    this.m_layer.className = "YesNoDialogLayer";

    var dialog = document.createElement("div");
    dialog.className = "YesNoDialog";

    var titleElement = document.createElement("div");
    titleElement.className = "YesNoDialogTitle";
    titleElement.appendChild(document.createTextNode(title));
    titleElement.onmousedown = function (event) {
      new Drag(this.parentNode, event);
    };
    dialog.appendChild(titleElement);

    var contentWrapper = document.createElement("div");
    contentWrapper.className = "YesNoDialogContentWrapper";

    var contentElement = document.createElement("div");
    contentElement.className = "YesNoDialogContent";

    if (this.m_contentType == "html") {
      contentElement.innerHTML = content;
    } else {
      contentElement.appendChild(document.createTextNode(content));
    }

    contentWrapper.appendChild(contentElement);

    dialog.appendChild(contentWrapper);

    var footer = document.createElement("div");
    footer.className = "YesNoDialogFooter";

    var yesButton = document.createElement("div");
    yesButton.className = "YesNoDialog_yesButton borderRadius5px colorGradient50px";
    yesButton.appendChild(document.createTextNode(translateKey('btnOk')));
    yesButton.onclick = function () {
      _this_.yes();
    };
    footer.appendChild(yesButton);

    var noButton = document.createElement("div");
    noButton.className = "YesNoDialog_noButton borderRadius5px colorGradient50px";
    noButton.appendChild(document.createTextNode(translateKey('dialogBack')));
    noButton.onclick = function () {
      _this_.no();
    };
    footer.appendChild(noButton);

    dialog.appendChild(footer);

    this.m_layer.appendChild(dialog);

    Layer.add(this.m_layer);

    this.__activateSubDialog();

    //AG sorgt dafür, daß die Dialoghöhe sich dynamisch dem Content anpasst.
    jQuery(".YesNoDialog").css("height", jQuery(".YesNoDialogContentWrapper").height() + 78);
    jQuery(".YesNoDialogFooter").css("top", jQuery(".YesNoDialogContentWrapper").height() + 26);

    this.__bindEvents();
    translatePage('#RGBWControllerColor');

  },

  __bindEvents: function() {
    var self = this;
    if (this.__showWhiteElement()) {
      this.sliderInfoElm.blur(function () {
        self.whiteLevel = jQuery(this).val();
        if (isNaN(self.whiteLevel)){
          self.whiteLevel = 255;
        } else if (parseInt(self.whiteLevel) < 0) {
          self.whiteLevel = 0;
        } else if (parseInt(self.whiteLevel) > 255) {
          self.whiteLevel = 255;
        }
        jQuery(this).val(parseInt(self.whiteLevel));
        self.sliderElm.slider("value", self.whiteLevel);
      });
    }
  },

  // This creates the content of the dialog.
  __activateSubDialog: function () {
    if(this.__showWhiteElement()) {
      this.whiteLevel = parseInt(this.curValue.split(",")[3]);
      jQuery("#whiteLevelElm").show();
      this.__initSliderElm();
    };

    jQuery("#RGBWControllerColor").show();
    this.__activateColorPicker();
  },

  __showWhiteElement: function() {
    // There is a change with the parameters for color and color temperature
    // Before, the color and the color temperature was one parameter .
    // The color was set with the color picker, the color temp was set with the slider.

    // Now the color temperature is an extra parameter, so we don't need the slider for this value any more at this point.
    // The color temperature is now bound to the parameter COLOR_TEMPERATURE
    return false;
    //return (this.param == "RGBW") ? true : false;
  },

  __initSliderElm: function() {
    var self = this;

    this.sliderElm = jQuery("#hookSlider");

    this.sliderInfoElm = jQuery("#infoSliderPos");
    this.sliderInfoElm.val(this.whiteLevel);

    var self = this;
    this.sliderElm.slider({
      animate: "fast",
      value: this.whiteLevel,
      min: 0,
      max: 255,
      step: 5,
      orientation: "horizontal",
      slide: function (event, ui) {},
      stop: function( event, ui ) {}
    });
    this.sliderElm.on("slide", function (event, ui) {
      self.__onSliderChange(ui.value);
    });

    this.sliderElm.on("slidestop", function(event, ui){
      self.__onSliderStop(ui.value);
    });
  },



  // PUBLIC
  getConfigString: function () {
    return this.configString;
  },


  __onSliderChange: function (val) {
    this.sliderPos = val;
    this.sliderInfoElm.val(this.sliderPos);
    this.whiteLevel = this.sliderPos;
  },

  __onSliderStop: function(val) {
    this.whiteLevel = this.sliderPos;
  },

  __getColor: function () {
    var color = jQuery("#colorRGBControllerColor").val();
    return color;
  },


  __setConfigString: function () {
    this.configString = this.__getColor();

    //if (this.__showWhiteElement()) {
      var tmp = this.configString.split(",");

      if (tmp.size() > 3) {
        this.configString = tmp[0] +","+tmp[1]+","+tmp[2]+","+ this.whiteLevel + ")";
      } else {
        this.configString = this.configString.slice(0, -1) + "," + this.whiteLevel + ")";
      }
    //}
  },

  __activateColorPicker: function () {

    var colorPickerVal = this.curValue;
    if (this.arCurValue.size() > 3) {
      colorPickerVal = this.arCurValue[0]+ ", " + this.arCurValue[1] + ", " + this.arCurValue[2] + ")";
    }

    jQuery("#colorRGBControllerColor").val(colorPickerVal);

    jQuery("#colorRGBControllerColor").spectrum({
      preferredFormat: 'rgb',
      showInput: true,
      color: this.curValue,
      showPalette: true,
      palette: ['white'],
      cancelText: translateKey('btnCancel'),
      chooseText: translateKey('btnOk')
    });
  },


  close: function (result) {
    Layer.remove(this.m_layer);
    if (this.m_callback) {
      this.m_callback(result);
    }
  },

  yes: function () {
    this.__setConfigString();
    this.close(YesNoDialog.RESULT_YES);
  },

  no: function () {
    this.close(YesNoDialog.RESULT_NO);
  }

});

YesNoDialog.RESULT_NO = 0;
YesNoDialog.RESULT_YES = 1;
