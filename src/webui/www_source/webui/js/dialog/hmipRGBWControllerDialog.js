hmipRGBWControllerDialog = Class.create({
  initialize: function(title, content, deviceType, chnAddress, value, callback, contentType)
  {
    showRamptimeOff = false; // This we need among other things for certain COMBINED_PARAMETER help dialogs.
    var _this_ = this;

    this.m_contentType = contentType;
    this.m_callback = callback;
    this.m_layer = document.createElement("div");
    this.m_layer.className = "YesNoDialogLayer";

    this.RESULT_NO = 0;
    this.RESULT_YES = 1;

    this.iface = "HmIP-RF";

    this.rgbw = "HmIP-RGBW";
    this.drgDali = "HmIP-DRG-DALI";
    this.lss = "HmIP-LSC";

    this.deviceType = deviceType;
    this.chnAddress = chnAddress;
    this.initValue = value;

    this.oChannel = DeviceList.getChannelByAddress(this.chnAddress);

    this.maxOnTime = 111600;

    this.valHCL = 10200;
    this.valDim2Warm = 10150;
    this.ColorTempDefaultMin = 2000;
    this.ColorTempDefaultMax = 6500;


    this.arNoOntimeAvailable = [];
    this.showRampTimeOffElm = [this.rgbw, this.drgDali, this.lss];

    this.chnDescription = homematic("Interface.getParamset", {'interface': this.iface, 'address': this.chnAddress, 'paramsetKey': 'MASTER'});
    this.maxCap = (this.deviceType != this.lss) ? parseInt(this.chnDescription.UNIVERSAL_LIGHT_MAX_CAPABILITIES) : 3;



    this.effectModePrg = "unknown";

    var dialog = document.createElement("div");
    dialog.className = "YesNoDialog";

    var titleElement = document.createElement("div");
    titleElement.className = "YesNoDialogTitle";
    //titleElement.appendChild(document.createTextNode(title + " " + deviceType + " - " + chnAddress));
    titleElement.appendChild(document.createTextNode(deviceType + " - " + chnAddress));
    titleElement.onmousedown = function(event) { new Drag(this.parentNode, event); };
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
    footer.className= "YesNoDialogFooter";

    var yesButton = document.createElement("div");
    yesButton.className = "YesNoDialog_yesButton borderRadius5px colorGradient50px";
    yesButton.appendChild(document.createTextNode(translateKey('dialogYes')));
    yesButton.onclick = function() { _this_.yes(); };
    yesButton.id="btnYes";
    footer.appendChild(yesButton);

    var noButton = document.createElement("div");
    noButton.className = "YesNoDialog_noButton borderRadius5px colorGradient50px";
    noButton.appendChild(document.createTextNode(translateKey('dialogNo')));
    noButton.onclick = function() { _this_.no(); };
    noButton.id = "btnNo";
    footer.appendChild(noButton);

    dialog.appendChild(footer);

    this.m_layer.appendChild(dialog);
    Layer.add(this.m_layer);

    translatePage(".YesNoDialog");

    this.setHeight();

    this.setDialogElements();
    this.initDialog();

  },

  isOntimeAvailable: function() {
    return (this.arNoOntimeAvailable.indexOf(this.deviceType) == -1) ? true : false;
  },

  setDialogElements: function() {
    var self = this;
    this.activeDialog = "color"; // default = color - valid values: color, colorTemp, effect
    this.btnColorDialog = jQuery("#dialogColor");
    this.btnColorTempDialog = jQuery("#dialogColorTemp");
    this.btnEffectDialog = jQuery("#dialogEffect");

    this.anchorColorPicker = jQuery("#anchorColorPicker");
    this.trColor = jQuery(".j_trColor");
    this.trDurationElms = jQuery("[name='trDuration']");
    this.trRampTimeElms = jQuery("[name='trRampTime']");
    this.trRampTimeOff = jQuery("#trRampTimeOff");
    this.levelElm = jQuery("#combinedParam_Level");

    this.levelFreeValElm = jQuery("#prgDimmerEnterFreeLevel");
    this.divLevelFreeValElm = jQuery("#divLevelEnterFreeValue");
    this.levelFreeValActive = false;

    this.lblBrightnessLevelElm = jQuery("#lblBrightnessLevel");
    this.lblRampTimeElm = jQuery("#lblRampTime");
    this.chkBoxTimeLimitElm = jQuery("#chkBoxTimeLimit");
    this.durationValueElm = jQuery("#combinedParam_DurationValue");
    this.durationUnitElm = jQuery("#combinedParam_DurationUnit");
    this.rampTimeUnitElm = jQuery("#combinedParam_RampTimeUnit");
    this.rampTimeValueElm = jQuery("#combinedParam_RampTimeValue");
    this.rampTimeOffUnitElm = jQuery("#combinedParam_RampTimeOffUnit");
    this.rampTimeOffValueElm = jQuery("#combinedParam_RampTimeOffValue");

    this.onTimePanel = jQuery(".j_trOnTimePanel");


    this.colorPreviewElm = jQuery("#bckGndElm");

    this.colorPicker;
    this.colorPickerInit = {}; // level, hue, saturation

    this.hueElm = jQuery("#hueElm");
    this.satElm = jQuery("#satElm");

    this.HUE;
    this.SATURATION;

    this.lastValHueElm = jQuery("#lastValHue");
    this.lastValSatElm = jQuery("#lastValSat");

    // TW
    this.trTWSlider = jQuery("[name='trTWSlider']");
    this.sliderElm = jQuery("#twSlider");
    this.sliderInfoElm = jQuery("#tcInfoField");
    this.btnHCLElm = jQuery("#btnHCL");
    this.btnDim2WarmElm = jQuery("#btnDim2Warm");
    this.colorTemperature = 0;

    // Effects
    this.trEffects = jQuery("[name='trEffects']");
    this.effectSelBox = jQuery("#effectSelBox");
    this.effectModeChkBox = jQuery("#effectModePrg");
  },

  _getOnTimeVal: function(val, unit) {
    var result;

    if (parseInt(val) >= 31 && parseInt(unit) == 2) {
      return 0;
    }

    if (unit == 0) {
      result = val;
    } else if (unit == 1) {
      result = val * 60;
    } else if (unit == 2) {
      result = val * 3600;
    }
    return parseInt(result);
  },

  _getRampTimeVal: function(val, unit) {
    var result;
    if (unit == 0) {
      result = val;
    } else if (unit == 1) {
      result = parseInt(val * 60);
    } else if (unit == 3) {
      result = parseFloat(val / 100);
    }
    return result;
  },

  _getUnitInDU4OnTime: function(time) {
    var result = 0,
      hr = time / 3600,
      min= time / 60;

    if (hr == parseInt(hr)) {
      result = 2;
    } else if  (min == parseInt(min)) {
      result = 1;
    }
    return result;
  },

  _getUnitInDU4RampTime: function(time) {
    var t = parseFloat(time),
      result = 0,
      min = t / 60;

    if (parseInt(time) == 0) {
      return 0;
    }

    if (min == parseInt(min)) {
      result = 1;
    } else if ((Number(t) === t) && (t % 1 !== 0)) {
      // time in float
      result = 3;
    }
    return result;
  },

  hideOnTimeElems: function() {
    jQuery("[name='trRampTime']").first().nextAll().hide();
    this.setHeight();
  },

  getActiveDialog: function() {return this.activeDialog;},

  initSubDialogs: function() {
    var self = this;

    this.btnColorDialog.click(function() {
      JControlBtn.on(jQuery(this));
      JControlBtn.off(jQuery(self.btnColorTempDialog));
      JControlBtn.off(jQuery(self.btnEffectDialog));
      self.trTWSlider.hide();
      self.trEffects.hide();
      self.trRampTimeElms.show();
      self.trColor.show();
      self.onTimePanel.show();
      self.resetHeight();
      self.activeDialog = "color";
    });

    if ((this.deviceType == this.drgDali) || (this.deviceType == this.rgbw) || (this.deviceType == this.lss)) {
      this.btnColorTempDialog.click(function () {
        JControlBtn.on(jQuery(this));
        JControlBtn.off(jQuery(self.btnColorDialog));
        JControlBtn.off(jQuery(self.btnEffectDialog));
        self.trColor.hide();
        self.trEffects.hide();
        self.trRampTimeElms.show();
        self.trTWSlider.show();
        self.onTimePanel.show();
        self.resetHeight();
        self.activeDialog = "colorTemp";
      });
    }

    if (this.deviceType == this.drgDali) {
      switch (this.maxCap) {
        case 2:
          this.btnColorDialog.hide();
          this.btnColorTempDialog.click();
      }
    }

    if ((this.deviceType == this.rgbw)) {
      switch (this.maxCap) {
        case 2:
          this.btnColorDialog.hide();
          this.btnColorTempDialog.click();
          break;
        case 3:
          this.btnColorTempDialog.hide();
          this.btnColorDialog.click();
      }
    }

    this.btnEffectDialog.click(function() {
      JControlBtn.on(jQuery(this));
      JControlBtn.off(jQuery(self.btnColorDialog));
      JControlBtn.off(jQuery(self.btnColorTempDialog));
      self.trRampTimeElms.hide();
      self.trColor.hide();
      self.trTWSlider.hide();
      self.onTimePanel.hide();
      self.trEffects.show();
      self.resetHeight();
      self.activeDialog = "effect";
    });
  },

  setEffectNames: function() {
    var oDevice = DeviceList.getDeviceByAddress(this.chnAddress.split(":")[0]),
     effectName, effectNo,
      arEffectValue = [0,1,3,5,7,9,11,13,15,17,19];

    this.effectSelBox.empty().append("<option value='0'>"+translateKey('optionStopEffect')+"</option>");
    for (effectNo = 1; effectNo <= 10; effectNo++) {
      effectName =  homematic("Interface.getMetadata", {"objectId": oDevice.id, "dataId": "effectName_" + effectNo});
      if ((effectName == "") || (effectName == "null")) {effectName = translateKey("lblEffect") + " " + effectNo;}
      this.effectSelBox.append("<option value='"+arEffectValue[effectNo]+"'>"+effectName+"</option>");
    }
  },


  setULReffectModePrg: function() {
    this.effectModePrg = (jQuery(this.effectModeChkBox).is(":checked")) ? true : false;
    homematic("Interface.setMetadata", {"objectId": this.oChannel.id, "dataId": "effectModePrg", "value": this.effectModePrg});
  },


  showHideLevelFreeValue: function() {
    if (this.levelElm.val() == "99999998") {
      this.divLevelFreeValElm.show();
      this.levelFreeValActive = true;
      this.setHeight();
    } else {
      this.divLevelFreeValElm.hide();
      this.levelFreeValActive = false;
      this.setHeight();
    }

  },

  isLevelValid: function(elm) {
    var val = parseInt(elm.value);
    if ((isNaN(val) || val < 0)) {val = 0;} else if (val > 100) {val = 100;}
    elm.value = val;
  },

  initDialog: function() {
    var self = this;

    this.levelElm.change(function() {self.showHideLevelFreeValue();});
    this.levelFreeValElm.blur(function() {self.isLevelValid(this);});

    var arElmValues, valueL, iValueL, valueDV, valueDVtmp, valueDU, valueRTV, valueRTVtmp, valueRTU, valueC, valueCB, valueRTTOU, valueRTTOV, permanentHR, permanentHR_0, minDuration, maxDuration;

    arElmValues = this.initValue.split(",");
    valueL = arElmValues[0].split("=")[1];
    iValueL = parseInt(valueL) / 10;


    var arElmValues, valueL, valueDV, valueDVtmp, valueDU, valueRTV, valueRTVtmp, valueRTU, valueSlider, valueRTTOU, valueRTTOV, permanentHR, permanentHR_0, minDuration, maxDuration, effect;

    // iValueL !== (iValueL | 0) = check if the value is not 0% - 100%
    if ( (iValueL !== (iValueL | 0)) && (valueL != "100.5") && (valueL != "101")) {
      this.levelFreeValActive = true;
      this.levelFreeValElm.val(valueL);
      this.divLevelFreeValElm.show();
    }

    this.initSubDialogs();

    if (this.isOntimeAvailable()) {
      arElmValues = this.initValue.split(",");

      valueL = this.getConfigStringValue("L");
      valueDVtmp = this.getConfigStringValue("OT");
      valueDU = this._getUnitInDU4OnTime(valueDVtmp);


      this.colorPickerInit.level = parseInt(valueL);
      this.colorPickerInit.hue = parseInt(this.getConfigStringValue("H"));
      this.colorPickerInit.saturation = parseInt(this.getConfigStringValue("SAT"));
      this.colorTemperature = parseInt(this.getConfigStringValue("TC"));

      this.HUE = this.colorPickerInit.hue;
      this.SATURATION = this.colorPickerInit.saturation;

      if ((this.colorTemperature == -1) || (this.colorTemperature == 0)) {
        if (this.HUE > 360) {this.hueElm.hide();this.hueElm.parent().next().hide();this.lastValHueElm.prop("checked", true);}
        if (this.SATURATION > 100) {this.satElm.hide(); this.satElm.parent().next().hide();this.lastValSatElm.prop("checked", true);}
      } else {
        this.HUE = 0;
        this.SATURATION = 0;
        this.colorPickerInit.hue = this.HUE;
        this.colorPickerInit.saturation = this.SATURATION;
      }

      if (valueDU == 2) {
        valueDV = parseInt(valueDVtmp / 3600);
      } else if (valueDU == 1) {
        valueDV = parseInt(valueDVtmp / 60);
      } else {
        valueDV = valueDVtmp;
      }

      //valueRTVtmp = arElmValues[2].split("=")[1];
      valueRTVtmp = this.getConfigStringValue("RT");

      valueRTU = this._getUnitInDU4RampTime(valueRTVtmp);

      if (valueRTU == 3) {
        valueRTV = valueRTVtmp * 100;
      } else if (valueRTU == 1) {
        valueRTV = parseInt(valueRTVtmp / 60);
      } else {
        valueRTV = valueRTVtmp;
      }

      permanentHR = 31;
      permanentHR_0 = 0;
      minDuration = 0;
      maxDuration = 16343;
    }
    // RAMPTIME_OFF

    if (arElmValues.length >= 7) {
      valueRTTOV = this.getConfigStringValue("RTTOV");
      valueRTTOV = (valueRTTOV == -1 ) ? this.getConfigStringValue("RTTDV") : valueRTTOV;

      valueRTTOU = this.getConfigStringValue("RTTOU");
      valueRTTOU = (valueRTTOU == -1 ) ? this.getConfigStringValue("RTTDU") : valueRTTOU;

      this.rampTimeOffUnitElm.val(valueRTTOU);
      this.rampTimeOffValueElm.val(valueRTTOV);
    }

    if (this.levelFreeValActive) {
      this.levelElm.val("99999998");
    } else {
      this.levelElm.val(valueL);
    }

    this.durationValueElm.val(valueDV);
    this.durationUnitElm.val(valueDU);

    this.rampTimeUnitElm.val(valueRTU);
    this.rampTimeValueElm.val(valueRTV);

    this.hueElm.val(this.HUE);
    this.satElm.val(this.SATURATION);

    if ((this.durationValueElm.val() == permanentHR && this.durationUnitElm.val() == 2) || (this.isOntimeAvailable() && valueDV == 0)) {
      this.chkBoxTimeLimitElm.prop("checked", false);
      this.durationValueElm.prop('disabled', true);
      this.durationUnitElm.prop('disabled', true);
      this.trDurationElms.css("visibility", "visible");
      this.trRampTimeElms.css("visibility", "visible");
      this.trDurationElms.css("opacity", "0.2");
      if (this.showRampTimeOffElm.indexOf(this.deviceType) != -1) {
        this.trRampTimeOff.css("visibility", "visible").css("opacity", "0.2");
        showRamptimeOff = true;
      }
    } else {
      this.chkBoxTimeLimitElm.prop("checked", true);
      this.durationValueElm.prop('disabled', false);
      this.durationUnitElm.prop('disabled', false);
      this.trDurationElms.css("visibility", "visible");
      this.trRampTimeElms.css("visibility", "visible");
      if (this.showRampTimeOffElm.indexOf(this.deviceType) != -1) {
        this.trRampTimeOff.css("visibility", "visible");
        showRamptimeOff = true;
      }
    }

    this.chkBoxTimeLimitElm.bind("change", function() {
      if (this.checked) {
        self.durationValueElm.prop('disabled', false);
        self.durationUnitElm.prop('disabled', false);
        self.trDurationElms.fadeTo(1000, 1);
        self.trRampTimeElms.fadeTo(1000, 1);
        if (self.showRampTimeOffElm.indexOf(self.deviceType) != -1) {
          self.trRampTimeOff.fadeTo(1000,1);
        }
      } else {
        self.durationValueElm.prop('disabled', true);
        self.durationUnitElm.prop('disabled', true);
        self.rampTimeValueElm.prop('disabled', false); //.val(0);
        self.rampTimeUnitElm.prop('disabled', false); //.val(0);
        self.trDurationElms.fadeTo(1000, 0.2);
        self.trRampTimeElms.fadeTo(1000, 1);

        if (self.showRampTimeOffElm.indexOf(self.deviceType) != -1) {
          self.trRampTimeOff.fadeTo(1000,0.2);
        }

        if (self.isOntimeAvailable()) {
          self.durationValueElm.val(permanentHR_0);
        } else {
          self.durationValueElm.val(permanentHR);
          self.rampTimeOffUnitElm.val(3);
          self.rampTimeOffValueElm.val(0);
        }
        self.durationUnitElm.val(2);
      }
    });

    this.durationValueElm.bind("keyup", function() {
      var min = minDuration,
        max = (parseInt(self.durationUnitElm.val()) == 2) ? permanentHR: maxDuration;
      this.value = self.checkValidity(this.value,min,max);
      if (parseInt(this.value) == 31) {
        self.rampTimeOffValueElm.val(min);
        self.rampTimeOffUnitElm.val(3); // = 10ms
      }
    });

    this.durationValueElm.bind("blur", function() {
      var val = parseInt(this.value);

      if (isNaN(val)) {
        this.value = (parseInt(self.durationUnitElm.val()) == 2) ? permanentHR :  maxDuration;
      } else {
        this.value = val;
      }
      self.durationValueElm.keyup();
    });

    this.durationUnitElm.bind("change", function(){
      self.durationValueElm.keyup();
    });

    this.rampTimeValueElm.bind("keyup", function() {
      var min = minDuration,
        max = maxDuration;
      this.value = self.checkValidity(this.value,min,max);
    });

    this.rampTimeValueElm.bind("blur", function() {
      var val = parseInt(this.value);

      if (isNaN(val)) {
        this.value = maxDuration;
      } else {
        this.value = val;
      }
    });

    this.rampTimeUnitElm.bind("change", function(){
      self.rampTimeValueElm.keyup();
    });

    /**********************/
    this.rampTimeOffValueElm.bind("keyup", function() {
      var min = minDuration,
        max = maxDuration;
      this.value = self.checkValidity(this.value,min,max);
    });

    this.rampTimeOffValueElm.bind("blur", function() {
      var val = parseInt(this.value);

      if (isNaN(val)) {
        this.value = maxDuration;
      } else {
        this.value = (parseInt(self.durationValueElm.val()) <= 30) ? val : minDuration;
        if (parseInt(self.durationValueElm.val()) >= 31) {
          self.rampTimeOffUnitElm.val(3); // 10ms
        }
      }
    });

    this.rampTimeUnitElm.bind("change", function(){
      self.rampTimeOffValueElm.keyup();
    });

    this.hueElm.keyup(function(event) {
      var val;
      if (event.keyCode == 13) {
        val = parseInt(jQuery(this).val());
        if (val < 0 || isNaN(val)) {self.HUE = 0;} else if (val > 360) {self.HUE = 360;} else {self.HUE = val;};
        jQuery(this).val(self.HUE);
        self.colorPicker.color.hsv = {h: self.HUE, s: self.SATURATION, v: 0};
        self.setColorPreviewElm(self.HUE, self.SATURATION);

        // Set the TW slider to --
        self.resetColorTemp();
      }
    });

    this.hueElm.blur(function(event) {
      var val;
      val = parseInt(jQuery(this).val());
      if (val < 0 || isNaN(val)) {self.HUE = 0;} else if (val > 360) {self.HUE = 360;} else {self.HUE = val;};
      jQuery(this).val(self.HUE);
      self.colorPicker.color.hsv = {h: self.HUE, s: self.SATURATION, v: 0};
      self.setColorPreviewElm(self.HUE, self.SATURATION);

      // Set the TW slider to --
      self.resetColorTemp();
    });


    this.satElm.keyup(function(event) {
      var val;
      if (event.keyCode == 13) {
        val = parseInt(jQuery(this).val());
        if (val < 0 || isNaN(val)) {self.SATURATION = 0;} else if (val > 100) {self.SATURATION = 100;} else {self.SATURATION = val;};
        jQuery(this).val(parseInt(self.SATURATION));
        self.colorPicker.color.hsv = {h: self.HUE, s: self.SATURATION, v: 0};
        self.setColorPreviewElm(self.HUE, self.SATURATION);

        // Set the TW slider to --
        self.resetColorTemp();
      }
    });

    this.satElm.blur(function(event) {
      var val;
      val = parseInt(jQuery(this).val());
      if (val < 0 || isNaN(val)) {self.SATURATION = 0;} else if (val > 100) {self.SATURATION = 100;} else {self.SATURATION = val;};
      jQuery(this).val(parseInt(self.SATURATION));
      self.colorPicker.color.hsv = {h: self.HUE, s: self.SATURATION, v: 0};
      self.setColorPreviewElm(self.HUE, self.SATURATION);

      // Set the TW slider to --
      self.resetColorTemp();
    });
    /**********************/

    this.showHideColorPicker();

    this.lastValHueElm.click(function() {
      var selected = jQuery(this).prop("checked");

      self.showHideColorPicker();

      if (selected) {
        self.HUE = 361;
        self.hueElm.val(self.HUE);
        self.hueElm.hide();self.hueElm.parent().next().hide();
        self.resetColorTemp();
      } else {
        self.HUE = 0;
        self.hueElm.val(self.HUE);
        self.hueElm.show();self.hueElm.parent().next().show();
        self.colorPicker.color.hsv = {h: self.HUE, s: 100, v: 100};
      }
      self.freezeColorTemp();
      self.setColorPreviewElm(self.HUE, 100);
      self.resetHeight();
    });

    this.lastValSatElm.click(function() {
      var selected = jQuery(this).prop("checked");

      self.showHideColorPicker();

      if (selected) {
        self.SATURATION = 101;
        self.satElm.val(self.SATURATION);
        self.satElm.hide();self.satElm.parent().next().hide();
        self.colorPicker.color.hsv = {h: self.HUE, s: 100, v: 100};
        self.resetColorTemp();
      } else {
        self.SATURATION = 100;
        self.satElm.val(self.SATURATION);
        self.satElm.show();self.satElm.parent().next().show();
        self.colorPicker.color.hsv = {h: self.HUE, s: 100, v: 100};
      }
      self.freezeColorTemp();
      self.setColorPreviewElm(self.HUE, 100);
      self.resetHeight();
    });

    // TW Slider

    this.btnHCLElm.bind("click", function() {
      JControlBtn.on(jQuery(this));
      JControlBtn.off(jQuery(self.btnDim2WarmElm));
      window.setTimeout(function() {jQuery(".ui-slider-handle").removeClass("ui-state-active");},100);
      self.colorTemperature = self.valHCL;
      self.sliderInfoElm.val("--");
      self.sliderElm.slider('value', 0);
      self.resetColorPicker();
    });

    this.btnDim2WarmElm.bind("click", function() {
      JControlBtn.on(jQuery(this));
      JControlBtn.off(jQuery(self.btnHCLElm));
      window.setTimeout(function() {jQuery(".ui-slider-handle").removeClass("ui-state-active");},100);
      self.colorTemperature = self.valDim2Warm;
      self.sliderInfoElm.val("--");
      self.sliderElm.slider('value', 0);
      self.resetColorPicker();
    });

    // TW Slider
    //var tcValueElm = this.getConfigStringValue("TC");
    this.sliderElm.slider(this.getSliderOpts());

    valueSlider = parseInt(this.getConfigStringValue("TC"));
    valueSlider = (valueSlider != -1) ? valueSlider : 0;

    this.sliderInfoElm.val((valueSlider == 0) ? "--" : valueSlider);
    this.sliderElm.val(valueSlider);
    this.sliderElm.slider('value', (valueSlider > 10100) ? 0 : valueSlider);
    this.colorTemperature = valueSlider;

    this.sliderElm.on("slide", function (event, ui) {
      self.sliderInfoElm.val(ui.value);
    });

    this.sliderElm.on("slidestop", function(event, ui){
      JControlBtn.off(jQuery(self.btnDim2WarmElm));
      JControlBtn.off(jQuery(self.btnHCLElm));
      window.setTimeout(function() {jQuery(".ui-slider-handle").addClass("ui-state-active");},100);
      self.colorTemperature = ui.value;

      // Set Hue and Saturation to 0
      self.resetColorPicker();

    });

    if ((valueSlider > 0) && (valueSlider < 10000)) {jQuery(".ui-slider-handle").addClass("ui-state-active");} else {jQuery(".ui-slider-handle").removeClass("ui-state-active");}
    if (valueSlider == this.valHCL) {jQuery(".ui-slider-handle").removeClass("ui-state-active"); JControlBtn.on(this.btnHCLElm);this.sliderInfoElm.val("--");}
    if (valueSlider == this.valDim2Warm) {jQuery(".ui-slider-handle").removeClass("ui-state-active"); JControlBtn.on(this.btnDim2WarmElm);this.sliderInfoElm.val("--");}
    this.TWEvents = jQuery("[name='trTWSlider']").css("pointer-events");

    // Effects
    effect = parseInt(this.getConfigStringValue("E"));

    if (effect % 2 == 0) {
      this.effectModePrg = true;
      homematic("Interface.setMetadata", {"objectId": this.oChannel.id, "dataId": "effectModePrg", "value": this.effectModePrg});
      this.effectModeChkBox.prop("checked", true);
      effect =  (effect - 1);
    } else {
      this.effectModePrg = false;
      this.effectModeChkBox.prop("checked", false);
    }

    effect = (effect != -1) ? effect : 0; // 0 = default (No Effect)
    this.setEffectNames();

    this.effectSelBox.val(effect);

    // Show the appropriate dialog (Color, Color Temp, Effect)
    if ((effect != 0) || ((valueL == 0) && (arElmValues.length == 1))) {
      this.btnEffectDialog.click();
    } else if (parseInt(this.getConfigStringValue("TC")) > 0) {
      this.btnColorTempDialog.click();
    } else {
      if ((this.deviceType == this.lss) && (this.initValue.includes("TC="))) { //
        this.btnColorTempDialog.click();
      } else {
        this.btnColorDialog.click();
      }
    }

  },

  initColorPickerEvents: function() {
    var self = this;
    this.colorPicker.on("mount", function(color) {
      self.setColorPreviewElm(self.colorPickerInit.hue, self.colorPickerInit.saturation);
    });

    this.colorPicker.on("input:end", function(color) {
      var hsv_H = parseInt(color.hsv.h),
        hsv_S,
        hsv_S_Percent = parseInt(color.hsv.s);

      self.hueElm.val(hsv_H);
      self.HUE = hsv_H;

      self.satElm.val(hsv_S_Percent);
      self.SATURATION = hsv_S_Percent;

      self.setColorPreviewElm(color.hsv.h, color.hsv.s);

      // Set the TW slider to --
      self.resetColorTemp();

    });
  },

  resetColorPicker: function() {
    // Set Hue and Saturation to 0
    this.hueElm.val(0); this.HUE = 0;
    this.satElm.val(0); this.SATURATION = 0;
    this.colorPicker.color.hsv = {h: this.HUE, s: this.SATURATION, v: 0};
    this.setColorPreviewElm(this.HUE, this.SATURATION);
  },

  resetColorTemp: function() {
    JControlBtn.off(jQuery(this.btnHCLElm));
    JControlBtn.off(jQuery(this.btnDim2WarmElm));
    window.setTimeout(function() {jQuery(".ui-slider-handle").removeClass("ui-state-active");},100);
    this.colorTemperature = 0;
    this.sliderInfoElm.val("--");
    this.sliderElm.val(this.colorTemperature);
    this.sliderElm.slider('value', this.colorTemperature);
  },

  freezeColorTemp: function() {
    if ((this.HUE == 361) || (this.SATURATION == 101)) {
      this.trTWSlider.css("pointer-events","none"); // freeze the tw slider
    } else {
      this.trTWSlider.css("pointer-events",this.TWEvents); // unfreeze the tw slider
    }
  },

  getHSVColorPicker: function() {
    var self = this;
    jQuery("#anchorColorPicker").html("");

    this.colorPicker = new iro.ColorPicker("#anchorColorPicker", {
        // Set the size of the color picker
        width: 90,
        color: {h: self.colorPickerInit.hue, s: self.colorPickerInit.saturation, v: self.colorPickerInit.level},
        wheelLightness: false, // If set to false, the color wheel will not fade to black when the lightness decreases.
        layout: [{component: iro.ui.Wheel}], // don't show the V slider below the wheel - this value comes from the dimmer slider
        handleRadius: 4
      }
    );
    this.initColorPickerEvents();
  },

  getHSVColorSlider: function() {
    var self = this;
    jQuery("#anchorColorPicker").html("");

    this.colorPicker = new iro.ColorPicker("#anchorColorPicker", {
      width: 90,
      sliderSize: 20, // height
      color: {h: self.colorPickerInit.hue, s: 100, v: 100},
      handleSvg: '#handle',
      layout: [
        {
          component: iro.ui.Slider,
          options: {
            sliderType: 'hue',
            edgeRadius: 0
          }
        }
      ]
    });
    this.initColorPickerEvents();
  },

  getSliderOpts: function() {
    var opts = {};
    opts.animate = "fast";
    opts.min = parseInt(this.chnDescription.HARDWARE_COLOR_TEMPERATURE_WARM_WHITE);
    opts.min = (!isNaN(opts.min)) ? opts.min : this.ColorTempDefaultMin;
    opts.max = parseInt(this.chnDescription.HARDWARE_COLOR_TEMPERATURE_COLD_WHITE);
    opts.max = (!isNaN(opts.max)) ? opts.max : this.ColorTempDefaultMax;
    opts.value = ((opts.max - opts.min) / 2);
    opts.step = 50;
    opts.orientation = "horizontal";
    return opts;
  },

  showHideColorPicker: function () {
    var hueLastValue = this.lastValHueElm.prop("checked"),
      satLastValue = this.lastValSatElm.prop("checked");

    if (hueLastValue && satLastValue) {
      this.anchorColorPicker.hide();
      this.colorPreviewElm.hide();
    } else if (hueLastValue && ! satLastValue) {
      this.anchorColorPicker.hide();
      this.colorPreviewElm.hide();
    } else if (! hueLastValue && satLastValue) {
      this.getHSVColorSlider();
      this.anchorColorPicker.show();
      this.colorPreviewElm.show();
    } else {
      this.getHSVColorPicker();
      this.anchorColorPicker.show();
      this.colorPreviewElm.show();
    }
    this.resetHeight();
  },

  setColorPreviewElm: function(hue, sat) {
    var rgbVal;
    rgbVal = hsvToRgb(hue, sat, 100);
    this.colorPreviewElm.css("background-color", "rgb("+rgbVal.r+","+rgbVal.g+","+rgbVal.b+")");
  },

  checkValidity: function(val, min, max) {
    var result = val;
    if (val == "") {result = "";}

    if (parseInt(val) < 0) {result = min;}
    if (parseInt(val) > max) {result = max;}
    return result;
  },

  getConfigString: function() {
    var self = this,
      result,
      level,
      durationUnit = (this.chkBoxTimeLimitElm.prop("checked") == false) ? 2 : this.durationUnitElm.val(), // 2  = unit hour
      durationValue = (this.chkBoxTimeLimitElm.prop("checked") == false) ? 31 : this.durationValueElm.val(),
      ramptimeUnit = this.rampTimeUnitElm.val(),
      ramptimeValue = this.rampTimeValueElm.val(),
      lastValueHueSelected = this.lastValHueElm.prop("checked"),
      lastValueSatSelected = this.lastValSatElm.prop("checked"),
      hueValue = (lastValueHueSelected || (this.colorTemperature > 0)) ? 361 : this.HUE,
      satValue = (lastValueSatSelected  || (this.colorTemperature > 0)) ? 101 : this.SATURATION,
      colorTemperature =  this.colorTemperature,
      tempColorID = "",
      effectID = "",
      effect = parseInt(this.effectSelBox.val()),
      activeDialog = "color";

    level = (this.levelFreeValActive) ? this.levelFreeValElm.val() : this.levelElm.val();

    this.setULReffectModePrg();

    effect =  (this.effectModePrg == true)  ? (effect + 1) : effect;

    if ((this.deviceType == this.drgDali) || (this.deviceType == this.lss) || ((this.deviceType == this.rgbw) && ((this.maxCap == 2) || (this.maxCap == 4)))) {
      tempColorID = ",TC=" + colorTemperature;
      effectID = ",E=" + effect;
    }

    if (this.btnColorTempDialog.hasClass("ControlBtnOn")) {
      activeDialog = "colorTemp";
    } else if (this.btnEffectDialog.hasClass("ControlBtnOn")) {
      activeDialog = "effect";
      if (effect == 0) {
        result = "L=0";
      } else {
        result = "L=" + level + ",E=" + effect;
      }
      return result;
    }

    if (this.isOntimeAvailable()) {

      if (this.chkBoxTimeLimitElm.prop("checked") == false) {
        var _rampTimeValue = parseInt(this._getRampTimeVal(ramptimeValue, ramptimeUnit));

        if (_rampTimeValue > 0) {
          if (this.deviceType == this.lss) {
            if (this.getActiveDialog() == "color") {
              result = "L=" + level + ",OT=" + this.maxOnTime + ",RT=" + _rampTimeValue + ",H=" + hueValue + ",SAT=" + satValue;
            } else {
              // This is for the HmIP-LSS colorTemp
              result = "L=" + level + ",OT=" + this.maxOnTime + ",RT=" + _rampTimeValue;
            }
          } else {
            result = "L=" + level + ",OT=" + this.maxOnTime + ",RT=" + _rampTimeValue + ",H=" + hueValue + ",SAT=" + satValue;
          }
        } else {
          if (this.deviceType == this.lss) {
            if (this.getActiveDialog() == "color") {
              result = "L=" + level + ",OT=0,RT=0,H=" + hueValue + ",SAT=" + satValue;
            } else {
              // this ist for the HmIP-LSS color temp
              result = "L=" + level + ",OT=0,RT=0";
            }
          } else {
            result = "L=" + level + ",OT=0,RT=0,H=" + hueValue + ",SAT=" + satValue;
          }
        }

      } else {
        if (durationValue == 0) {

          if (this.deviceType == this.lss) {
            if (this.getActiveDialog() == "color") {
              result = "L=" + level + ",OT=" + this._getOnTimeVal(durationValue, durationUnit) + ",RT=0,H=" + hueValue + ",SAT=" + satValue;
            } else {
              // this ist for the HmIP-LSS color temp
              result = "L=" + level + ",OT=" + this._getOnTimeVal(durationValue, durationUnit) + ",RT=0";
            }
          } else {
            result = "L=" + level + ",OT=" + this._getOnTimeVal(durationValue, durationUnit) + ",RT=0,H=" + hueValue + ",SAT=" + satValue;
          }
        } else {
          if (this.deviceType == this.lss) {
            if (this.getActiveDialog() == "color") {
              result = "L=" + level + ",OT=" + this._getOnTimeVal(durationValue, durationUnit) + ",RT=" + this._getRampTimeVal(ramptimeValue, ramptimeUnit) + ",H=" + hueValue + ",SAT=" + satValue;
            } else {
              // this ist for the HmIP-LSS color temp
              result = "L=" + level + ",OT=" + this._getOnTimeVal(durationValue, durationUnit) + ",RT=" + this._getRampTimeVal(ramptimeValue, ramptimeUnit);
            }
          } else {
            result = "L=" + level + ",OT=" + this._getOnTimeVal(durationValue, durationUnit) + ",RT=" + this._getRampTimeVal(ramptimeValue, ramptimeUnit) + ",H=" + hueValue + ",SAT=" + satValue;
          }
        }
      }
    } else {
      result = "L=" + level + ",DV=" + durationValue + ",DU=" + durationUnit + ",RTV=" + ramptimeValue + ",RTU=" + ramptimeUnit + ",H=" + hueValue + ",SAT=" + satValue;
    }

    jQuery.each(this.showRampTimeOffElm, function(index, val) {
      if (self.deviceType == val) {
        return false; // leave each loop
      }
    });

    if (showRamptimeOff) {
      result += ",RTTOV=" + this.rampTimeOffValueElm.val() + ",RTTOU=" + this.rampTimeOffUnitElm.val();
    }

    if ((this.deviceType == this.drgDali) || ((this.deviceType == this.lss) && (this.getActiveDialog() == "colorTemp")) || ((this.deviceType == this.rgbw) && ((this.maxCap == 2) || (this.maxCap == 4)))) {
      result += tempColorID;
    }

    return result;
  },

  // Get the value of a given shortcut of the config string
  // Example config string > "L=100,OT=0,RT=0,H=0,SAT=0,RTTOV=0,RTTOU=0,TC=10150"
  getConfigStringValue: function(sShortCut) {
    var arElmValues = this.initValue.split(","),
      result = "-1", arKey;

    jQuery.each(arElmValues, function(index,val) {
      arKey = val.split("=");
      if (arKey[0] == sShortCut) {
        result = arKey[1];
      }
    });
    return (result == -1) ? 0 : result;
  },

  close: function(result)
  {
    Layer.remove(this.m_layer);
    if (this.m_callback) { this.m_callback(result); }
    if (showRamptimeOff) {window.setTimeout(function() {delete showRamptimeOff;},100);}
  },

  yes: function()
  {
    this.close(YesNoDialog.RESULT_YES);
  },

  no: function()
  {
    this.close(YesNoDialog.RESULT_NO);
  },

  btnTextYes: function(btnTxt) {
    jQuery(".YesNoDialog_yesButton").text(btnTxt);
  },

  btnYesHide: function() {
    jQuery("#btnYes").addClass("hidden");
  },

  btnYesShow: function() {
    jQuery("#btnYes").removeClass("hidden");
  },

  btnTextNo: function(btnTxt) {
    jQuery(".YesNoDialog_noButton").text(btnTxt);
  },

  btnNoHide: function() {
    jQuery("#btnNo").addClass("hidden");
  },

  btnNoShow: function() {
    jQuery("#btnNo").removeClass("hidden");
  },

  setHeight: function() {
    var heightContentWrapper = jQuery(".YesNoDialogContentWrapper").height(),
      yesNoElm = jQuery(".YesNoDialog"),
      footerElm = jQuery(".YesNoDialogFooter");

    yesNoElm.css("height", heightContentWrapper + 78);
    footerElm.css("top", heightContentWrapper + 26);
    yesNoElm.css("top", (window.innerHeight / 2) - (yesNoElm.height() / 2));
  },

  resetHeight: function() {
    this.setHeight();
  },

  setWidth: function(dlgWidth) {
    var yesNoDialogElm = jQuery(".YesNoDialog"),
      yesNoDialogContentWrapperElm = jQuery(".YesNoDialogContentWrapper"),
      yesNoDialogFooterElm = jQuery(".YesNoDialogFooter"),
      yesNoDialogTitleElm = jQuery(".YesNoDialogTitle"),
      yesNoDialogYesButton = jQuery(".YesNoDialog_yesButton");

    var defaultWith = 600,
      offsetWidth = 4,
      offsetPosYesButton = 109,
      offsetDialogHeight = 78,
      offsetDialogFooterHeight = 26;

    var width = dlgWidth - offsetWidth,
      yesButtonPos = dlgWidth - offsetPosYesButton,
      position = yesNoDialogElm.position();

    // dlgWidth = (defaultWith < dlgWidth) ? defaultWith : dlgWidth;

    yesNoDialogElm.width(dlgWidth).css({left: position.left + ((defaultWith - dlgWidth) / 2)});
    yesNoDialogTitleElm.width(width);
    yesNoDialogContentWrapperElm.width(width);
    yesNoDialogFooterElm.width(width);
    yesNoDialogYesButton.css("left", yesButtonPos);

    //Dialoghöhe an Content anpassen.
    yesNoDialogElm.css("height", yesNoDialogContentWrapperElm.height() + offsetDialogHeight);
    yesNoDialogFooterElm.css("top", yesNoDialogContentWrapperElm.height() + offsetDialogFooterHeight);
  }

});