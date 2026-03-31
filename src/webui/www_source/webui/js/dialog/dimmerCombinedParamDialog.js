DimmerCombinedParamDialog = Class.create({
  initialize: function(title, content, deviceType, chnAddress, value, callback, contentType)
  {
    showRamptimeOff = false; // This we need among other things for certain COMBINED_PARAMETER help dialogs.
    var _this_ = this;

    this.iFace = "HmIP-RF";

    this.m_contentType = contentType;
    this.m_callback = callback;
    this.m_layer = document.createElement("div");
    this.m_layer.className = "YesNoDialogLayer"; 

    this.RESULT_NO = 0;
    this.RESULT_YES = 1;

    this.deviceType = deviceType;
    this.chnAddress = chnAddress;
    this.initValue = value;

    this.maxOnTime = 111600;

    this.devDescr = homematic("Interface.getDeviceDescription", {
      "interface" : this.iFace,
      "address" : this.chnAddress.split(":")[0]
    });

    this.devFirmware = this.devDescr.firmware.split(".");
    this.fwMajor = parseInt(this.devFirmware[0]);
    // this.fwMinor = parseInt(this.devFirmware[1]); // currently not in use
    // this.fwPatch = parseInt(this.devFirmware[2]); // currently not in use

    this.isUniversalActor = ((this.deviceType == "HmIP-WUA") || (this.deviceType == "ELV-SH-WUA")) ? true : false;
    this.isServoController = ((this.deviceType == "HmIP-WSC") || (this.deviceType == "ELV-SH-WSC")) ? true : false;
    this.isWGS = ((this.deviceType == "HmIP-WGS") || (this.deviceType == "HmIP-WGS-A") || this.deviceType == "HmIPW-WGS") ? true : false;
    this.arNoOntimeAvailable = ["HmIP-MP3P", "HmIP-BSL", "HmIPW-WRC6", "HmIPW-WRC6-A", "HmIP-WRC6-230", "HmIP-WRC6-230-A"];
    this.arNoRamptimeAvailable = ["HmIP-WGS", "HmIP-WGS-A", "HmIPW-WGS"];
    this.showRampTimeOffElm = ["HmIPW-WRC6", "HmIPW-WRC6-A", "HmIP-WRC6-230", "HmIP-WRC6-230-A"];
    this.showColorElms = ["HmIP-MP3P", "HmIP-BSL", "HmIPW-WRC6", "HmIPW-WRC6-A", "HmIP-WRC6-230", "HmIP-WRC6-230-A"];
    this.showBehaviourElms = ["HmIPW-WRC6", "HmIPW-WRC6-A", "HmIP-WRC6-230", "HmIP-WRC6-230-A"];

    // SPHM-1268
    if ((this.deviceType == "HmIP-BSL") && (this.fwMajor >= 2)) {
      this.showBehaviourElms.push(this.deviceType);
    }

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

    this.colorElmVisible = false;
    this.behaviourElmVisible = false;
    this.setDialogElements();

    if (! this.isWGS) {
      this.initDialog();
    } else {
      this.initDialog_WGS();
    }
  },

  isOntimeAvailable: function() {
    return (this.arNoOntimeAvailable.indexOf(this.deviceType) == -1) ? true : false;
  },

  setDialogElements: function() {

    this.trSelectColorElm = jQuery("#trSelectColor");
    this.trSelectBehaviourElm = jQuery("#trSelectBehaviour");
    this.trDurationElms = jQuery("[name='trDuration']");
    this.trRampTimeElms = jQuery("[name='trRampTime']");
    this.trRampTimeOff = jQuery("#trRampTimeOff");
    this.selectColorElm = jQuery("#combinedParam_Color");
    this.selectBehaviourElm = jQuery("#combinedParam_Behaviour");
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

  },
  
  // For those devices who are able to change the color show the color selector.
  _showColorElm: function() {
     var self = this;
     jQuery.each(this.showColorElms, function(index, val) {
       if (self.deviceType == val) {
         self.trSelectColorElm.css("visibility", "visible");
         self.colorElmVisible = true;
         return false; // leave each loop
       }
     });
  },

  // For those devices who are able to change the behaviour (slow blinking, fast blinking and so on) show the color selector.
  _showBehaviourElm: function() {
    var self = this;
    jQuery.each(this.showBehaviourElms, function(index, val) {
      if (self.deviceType == val) {
        self.trSelectBehaviourElm.css("visibility", "visible");
        self.behaviourElmVisible = true;
        return false; // leave each loop
      }
    });
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

    if (this.isUniversalActor) { // WUA
      this.lblBrightnessLevelElm.text(translateKey("lblOperatingVoltage"));
    } else if (this.isServoController) {
      this.hideOnTimeElems();
      this.lblBrightnessLevelElm.text(translateKey("stringTableServoLevel"));
      this.levelElm.find("option[value='0']").text("0 %");
      this.lblRampTimeElm .text(translateKey("stringTableServoRamp"));
    }

    // iValueL !== (iValueL | 0) = check if the value is not 0% - 100%
    if ( (iValueL !== (iValueL | 0)) && (valueL != "100.5") && (valueL != "101")) {
      this.levelFreeValActive = true;
      this.levelFreeValElm.val(valueL);
      this.divLevelFreeValElm.show();
    }
    this._showColorElm();
    this._showBehaviourElm();
    if (this.isOntimeAvailable()) {
      valueDVtmp = arElmValues[1].split("=")[1];
      valueDU = this._getUnitInDU4OnTime(valueDVtmp);

      if (valueDU == 2) {
        valueDV = parseInt(valueDVtmp / 3600);
      } else if (valueDU == 1) {
        valueDV = parseInt(valueDVtmp / 60);
      } else {
        valueDV = valueDVtmp;
      }

      valueRTVtmp = (!this.isServoController) ? arElmValues[2].split("=")[1] : arElmValues[1].split("=")[1];

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
    } else {
      valueDV = arElmValues[1].split("=")[1];
      valueDU = arElmValues[2].split("=")[1];
      valueRTV = arElmValues[3].split("=")[1];
      valueRTU = arElmValues[4].split("=")[1];
      valueC = 7;
      permanentHR = 31;
      minDuration = 0;
      maxDuration = 16343;
    }

    // Color
    if (this.colorElmVisible && (arElmValues.length >= 6)) {
      valueC = arElmValues[5].split("=")[1];
      this.selectColorElm.val(valueC);
    }

    // Behaviour (blink slow, blink fast, ....)
    if (this.behaviourElmVisible && (arElmValues.length >= 7)) {
      valueCB = arElmValues[6].split("=")[1];
      this.selectBehaviourElm.val(valueCB);
    }

    // RAMPTIME_OFF
    if (arElmValues.length >= 9) {
      valueRTTOV = arElmValues[7].split("=")[1];
      valueRTTOU = arElmValues[8].split("=")[1];
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
      if (this.arNoRamptimeAvailable.indexOf(this.deviceType) == -1) {
        this.trRampTimeElms.css("visibility", "visible");
      } else {
        this.trRampTimeElms.css("visibility", "hidden");
      }

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
          self.trRampTimeOff.fadeTo(1000, 1);
        }
      } else {
        self.durationValueElm.prop('disabled', true);
        self.durationUnitElm.prop('disabled', true);
        self.rampTimeValueElm.prop('disabled', false); //.val(0);
        self.rampTimeUnitElm.prop('disabled', false); //.val(0);
        self.trDurationElms.fadeTo(1000, 0.2);
        self.trRampTimeElms.fadeTo(1000, 1);

        if (self.showRampTimeOffElm.indexOf(self.deviceType) != -1) {
          self.trRampTimeOff.fadeTo(1000, 0.2);
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

    this.durationValueElm.bind("keyup", function () {
      var min = minDuration,
        max = (parseInt(self.durationUnitElm.val()) == 2) ? permanentHR : maxDuration;
      this.value = self.checkValidity(this.value, min, max);
      if (parseInt(this.value) == 31) {
        self.rampTimeOffValueElm.val(min);
        self.rampTimeOffUnitElm.val(3); // = 10ms
      }
    });

    this.durationValueElm.bind("blur", function () {
      var val = parseInt(this.value);

      if (isNaN(val)) {
        this.value = (parseInt(self.durationUnitElm.val()) == 2) ? permanentHR : maxDuration;
      } else {
        this.value = val;
      }
      self.durationValueElm.keyup();
    });

    this.durationUnitElm.bind("change", function () {
      self.durationValueElm.keyup();
    });

    this.rampTimeValueElm.bind("keyup", function () {
      var min = minDuration,
        max = maxDuration;
      this.value = self.checkValidity(this.value, min, max);
    });

    this.rampTimeValueElm.bind("blur", function () {
      var val = parseInt(this.value);

      if (isNaN(val)) {
        this.value = maxDuration;
      } else {
        this.value = val;
      }
    });

    this.rampTimeUnitElm.bind("change", function () {
      self.rampTimeValueElm.keyup();
    });

    /**********************/
    this.rampTimeOffValueElm.bind("keyup", function () {
      var min = minDuration,
        max = maxDuration;
      this.value = self.checkValidity(this.value, min, max);
    });

    this.rampTimeOffValueElm.bind("blur", function () {
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

    this.rampTimeUnitElm.bind("change", function () {
      self.rampTimeOffValueElm.keyup();
    });
    /**********************/

  },

  initDialog_WGS: function() {
    var self = this;

    this.levelElm.change(function () {
      self.showHideLevelFreeValue();
    });
    this.levelFreeValElm.blur(function () {
      self.isLevelValid(this);
    });

    var arElmValues, valueL, iValueL, valueDV, valueDVtmp, valueDU, permanentHR, permanentHR_0;

    arElmValues = this.initValue.split(",");
    valueL = arElmValues[0].split("=")[1];
    iValueL = parseInt(valueL) / 10;

    // iValueL !== (iValueL | 0) = check if the value is not 0% - 100%
    if ((iValueL !== (iValueL | 0)) && (valueL != "100.5") && (valueL != "101")) {
      this.levelFreeValActive = true;
      this.levelFreeValElm.val(valueL);
      this.divLevelFreeValElm.show();
    }

    valueDVtmp = arElmValues[1].split("=")[1];
    valueDU = this._getUnitInDU4OnTime(valueDVtmp);

    if (valueDU == 2) {
      valueDV = parseInt(valueDVtmp / 3600);
    } else if (valueDU == 1) {
      valueDV = parseInt(valueDVtmp / 60);
    } else {
      valueDV = valueDVtmp;
    }

    permanentHR = 31;
    permanentHR_0 = 0;

    this.levelElm.val(valueL);

    this.durationValueElm.val(valueDV);
    this.durationUnitElm.val(valueDU);

    if ((this.durationValueElm.val() == permanentHR && this.durationUnitElm.val() == 2) || (this.isOntimeAvailable() && valueDV == 0)) {
      this.chkBoxTimeLimitElm.prop("checked", false);
      this.durationValueElm.prop('disabled', true);
      this.durationUnitElm.prop('disabled', true);
      this.trDurationElms.css("visibility", "visible");
      this.trRampTimeElms.css("visibility", "hidden");
      this.trDurationElms.css("opacity", "0.2");
    } else {
      this.chkBoxTimeLimitElm.prop("checked", true);
      this.durationValueElm.prop('disabled', false);
      this.durationUnitElm.prop('disabled', false);
      this.trDurationElms.css("visibility", "visible");
      this.trRampTimeElms.css("visibility", "hidden");
    }

    this.chkBoxTimeLimitElm.bind("change", function () {
      if (this.checked) {
        self.durationValueElm.prop('disabled', false);
        self.durationUnitElm.prop('disabled', false);
        self.trDurationElms.fadeTo(1000, 1);
        self.durationValueElm.val(permanentHR);
      } else {
        self.durationValueElm.prop('disabled', true);
        self.durationUnitElm.prop('disabled', true);
        self.trDurationElms.fadeTo(1000, 0.2);
        self.durationValueElm.val(permanentHR_0);
        self.durationUnitElm.val(2);
      }
    });
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
      valColor = "",
      valBehaviour = "";

      level = (this.levelFreeValActive) ? this.levelFreeValElm.val() : this.levelElm.val();

    if (this.colorElmVisible || this.behaviourElmVisible) {
      if (this.colorElmVisible && ! this.behaviourElmVisible) {
        valColor = this.selectColorElm.val();
        result = "L=" + level + ",DV=" + durationValue + ",DU=" + durationUnit + ",RTV=" + ramptimeValue + ",RTU=" + ramptimeUnit + ",C=" + valColor;
      }

      if (! this.colorElmVisible && this.behaviourElmVisible) {
        valBehaviour = this.selectBehaviourElm.val();
        result = "L=" + level + ",DV=" + durationValue + ",DU=" + durationUnit + ",RTV=" + ramptimeValue + ",RTU=" + ramptimeUnit + ",CB=" +valBehaviour;
      }

      if (this.colorElmVisible && this.behaviourElmVisible) {
        valColor = this.selectColorElm.val();
        valBehaviour = this.selectBehaviourElm.val();
        result = "L=" + level + ",DV=" + durationValue + ",DU=" + durationUnit + ",RTV=" + ramptimeValue + ",RTU=" + ramptimeUnit + ",C=" + valColor + ",CB=" +valBehaviour;
      }

    } else {
      if (this.isOntimeAvailable()) {
        if (this.chkBoxTimeLimitElm.prop("checked") == false) {
          var _rampTimeValue = parseInt(this._getRampTimeVal(ramptimeValue, ramptimeUnit));

          if (this.isServoController) {
            result = "L=" + level + ",RT=" + _rampTimeValue; // ON_TIME is for the Hmip-WSC not allowed (see SPHM-942)
          } else {
            if ((! this.isWGS)  &&  (_rampTimeValue > 0) ) {
              result = "L=" + level + ",OT=" + this.maxOnTime + ",RT=" + _rampTimeValue; // ON_TIME = permanently ON
            } else {
              if (! this.isWGS) {
                result = "L=" + level + ",OT=0,RT=0";
              } else {
                result = "L=" + level + ",OT=0";
              }
            }
          }
        } else {
          if (this.isServoController) {
            var _rampTimeValue = parseInt(this._getRampTimeVal(ramptimeValue, ramptimeUnit));
            result = "L=" + level + ",RT=" + _rampTimeValue; // ON_TIME is for the Hmip-WSC not allowed (see SPHM-942)
          } else {

            if (! this.isWGS) {
              if (durationValue == 0) {
                result = "L=" + level + ",OT=" + this._getOnTimeVal(durationValue, durationUnit) + ",RT=0";
              } else {
                result = "L=" + level + ",OT=" + this._getOnTimeVal(durationValue, durationUnit) + ",RT=" + this._getRampTimeVal(ramptimeValue, ramptimeUnit);
              }
            } else {
              if (durationValue == 0) {
                result = "L=" + level + ",OT=" + this._getOnTimeVal(durationValue, durationUnit) ;
              } else {
                result = "L=" + level + ",OT=" + this._getOnTimeVal(durationValue, durationUnit) ;
              }
            }

          }
        }
      } else {
        result = "L=" + level + ",DV=" + durationValue + ",DU=" + durationUnit + ",RTV=" + ramptimeValue + ",RTU=" + ramptimeUnit;
      }
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
