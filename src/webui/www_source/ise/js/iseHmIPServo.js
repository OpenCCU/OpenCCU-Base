iseHmIPServo = Class.create();
iseHmIPServo.prototype = {

  initialize: function(chId, opts)
  {
    var self = this,
    metaRampTime;
    conInfo("iseHmIPServo");
    this.id = chId;
    this.opts = opts;

    this.minServoPos = 0;
    this.maxServoPos = 100;
    this.stepServoPos = 0.5;

    this.minRampTime = 0;
    this.maxRampTime = 50;
    this.stepRampTime = 1;

    this.sliderPosInfoElm = jQuery("#infoSliderPos" + this.id);
    this.sliderPosElm = jQuery("#sliderPos" + this.id);
    this.levelServoPos = this.opts.levelServoPos;

    this.sliderRampInfoElm = jQuery("#infoSliderRamp" + this.id);
    this.sliderRampElm = jQuery("#sliderRamp" + this.id);

    this.levelServoRampTime = 0;

    // This is to store the ramp time value which is only writable (RAMP_TIME = operations 2)
    // This remembers the last setting. Otherwise the value would always be set to 0 after a status message.
    if (typeof tmpRampTime == "undefined") {
      tmpRampTime = [];
      metaRampTime = homematic("Interface.getMetadata", {"objectId": this.id, "dataId": "rampTime"});
      this.levelServoRampTime = (metaRampTime != "null") ? metaRampTime : 0;
      tmpRampTime['a_' + this.id] = this.levelServoRampTime;
    } else if (typeof tmpRampTime['a_' + this.id] != "undefined") {
      this.levelServoRampTime = tmpRampTime['a_' + this.id];
    }

    this.initSliderPosInfoElm();
    this.initSliderPos();
    this.initSliderRampInfoElm();
    this.initSliderRamp();
    this.sliderPosElm.slider('value', this.opts.levelServoPos * 100);
    this.sliderPosInfoElm.val(parseFloat(this.opts.levelServoPos * 100).toFixed(1));
    this.sliderRampElm.slider('value', this.levelServoRampTime);
    this.sliderRampInfoElm.val(parseInt(this.levelServoRampTime));
  },

  initSliderPos: function () {
    var self = this;
    this.sliderPosElm.slider({
      animate: "fast",
      min: self.minServoPos,
      max: self.maxServoPos,
      step: self.stepServoPos,
      orientation: "horizontal",
      slide: function (event, ui) {},
      stop: function( event, ui ) {}
    });
    this.sliderPosElm.on("slide", function (event, ui) {
      self.onSliderPosChange(parseFloat(ui.value.toString().replace(",", ".")).toFixed(1));
    });

    this.sliderPosElm.on("slidestop", function(event, ui){
      self.onSliderPosStop(ui.value);
    });
  },

  initSliderRamp: function () {
    var self = this;
    this.sliderRampElm.slider({
      animate: "fast",
      min: self.minRampTime,
      max: self.maxRampTime,
      step: self.stepRampTime,
      orientation: "horizontal",
      slide: function (event, ui) {},
      stop: function( event, ui ) {}
    });
    this.sliderRampElm.on("slide", function (event, ui) {
      self.onSliderRampChange(ui.value);
    });

    this.sliderRampElm.on("slidestop", function(event, ui){
      self.onSliderRampStop(ui.value);
    });
  },


  initSliderPosInfoElm: function() {
    var self = this;
    this.sliderPosInfoElm.change(function() {
      var value = jQuery(this).val().replace(",", "."),
        min = 0, max = 100;

      if ((value < min) || (isNaN(value))) {value = min;}
      if (value > max) {value = max;}
      self.levelServoPos =  roundValue05(parseFloat(value).toFixed(1));
      self.sliderPosElm.slider("value", self.levelServoPos);
      self.sliderPosInfoElm.val(self.levelServoPos);
      self.saveSliderPosValue();
    });
  },

  initSliderRampInfoElm: function() {
    var self = this;
    this.sliderRampInfoElm.change(function() {
      var value = jQuery(this).val(),
        min = self.minRampTime, max = self.maxRampTime;

      if ((value < min) || (isNaN(value))) {value = min;}
      if (value > max) {value = max;}
      self.levelServoRampTime = value;

      if ((value < self.minRampTime) || (value > self.minRampTime)) {
        self.sliderRampElm.slider("value", self.levelServoRampTime);
        self.sliderRampInfoElm.val(self.levelServoRampTime);
      }

      tmpRampTime['a_'+ self.id] = self.levelServoRampTime;

    });
  },

  onSliderPosChange: function (val) {
    this.levelServoPos = val;
    this.sliderPosInfoElm.val(parseFloat(this.levelServoPos).toFixed(1));
  },

  onSliderPosStop: function(val) {
    this.levelServoPos = val;
    this.sliderPosInfoElm.val(this.levelServoPos).change();
  },

  onSliderRampChange: function (val) {
    this.levelServoRampTime = val;
    this.sliderRampInfoElm.val(parseInt(this.levelServoRampTime));
  },

  onSliderRampStop: function(val) {
    this.levelServoRampTime = val;
    this.sliderRampInfoElm.val(this.levelServoRampTime).change();
    homematic("Interface.setMetadata", {"objectId": this.id, "dataId": "rampTime", "value": this.levelServoRampTime});
  },


  saveSliderPosValue: function() {
    this.levelServoRampTime = (this.levelServoRampTime == "") ? this.minRampTime : this.levelServoRampTime;
    homematic("Interface.putParamset",{'interface': this.opts.chnInterface, 'address' : this.opts.chnAddress, 'paramsetKey' : 'VALUES', 'set':
        [
          {name:'LEVEL', type: 'double', value: this.levelServoPos / 100},
          {name:'RAMP_TIME', type: 'double', value: this.levelServoRampTime}
        ]
    },function(result){conInfo(result);});
  }

};