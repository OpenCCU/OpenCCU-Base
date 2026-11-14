/**
 * rgbwcontrollerdialog.js
 **/

/**
 * Dialogbox mit den Schaltflächen "Ja" und "Neine"
 * Normalerweise wird als content Text übergeben,
 * wenn contentType 'html' gesetzt ist, kann auch HTML übergeben werden.
 * Die Höhe des Dialoges sollte sich dynamisch der Contentgröße anpassen.
 **/
RGBWControllerDialog = Class.create({

  initialize: function (title, content, param, curValue, callback, contentType) {
    var _this_ = this;
    this.configString = "not initialized";

    var sCurValue = (curValue != this.configString) ? curValue.replace(/'/g, "\"") : curValue;

    this.DEFAULTBRIGHTNESS = 200;
    this.m_contentType = contentType;
    this.m_callback = callback;
    this.m_layer = document.createElement("div");
    this.m_layer.className = "YesNoDialogLayer";
    this.param = param;
    this.curValues = (sCurValue != this.configString) ? JSON.parse(sCurValue) : sCurValue ;
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

    translatePage('#RGBWControllerColor, #RGBWControllerProgram');
  },

  // This creates the content of the dialog.
  __activateSubDialog: function () {
    switch (this.param) {
      /*
       case "BRIGHTNESS" :
       jQuery("#RGBWControllerBrightness").show();
       break;
       */
      //case "RGBW_COLOR" :
      case "USER_COLOR" :
        /*
         Array curValues
         0 Color val
         1 Brightness
         2 Ramptime
         3 OnTime
         */
        var elmBrightness = jQuery("#colorRGBControllerBrightness"),
          elmRampTime = jQuery("#colorRGBControllerRampTime"),
          elmOnTime = jQuery("#colorRGBControllerOnTime"),
          elmFreeRampTimeContainer = jQuery("#colorRGBEnterFreeRampTimeContainer"),
          elmFreeOnTimeContainer = jQuery("#colorRGBEnterFreeOnTimeContainer");

        elmBrightness.val(this.curValues["ACT_BRIGHTNESS_STORE"] || this.DEFAULTBRIGHTNESS);

        // RampTime
        if (this.curValues["RAMP_TIME_STORE"]) {
          elmRampTime.val(this.__setTimeValue("colorRGBControllerRampTime", this.curValues["RAMP_TIME_STORE"]));
        } else {
          elmRampTime.val(0.5);
        }

        // OnTime
        if (this.curValues["ON_TIME_STORE"]) {
          elmOnTime.val(this.__setTimeValue("colorRGBControllerOnTime", this.curValues["ON_TIME_STORE"]));
        } else {
          elmOnTime.val(118000);
        }
        elmRampTime.change(function (e) {
          if (jQuery(this).val() == "99999999") {
            elmFreeRampTimeContainer.show();
          } else {
            elmFreeRampTimeContainer.hide();
          }
        });

        elmOnTime.change(function (e) {
          if (jQuery(this).val() == "99999999") {
            elmFreeOnTimeContainer.show();
          } else {
            elmFreeOnTimeContainer.hide();
          }
        });

        jQuery("#RGBWControllerColor").show();
        this.__activateColorPicker();
        break;
      //case "RGBW_AUTOMATIC":
      case "USER_PROGRAM":
        /*
         Array curValues
         0 Program
         1 Brightness
         2 Ramptime
         3 OnTime
         4 Color Min
         5 Color Max
         */
        var
          self = this,
          elmProgram = jQuery("#prgRGBControllerProgram"),
          elmBrightness = jQuery("#prgRGBControllerBrightness"),
          elmRampTime = jQuery("#prgRGBControllerRampTime"),
          elmOnTime = jQuery("#prgRGBControllerOnTime"),
          elmFreeRampTimeContainer = jQuery("#prgRGBEnterFreeRampTimeContainer"),
          elmFreeOnTimeContainer = jQuery("#prgRGBEnterFreeOnTimeContainer"),
          elmMinColorActive = jQuery("#prgRGBControllerMinColorActive"),
          elmMaxColorActive = jQuery("#prgRGBControllerMaxColorActive");

        elmProgram.val(this.curValues["ACT_COLOR_PROGRAM_STORE"] || 0);
        elmBrightness.val(this.curValues["ACT_BRIGHTNESS_STORE"] || this.DEFAULTBRIGHTNESS);

        // RampTime
        if (this.curValues["RAMP_TIME_STORE"]) {
          elmRampTime.val(this.__setTimeValue("prgRGBControllerRampTime", this.curValues["RAMP_TIME_STORE"]));
        } else {
          elmRampTime.val(0.5);
        }

        // OnTime
        if (this.curValues["ON_TIME_STORE"]) {
          elmOnTime.val(this.__setTimeValue("prgRGBControllerOnTime", this.curValues["ON_TIME_STORE"]));
        } else {
          elmOnTime.val(118000);
        }

        elmRampTime.change(function (e) {
          if (jQuery(this).val() == "99999999") {
            elmFreeRampTimeContainer.show();
          } else {
            elmFreeRampTimeContainer.hide();
          }
        });

        elmOnTime.change(function (e) {
          if (jQuery(this).val() == "99999999") {
            elmFreeOnTimeContainer.show();
          } else {
            elmFreeOnTimeContainer.hide();
          }
        });

        elmMinColorActive.change(function (e) {
          if (jQuery(this).prop("checked")) {
            if (self.curValues[4] == 255) {
              jQuery("#prgRGBControllerMinColor").spectrum("set", "hsv(0,100%,100%)");
            }
            jQuery("#prgRGBControllerMinColor").spectrum("enable");
          } else {
            jQuery("#prgRGBControllerMinColor").spectrum("disable");
          }
        });

        elmMaxColorActive.change(function (e) {
          if (jQuery(this).prop("checked")) {
            if (self.curValues[5] == 255) {
              jQuery("#prgRGBControllerMaxColor").spectrum("set", "hsv(359,100%,100%)");
            }
            jQuery("#prgRGBControllerMaxColor").spectrum("enable");
          } else {
            jQuery("#prgRGBControllerMaxColor").spectrum("disable");
          }
        });

        jQuery("#RGBWControllerProgram").show();
        this.__activateColorPickerMinMax("min");
        this.__activateColorPickerMinMax("max");
        break;
    }
  },

  // PUBLIC
  getConfigString: function () {
    return this.configString;
  },


  __setTimeValue: function (strTimeElem, value) {
    var maxValue = 85825945;
    var optionAvailable = false;
    var fullSeconds = (value <= maxValue) ? value : maxValue;

    jQuery("#" + strTimeElem + " > option").each(function () {
      if (this.value == fullSeconds) {
        optionAvailable = true;
      }
    });

    if (!optionAvailable) {
      var days = Math.floor(fullSeconds / 86400);
      var hours = Math.floor(fullSeconds / 3600) % 24;
      var minutes = Math.floor((fullSeconds - (hours * 3600)) / 60) % 60;
      var seconds = fullSeconds - ((days * 86400) + (hours * 3600) + (minutes * 60));
      var sHour = (hours < 10) ? "0" + hours : hours,
        sMinutes = (minutes < 10) ? "0" + minutes : minutes,
        sSeconds = (seconds < 10) ? "0" + seconds : seconds;

      if (days > 0) {
        jQuery("#" + strTimeElem).append("<option value='" + fullSeconds + "'>" + days + " days - " + sHour + ":" + sMinutes + ":" + sSeconds + "</value>");
      } else {
        jQuery("#" + strTimeElem).append("<option value='" + fullSeconds + "'>" + sHour + ":" + sMinutes + ":" + sSeconds + "</value>");
      }
    }
    return fullSeconds;
  },


  __getBrightness: function () {
  },

  __getColor: function () {
    var color = jQuery("#colorRGBControllerColor").val(),
      brightness = jQuery("#colorRGBControllerBrightness").val(),
      rampTime = jQuery("#colorRGBControllerRampTime").val(),
      onTime = jQuery("#colorRGBControllerOnTime").val();

    var result = "{'ACT_HSV_COLOR_VALUE_STORE':"+color+
      ",'ACT_BRIGHTNESS_STORE':" + brightness +
      ",'RAMP_TIME_STORE':" + rampTime +
      ",'ON_TIME_STORE':" + onTime +
      "}";

    //return color + "," + brightness + "," + rampTime + "," + onTime;
    return result;
  },

  __getProgram: function () {
    var program = jQuery("#prgRGBControllerProgram").val(),
      brightness = jQuery("#prgRGBControllerBrightness").val(),
      rampTime = jQuery("#prgRGBControllerRampTime").val(),
      onTime = jQuery("#prgRGBControllerOnTime").val(),
      minColor = jQuery("#prgRGBControllerMinColor").val(),
      maxColor = jQuery("#prgRGBControllerMaxColor").val(),
      elmMinColorActive = jQuery("#prgRGBControllerMinColorActive"),
      elmMaxColorActive = jQuery("#prgRGBControllerMaxColorActive");

    if (!elmMinColorActive.prop("checked")) {
      minColor = 255;
    }
    if (!elmMaxColorActive.prop("checked")) {
      maxColor = 255;
    }

    var result = "{'ACT_COLOR_PROGRAM_STORE':" + program +
      ",'ACT_BRIGHTNESS_STORE':" + brightness +
      ",'RAMP_TIME_STORE':" + rampTime +
      ",'ON_TIME_STORE':" + onTime +
      ",'ACT_MIN_BORDER_STORE':" + minColor +
      ",'ACT_MAX_BORDER_STORE':" + maxColor +
      "}";

    //return program + "," + brightness + "," + rampTime + "," + onTime + "," + minColor + "," + maxColor;
    return result;
  },

  __setConfigString: function () {
    switch (this.param) {
      case "BRIGHTNESS" :
        this.configString = this.__getBrightness();
        break;
      //case "RGBW_COLOR" :
      case "USER_COLOR" :
        var freeRampTimeHour = jQuery("#colorRGBEnterFreeRampTimeHour").val(),
          freeRampTimeMin = jQuery("#colorRGBEnterFreeRampTimeMin").val(),
          freeRampTimeSec = jQuery("#colorRGBEnterFreeRampTimeSec").val();

        var freeOnTimeHour = jQuery("#colorRGBEnterFreeOnTimeHour").val(),
          freeOnTimeMin = jQuery("#colorRGBEnterFreeOnTimeMin").val(),
          freeOnTimeSec = jQuery("#colorRGBEnterFreeOnTimeSec").val();

        if (jQuery("#colorRGBEnterFreeRampTimeContainer").css("display") != "none") {
          var userValRampTime = parseInt(freeRampTimeHour * 3600) + parseInt(freeRampTimeMin * 60) + parseInt(freeRampTimeSec);
          jQuery("#colorRGBControllerRampTime").append("<option value='" + userValRampTime + "'>" + userValRampTime + "</value>");
          jQuery("#colorRGBControllerRampTime").val(userValRampTime);
        }

        if (jQuery("#colorRGBEnterFreeOnTimeContainer").css("display") != "none") {
          var userValOnTime = parseInt(freeOnTimeHour * 3600) + parseInt(freeOnTimeMin * 60) + parseInt(freeOnTimeSec);
          jQuery("#colorRGBControllerOnTime").append("<option value='" + userValOnTime + "'>" + userValOnTime + "</value>");
          jQuery("#colorRGBControllerOnTime").val(userValOnTime);
        }

        this.configString = this.__getColor();
        break;
      //case "RGBW_AUTOMATIC":
      case "USER_PROGRAM":

        var freeRampTimeHour = jQuery("#prgRGBEnterFreeRampTimeHour").val(),
          freeRampTimeMin = jQuery("#prgRGBEnterFreeRampTimeMin").val(),
          freeRampTimeSec = jQuery("#prgRGBEnterFreeRampTimeSec").val();

        var freeOnTimeHour = jQuery("#prgRGBEnterFreeOnTimeHour").val(),
          freeOnTimeMin = jQuery("#prgRGBEnterFreeOnTimeMin").val(),
          freeOnTimeSec = jQuery("#prgRGBEnterFreeOnTimeSec").val();

        if (jQuery("#prgRGBEnterFreeRampTimeContainer").css("display") != "none") {
          var userValRampTime = parseInt(freeRampTimeHour * 3600) + parseInt(freeRampTimeMin * 60) + parseInt(freeRampTimeSec);
          jQuery("#prgRGBControllerRampTime").append("<option value='" + userValRampTime + "'>" + userValRampTime + "</value>");
          jQuery("#prgRGBControllerRampTime").val(userValRampTime);
        }

        if (jQuery("#prgRGBEnterFreeOnTimeContainer").css("display") != "none") {
          var userValOnTime = parseInt(freeOnTimeHour * 3600) + parseInt(freeOnTimeMin * 60) + parseInt(freeOnTimeSec);
          jQuery("#prgRGBControllerOnTime").append("<option value='" + userValOnTime + "'>" + userValOnTime + "</value>");
          jQuery("#prgRGBControllerOnTime").val(userValOnTime);
        }

        this.configString = this.__getProgram();
        break;
    }
  },

  __activateColorPicker: function () {
    var saturation = "100%";
    if (this.curValues["ACT_HSV_COLOR_VALUE_STORE"] > 199) {
      this.curValues["ACT_HSV_COLOR_VALUE_STORE"] = 200;
      saturation = "0%";
    }
    var hsvVal = parseInt(this.curValues["ACT_HSV_COLOR_VALUE_STORE"] / 199 * 360);
    jQuery("#colorRGBControllerColor").val(this.curValues["ACT_HSV_COLOR_VALUE_STORE"]);

    jQuery("#colorRGBControllerColor").spectrum({
      preferredFormat: "convert360To200",
      showInput: false,
      color: "hsv(" + hsvVal + "," + saturation + ",100%)",
      showPalette: true,
      palette: ["white"],
      cancelText: translateKey("btnCancel"),
      chooseText: translateKey("btnOk")
    });
  },

  __activateColorPickerMinMax: function (mode) {
    var strColorElm,
      strColorActiveElm,
      curValue,
      pickerState;

    if (mode == "min") {
      strColorElm = "#prgRGBControllerMinColor";
      strColorActiveElm = "#prgRGBControllerMinColorActive";
      curValue = (this.curValues["ACT_MIN_BORDER_STORE"]) ? this.curValues["ACT_MIN_BORDER_STORE"] : 0;
    } else {
      strColorElm = "#prgRGBControllerMaxColor";
      strColorActiveElm = "#prgRGBControllerMaxColorActive";
      curValue = (this.curValues["ACT_MAX_BORDER_STORE"]) ? this.curValues["ACT_MAX_BORDER_STORE"] : 199;
    }

    if (curValue == 255) {
      jQuery(strColorActiveElm).prop("checked", false);
    } else {
      jQuery(strColorActiveElm).prop("checked", true);
    }

    pickerState = (curValue == 255) ? true : false;

    var saturation = "100%";
    var hsvVal = parseInt(curValue / 199 * 360);
    jQuery(strColorElm).val(curValue);

    jQuery(strColorElm).spectrum({
      preferredFormat: "convert360To200",
      showInput: false,
      color: "hsv(" + hsvVal + "," + saturation + ",100%)",
      showPalette: true,
      disabled: pickerState,
      palette: [],
      cancelText: translateKey("btnCancel"),
      chooseText: translateKey("btnOk")
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
