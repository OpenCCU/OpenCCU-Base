SwitchCombinedParamDialog = Class.create({
 
  initialize: function(title, content, deviceType, chnAddress, value, callback, contentType)
  {
    var _this_ = this;

    this.m_contentType = contentType;
    this.m_callback = callback;
    this.m_layer = document.createElement("div");
    this.m_layer.className = "YesNoDialogLayer"; 

    this.RESULT_NO = 0;
    this.RESULT_YES = 1;

    this.deviceType = deviceType;
    this.chnAddress = chnAddress;
    this.initValue = value;

    this.windowDrive = "HmIP-MOD-WD-VK";
    this.dali = "HmIP-DRG-DALI";

    this.arNoOntimeAvailable = []; // here we can add devices with Value and Unit instead of Ontime in seconds

    var dialog = document.createElement("div");
    dialog.className = "YesNoDialog";
    
    var titleElement = document.createElement("div");
    titleElement.className = "YesNoDialogTitle";
    //titleElement.appendChild(document.createTextNode(title));
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

  _deviceIsWindowDrive: function() {
    return (this.deviceType == this.windowDrive) ? true : false;
  },

  _deviceIsDali: function() {
    return (this.deviceType == this.dali) ? true : false;
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

  isOntimeAvailable: function() {
    return (this.arNoOntimeAvailable.indexOf(this.deviceType) == -1) ? true : false;
  },

  setDialogElements: function() {
    this.trDurationElms = jQuery("[name='trDuration']");
    this.stateElm = jQuery("#combinedParam_State");
    this.chkBoxTimeLimitElm = jQuery("#chkBoxTimeLimit");
    this.durationValueElm = jQuery("#combinedParam_DurationValue");
    this.durationUnitElm = jQuery("#combinedParam_DurationUnit");
  },

  getConfigString: function() {
    var result,
      state = this.stateElm.val(),
      durationUnit = (this.chkBoxTimeLimitElm.prop("checked") == false) ? 2 : this.durationUnitElm.val(), // 2  = unit hour
      durationValue = (this.chkBoxTimeLimitElm.prop("checked") == false) ? 31 : this.durationValueElm.val();

      if (this.isOntimeAvailable()) {
        if ((this.chkBoxTimeLimitElm.prop("checked") == false) || (durationValue == 0) || (durationValue == 31 && durationUnit == 2)) {
          if (! this._deviceIsWindowDrive()) {
            if (! this._deviceIsDali()) {
              result = "S=" + state;
            } else {
              // The DALI STATE is LEVEL
              state = (state == "true") ? 100 : 0;
              result = "L=" + state;
            }
          } else {
            result = "VL=" + state;
          }

        } else {
          if (! this._deviceIsWindowDrive()) {
            if (! this._deviceIsDali()) {
              result = "S=" + state + ",OT=" + this._getOnTimeVal(durationValue, durationUnit);
            } else {
              // The DALI STATE is LEVEL
              state = (state == "true") ? 100 : 0;
              result = "L=" + state + ",OT=" + this._getOnTimeVal(durationValue, durationUnit);
            }

          } else {
            result = "VL=" + state + ",OT=" + this._getOnTimeVal(durationValue, durationUnit);
          }
        }
      } else {
        result = "S=" + state + ",DV=" + durationValue + ",DU=" + durationUnit;
      }
    return result;
  },

  checkValidity: function(val, min, max) {
    var result = val;
    if (val == "") {result = "";}

    if (parseInt(val) < 0) {result = min;}
    if (parseInt(val) > max) {result = max;}
    return result;
  },

  initDialog: function() {
    
    var self = this;

    var arElmValues, valueS, valueDV, valueDVtmp, valueDU, permanentHR, permanentHR_0, minDuration, maxDuration;

    if (this.isOntimeAvailable()) {
      arElmValues = this.initValue.split(",");

      if (arElmValues.length > 1) {
        valueS = arElmValues[0].split("=")[1];
        valueDVtmp = arElmValues[1].split("=")[1];
        valueDU = this._getUnitInDU4OnTime(valueDVtmp);

        if (valueDU == 2) {
          valueDV = parseInt(valueDVtmp / 3600);
        } else if (valueDU == 1) {
          valueDV = parseInt(valueDVtmp / 60);
        } else {
          valueDV = valueDVtmp;
        }
      } else {
        valueS = arElmValues[0].split("=")[1];
        valueDV = 0;
        valueDU = 2;
      }
      permanentHR = 31;
      permanentHR_0 = 0;
      minDuration = 0;
      maxDuration = 16343;
    } else {
      arElmValues = this.initValue.split(",");
      valueS = arElmValues[0].split("=")[1];
      valueDV = arElmValues[1].split("=")[1];
      valueDU = arElmValues[2].split("=")[1];
      permanentHR = 31;
      minDuration = 0;
      maxDuration = 16343;
    }

    if (this._deviceIsDali()) {
      valueS = (valueS == "100") ? "true" : "false";
    }

    this.stateElm.val(valueS);
    this.durationValueElm.val(valueDV);
    this.durationUnitElm.val(valueDU);
    
    if ((this.durationValueElm.val() == permanentHR && this.durationUnitElm.val() == 2) || (this.isOntimeAvailable() && valueDV == 0)) {
      this.chkBoxTimeLimitElm.prop("checked", false);
      this.trDurationElms.css("visibility", "visible");
      this.trDurationElms.css("opacity", "0.2");
    } else {
      this.chkBoxTimeLimitElm.prop("checked", true);
      this.trDurationElms.css("visibility", "visible");
    }

    this.chkBoxTimeLimitElm.bind("change", function(){
      if (this.checked) {
        self.durationValueElm.prop('disabled', false);
        self.durationUnitElm.prop('disabled', false);
        self.trDurationElms.fadeTo(1000, 1);
      } else {
        self.durationValueElm.prop('disabled', true);
        self.durationUnitElm.prop('disabled', true);
        self.trDurationElms.fadeTo(1000, 0.2);

        if (self.isOntimeAvailable()) {
          self.durationValueElm.val(permanentHR_0);
        } else {
          self.durationValueElm.val(permanentHR);
        }

        self.durationUnitElm.val(2);
      }
    });

    this.durationValueElm.bind("keyup", function() {
      var min = 0,
        max = (parseInt(self.durationUnitElm.val()) == 2) ? permanentHR: maxDuration;
      this.value = self.checkValidity(this.value,min,max);
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

  },
  
  close: function(result)
  {
    Layer.remove(this.m_layer);
    if (this.m_callback) { this.m_callback(result); }
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
    var defaultWith = 600,
      offsetWidth = 4,
      offsetPosYesButton = 109,
      offsetDialogHeight = 78,
      offsetDialogFooterHeight = 26;

    var width = dlgWidth - offsetWidth,
      yesButtonPos = dlgWidth - offsetPosYesButton,
      position = jQuery(".YesNoDialog").position();

    // dlgWidth = (defaultWith < dlgWidth) ? defaultWith : dlgWidth;

    jQuery(".YesNoDialog").width(dlgWidth).css({left: position.left + ((defaultWith - dlgWidth) / 2)});
    jQuery(".YesNoDialogTitle").width(width);
    jQuery(".YesNoDialogContentWrapper").width(width);
    jQuery(".YesNoDialogFooter").width(width);
    jQuery(".YesNoDialog_yesButton").css("left", yesButtonPos);

    //Dialoghöhe an Content anpassen.
    jQuery(".YesNoDialog").css("height", jQuery(".YesNoDialogContentWrapper").height() + offsetDialogHeight);
    jQuery(".YesNoDialogFooter").css("top", jQuery(".YesNoDialogContentWrapper").height() + offsetDialogFooterHeight);
  }
  
});
