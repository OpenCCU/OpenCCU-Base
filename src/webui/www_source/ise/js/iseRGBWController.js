// This class is currently not in use
// See esp/controls/rgbw.fn - CreateRGBWAutomaticControl

iseRGBWController = Class.create();

iseRGBWController.prototype = {


  initialize: function (chnId, opts) {
    conInfo("iseRGBWController");
    this.chnId  = chnId;
    this.opts = opts;
    this.program = this.opts.valProgram;
    jQuery("#"+ this.chnId).text(this.program);

    this.brightness = this.opts.valBrightness;
    this.rampTime = this.opts.valRampTime;
    this.onTime = this.opts.valOnTime;
    this.minColor = this.opts.valMinBorder;
    this.maxColor = this.opts.valMaxBorder;

    this.chn = homematic("Device.get",{"id": this.chnId});
    this.value =
      this.program + "," +
      this.brightness + "," +
      this.rampTime + "," +
      this.onTime + "," +
      this.minColor + "," +
      this.maxColor;

    this.bindEvents();
  },

  bindEvents: function() {
    var self = this;
    jQuery("#"+ this.chnId).bind("click", function(){
      conInfo("show program dialog");


      /*************/
        var url = "/pages/msg/setRGBWController.htm";

        var req = jQuery.ajax({
          url : url +"?sid=" + SessionId,
          cache: false,
          dataType: "html"
        });

        req.done(function(htmlContent) {

          //var rgbwControllerDialog = new RGBWControllerDialog(translateKey("titleRGBWControllerDialogTitle"), htmlContent, "RGBW_AUTOMATIC", self.value, function(result) {
          var rgbwControllerDialog = new RGBWControllerDialog(translateKey("titleRGBWControllerDialogTitle"), htmlContent, "USER_PROGRAM", self.value, function(result) {
             if (result == 1) {
              conInfo("SetRGBWController: Yes pressed");
              var arConfigString = this.getConfigString().split(",");
              conInfo("rgbwControllerDialog: configString: " + arConfigString);

               homematic("Interface.putParamset",{'interface': self.chn["interface"], 'address' : self.chn["address"], 'paramsetKey' : 'VALUES', 'set':
                  [
                    {name:'PROGRAM', type: 'string', value: arConfigString[0]},
                    {name:'ACT_BRIGHTNESS', type: 'string', value: arConfigString[1]},
                    {name:'RAMP_TIME', type: 'string', value: arConfigString[2]},
                    {name:'ON_TIME', type: 'string', value: arConfigString[3]},
                    {name:'ACT_MIN_BOARDER', type: 'string', value: arConfigString[4]},
                    {name:'ACT_MAX_BOARDER', type: 'string', value: arConfigString[5]}
                  ]
               }, function(result){jQuery("#"+ self.chnId).text( arConfigString[0] );});
            }
          }, "html");
        });

        req.fail(function() {
          alert("Error while loading " + url);
        });
    });
  }

};



// This class is responsible for the handling of the CONTROL
iseVIR_LGRGBColorControl = Class.create();

iseVIR_LGRGBColorControl.prototype = {

  initialize: function (chnId, opts) {

    this.chnId = chnId;

    this.rgbType = opts.Type;
    this.rgbID = opts.rgbID;
    this.rgbVal = opts.rgbVal;
    //this.defaultRGBW = opts.rgbDefault;
    this.defaultRGBW = "rgb(255,255,255,255)";

    this.rgbwID = "RGBW";


    this.bindEvents();

    if (this.rgbType == this.rgbwID) {
      // this.initSlider();
    }
  },

  bindEvents: function() {
    var self = this;
    jQuery(".sp-container #rgbwBtnColor" +this.chnId).bind("click", function() {
      var color = jQuery("#colorPicker"+self.chnId).val();
      if (!color) {color = self.defaultRGBW;}
      self.rgbVal = color;
      setDpState(self.rgbID,self.getColorString());
    });
  },

  getColorString: function() {
    var colorString = "",
      arParamVal = this.rgbVal.split(",");

    if (this.rgbType == this.rgbwID) {
      if (arParamVal.size() > 3) {
        colorString = arParamVal[0] +","+arParamVal[1]+","+arParamVal[2]+",255)";
      } else {
        colorString = this.rgbVal.slice(0, -1) + ",255)";
      }
    } else {
      // This is a RGB device
      colorString = this.rgbVal;
    }

    return colorString;
  }

};

iseVIR_LGWhiteLevelControl = Class.create();

iseVIR_LGWhiteLevelControl.prototype = {

  initialize: function (chnId, opts) {
    var self = this;
    conInfo("iseVIR_LGWhiteLevelControl");

    this.chnId = chnId;
    this.whiteLevelId = opts.whiteLevelID;
    this.whiteLevel = parseInt(opts.whiteLevelValue);
    this.whiteLevelMin = parseInt(opts.whiteLevelMin);
    this.whiteLevelMax = parseInt(opts.whiteLevelMax);

    this.whiteLevelStep = parseInt(opts.whiteLevelStep);

    window.setTimeout(function() {
      self.sliderInfoElm = jQuery("#infoSliderPos" + self.chnId);
      self.sliderElm = jQuery("#slider" + self.chnId);
      self.initSlider();
    },100);
  },

  initSlider: function () {

    if (this.whiteLevel == "-1") {this.whiteLevel = this.whiteLevelMin;}
    this.sliderInfoElm.val(this.whiteLevel);

    var self = this;
    this.sliderElm.slider({
      animate: "fast",
      value: this.whiteLevel,
      min: this.whiteLevelMin,
      max: this.whiteLevelMax,
      step: this.whiteLevelStep,
      orientation: "horizontal",
      slide: function (event, ui) {},
      stop: function( event, ui ) {}
    });
    this.sliderElm.on("slide", function (event, ui) {
      self.onSliderChange(ui.value);
    });

    this.sliderElm.on("slidestop", function(event, ui){
      self.onSliderStop(ui.value);
    });
  },

  onSliderChange: function (val) {
    this.sliderInfoElm.val(val);
    this.whiteLevel = val;
  },

  onSliderStop: function(val) {
    setDpState(this.whiteLevelId,this.whiteLevel);
  }

};


