RGBWTunableWhiteControllerDialog = Class.create({
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

    this.iFace = "HmIP-RF";

    this.deviceType = deviceType;
    this.chnAddress = chnAddress;
    this.initValue = value;

    this.maxOnTime = 111600;

    this.valHCL = 10200;
    this.valDim2Warm = 10150;

    this.arNoOntimeAvailable = [];
    this.showRampTimeOffElm = ["HmIP-RGBW", "HmIP-DRG-DALI"];

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
    this.trDurationElms = jQuery("[name='trDuration']");
    this.trRampTimeElms = jQuery("[name='trRampTime']");
    this.trRampTimeOff = jQuery("#trRampTimeOff");
    this.levelElm = jQuery("#combinedParam_Level");
    this.lblBrightnessLevelElm = jQuery("#lblBrightnessLevel");
    this.lblRampTimeElm = jQuery("#lblRampTime");
    this.chkBoxTimeLimitElm = jQuery("#chkBoxTimeLimit");
    this.durationValueElm = jQuery("#combinedParam_DurationValue");
    this.durationUnitElm = jQuery("#combinedParam_DurationUnit");
    this.rampTimeUnitElm = jQuery("#combinedParam_RampTimeUnit");
    this.rampTimeValueElm = jQuery("#combinedParam_RampTimeValue");
    this.rampTimeOffUnitElm = jQuery("#combinedParam_RampTimeOffUnit");
    this.rampTimeOffValueElm = jQuery("#combinedParam_RampTimeOffValue");

    this.sliderElm = jQuery("#twSlider");
    this.sliderInfoElm = jQuery("#tcInfoField");
    this.btnHCLElm = jQuery("#btnHCL");
    this.btnDim2WarmElm = jQuery("#btnDim2Warm");
    this.colorTemperature = 0;

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


  initDialog: function() {
    var self = this;

    var arElmValues, valueL, valueDV, valueDVtmp, valueDU, valueRTV, valueRTVtmp, valueRTU, valueC, valueSlider, valueRTTOU, valueRTTOV, permanentHR, permanentHR_0, minDuration, maxDuration;

    if (this.isOntimeAvailable()) {
      arElmValues = this.initValue.split(",");

      valueL = arElmValues[0].split("=")[1];
      valueDVtmp = arElmValues[1].split("=")[1];
      valueDU = this._getUnitInDU4OnTime(valueDVtmp);

      valueSlider = parseInt(arElmValues[3].split("=")[1]);

      if (valueDU == 2) {
        valueDV = parseInt(valueDVtmp / 3600);
      } else if (valueDU == 1) {
        valueDV = parseInt(valueDVtmp / 60);
      } else {
        valueDV = valueDVtmp;
      }

      valueRTVtmp = arElmValues[2].split("=")[1];

      valueRTU = this._getUnitInDU4RampTime(valueRTVtmp);

      if (valueRTU == 3) {
        valueRTV = valueRTVtmp * 100;
      } else if (valueRTU == 1) {
        valueRTV = parseInt(valueRTVtmp / 60);
      } else {
        valueRTV = valueRTVtmp;
      }

      valueC = 7;
      permanentHR = 31;
      permanentHR_0 = 0;
      minDuration = 0;
      maxDuration = 16343;
    }
    // RAMPTIME_OFF
    if (arElmValues.length >= 6) {
      valueRTTOV = arElmValues[4].split("=")[1];
      valueRTTOU = arElmValues[5].split("=")[1];
      this.rampTimeOffUnitElm.val(valueRTTOU);
      this.rampTimeOffValueElm.val(valueRTTOV);
    }

    this.levelElm.val(valueL);
    this.durationValueElm.val(valueDV);
    this.durationUnitElm.val(valueDU);

    this.rampTimeUnitElm.val(valueRTU);
    this.rampTimeValueElm.val(valueRTV);

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
    /**********************/

    this.btnHCLElm.bind("click", function() {
      JControlBtn.on(jQuery(this));
      JControlBtn.off(jQuery(self.btnDim2WarmElm));
      window.setTimeout(function() {jQuery(".ui-slider-handle").removeClass("ui-state-active");},100);
      self.colorTemperature = self.valHCL;
      self.sliderInfoElm.val("--");
    });

    this.btnDim2WarmElm.bind("click", function() {
      JControlBtn.on(jQuery(this));
      JControlBtn.off(jQuery(self.btnHCLElm));
      window.setTimeout(function() {jQuery(".ui-slider-handle").removeClass("ui-state-active");},100);
      self.colorTemperature = self.valDim2Warm;
      self.sliderInfoElm.val("--");
    });

    this.sliderElm.slider(this.getSliderOpts());


    this.sliderInfoElm.val(valueSlider);
    this.sliderElm.val(valueSlider);
    this.sliderElm.slider('value', valueSlider);
    this.colorTemperature = valueSlider;

    this.sliderElm.on("slide", function (event, ui) {
       self.sliderInfoElm.val(ui.value);
    });

    this.sliderElm.on("slidestop", function(event, ui){
      JControlBtn.off(jQuery(self.btnDim2WarmElm));
      JControlBtn.off(jQuery(self.btnHCLElm));
      window.setTimeout(function() {jQuery(".ui-slider-handle").addClass("ui-state-active");},100);
      self.colorTemperature = ui.value;
    });

    if (valueSlider < 10000) {jQuery(".ui-slider-handle").addClass("ui-state-active");} else {jQuery(".ui-slider-handle").removeClass("ui-state-active");}
    if (valueSlider == this.valHCL) {jQuery(".ui-slider-handle").removeClass("ui-state-active"); JControlBtn.on(this.btnHCLElm);this.sliderInfoElm.val("--");}
    if (valueSlider == this.valDim2Warm) {jQuery(".ui-slider-handle").removeClass("ui-state-active"); JControlBtn.on(this.btnDim2WarmElm);this.sliderInfoElm.val("--");}


  },

  getSliderOpts: function() {

    var chnDescription = homematic("Interface.getParamset", {'interface': this.iFace, 'address': this.chnAddress, 'paramsetKey': 'MASTER'});

    var opts = {};
    opts.animate = "fast";
    opts.min = parseInt(chnDescription.HARDWARE_COLOR_TEMPERATURE_WARM_WHITE);
    opts.max = parseInt(chnDescription.HARDWARE_COLOR_TEMPERATURE_COLD_WHITE);
    opts.value = ((opts.max - opts.min) / 2);
    opts.step = 50;
    opts.orientation = "horizontal";
    return opts;
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
      level = this.levelElm.val(),
      durationUnit = (this.chkBoxTimeLimitElm.prop("checked") == false) ? 2 : this.durationUnitElm.val(), // 2  = unit hour
      durationValue = (this.chkBoxTimeLimitElm.prop("checked") == false) ? 31 : this.durationValueElm.val(),
      ramptimeUnit = this.rampTimeUnitElm.val(),
      ramptimeValue = this.rampTimeValueElm.val(),
      colorTemperature =  this.colorTemperature;

    if (this.isOntimeAvailable()) {
      if (this.chkBoxTimeLimitElm.prop("checked") == false) {
        var _rampTimeValue = parseInt(this._getRampTimeVal(ramptimeValue, ramptimeUnit));

        if (_rampTimeValue > 0) {
          result = "L=" + level + ",OT=" + this.maxOnTime + ",RT=" + _rampTimeValue + ",TC=" + colorTemperature; // ON_TIME = permanently ON
        } else {
          result = "L=" + level + ",OT=0,RT=0,TC=" + colorTemperature;
        }

      } else {
        if (durationValue == 0) {
          result = "L=" + level + ",OT=" + this._getOnTimeVal(durationValue, durationUnit) + ",RT=0,TC=" + colorTemperature;
        } else {
          result = "L=" + level + ",OT=" + this._getOnTimeVal(durationValue, durationUnit) + ",RT=" + this._getRampTimeVal(ramptimeValue, ramptimeUnit) + ",TC=" + colorTemperature;
        }
      }
    } else {
      result = "L=" + level + ",DV=" + durationValue + ",DU=" + durationUnit + ",RTV=" + ramptimeValue + ",RTU=" + ramptimeUnit + ",TC=" + colorTemperature;
    }

    jQuery.each(this.showRampTimeOffElm, function(index, val) {
      if (self.deviceType == val) {
        return false; // leave each loop
      }
    });

    if (showRamptimeOff) {
      result += ",RTTOV=" + this.rampTimeOffValueElm.val() + ",RTTOU=" + this.rampTimeOffUnitElm.val();
    }
    return result;
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
