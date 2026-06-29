/* * * * * * * * * * * * * * * * * * * * * * * *
 * iseButtonsShutter                           *
 * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * @class
 **/
iseButtonsShutter = Class.create();

iseButtonsShutter.prototype = {
  /*
   * id = datapoint-ID of switch
   * initState = Creation State (0 or 1)
   */
  initialize: function(id, initState, dpLevel, dpStop, iViewOnly, opts) {
    this.id = id;
    this.state = initState;
    this.dpLevel = dpLevel;
    this.dpStop = dpStop;
    this.Perc = $(this.id + "Perc");
    this.divPercUp = $(this.id + "PercUp");
    this.divPercDown = $(this.id + "PercDown");
    this.divStop = $(this.id + "Stop");
    this.divUp = $(this.id + "Up");
    this.divDown = $(this.id + "Down");

    this.shutter = new shutterControl(id, initState);

    this.Perc.value = initState;
    this.shutter.setValue(initState);
    this.opts = opts;

    this.chMaintenance = this.opts.devAddress + ":0";

    this.pressedUpDown = "";

    if (typeof blindLevelDestination == "undefined") {
      blindLevelDestination = [];
    }

    blindLevelDestination[id] = (blindLevelDestination[id] != null) ? blindLevelDestination[id] : null;

    // Add event handlers
    if (iViewOnly === 0)
    {
      this.clickCtrl = this.onClickShutter.bindAsEventListener(this);
      Event.observe(this.shutter.divShutterBg, 'mousedown', this.clickCtrl);
      this.clickPercUp = this.onClickPercUp.bindAsEventListener(this);
      Event.observe(this.divPercUp, 'click', this.clickPercUp);
      this.clickPercDown = this.onClickPercDown.bindAsEventListener(this);
      Event.observe(this.divPercDown, 'click', this.clickPercDown);
      this.changePerc = this.onChangePerc.bindAsEventListener(this);
      Event.observe(this.Perc, 'change', this.changePerc);

      this.clickUp = this.onClickUp.bindAsEventListener(this);
      Event.observe(this.divUp, 'mousedown', this.clickUp);
      this.clickDown = this.onClickDown.bindAsEventListener(this);
      Event.observe(this.divDown, 'mousedown', this.clickDown);

      this.clickStop = this.onClickStop.bindAsEventListener(this);
      Event.observe(this.divStop, 'mousedown', this.clickStop);
    }

    if (opts.chnLabel.includes("HmIP-M-TD"))  {
      this.checkEndPos();
    }

    this.initJalousie();
    this.initHmIPJalousieShutter();

  },

  initJalousie: function() {},
  initHmIPJalousieShutter: function() {},

  checkEndPos: function() {
    this.topSet = parseInt(homematic("Interface.getValue", {'interface': this.opts.iface, 'address' : this.chMaintenance, 'valueKey': 'MANUAL_SELF_CALIBRATION_TOP_POS_SET'}));
    this.bottomSet = parseInt(homematic("Interface.getValue", {'interface': this.opts.iface, 'address' : this.chMaintenance, 'valueKey': 'MANUAL_SELF_CALIBRATION_BOTTOM_POS_SET'}));

    if ((this.topSet == 0) || (this.bottomSet == 0)) {
      jQuery("#endPosNotSaved" + this.id).show();
    } else {
      jQuery("#endPosNotSaved" + this.id).hide();
    }

  },

  onClickShutter: function(ev) {
    var pos = Position.page(this.shutter.divShutterBg);
    var offset = ev.clientY - pos[1];
    var val = 100 - (( offset * 100 ) / this.shutter.MAX_HEIGHT);
    var setVal = 0;
    if ( (val >  0) && ( val <= 20) ) setVal = 0;
    if ( (val > 20) && ( val <= 40) ) setVal = 25;
    if ( (val > 40) && ( val <= 60) ) setVal = 50;
    if ( (val > 60) && ( val <= 80) ) setVal = 75;
    if ( (val > 80) && ( val <= 100) ) setVal = 100;
    this.pressedUpDown = "";
    this.state = setVal;
    this.Perc.value = this.state;
    this.shutter.setValue(this.state);
    this.saveValue();
  },

  onClickPercUp: function() {
    if ((this.state % 10)  != 0) {
      this.state = Math.round(Math.ceil(this.state / 10) * 10);
    } else {
      this.state += 10;
    }
    this.pressedUpDown = "";
    if (this.state > 100)
      this.state = 100;
    this.Perc.value = this.state;
    this.shutter.setValue(this.state);
    this.saveValue();
  },

  onClickPercDown: function() {
    if ((this.state % 10 ) != 0) {
      this.state = Math.round(Math.floor(this.state / 10) * 10);
    } else {
      this.state -= 10;
    }

    this.pressedUpDown = "";
    if (this.state < 0)
      this.state = 0;
    this.Perc.value = this.state;
    this.shutter.setValue(this.state);
    this.saveValue();
  },

  onChangePerc: function() {
    var min = 0, max = 100,
      value = this.Perc.value;

    this.pressedUpDown = "";

    if ((value < min) || (isNaN(value))) {value = min;}
    if (value > max) {value = max;}

    this.Perc.value = value;
    this.state = parseInt(value);
    this.shutter.setValue(this.state);
    this.saveValue();
  },

  onClickUp: function() {
    //jQuery("#" +this.id + "Stop").text(translateKey("actionStatusControlStop"));
    this.state = 100;
    this.Perc.value = this.state;
    ControlBtn.pushed(this.divUp);
    this.shutter.setValue(this.state);
    this.pressedUpDown = "UP";
    this.saveValue();
    var t = this;
    new PeriodicalExecuter(function(pe) {
      ControlBtn.off(t.divUp);
      pe.stop();
    }, 1);
  },

  onClickDown: function() {
    //jQuery("#" +this.id + "Stop").text(translateKey("actionStatusControlStop"));
    this.state = 0;
    this.Perc.value = this.state;
    this.shutter.setValue(this.state);
    this.pressedUpDown = "DOWN";
    this.saveValue();
    var t = this;
    ControlBtn.pushed(this.divDown);
    new PeriodicalExecuter(function(pe) {
      ControlBtn.off(t.divDown);
      pe.stop();
    }, 1);
  },

  onClickStop: function() {
    ControlBtn.pushed(this.divStop);
    this.pressedUpDown = "";
    setDpState(this.dpStop, 1);
    var t = this;
    new PeriodicalExecuter(function(pe) {
      ControlBtn.off(t.divStop);
      pe.stop();
    }, 1);
    blindLevelDestination[this.id] = null;
  },

  saveValue: function() {
    setDpState(this.dpLevel, this.state / 100);
  }
};

iseButtonsJalousie = Class.create(iseButtonsShutter, {
  initJalousie: function () {
    conInfo("initJalousie");
    this.sliderInfoElm = jQuery("#infoSliderPos" + this.id);
    this.sliderElm = jQuery("#slider" + this.id);
    this.btnSendData = jQuery("#btnSendValues" + this.id);

    this.levelSlats = this.opts.levelSlatsValue;
    this.dpLevelCombined = this.opts.levelCombinedID;
    this.initSliderInfoElm();
    this.initSendBtn();
    this.initSlider();
    this.sliderElm.slider('value', this.opts.levelSlatsValue);
    this.sliderInfoElm.val(this.opts.levelSlatsValue);

  },



  initSliderInfoElm: function() {
    var self = this;
    this.sliderInfoElm.change(function() {
      self.levelSlats = jQuery(this).val();
      self.sliderElm.slider("value", self.levelSlats);
    });
  },

  initSendBtn: function () {
    var self = this;

    this.btnSendData.click(function () {
      self.saveValue();
      JControlBtn.pushed(self.btnSendData);

      new PeriodicalExecuter(function (pe) {
        JControlBtn.off(self.btnSendData);
        pe.stop();
      }, 1);
    });
  },

  initSlider: function () {
    var self = this;
    this.sliderElm.slider({
      animate: "fast",
      min: 0,
      max: 100,
      step: 5,
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
    this.levelSlats = val; //this.sliderElm.slider("value");
    this.sliderInfoElm.val(this.levelSlats);
    //this.saveValue();
  },

  onSliderStop: function() {
    this.pressedUpDown = "";
  },

  onClickShutter: function (ev) {
    var pos = Position.page(this.shutter.divShutterBg);
    var offset = ev.clientY - pos[1];
    var val = 100 - (( offset * 100 ) / this.shutter.MAX_HEIGHT);
    var setVal = 0;
    if ((val > 0) && ( val <= 20)) setVal = 0;
    if ((val > 20) && ( val <= 40)) setVal = 25;
    if ((val > 40) && ( val <= 60)) setVal = 50;
    if ((val > 60) && ( val <= 80)) setVal = 75;
    if ((val > 80) && ( val <= 100)) setVal = 100;
    this.state = setVal;
    this.Perc.value = this.state;
    this.shutter.setValue(this.state);
    //this.saveValue();
  },



  onChangePerc: function() {
    if (isNaN(this.Perc.value)) { return; }
    this.pressedUpDown = "";
    this.state = parseInt(this.Perc.value);
    //this.shutter.setValue(this.state);
    //this.saveValue();
  },


  saveValue: function() {
    var level = parseInt(this.state) * 2,
      levelHex = level.toString(16),
      levelSlat = parseInt(this.levelSlats) * 2,
      levelSlatHex = levelSlat.toString(16),
      dpValue;

    dpValue = (levelHex.length < 2) ? "0X0" + levelHex : "0X" + levelHex;
    dpValue += ",";
    dpValue += (levelSlatHex.length < 2) ? "0X0" + levelSlatHex : "0X" + levelSlatHex;
    setDpState(this.dpLevelCombined, dpValue);
  }

});

iseHmIPJalousieShutter = Class.create(iseButtonsShutter, {
  initHmIPJalousieShutter: function() {

    this.Interface = this.opts.chnInterface;
    this.levelStatus = parseInt(this.opts.levelStatusRealChannel);
    this.levelSlatsStatus = parseInt(this.opts.levelStatusRealChannelSlatsValue);

    jQuery("#hintVirtualChannelChanged" + this.id).click(function() {
      MessageBox.show(translateKey("hintVirtualChannelChanged"), translateKey("hintVirtualChannelChangedMsg"), "", 500, 150);
    });


    // TWIST-1873
    // When the expert mode is active the visual shutter element will be displayed on the transmitter channel (real channel)
    // Then this element shouldn't be visible with the virtual channels.
    if (! this.opts.easyLinkMode) {
      this.shutterAnchor = jQuery("#shutter"+this.id );
      this.shutterAnchor.hide();
    }

    // When the user is no expert the first virtual channel displays the value of the hidden real channel
    this.initState = (this.opts.easyLinkMode) ? parseInt(this.opts.levelRealChannel * 100) : this.opts.levelValue;
    this.initState = (parseInt(this.initState) == -100) ? -1 : this.initState;

    this.state = this.initState;

    this.Perc.value = (this.levelStatus > 0 || this.initState == -1) ? "??" : this.initState;

    if (this.initState == -1) {
      this.shutter.setValue(0);
    } else {
      this.shutter.setValue(this.initState);
    }

    this.hasSlats = (this.opts.levelSlatsID) ? true : false;
    if (this.hasSlats) {
      this.sliderInfoElm = jQuery("#infoSliderPos" + this.id);
      this.sliderElm = jQuery("#slider" + this.id);

      // When the user is no expert the first virtual channel displays the value of the hidden real channel
      this.levelSlats = (this.opts.easyLinkMode) ? parseInt(this.opts.levelRealChannelSlatsValue * 100) : this.opts.levelSlatsValue;
      this.levelSlats = (parseInt(this.levelSlats) == -100) ? 0 : this.levelSlats;

      this.levelReal = this.opts.levelRealChannel;
      this.initSliderInfoElm();
      this.initSlider();
      this.sliderElm.slider('value', this.levelSlats);
      this.sliderInfoElm.val((this.levelSlatsStatus > 0) ? "??" : this.levelSlats);
    }
    this.blockElements();
  },

  // Prevent certain elements to work when the levelStatus or levelSlatsStatus != 0 (NORMAL)
  blockElements: function() {
    var controlOperableNone = "none",
    controlOperableAuto = "auto",
      activityState = parseInt(this.opts.activityStateRealChannel);

    if (this.levelStatus > 0) {
      jQuery("#shutter" + this.id).css("pointer-events", controlOperableNone); // visual blind element
      jQuery("#" + this.Perc.id).css("pointer-events", controlOperableNone);   // text value in %
      $(this.divPercUp).stopObserving('click');                                // up button
      $(this.divPercDown).stopObserving('click');                              // down button

      // Only when the ACTIVITY_STATE of the device = 0 (UNKNOWN) or 3 (STABLE)  -- the other values ara 1/2 = UP/DOWN
      //if ((activityState == 0) || (activityState == 3)) {
      //  jQuery("#" + this.id + "Stop").text("Refresh");                          // Refresh
      //}
    }

    if (! isNaN(this.levelSlatsStatus) && this.levelSlatsStatus > 0) {
      jQuery("#tdSlider" + this.id).css("pointer-events", controlOperableNone);
    }
  },

  initSliderInfoElm: function() {
    var self = this;
    this.sliderInfoElm.change(function() {
      var value = jQuery(this).val(),
        min = 0, max = 100;

      if ((value < min) || (isNaN(value))) {value = min;}
      if (value > max) {value = max;}
      this.value = value;
      self.levelSlats = value;
      self.sliderElm.slider("value", self.levelSlats);
      self.saveSliderValue();
    });
  },

  initSlider: function () {
    var self = this;
    this.sliderElm.slider({
      animate: "fast",
      min: 0,
      max: 100,
      step: 5,
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
    this.levelSlats = val; //this.sliderElm.slider("value");
    this.sliderInfoElm.val(this.levelSlats);
  },

  onSliderStop: function() {
    this.saveSliderValue();
  },

  saveSliderValue: function() {
    //var levelValue = (blindLevelDestination[this.id] != null) ? blindLevelDestination[this.id] : this.levelReal;
    homematic("Interface.putParamset",{'interface': this.Interface, 'address' : this.opts.chnAddress, 'paramsetKey' : 'VALUES', 'set':
      [
        //{name:'LEVEL', type: 'double', value: 1.005},
        // {name:'LEVEL', type: 'double', value: levelValue}, LEVEL wird intern vom Realkanal durch den crRFD ermittelt.
        {name:'LEVEL_2', type: 'double', value: this.levelSlats / 100}
      ]
    },function(result){conInfo(result);});
  },

  // See SPHM-1301 - this was the previous version
  _saveValue: function() {
    var level2Value;

    this.state = (this.state != -1) ? this.state : 0;

    if (this.hasSlats) {
      blindLevelDestination[this.id] = this.state;

      if (this.pressedUpDown == "DOWN") {
        level2Value = 0;
      } else if (this.pressedUpDown == "UP") {
        level2Value = 1;
      }

      // twist-1551
      if ((this.pressedUpDown != "") && (typeof level2Value != "undefined") ) {
        homematic("Interface.putParamset", {
          'interface': this.Interface, 'address': this.opts.chnAddress, 'paramsetKey': 'VALUES', 'set': [
            {name: 'LEVEL', type: 'double', value: this.state / 100},
            {name: 'LEVEL_2', type: 'double', value: level2Value}
          ]
        }, function (result) {
          conInfo(result);
        });
      } else {

        setDpState(this.dpLevel, this.state / 100);

      }
    } else {
      setDpState(this.dpLevel, this.state / 100);
    }
  },

  // See SPHM-1301
  saveValue: function() {
    var self=this, level2Value;
    this.state = (this.state != -1) ? this.state : 0;
      blindLevelDestination[this.id] = this.state;
      if (this.pressedUpDown == "DOWN") {
        level2Value = 0;
      } else if (this.pressedUpDown == "UP") {
        level2Value = 1;
      }
      level2Value = (typeof level2Value == "undefined") ? 0 : level2Value;
      homematic("Interface.putParamset", {
        'interface': this.Interface, 'address': this.opts.chnAddress, 'paramsetKey': 'VALUES', 'set': [
          {name: 'LEVEL', type: 'double', value: this.state / 100},
          {name: 'LEVEL_2', type: 'double', value: level2Value}
        ]
      }, function (result) {
        conInfo("1st try: " + result);
        if (result == null) {
          homematic("Interface.putParamset", {
            'interface': self.Interface, 'address': self.opts.chnAddress, 'paramsetKey': 'VALUES', 'set': [
              {name: 'LEVEL', type: 'double', value: self.state / 100}
            ]
          }, function (result) {
            conInfo("2nd try: " + result);
          });
        }
     });
  }
});
