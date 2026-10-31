iseButtonsSwitch = Class.create();

iseButtonsSwitch.prototype = {
  /*
   * id = datapoint-ID of switch
   * initState = Creation State (0 or 1)
   * idDpState = id of state-DP
   */
  initialize: function(id, initState, idDpState, iViewOnly, chnAddress, chnLabel, iFace) {
    var self = this;
    this.id = id;
    this.state = initState;
    this.divOn = $(this.id + "On");
    this.divOff = $(this.id + "Off");
    this.idDpState = idDpState;
    this.chnAddress = chnAddress;
    this.chnLabel = chnLabel;
    this.iFace = iFace;
    this.HmIPInterfaceID = "HmIP-RF";
    this.labelGarageDoorController = "HmIP-WGC";
    this.labelVIR_LG_ONOFF = "VIR-LG-ONOFF";
    this.labeldrgDali = "HmIP-DRG-DALI";

    this.garageDoorControllerOnTime = 0.5;
    
    if (initState) { ControlBtn.on(this.divOn); }
    else  { ControlBtn.on(this.divOff); }
      
    // Add event handlers
    if (iViewOnly === 0)
    {
      this.clickOff = this.onClickOff.bindAsEventListener(this);
      Event.observe(this.divOff, 'mousedown', this.clickOff);
      
      this.clickOn = this.onClickOn.bindAsEventListener(this);
      Event.observe(this.divOn, 'mousedown', this.clickOn);


      if (this.chnLabel.indexOf("HmIP-WSM") != -1) {

        this.durationValueElm = jQuery("#" + this.id + "durationValue");
        this.durationUnitElm = jQuery("#" + this.id + "durationUnit");
        this.valueLitersElm = jQuery("#" + this.id + "valueLiters");
        this.unitLitersElm = jQuery("#" + this.id + "unitLiters");

        this.divOnCond = jQuery("#" + this.id + "OnCond").on("click", function() {
          jQuery("#" + self.id + "tdFlowRate").toggle();
        });

        this.setCondition = jQuery("#" + this.id + "Set").on("click", function() {
          var elm = jQuery(this);
          elm.removeClass("ControlBtnOff").addClass("ControlBtnOn");
          window.setTimeout(function(){elm.removeClass("ControlBtnOn").addClass("ControlBtnOff");},500);
          self.setConditionWaterFlow();
        });
      }

    }
  },

  setConditionWaterFlow: function() {

    var durationValue = parseInt(this.durationValueElm.val()),
      durationUnit = parseInt(this.durationUnitElm.val()),
      valueLiters =  parseInt(this.valueLitersElm.val()),
      unitLiters = parseInt(this.unitLitersElm.val());


    if ((durationValue == 0) && (valueLiters > 0))  {
      homematic("Interface.putParamset",{'interface': this.HmIPInterfaceID, 'address' : this.chnAddress, 'paramsetKey' : 'VALUES', 'set':
        [
          {name:'OUTPUT_BEHAVIOUR', type: 'int', value: (valueLiters + unitLiters) }
        ]
      },function(result){conInfo(result);});
    } else if  ((durationValue > 0) && (valueLiters > 0)) {
      homematic("Interface.putParamset",{'interface': this.HmIPInterfaceID, 'address' : this.chnAddress, 'paramsetKey' : 'VALUES', 'set':
        [
          {name:'DURATION_VALUE', type: 'int', value: durationValue},
          {name:'DURATION_UNIT', type: 'int', value: durationUnit},
          {name:'OUTPUT_BEHAVIOUR', type: 'int', value: (valueLiters + unitLiters) }
        ]
      },function(result){conInfo(result);});
    } else if ((durationValue > 0) && (valueLiters == 0)) {
      homematic("Interface.putParamset",{'interface': this.HmIPInterfaceID, 'address' : this.chnAddress, 'paramsetKey' : 'VALUES', 'set':
        [
          {name:'DURATION_VALUE', type: 'int', value: durationValue},
          {name:'DURATION_UNIT', type: 'int', value: durationUnit}
        ]
      },function(result){conInfo(result);});
    }
  },

  onClickOff: function() {
    ControlBtn.pushed(this.divOff);
    //this.state = false;
    if ((this.chnLabel != this.labelVIR_LG_ONOFF) && (this.chnLabel != this.labeldrgDali)) {
      setDpState(this.idDpState, 0, true);
    } else {
      setDpState(this.idDpState,0);
    }
    var t = this;
    new PeriodicalExecuter(function(pe) {
      t.refresh();
      pe.stop();
    }, 1);
  },
 
  onClickOn: function() {
    ControlBtn.pushed(this.divOn);
    if (this.chnLabel != this.labelGarageDoorController) {
      if ((this.chnLabel != this.labelVIR_LG_ONOFF) && (this.chnLabel != this.labeldrgDali)) {
        setDpState(this.idDpState, 1, true);
      } else {
        setDpState(this.idDpState,1);
      }
    } else {
      homematic("Interface.putParamset",{'interface': this.HmIPInterfaceID, 'address' : this.chnAddress, 'paramsetKey' : 'VALUES', 'set':
        [
          {name:'ON_TIME', type: 'double', value: this.garageDoorControllerOnTime},
          {name:'STATE', type: 'bool', value: true}
        ]
      },function(result){conInfo(result);});
    }
    var t = this;
    new PeriodicalExecuter(function(pe) {
      t.refresh();
      pe.stop();
    }, 1);
  },


  refresh: function() {
    if (this.state) {
      ControlBtn.on(this.divOn);
      ControlBtn.off(this.divOff);
    }
    else {
      ControlBtn.off(this.divOn);
      ControlBtn.on(this.divOff);
    }
  }

};

/**
 * @class
 **/
iseButtonsKey = Class.create();

iseButtonsKey.prototype = {
  /*
   * id = datapoint-ID of switch
   */
  initialize: function(id, shortId, longId, iViewOnly) {
    this.id = id;
    this.divShort = $(this.id + "Short");
    this.divLong = $(this.id + "Long");
    this.shortId = shortId;
    this.longId = longId;
    
    if( this.divShort ) { ControlBtn.off(this.divShort); }
    if( this.divLong ) { ControlBtn.off(this.divLong); }
    
    // Add event handlers
    if (iViewOnly === 0)
    {
      if (this.divShort) {
        this.clickShort = this.onClickShort.bindAsEventListener(this);
        Event.observe(this.divShort, 'mousedown', this.clickShort);
      }
      if (this.divLong) {
        this.clickLong = this.onClickLong.bindAsEventListener(this);
        Event.observe(this.divLong, 'mousedown', this.clickLong);
      }
    }
  },
  
  onClickShort: function() {
    setDpState(this.shortId, 1);
    ControlBtn.pushed(this.divShort);
    $("btn" + this.shortId + "s").src = "/ise/img/btn_press.png";
    var t = this;
    new PeriodicalExecuter(function(pe) {
      ControlBtn.off(t.divShort);
      $("btn" + t.shortId + "s").src = "/ise/img/btn_no_press.png";
      pe.stop();
    }, 1);
  },
  
  onClickLong: function() {
    setDpState(this.longId, 1);
    ControlBtn.pushed(this.divLong);
    $("btn" + this.longId + "l").src = "/ise/img/btn_press.png";
    var t = this;
    new PeriodicalExecuter(function(pe) {
      ControlBtn.off(t.divLong);
      $("btn" + t.longId + "l").src = "/ise/img/btn_no_press.png";
      pe.stop();
    }, 1);
  }
};

/**
 * @class
 **/
iseButtonProg = Class.create();
iseButtonProg.prototype = {
  initialize: function(id, progActive, thenOnly) {
    this.id = id;
    this.progActive = progActive;
    
    this.startBtn = (thenOnly === true) ? $(id + "StartThenOnly") : $(id + "Start");
    this.actBtn = $(id + "Act");
    
    this.thenOnly = thenOnly;
    
    if ( progActive ) { ControlBtn.on(this.actBtn); }
    
    // Add event handlers
    this.clickStart = this.onClickStart.bindAsEventListener(this);
    Event.observe(this.startBtn, 'mousedown', this.clickStart);
  },
  
  onClickStart: function() 
  {
    ControlBtn.pushed(this.startBtn);
    ExecuteProgram(this.id, this.thenOnly);
    var t = this;
    new PeriodicalExecuter(function(pe)
    {
      ControlBtn.off(t.startBtn);
      pe.stop();
    }, 1);
  }
};

/**
 * @class
 **/
iseButtonsEvent = Class.create();
iseButtonsEvent.prototype = {
  /*
   * id = datapoint-ID of Event
   */
  initialize: function(id, eventId, iViewOnly) {
    this.id = id;
    this.divEvent = $(this.id + "event");
    this.eventId = eventId;
    
    if( this.divEvent ) { ControlBtn.off(this.divEvent); }
    
    // Add event handlers
    if (iViewOnly === 0)
    {
      this.clickEvent = this.onClickEvent.bindAsEventListener(this);
      Event.observe(this.divEvent, 'mousedown', this.clickEvent);
    }
  },
  
  onClickEvent: function() {
    setDpState(this.eventId, 1);
    ControlBtn.pushed(this.divEvent);
    $("btn" + this.id + "s").src = "/ise/img/btn_press.png";
    var t = this;
    new PeriodicalExecuter(function(pe) {
      ControlBtn.off(t.divEvent);
      $("btn" + t.id + "s").src = "/ise/img/btn_no_press.png";
      pe.stop();
    }, 1);
  }
};