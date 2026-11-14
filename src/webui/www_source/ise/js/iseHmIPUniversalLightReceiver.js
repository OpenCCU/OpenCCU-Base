iseUniversalLightReceiver = Class.create();
iseUniversalLightReceiver.prototype = {

  initialize: function(chnId, opts) {
    conInfo("opts", opts);
    var self = this;
    this.oDimmerElm = opts.oDimmerSlider;
    this.colorPicker;
    selectedColor = "not set";
    this.chnId = chnId;
    this.iface = opts.iface;
    this.chAddress = opts.chAddress;
    this.HUE = opts.hue;
    this.SATURATION = opts.saturation;
    this.LEVEL = opts.level;
    this.EFFECT = opts.effect;
    this.DURATION_VALUE = 31;
    this.DURATION_UNIT = 2; // hour

    this.hueStatus = opts.hueStatus;

    this.colorPickerInit = {
      hue : parseInt(opts.hue),
      saturation : parseInt((opts.saturation*100)),
      level : parseInt((opts.level * 100) )
    };

    this.percLevelElm = jQuery("#"+this.chnId+"Perc");

    this.hueElm = jQuery("#hueElmId_"+ this.chnId);
    this.satElm = jQuery("#satElmId_"+ this.chnId);
    this.bckGndElm = jQuery("#bckGndlmId_"+ this.chnId);
    this.setActiveElm();
    this.setKeyHandler();
    this.getColorPicker();
  },

  setActiveElm: function() {
    if (this.EFFECT > 0) {
      jQuery(".j_ControlBtnInfo").first().removeClass("ControlBtnInfoActive").addClass("ControlBtnInfo");

      // set the active effect button blue
      var btnEffect = jQuery("#ulrEffect_"+(parseInt(parseInt(this.EFFECT + (this.EFFECT % 2)) / 2))+"_" + this.chnId);
      jQuery("[name='urlEffect_"+this.chnId+"']").switchClass("ControlBtnOn","ControlBtnOff", 0); // deactivate all effect buttons
      btnEffect.switchClass('ControlBtnOff', 'ControlBtnOn', 0); // activate the active effect button

    } else {
      jQuery(".j_ControlBtnInfo").first().removeClass("ControlBtnInfo").addClass("ControlBtnInfoActive");
    }
  },


  setKeyHandler: function() {
    var self = this;

    this.bckGndElm.click(function(event) {
      self.activateColorPicker();
      jQuery(this).change();
      self.saveColor();
    });

    this.hueElm.keyup(function(event) {
      var val;
      if (event.keyCode == 13) {
        val = parseInt(jQuery(this).val());
        if (val < 0 || isNaN(val)) {self.HUE = 0;} else if (val > 360) {self.HUE = 360;} else {self.HUE = val;};
        jQuery(this).val(self.HUE);
        self.saveColor();
      }
    });

    this.hueElm.blur(function(event) {
      self.activateColorPicker();
      var val;
        val = parseInt(jQuery(this).val());
        if (val < 0 || isNaN(val)) {self.HUE = 0;} else if (val > 360) {self.HUE = 360;} else {self.HUE = val;};
        jQuery(this).val(self.HUE);
        self.saveColor();
    });

    this.satElm.keyup(function(event) {
      var val;
      if (event.keyCode == 13) {
        val = (parseInt(jQuery(this).val()) / 100);
        if (val < 0 || isNaN(val)) {self.SATURATION = 0;} else if (val > 1) {self.SATURATION = 1;} else {self.SATURATION = val;};
        jQuery(this).val(parseInt((self.SATURATION * 100)));
        self.saveColor();
      }
    });

    this.satElm.blur(function(event) {
      self.activateColorPicker();
      var val;
      val = (parseInt(jQuery(this).val()) / 100);
      if (val < 0 || isNaN(val)) {self.SATURATION = 0;} else if (val > 1) {self.SATURATION = 1;} else {self.SATURATION = val;};
      jQuery(this).val(parseInt((self.SATURATION * 100)));
      self.saveColor();
    });

  },

  onPercChange: function() {
    conInfo("***** onPercChange *****");
  },

  onHandleClick: function() {
    this.LEVEL = (parseInt(this.percLevelElm.val()) / 100);
    //this.saveColor();
    //this.btnOkElm.click();
  },

  getColorPicker: function() {
    var self = this,
      lastHueSat;

      if (this.hueStatus == 0) {
        this.hueElm.val(this.colorPickerInit.hue);
        this.satElm.val(this.colorPickerInit.saturation);

        homematic("Interface.setMetadata", {
          "objectId": self.chnId,
          "dataId": "lastValHueSat",
          "value": "HUE:" + self.HUE + ",SAT:" + self.SATURATION * 100
        }, function (result) {
          conInfo("iseUniversalLightReceiver getColorPicker - Metadata set: " + result);
        });

      } else {
        lastHueSat = homematic("Interface.getMetadata", {
          "objectId": this.chnId,
          "dataId": "lastValHueSat"
        });

        // get the meta data HUE and SAT
        var arHueSat = [],
          valHUE = 0,
          valSATPercent = 0,
          valSAT = 0;

        if (lastHueSat != "null") {
          arHueSat = lastHueSat.split(",");
          valHUE = arHueSat[0].split(":")[1];
          valSATPercent = parseInt(arHueSat[1].split(":")[1]);
          valSAT = valSATPercent / 100;
        }

        this.HUE = valHUE;
        this.SATURATION = valSAT;
        this.hueElm.val(this.HUE);
        this.satElm.val(valSATPercent);

        this.colorPickerInit.hue = this.HUE;
        this.colorPickerInit.saturation = valSATPercent;
      }


    this.oDimmerElm.percChange = this.onPercChange.bindAsEventListener(this);
    Event.observe($(this.chnId + "Perc"), 'change', this.oDimmerElm.percChange);

    this.oDimmerElm.handleClick = this.onHandleClick.bindAsEventListener(this);
    Event.observe($("slidCtrl" + this.chnId), 'mouseup', this.oDimmerElm.handleClick);

    this.colorPicker = new iro.ColorPicker("#colorPicker_" + this.chnId, {
      // Set the size of the color picker
      width: 90,
      color: {h: self.colorPickerInit.hue, s: self.colorPickerInit.saturation, v: self.colorPickerInit.level},
      wheelLightness: false, // If set to false, the color wheel will not fade to black when the lightness decreases.
      layout: [{component: iro.ui.Wheel}], // don't show the V slider below the wheel - this value comes from the dimmer slider
      handleRadius: 4
      }
    );

    this.colorPicker.on("mount", function(color) {
      //var rgbVal = hsvToRgb(self.colorPickerInit.hue, self.colorPickerInit.saturation, self.colorPickerInit.level);
      var rgbVal = hsvToRgb(self.colorPickerInit.hue, self.colorPickerInit.saturation, 100);
      self.bckGndElm.css("background-color", "rgb("+rgbVal.r+","+rgbVal.g+","+rgbVal.b+")");
    });

    this.colorPicker.on("input:end", function(color) {
      var hsv_H = color.hsv.h,
        hsv_S,
        hsv_S_Percent = parseInt(color.hsv.s),
        rgbVal;

      hsv_S = (parseInt(color.hsv.s) / 100);

      self.hueElm.val(hsv_H).change();
      self.HUE = hsv_H;

      self.satElm.val(hsv_S_Percent).change();
      self.SATURATION = hsv_S;

      rgbVal = hsvToRgb(color.hsv.h, color.hsv.s, 100);
      self.bckGndElm.css("background-color", "rgb("+rgbVal.r+","+rgbVal.g+","+rgbVal.b+")");

      self.saveColor();
    });
  },

  activateColorPicker: function() {
    jQuery("[name='urlEffect_"+this.chnId+"']").switchClass("ControlBtnOn","ControlBtnOff", 0); // deactivate all effect buttons
    jQuery(".j_ControlBtnInfo").first().removeClass("ControlBtnInfoNotActive").addClass("ControlBtnInfoActive");
  },

  saveColor: function() {
    var self = this;
    conInfo(
      "saveColor - HUE: " + self.HUE +
      " - SAT: " + self.SATURATION +
      " - LEVEL: " + this.LEVEL
    );

    homematic("Interface.putParamset", {
      'interface': self.iface,
      'address': self.chAddress,
      'paramsetKey': 'VALUES',
      'set':
        [
          {name: 'LEVEL', type: 'double', value: self.LEVEL},
          {name: 'HUE', type: 'int', value: self.HUE},
          {name: 'SATURATION', type: 'double', value: self.SATURATION},
          {name: 'DURATION_VALUE', type: 'int', value: self.DURATION_VALUE},
          {name: 'DURATION_UNIT', type: 'int', value: self.DURATION_UNIT}

        ]
    }, function (result) {
      if (result) {
        conInfo("storeHueSat as MetaData");
        homematic("Interface.setMetadata", {
          "objectId" : self.chnId,
          "dataId" : "lastValHueSat",
          "value" : "HUE:" + self.HUE + ",SAT:" + self.SATURATION * 100
        }, function(result) {
          conInfo("iseUniversalLightReceiver saveColor - Metadata set: " +  result);
        });
      }
    });
  }
};

function setULREffectToolTips (chnId) {
  jQuery("#ulrEffect_0_"+chnId).powerTip({placement: "ne"}).data("powertip", "Effekt <b>Beenden</b>");
  for (var loop = 1; loop <= 10; loop++) {
    jQuery("#ulrEffect_"+loop+"_"+chnId).powerTip({placement: "ne"}).data("powertip", "Effekt Nr: <b>"+loop+"</b>");
  }
}

function setURLEffectBtnActive (chnId, devAddress) {
  // 01_EFFECT_01_COLOR_HUE_SATURATION_COLOR_TEMPERATURE_TYPE
  var paramSet = homematic("Interface.getParamset", {"interface": "HmIP-RF", "address" : devAddress, "paramsetKey" :"MASTER"}),
    effect, subEffect, btnEffect,
    effectActive = false, cntSubEffect;

  for (var effect = 1; effect <= 10; effect++) {
    btnEffect =  jQuery("#ulrEffect_" + effect + "_" + chnId);
    cntSubEffect = 0;

    for (var subEffect = 1; subEffect <= 8; subEffect++) {
      if (parseInt(paramSet[addLeadingZero(effect) + "_EFFECT_" + addLeadingZero(subEffect) + "_COLOR_HUE_SATURATION_COLOR_TEMPERATURE_TYPE"]) == 0) {
        cntSubEffect++;
      }
    }
    if (cntSubEffect == 8) { // No subEffect active
      btnEffect.css("color", "grey");
      btnEffect.removeAttr("onclick");
      btnEffect.click(function() {
        alert(translateKey("lblEffectNotActive"));
      });
    }
  }
}


// Not in use - the effect name will be set by ReGa
function setULREffectName (chnId, devId) {
  var effectName, effectNo;
  for (effectNo = 1; effectNo <= 10; effectNo++) {
    effectName =  homematic("Interface.getMetadata", {"objectId": devId, "dataId": "effectName_" + effectNo});
    if ((effectName == "") || (effectName == "null")) {effectName = translateKey("lblEffect") + " " + effectNo;}
    jQuery("#ulrEffect_"+effectNo+"_"+chnId).text(effectName);
  }
}

// This determines if the old status will be restored after the effect has come to its end.
// true = Restore the old status - false = the light will be switched off.
function setULReffectMode(chkBox, chnId) {
  var mode = (jQuery(chkBox).is(":checked")) ? true : false;
  homematic("Interface.setMetadata", {"objectId": chnId, "dataId": "effectMode", "value": mode});
}

function setULREffect (elm, effectNr, chnAddress, level) {
  var effectLevel = level,
  chnId = elm.id.split("_")[2],
  saveOldStatus = homematic("Interface.getMetadata", {"objectId": chnId, "dataId": "effectMode"});
  jQuery(".j_Button_"+chnId).switchClass("ControlBtnOn","ControlBtnOff", 0); // deactivate the color temp slider and the buttons HCL / Dim2Warm
  jQuery(".ControlBtnInfoActive").switchClass("ControlBtnInfoActive","ControlBtnInfoNotActive", 0); // deactivate the colorpicker panel for the H and S value

  jQuery("[name='urlEffect_"+chnId+"']").switchClass("ControlBtnOn","ControlBtnOff", 0); // deactivate all effect buttons
  jQuery(elm).switchClass('ControlBtnOff', 'ControlBtnOn', 0); // activate the selected button.

  // saveOldStatus 'true' will determine if the old status will be restored. Otherwise the light will be switched off.
  if (saveOldStatus == "true") {effectNr++;}

  homematic("Interface.putParamset", {
    'interface': "HmIP-RF",
    'address': chnAddress,
    'paramsetKey': 'VALUES',
    'set':
      [
        {name: 'LEVEL', type: 'double', value: effectLevel},
        {name: 'EFFECT', type: 'int', value: effectNr}
      ]
  }, function (result) {
    conInfo("setULREffect: ",result);
  });
}

