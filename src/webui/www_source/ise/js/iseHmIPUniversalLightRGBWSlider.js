
iseHmIPUniversalLightRGBWSlider = Class.create();
iseHmIPUniversalLightRGBWSlider.prototype = {


  initialize: function (chnId, opts) {
    var self = this;
    conInfo("opts", opts);

    this.iface = opts.iface;
    this.chAddress = opts.chAddress;
    this.chnId = chnId;
    this.defaultColorTmpMin = 2000;
    this.defaultColorTmpMax = 6500;

    this.colorTempID = opts.colorTempID;
    this.colorTempMin = (!isNaN(opts.colorTempMin)) ? opts.colorTempMin : this.defaultColorTmpMin;
    this.colorTempMax = (!isNaN(opts.colorTempMax)) ? opts.colorTempMax : this.defaultColorTmpMax;

    this.colorTemp = parseInt(opts.colorTemp);
    this.colorTempUnknown = (this.colorTemp < this.colorTempMin) ? true : false;

    this.valHCLVal = 10200;
    this.valDim2WarmVal = 10150;

    this.hueStatus = opts.hueStatus;

    this.valHCL =  (opts.hcl == 'true') ? true : false;
    this.valDim2Warm = (opts.dim2Warm == 'true') ? true : false;

    this.hideSliderPanel = (this.valHCL || this.valDim2Warm) ? true : false;

    this.colorTemp = (this.colorTemp < this.colorTempMin) ? this.colorTempMin : this.colorTemp;

    this.devIsDali = (opts.devLabel == "HmIP-DRG-DALI") ? true : false;

    this.effect = opts.effect;

    window.setTimeout(function() {
      self.btnColorTemp = jQuery("#btnColorTemp" + self.chnId);
      self.trBtnColorTemp = jQuery("#trBtnColorTemp" + self.chnId);
      self.btnHCL = jQuery("#btnHCL" + self.chnId);
      self.btnDim2Warm = jQuery("#btnDim2Warm" + self.chnId);
      self.sliderPanel = jQuery("#sliderPanel" + self.chnId);
      self.trSliderPanel = jQuery("#trSliderPanel" + self.chnId);
      self.sliderInfoElm = jQuery("#infoSliderPos" + self.chnId);
      self.sliderElm = opts.oSlider;
      self.colorSetElm = jQuery(".j_ControlBtnInfo")[0];
      self.hueElm = jQuery("#hueElmId_" + self.chnId);
      self.satElm = jQuery("#satElmId_" + self.chnId);
      self.bckGndElm = jQuery("#bckGndlmId_"+ self.chnId);

      homematic("Interface.setMetadata", {
        "objectId": this.chnId,
        "dataId": "lastValColorTemp",
        "value": this.colorTemp
      }, function (result) {
        conInfo("iseUniversalLightReceiver setColorTemp - Metadata set: " + result);
      });

      self.initButtons();
      self.initSlider();
    },20);
  },

  showHideSliderPanel: function () {
    if (this.hideSliderPanel) {
      this.trSliderPanel.hide();
      this.trBtnColorTemp.show();
    } else {
      this.trBtnColorTemp.hide();
      this.trSliderPanel.show();
    }
  },

  initButtons: function() {
    var self = this;

    if (this.effect == 0) {
      if (self.valHCL) {
        self.setElmColorActive("hcl");
      } else if (self.valDim2Warm) {
        self.setElmColorActive("dim2Warm");
      } else {
        self.setElmColorActive("sliderPanel");
      }
    } else {
      // set the active effect button blue
      var btnEffect = jQuery("#ulrEffect_"+(parseInt(parseInt(self.effect + (self.effect % 2)) / 2))+"_" + self.chnId);
      jQuery("[name='urlEffect_"+self.chnId+"']").switchClass("ControlBtnOn","ControlBtnOff", 0); // deactivate all effect buttons
      btnEffect.switchClass('ControlBtnOff', 'ControlBtnOn', 0); // activate the active effect button
    }

    self.showHideSliderPanel();

    this.hueElm.change(function() {
      self.setAll2Off();
      jQuery(self.colorSetElm).removeClass("ControlBtnInfo").addClass("ControlBtnInfoActive");
    });

    this.satElm.change(function() {
      self.setAll2Off();
      jQuery(self.colorSetElm).removeClass("ControlBtnInfo").addClass("ControlBtnInfoActive");
    });

    this.bckGndElm.change(function() {
      self.setAll2Off();
      jQuery(self.colorSetElm).removeClass("ControlBtnInfo").addClass("ControlBtnInfoActive");
    });

    this.bckGndElm.click(function() {
      self.setAll2Off();
      jQuery(self.colorSetElm).removeClass("ControlBtnInfo").addClass("ControlBtnInfoActive");
    });

    this.btnColorTemp.click(function() {
      self.hideSliderPanel = false;
      self.hueStatus == 1;
      setDpState(self.colorTempID,self.colorTemp);
      self.sliderElm.slider('value', self.colorTemp);
      self.sliderInfoElm.val(self.colorTemp);
      self.setElmColorActive("sliderPanel");
      self.showHideSliderPanel();
    });

    this.btnHCL.click(function() {
      self.hideSliderPanel = true;
      self.hueStatus == 1;
      setDpState(self.colorTempID,self.valHCLVal);
      self.setElmColorActive("hcl");
      self.showHideSliderPanel();
    });

    this.btnDim2Warm.click(function() {
      self.hideSliderPanel = true;
      self.hueStatus == 1;
      setDpState(self.colorTempID,self.valDim2WarmVal);
      self.setElmColorActive("dim2Warm");
      self.showHideSliderPanel();
    });
  },

  setAll2Off: function() {
    jQuery(this.colorSetElm).removeClass("ControlBtnInfoActive").addClass("ControlBtnInfo");
    jQuery(this.btnHCL).removeClass("ControlBtnOn").addClass("ControlBtnOff");
    jQuery(this.btnDim2Warm).removeClass("ControlBtnOn").addClass("ControlBtnOff");
    jQuery(this.sliderPanel).removeClass("ControlBtnOn").addClass("ControlBtnOff");
    jQuery("[name='urlEffect_"+this.chnId+"']").switchClass("ControlBtnOn","ControlBtnOff", 0); // deactivate all effect buttons
  },

  setElmColorActive: function (activeElm) {
    this.setAll2Off();

    if (this.hueStatus == 0) {
      jQuery(this.colorSetElm).removeClass("ControlBtnInfo").addClass("ControlBtnInfoActive");
    } else {
      switch (activeElm) {
        case "hcl" :
          this.btnHCL.removeClass("ControlBtnOff").addClass("ControlBtnOn");
          break;
        case "dim2Warm" :
          this.btnDim2Warm.removeClass("ControlBtnOff").addClass("ControlBtnOn");
          break;
        case "sliderPanel" :
          this.sliderPanel.removeClass("ControlBtnOff").addClass("ControlBtnOn");
          break;
      }
    }
  },

  initSlider: function () {

    var metaColorTemp = homematic("Interface.getMetadata", {
      "objectId": this.chnId,
      "dataId": "lastValColorTemp"
    });

    if ((metaColorTemp == "-1") || (metaColorTemp== "null") || (metaColorTemp == null) || (isNaN(metaColorTemp))) {this.colorTemp = this.colorTempMin;}

    var self = this;

    if (! self.devIsDali) {
      this.sliderInfoElm.val(this.colorTemp);
      this.sliderElm.slider('value', this.colorTemp);

    } else  {
      if (this.colorTempUnknown) {
        this.sliderInfoElm.val("--");
      } else {
        this.sliderInfoElm.val(this.colorTemp);
      }
    }
    this.sliderElm.on("slide", function (event, ui) {
      self.onSliderChange(ui.value);
    });

    this.sliderElm.on("slidestop", function(event, ui){
      self.onSliderStop(ui.value);
    });
  },

  onSliderChange: function (val) {
    this.sliderInfoElm.val(val);
    this.colorTemp = val;
  },

  onSliderStop: function(val) {
    this.hueStatus = 1;
    this.setAll2Off();
    this.setElmColorActive("sliderPanel");
    setDpState(this.colorTempID,this.colorTemp);

    homematic("Interface.setMetadata", {
      "objectId": this.chnId,
      "dataId": "lastValColorTemp",
      "value": this.colorTemp
    }, function (result) {
      conInfo("iseUniversalLightReceiver setColorTemp - Metadata set: " + result);
    });

  }

};
