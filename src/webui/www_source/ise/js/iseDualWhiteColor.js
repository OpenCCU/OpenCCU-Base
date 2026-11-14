iseDualWhiteColorController = Class.create();

iseDualWhiteColorController.prototype = {
  initialize: function (opts) {
    var self = this;
    this.iFace = "BidCos-RF";
    this.id = opts.chnId;
    this.chnAddress = opts.chnAddress;
    this.devAddress = this.chnAddress.split(":")[0];
    this.val = parseInt(opts.value * 100);
    this.minVal = 0;
    this.maxVal = 100;
    this.sliderStep = 5;
    this.colorPlusMinusTimeoutObject;
    this.colorPlusMinusTimeout = 1000;


    this.sliderInfoElm = jQuery("#infoSliderPos" + this.id);
    this.sliderElm = jQuery("#slider" + this.id);
    this.colorPlusElm = jQuery("#colorPlus" + this.id);
    this.colorMinusElm = jQuery("#colorMinus" + this.id);

    this.loadImg = "/ise/img/loading.gif";
    this.currentDiagram;

    this.diagramURLs = {} ;

    if (opts.isVirtualChannel == "false") {
      if (typeof hm_lc_dw_diagram == "undefined") {
        jQuery("[name='imgDualWhiteController']").attr("src", this.loadImg);
        hm_lc_dw_diagram = {};
        this.initDiagramURLs();
        this.getConfigAndSetDiagram();
      } else {
        // Here we have a hm_lc_dw_diagram object from another device
        if (typeof hm_lc_dw_diagram[this.devAddress] == "undefined") {
          jQuery("[name='imgDualWhiteController']").attr("src", this.loadImg);
          delete hm_lc_dw_diagram;
          hm_lc_dw_diagram = {};
          this.initDiagramURLs();
          this.getConfigAndSetDiagram();
        } else {
          this.currentDiagram = hm_lc_dw_diagram[this.devAddress];
          jQuery("[name='imgDualWhiteController']").attr("src", self.currentDiagram);
          this.initBigDiagram();
        }
      }
    } else {
      if (typeof hm_lc_dw_diagram == "undefined") {
        hm_lc_dw_diagram = {};
        var elem = jQuery("[name='imgDualWhiteController']")[0];
        hm_lc_dw_diagram[this.devAddress] = jQuery(elem).attr("src");
        this.currentDiagram = hm_lc_dw_diagram[this.devAddress];
        jQuery("[name='imgDualWhiteController']").attr("src", this.currentDiagram);
        this.initBigDiagram();
      } else {
        this.currentDiagram = hm_lc_dw_diagram[this.devAddress];
        jQuery("[name='imgDualWhiteController']").attr("src", self.currentDiagram);
        this.initBigDiagram();
      }
    }

    this.sliderVal;
    this.initSlider();
    this.sliderElm.slider('value', this.val);
    this.sliderInfoElm.val(this.val);

    if (typeof clearCachedPath != "undefined") {
      clearTimeout(clearCachedPath);
    }

    clearCachedPath = setTimeout(function() {delete hm_lc_dw_diagram;}, 15000);
    this.bindEvents();
  },

  bindEvents: function () {

    var self = this;
    this.sliderInfoElm.on('change',function() {
      thisElm = jQuery(this);
      var value = parseInt(thisElm.val());

      if (isNaN(value)) {
        value = self.minVal;
        thisElm.val(value);
      } else if (value < self.minVal) {
        value = this.minVal;
        thisElm.val(value);
      } else if (value > self.maxVal) {
        value = self.maxVal;
        thisElm.val(value);
      }
      self.sliderVal = value;
      self.sliderElm.slider("value", value);
      self.setValue();
    });

    this.sliderInfoElm.on('keypress', function (e) {
      if(e.which === 13){
        jQuery(this).change();
      }
    });

    this.colorPlusElm.on('click', function() {
      clearTimeout(self.colorPlusMinusTimeoutObject);
      var val = parseInt(self.sliderInfoElm.val());
      if (val < self.maxVal) {
        val++;
      }

      self.sliderInfoElm.val(val);
      self.colorPlusMinusTimeoutObject = setTimeout(function() {
        self.sliderInfoElm.change();
      }, self.colorPlusMinusTimeout);
    });

    this.colorMinusElm.on('click', function() {
      clearTimeout(self.colorPlusMinusTimeoutObject);
      var val = parseInt(self.sliderInfoElm.val());
      if (val > self.minVal) {
        val--;
      }
      self.sliderInfoElm.val(val);
      self.colorPlusMinusTimeoutObject = setTimeout(function(){
        self.sliderInfoElm.change();
      },self.colorPlusMinusTimeout);
    });

  },



  initDiagramURLs: function() {
    getDualWhiteControllerDiagramURLs();
    this.diagramURLs = dualWhiteControllerDiagramURL;
  },

  getConfigAndSetDiagram: function () {
    var self = this;
    var address = this.chnAddress;
    var iFace = this.iFace;
    var config = "";

    homematic("Interface.getParamset", {"interface": iFace, "address": address, "paramsetKey": "MASTER"}, function(result) {
      config += result.CHARACTERISTIC_BASETYPE;
      config += result.CHARACTERISTIC_LINSQUARETYPE;
      config += result.CHARACTERISTIC_LEVELLIMIT;
      config += result.CHARACTERISTIC_COLOURASSIGNMENT;

      self.showCurrentDiagram(config);
    });
  },

  showCurrentDiagram: function (config) {
    this.currentDiagram = this.diagramURLs[config];

    hm_lc_dw_diagram[this.devAddress] = this.currentDiagram;

    // All channels (real and virtual) are using the same picture
    jQuery("[name='imgDualWhiteController']").attr("src", this.currentDiagram);
    this.initBigDiagram();
  },

  initBigDiagram: function() {
    var srcDiagram = hm_lc_dw_diagram[this.devAddress];
    var tooltip = "<img src="+srcDiagram+" width=450; height=330 />";
    jQuery("[name='imgDualWhiteController']").data('powertip', tooltip);
    jQuery("[name='imgDualWhiteController']").powerTip({smartPlacement: true, followMouse: false});
  },

  initSlider: function () {
    var self = this;
    this.sliderElm.slider({
      animate: "fast",
      min: self.minVal,
      max: self.maxVal,
      step: self.sliderStep,
      orientation: "horizontal",
      slide: function (event, ui) {},
      stop: function( event, ui ) {}
    });
    this.sliderElm.on("slide", function (event, ui) {
      self.onSliderChange(ui.value);
    });

    this.sliderElm.on("slidestop", function(event, ui){
      self.onSliderStop();
    });
  },

  onSliderChange: function (val) {
    this.sliderVal = val;
    this.sliderInfoElm.val(this.sliderVal);
  },

  onSliderStop: function() {
    this.setValue();
  },

  setValue: function() {
    if (this.sliderVal >= this.minVal || this.sliderVal <= this.maxVal) {
      setDpState(this.id, (this.sliderVal / 100));
    }
  }

};