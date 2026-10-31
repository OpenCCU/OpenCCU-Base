/**
 * ise/iseThermostat.js
 **/

/**
 * @fileOverview ?
 * @author ise
 **/

 /**
 * @class
 **/
iseThermostat = Class.create();

iseThermostat.prototype = {
  /*
   * id = DOM-ID of switch
   * initState = Creation State 
   */
  initialize: function(id, initState, lvlDP, min, max, iViewOnly, bSliderPosFlag, opts)
  {
    conInfo( "iseThermostat: initialize()" );
    conInfo ("value(: " + initState + ") min(" + min + ") max(" + max + ")");
    this.id = id;
    this.state = initState;
    this.lvlDP = lvlDP;
    this.min = min;
    this.max = max;
    this.factor = 100/(max-min);
    this.unit = "&nbsp;&deg;C";
    this.statusOFF = "OFF";
    this.statusON = "ON";
    this.opts = opts;
    if(bSliderPosFlag)
    {
        this.bSliderPosFlag = bSliderPosFlag;
    }
    else
    {
        this.bSliderPosFlag = false;
    }
    // AG this.slider = new sliderControl( "thermo", this.id, initState, iViewOnly, this.bSliderPosFlag,this.min, this.max, this.factor, this.unit);
    this.slider = new sliderControl( "thermo", this.id, this.state, iViewOnly, this.bSliderPosFlag,this.min, this.max, this.factor, this.unit);
    this.hasRampClicked = false;
    this.txtDeg = $(this.id + "Deg");
    this.unitDegree = $(this.id + "unitDegree");
    // Add event handlers
    if (iViewOnly === 0)
    {
      this.mouseOut = this.onMouseOut.bindAsEventListener(this);
      Event.observe($("slidCtrl" + this.id), 'mouseout', this.mouseOut);

      this.rampClick = this.onRampClick.bindAsEventListener(this);
      Event.observe(this.slider.e_base, 'mousedown', this.rampClick);

      this.handleClick = this.onHandleClick.bindAsEventListener(this);
      Event.observe($("slidCtrl" + this.id), 'mouseup', this.handleClick);

      this.clickUp = this.onClickUp.bindAsEventListener(this);
      Event.observe($(this.id + "Up"), 'click', this.clickUp);

      this.clickDown = this.onClickDown.bindAsEventListener(this);
      Event.observe($(this.id + "Down"), 'click', this.clickDown);

      this.percChange = this.onPercChange.bindAsEventListener(this);
      Event.observe($(this.id + "Deg"), 'change', this.percChange);

      this.init2ndGen();
    }
    this.refresh(false);
  },

  init2ndGen: function() {},

  onMouseOut: function(event)
  {
    var e = event;
    if (!e) { e = window.event; }
    var relTarg = e.relatedTarget || e.fromElement;
    if( relTarg )
    {
      var b1 = (relTarg.id.indexOf("slider")!=-1);
      var b2 = (relTarg.id.indexOf("base")!=-1);
      var b3 = (relTarg.id.indexOf("green")!=-1);
      var b4 = (relTarg.id.indexOf("spec")!=-1);
      if( !b1 && !b2 && !b3 && !b4)
      {
        if( this.hasRampClicked )
        {
          conInfo( "iseThermostat: onMouseOut() ["+relTarg.id+"], wanna set: " + ( (this.slider.n_value/this.factor) + this.min)  );
          this.hasRampClicked = false;

          this.state = (this.slider.n_value/this.factor) + this.min;
          this.refresh();
        }
      }
    }
  },

  onRampClick: function(ev)
  {
     this.hasRampClicked = true;
     var pos = Position.page(this.slider.e_base);
     var offset = ev.clientX - pos[0];
     var val = ( offset * 100 ) / this.slider.n_controlWidth;
     this.slider.f_setValue(val);
     this.state = (Math.floor(val)/ this.factor) + this.min;
     conInfo( "iseThermostat: onRampClick() at ("+val + ") set-> " + this.state );
     // this.refresh();
     //window.setTimeout("ibd"+this.id+".refresh()",1000);
  },

  onHandleClick: function()
  {
    conInfo( "iseThermostat: onHandleClick() deg: "  + this.txtDeg.value);
    this.hasRampClicked = false;
    this.state = this.txtDeg.value;
    //this.refresh();

    if(this.state < this.min){ this.state = this.min; }
    if (this.state > this.max) this.state = this.max;
    conInfo("onHandleClick - setting DP "+this.lvlDP+" State -------> " + this.state);
    setDpState(this.lvlDP, (this.state));
  },

  onClickUp: function()
  {
    conInfo( "iseThermostat: onClickUp()" );
    // this.state = (this.slider.n_value/this.factor);
    this.state = Math.round(this.state  + 1);
    if (this.state > this.max)
      this.state = this.max;

    this.refresh();
  },

  onClickDown: function()
  {
    conInfo( "iseThermostat: onClickDown()" );
    // this.state = (this.slider.n_value/this.factor);
    this.state = Math.round(this.state - 1);
    if (this.state < this.min)
      this.state = this.min;

    this.refresh();
  },

  onPercChange: function()
  {
    conInfo( "iseThermostat: onPercChange()" );
    if( isNaN(this.txtDeg.value) ) return;
    if( parseInt(this.txtDeg.value) > this.max ) this.txtDeg.value = this.max;
    if( parseInt(this.txtDeg.value) < this.min ) this.txtDeg.value = this.min;
    this.state = this.txtDeg.value;
    this.refresh();
  },

  update: function(newVal)
  {
    conInfo( "iseThermostat: update()" );
    this.state = newVal;
    this.refresh(newVal);
  },

  hideUnitDegree: function()
  {
    if (this.unitDegree != null) {
      this.unitDegree.hide();
    }
  },

  refresh: function(setstate)
  {
    conInfo( "iseThermostat: refresh()"+this.state );
    if(this.state < 0){ this.state = 0; }
    if (this.state > this.max) this.state = this.max;
    this.slider.f_setValue((this.state -this.min) * this.factor, true);
    switch (this.state) {
      case 4.5:
        this.txtDeg.value = this.statusOFF;
        this.hideUnitDegree();
        break;
      case 30.5:
        this.txtDeg.value = this.statusON;
        this.hideUnitDegree();
        break;
      default:
        this.txtDeg.value = round(this.state, 1);
        if(this.unitDegree != null) {
          this.unitDegree.show();
        }
    }
    if(typeof setstate == "undefined")
    {
      conInfo("setting DP "+this.lvlDP+" State -------> " + this.state);
      setDpState(this.lvlDP, (this.state));
    }
  }
};

iseThermostat_2ndGen = Class.create(iseThermostat, {

  init2ndGen: function() {
    conInfo("2nd Generation");
    conInfo(this.opts);
    this.level = parseFloat(this.opts.stLevel);
    this.activeMode = this.opts.stControlMode; // stControlMode = 0 - 3 / AUTO, MANU, HOLIDAY, BOOST
    this.inputDegree = $(this.id + "Deg");
    this.btnHeatingUncertain = $(this.id + "HeatingModeUncertain");
    this.btnHeatingON = $(this.id + "modeON");
    this.btnHeatingOFF = $(this.id + "modeOFF");
    this.btnModeAuto = $(this.id + "mode0");
    this.btnModeManu = $(this.id + "mode1");
    this.btnModeParty = $(this.id + "mode2");
    this.btnModeBoost = $(this.id + "mode3");
    this.btnTempComfort = $(this.id + "tempCOMFORT");
    this.btnTempEco = $(this.id + "tempECO");
    this.chn = homematic("Device.get",{"id": this.id});

    if (this.opts.stTemp) {
      this.showActualTemp();
    }
    if (this.opts.stHum !== undefined) {
      this.showActualHumidity();                                      
    }
    if (this.opts.stBat) {
      this.showActualBat();
    }
    if (this.opts.stValve !== undefined) {
      this.showActualValve();
    }
    if (this.opts.idComfort && this.opts.idLowering) {
      this.showComfortEco();
    }
    if (this.opts.idAuto && this.opts.idManu && this.opts.idBoost) {
      this.showChangeMode();
    }

    if (this.opts.idPartyTemp) {
      this.showPartyMode();
    }

    this.showOnOff();

    // opts.stPartyStartTime and opts.stPartyStopTime will be overwritten here because ReGa is setting this values wrong.
    // This seems to be a 8 bit problem. ReGa sets a real value of 300 (5 hours since midnight ) e. g. to 44 which equals 300 - 256
    // As a result the party mode dialog will show a time of AM 5:00 as AM 0:44
    this.opts.stPartyStartTime = homematic("Interface.getValue", {'interface': this.chn["interface"], 'address' : this.chn["address"], 'valueKey': 'PARTY_START_TIME'});
    this.opts.stPartyStopTime = homematic("Interface.getValue", {'interface': this.chn["interface"], 'address' : this.chn["address"], 'valueKey': 'PARTY_STOP_TIME'});

    if (this.activeMode == 2) {
      // Holiday mode active
      this.showHolidayEndTime();
    }
    try {
      this.initButtons();
      this.bindEvents();
    } catch (e) {}
  },

  initButtons: function() {
    this.initOnOffBtn();
    this.setActiveMode();
  },

  initOnOffBtn: function() {

    if (this.level < 4.5 || this.level > 30.5) {
      //ControlBtn.on(this.btnHeatingUncertain);
      ControlBtn.off(this.btnHeatingON);
      ControlBtn.off(this.btnHeatingOFF);
      this.clearDegree();
      return;
    }

    if (this.level < 5.0) {
      ControlBtn.on(this.btnHeatingOFF);
      ControlBtn.off(this.btnHeatingON);
      this.clearDegree();
    } else if (this.level > 30.0) {
      ControlBtn.on(this.btnHeatingON);
      ControlBtn.off(this.btnHeatingOFF);
      this.clearDegree();
    } else {
      ControlBtn.off(this.btnHeatingON);
      ControlBtn.off(this.btnHeatingOFF);
    }
  },

  // sets the button of the active mode
  setActiveMode: function() {
    ControlBtn.on(this.id + "mode" + this.activeMode);
  },

  // Clears the temperature field whithin the temp. box
  clearDegree: function() {
    this.inputDegree.value = "";
  },

  bindEvents: function() {
    this.clickModeAuto = this.onClickModeAuto.bindAsEventListener(this);
    Event.observe(this.btnModeAuto, 'mousedown', this.clickModeAuto);

    this.clickModeManu = this.onClickModeManu.bindAsEventListener(this);
    Event.observe(this.btnModeManu, 'mousedown', this.clickModeManu);

    this.clickModeBoost = this.onClickModeBoost.bindAsEventListener(this);
    Event.observe(this.btnModeBoost, 'mousedown', this.clickModeBoost);

    // Party Mode currently deactivated
     this.clickModeParty = this.onClickModeParty.bindAsEventListener(this);
     Event.observe(this.btnModeParty, 'mousedown', this.clickModeParty);

    this.clickHeatingOFF = this.onClickHeatingOFF.bindAsEventListener(this);
    Event.observe(this.btnHeatingOFF, 'mousedown', this.clickHeatingOFF);

    this.clickHeatingON = this.onClickHeatingON.bindAsEventListener(this);
    Event.observe(this.btnHeatingON, 'mousedown', this.clickHeatingON);

    this.clickTempCOMFORT = this.onClickTempComfort.bindAsEventListener(this);
    Event.observe(this.btnTempComfort, 'mousedown', this.clickTempCOMFORT);

    this.clickTempECO = this.onClickTempEco.bindAsEventListener(this);
    Event.observe(this.btnTempEco, 'mousedown', this.clickTempECO);

  },

  onClickModeAuto: function() {
    conInfo("clickModeAuto");
    setDpState(this.opts.idAuto, 1);
    this.setModeButton(this.btnModeAuto);
  },

  onClickModeManu: function() {
    conInfo("clickModeManu");
    setDpState(this.opts.idManu, this.level);
    this.setModeButton(this.btnModeManu);
  },


  onClickModeBoost: function() {
    conInfo("clickModeBoost");
    setDpState(this.opts.idBoost, 1);
    this.setModeButton(this.btnModeBoost);
  },

  onClickModeParty: function() {
    conInfo("clickModeParty");
    var self = this;
    var url = "/pages/msg/setPartyMode.htm";

    var req = jQuery.ajax({
      url : url +"?sid=" + SessionId,
      cache: false,
      dataType: "html"
    });

    req.done(function(data) {

      partyModeDialog = new PartyModeDialog(translateKey("partyModeDialogTitle"), data, self.opts ,function(result) {
        if (result == 1) {
          conInfo("SET PARTYMODE");
          var oPartyMode = this.getPartyModeObject();
          conInfo("iseThermostat_2ndGen - oPartyMode: ");
          conInfo(oPartyMode);
          homematic("Interface.putThermParamset",{'interface': self.chn["interface"], 'address' : self.chn["address"], 'set':
            [
              {name:'PARTY_TEMPERATURE', type: 'string', value: oPartyMode.temp},
              {name:'PARTY_START_TIME', type: 'string', value: oPartyMode.startMinutesSinceMidnight},
              {name:'PARTY_START_DAY', type: 'string', value: oPartyMode.startDay},
              {name:'PARTY_START_MONTH', type: 'string', value: oPartyMode.startMonth},
              {name:'PARTY_START_YEAR', type: 'string', value: oPartyMode.startYear},
              {name:'PARTY_STOP_TIME', type: 'string', value: oPartyMode.stopMinutesSinceMidnight},
              {name:'PARTY_STOP_DAY', type: 'string', value: oPartyMode.stopDay},
              {name:'PARTY_STOP_MONTH', type: 'string', value: oPartyMode.stopMonth},
              {name:'PARTY_STOP_YEAR', type: 'string', value: oPartyMode.stopYear}
            ]
          });
          self.setModeButton(self.btnModeParty);
        }
      }, "html");
    });

    req.fail(function() {
      alert("Error while loading " + url);
    });
    //this.setModeButton(this.btnModeParty);
  },


  onClickHeatingOFF: function() {
    conInfo("clickTempOFF");
    setDpState(this.opts.idManu, 4.5);
    ControlBtn.on(this.btnHeatingOFF);
    ControlBtn.off(this.btnHeatingON);
  },

  onClickHeatingON: function() {
    conInfo("clickTempON");
    setDpState(this.opts.idManu, 30.5);
    ControlBtn.on(this.btnHeatingON);
    ControlBtn.off(this.btnHeatingOFF);
  },

  onClickTempComfort: function() {
    var self = this;
    conInfo("clickTempComfort");
    setDpState(this.opts.idComfort, 1);
    ControlBtn.pushed(this.btnTempComfort);
    setTimeout(function() {ControlBtn.off(self.btnTempComfort);},3000);
  },

  onClickTempEco: function() {
    var self = this;
    conInfo("clickTempEco");
    setDpState(this.opts.idLowering, 1);
    ControlBtn.pushed(this.btnTempEco);
    setTimeout(function() {ControlBtn.off(self.btnTempEco);},3000);
  },

  showActualTemp: function() {
    jQuery("#"+this.id +"actTemp").text(this.opts.stTemp);
    jQuery("#"+this.id +"tblShowTemp").show();
  },

  showActualHumidity: function() {
    if (parseInt(this.opts.stHum) === 20) {
      jQuery("#"+this.id +"actHumidity").text('n/a');
    } else {
      jQuery("#"+this.id +"actHumidity").text(this.opts.stHum);
      jQuery("#"+this.id +"actHumidityUnit").show();
    }
    jQuery("#"+this.id +"tblShowHumidity").show();
  }, 

  showActualBat: function() {
    jQuery("#"+this.id +"actBatteryState").text(this.opts.stBat);
    jQuery("#"+this.id +"tblShowBatteryState").show();
  },
  
  showActualValve: function() {
    jQuery("#"+this.id +"actValveState").text(this.opts.stValve);
    jQuery("#"+this.id +"tblShowValveState").show();
  },
  
  showHolidayEndTime: function() {
    var stopTime = this.getHolidayEndTime();
    if (stopTime) {
      jQuery("#"+this.id+"partyEndTime").text(stopTime);
      jQuery("#"+this.id+"showPartyEnd").show();
    }
  },

  showChangeMode: function() {
    jQuery("#tblChangeMode"+this.id).show();
  },

  showOnOff: function() {
    jQuery("#"+this.id+"modeOFF, #"+this.id+"modeON").show();
  },

  showComfortEco: function() {
    jQuery("#"+this.id+"tempECO, #"+this.id+"tempCOMFORT").show();
  },

  showPartyMode: function() {
    jQuery("#"+this.id + "mode2").show();
  },

  setModeButton: function(btn) {
    ControlBtn.off(this.btnModeAuto);
    ControlBtn.off(this.btnModeManu);
    ControlBtn.off(this.btnModeBoost);
    ControlBtn.off(this.btnModeParty);
    ControlBtn.on(btn);
  },

  getHolidayEndTime: function() {
    if (isNaN(parseInt(this.opts.stPartyStopTime))) {return false;}
    var stopTime = "";
    stopTime += addLeadingZero(this.opts.stPartyStopDay) + ".";
    stopTime += addLeadingZero(this.opts.stPartyStopMonth) + ".";
    stopTime += addLeadingZero(this.opts.stPartyStopYear) + " - ";
    stopTime += addLeadingZero(parseInt((parseInt(this.opts.stPartyStopTime) / 60))) + ":" + addLeadingZero(parseInt(this.opts.stPartyStopTime) % 60);

    // Device firmware  < 1.3
    if ((stopTime == "01.01.00 - 00:00") || (stopTime.indexOf("undefined") > -1)) {return false;}

    return stopTime;
  }

});

iseThermostatHMIP = Class.create();

iseThermostatHMIP.prototype = {

  initialize: function(opts) {
    conInfo(opts);
    var self = this;
    this.opts = opts;
    this.devId = opts.devID;
    this.chId = this.opts.chID;
    this.chAddress = this.opts.chAddress;
    this.iface = this.opts.chInterface;
    this.setPointID = this.opts.setPointID;
    this.setPointModeID = this.opts.setPointModeID;
    this.controlModeID = this.opts.controlModeID;
    this.boostID = this.opts.boostID;
    this.partyStartID = this.opts.partyStartID;
    this.partyEndID = this.opts.partyEndID;
    this.partyEndValue = this.opts.partyEndValue;
    this.partyMode = this.opts.partyMode;
    this.windowStateID = this.opts.windowStateID;
    this.statusOFF = "OFF";
    this.statusON = "ON";
    this.activeProfileID = this.opts.activeProfileID;

    var paramSetMaster = homematic('Interface.getParamset', {"interface": this.iface, "address" : this.chAddress, "paramsetKey" : "MASTER"});

    this.confTempMin = parseFloat(paramSetMaster["TEMPERATURE_MINIMUM"]); // Configured min temp (device settings)
    this.confTempMax = parseFloat(paramSetMaster["TEMPERATURE_MAXIMUM"]); // Configured max temp (device settings);

    this.offTemp = 4.5;
    this.onTemp = 30.5;
    this.min = (this.confTempMin < 5) ? 5 : this.confTempMin;
    this.max = (this.confTempMax > 30) ? 30 : this.confTempMax;
    this.off = (this.confTempMin < 5) ? this.offTemp : this.confTempMin;
    this.on = (this.confTempMax > 30) ? this.onTemp : this.confTempMax;
    this.unit = "&nbsp;&deg;C";
    this.factor = 100/(this.max-this.min);
    this.iViewOnly = false;
    this.bSliderPosFlag = false;
    this.hasRampClicked = false;
    this.btnUpElem = this.getElemByID("Up");
    this.btnDownElem = this.getElemByID("Down");
    this.percentElem = this.getElemByID("Deg");
    this.sliderControl = jQuery("#slidCtrl"+ this.chId);
    this.btnAuto = this.getElemByID("Auto");
    this.btnManu = this.getElemByID("Manu");
    this.btnBoost = this.getElemByID("Boost");
    this.btnParty = this.getElemByID("Party");
    this.btnON = this.getElemByID("On");
    this.btnOFF = this.getElemByID("Off");

    this.btnHeating = this.getElemByID("Heating");
    this.btnCooling = this.getElemByID("Cooling");

    this.activeProfileElm = this.getElemByID("ActiveProfile");
    this.unitDegree = this.getElemByID("unitDegree");
    this.unknownState = "--";

    this.heatingCoolingAllowed = (this.iface != 'VirtualDevices') ? true : false;
    this.checkIfHeatingCoolingAllowed();


    this.setResult(opts);
    this.setSControl("ACTUAL_TEMPERATURE", this.ACTUAL_TEMPERATURE);
    this.setSControl("HUMIDITY", this.HUMIDITY);
    this.setSControl("WINDOW_STATE", this.WINDOW_STATE);
    this.setSControl("BATTERY_STATE", this.BATTERY_STATE);
    this.setSControl("LEVEL", this.LEVEL);
    this.setSlider();
    this.initElements();
    this.bindEvents();
  },

  setResult: function(opts) {
    //conInfo(opts);
    this.ACTIVE_PROFILE = this.checkValue(opts.ACTIVE_PROFILE, 0);
    this.ACTUAL_TEMPERATURE = this.checkValue(parseFloat(opts.ACTUAL_TEMPERATURE).toFixed(2), this.unknownState);
    this.BOOST_MODE = (this.checkBool(opts.BOOST_MODE, false) == "true") ? true : false;
    //this.FROST_PROTECTION = opts.FROST_PROTECTION;
    this.HUMIDITY = this.checkValue(opts.HUMIDITY, this.unknownState);
    this.BATTERY_STATE = this.checkValue(opts.BATTERY_STATE, this.unknownState);
    this.LEVEL = this.checkValue(opts.LEVEL, this.unknownState);
    //this.PARTY_MODE = opts.PARTY_MODE;
    this.SET_POINT_MODE = this.checkValue(opts.SET_POINT_MODE, 0);
    this.SET_POINT_TEMPERATURE = this.checkValue(opts.SET_POINT_TEMPERATURE, 4.5);
    this.WINDOW_STATE = this.checkWindowState(opts.WINDOW_STATE);
    this.state = this.SET_POINT_TEMPERATURE;
   },

  checkValue: function(val, substitute) {
    return ((val == undefined) || (isNaN(parseFloat(val)))) ? substitute : val;
  },

  checkBool: function(val, substitute) {
    return ((val == undefined) || (val == "")) ? substitute : val;
  },

  checkWindowState: function(windowState) {
    var state = (windowState == undefined || windowState == "") ? this.unknownState : windowState;
    if (parseInt(state) == 0) {
      state = translateKey("infoStatusControlLblClosed");
    } else if (parseInt(state) == 1) {
      state =translateKey("infoStatusControlLblOpen");
    }
    return state;
  },

  initElements: function() {

    if (this.confTempMin == this.offTemp) {this.btnOFF.show().text(translateKey("actionStatusControlLblOff"));} else {this.btnOFF.show().html(translateKey("minTemp")).css("line-height", "");}
    if (this.confTempMax == this.onTemp) {this.btnON.show().text(translateKey("actionStatusControlLblOn"));} else {this.btnON.show().html(translateKey("maxTemp")).css("line-height", "");}


    if (this.BOOST_MODE) {
      JControlBtn.on(this.btnBoost);
    } else {
      JControlBtn.off(this.btnBoost);
    }

    switch (this.SET_POINT_MODE) {
      case "0":
        JControlBtn.off(this.btnManu);
        JControlBtn.off(this.btnParty);
        JControlBtn.on(this.btnAuto);
        break;
      case "1":
        JControlBtn.off(this.btnAuto);
        JControlBtn.off(this.btnParty);
        JControlBtn.on(this.btnManu);
        break;
      case "2":
        JControlBtn.off(this.btnAuto);
        JControlBtn.off(this.btnManu);
        JControlBtn.on(this.btnParty);
    }

    if (this.SET_POINT_TEMPERATURE < 5.0) {
      JControlBtn.off(this.btnAuto);
      JControlBtn.on(this.btnOFF);
      this.percentElem.val(this.statusOFF);
    } else if (this.SET_POINT_TEMPERATURE > 30.0) {
      JControlBtn.off(this.btnOFF);
      JControlBtn.on(this.btnON);
      this.percentElem.val(this.statusON);
    } else {
      JControlBtn.off(this.btnOFF);
      JControlBtn.off(this.btnON);
      this.percentElem.val(parseFloat(this.state).toFixed(1));
    }

    switch (this.percentElem.val()) {
      case this.statusOFF:
        this.hideUnitDegree();
        break;
      case this.statusON:
        this.hideUnitDegree();
        break;
      default:
        if(this.unitDegree != null) {
          this.unitDegree.show();
        }
    }
    this.activeProfileElm.val(this.ACTIVE_PROFILE);

    if (this.partyMode == "true") {
      // Holiday mode active
      this.showHolidayEndTime();
    }

  },

  getHolidayEndTime: function() {
    var result = null;
    if (this.partyEndValue) {
      var arTimeString = this.partyEndValue.split(" "),
        arDate = arTimeString[0].split("_"),
        oDate = {},
        time = arTimeString[1];

      oDate.year = addLeadingZero(parseInt(arDate[0]));
      oDate.month = addLeadingZero(parseInt(arDate[1]));
      oDate.day = addLeadingZero(parseInt(arDate[2]));

      result = oDate.day + "." + oDate.month + "." + oDate.year + " - " + time;
    }
    return result;
  },

  showHolidayEndTime: function() {
    var stopTime = this.getHolidayEndTime();
    if (stopTime != null) {
      jQuery("#"+this.chId+"partyEndTime").text(stopTime);
      jQuery("#"+this.chId+"showPartyEnd").show();
    }
  },

  getElemByID: function(elm) {
    var chId = this.chId;
    return jQuery("#"+chId + elm);
  },

  getElemByName: function(elm) {
    var chId = this.chId;
    return jQuery("[name='"+chId+elm+"']");
  },

  // Control with string value
  setSControl: function(elm, val) {
    this.getElemByID("val"+elm).text(val);
    this.getElemByID(elm).show();
  },

  setSlider: function() {
    this.slider = new sliderControl( "thermo", this.chId, this.SET_POINT_TEMPERATURE, this.iViewOnly, this.bSliderPosFlag,this.min, this.max, this.factor, this.unit);
    this.slider.f_setValue((this.SET_POINT_TEMPERATURE - this.min) * this.factor, true);
  },

  bindEvents: function() {

    this.sliderControl.bind("mouseout", {that: this}, this.onMouseOut);
    //this.sliderControl.bind("mouseup", {that: this}, this.onHandleClick);
    jQuery(this.slider.e_base).bind("mousedown", {that: this}, this.onRampClick);

    this.btnUpElem.bind("click", {that:this}, this.onClickUp);
    this.btnDownElem.bind("click", {that:this}, this.onClickDown);
    this.percentElem.bind("change", {that:this}, this.onPercChange);

    this.btnAuto.bind("click", {that:this}, this.onClickModeAuto);
    this.btnManu.bind("click", {that:this}, this.onClickModeManu);
    this.btnBoost.bind("click", {that:this}, this.onClickModeBoost);
    this.btnParty.bind("click", {that:this}, this.onClickModeParty);
    this.btnON.bind("click", {that:this}, this.onClickModeON);
    this.btnOFF.bind("click", {that:this}, this.onClickModeOFF);
  },

  onRampClick: function(event){
    event.data.that.hasRampClicked = true;

    var pos = Position.page(event.data.that.slider.e_base);
    var offset = event.clientX - pos[0];
    var val = ( offset * 100 ) / event.data.that.slider.n_controlWidth;
    event.data.that.slider.f_setValue(val);
    event.data.that.state = (Math.floor(val)/ event.data.that.factor) + event.data.that.min;
    conInfo( "iseThermostat: onRampClick() at ("+val + ") set-> " + event.data.that.state );
  },

  onMouseOut: function(event)
  {
    var e = event;
    if (!e) { e = window.event; }
    var relTarg = e.relatedTarget || e.fromElement;

    if( relTarg )
    {
      var b1 = (relTarg.id.indexOf("slider")!=-1);
      var b2 = (relTarg.id.indexOf("base")!=-1);
      var b3 = (relTarg.id.indexOf("green")!=-1);
      var b4 = (relTarg.id.indexOf("spec")!=-1);
      if( !b1 && !b2 && !b3 && !b4)
      {
        if( event.data.that.hasRampClicked )
        {
          event.data.that.hasRampClicked = false;
          event.data.that.state = event.data.that.percentElem.val();
          event.data.that.refresh(event.data.that.setPointID);
        }
      }
    }
  },

  onHandleClick: function(event) {
    conInfo("onHandleClick- hasRampClicked: " +event.data.that.hasRampClicked);
  },

  onClickUp: function(event)
  {
    conInfo( "iseThermostat: onClickUp()" );
    event.data.that.state = (parseFloat(event.data.that.state) + 1);
    if (event.data.that.state > event.data.that.max)
      event.data.that.state = event.data.that.max;
    event.data.that.refresh(event.data.that.setPointID);
  },

  onClickDown: function(event)
  {
    conInfo( "iseThermostat: onClickDown()" );
    event.data.that.state = (parseFloat(event.data.that.state) -1);
    if (event.data.that.state < event.data.that.min)
      event.data.that.state = event.data.that.min;
    event.data.that.refresh(event.data.that.setPointID);
  },

  onPercChange: function(event)
  {
    conInfo( "iseThermostat: onPercChange()" );
    if( isNaN(event.data.that.percentElem.val()) ) return;
    if( parseInt(event.data.that.percentElem.val()) > event.data.that.max ) event.data.that.percentElem.val(event.data.that.max);
    if( parseInt(event.data.that.percentElem.val()) < this.min ) event.data.that.percentElem.val(event.data.that.min);
    event.data.that.state = event.data.that.percentElem.val();

    event.data.that.refresh(event.data.that.setPointID);
  },

  onClickModeAuto: function(event) {
    conInfo("clickModeAuto");
    if (event.data.that.SET_POINT_MODE != 0) {
      setDpState(event.data.that.controlModeID, 0);
      //JControlBtn.off(event.data.that.btnManu);
      JControlBtn.pressed(event.data.that.btnAuto);
    }
  },

  onClickModeManu: function(event) {
    conInfo("clickModeManu");
    if (event.data.that.SET_POINT_MODE != 1) {
      setDpState(event.data.that.controlModeID, 1);
      //JControlBtn.off(event.data.that.btnAuto);
      JControlBtn.pressed(event.data.that.btnManu);
    }
  },

  onClickModeBoost: function(event) {
    conInfo("clickModeBoost");
    var self = event.data.that;
    if ((parseFloat(self.state) > 4.5) && (parseFloat(self.state) < 30.5)) {
      if (! self.BOOST_MODE) {
        setDpState(self.boostID, 1);
        JControlBtn.pressed(self.btnBoost);
      } else {
        setDpState(self.boostID, 0);
        JControlBtn.off(self.btnBoost);
      }
    } else {
      JControlBtn.pressed(self.btnBoost);
    }
  },

  onClickModeON: function(event) {
    conInfo("clickModeON");
    //setDpState(event.data.that.setPointID, event.data.that.on);
    //JControlBtn.off(event.data.that.btnOFF);

    var self = event.data.that;
    JControlBtn.pressed(self.btnON);
    homematic("Interface.putParamset",{'interface': self.iface, 'address' : self.chAddress, 'paramsetKey' : 'VALUES', 'set':
      [
        {name:'SET_POINT_MODE', type: 'int', value: "1"},
        {name:'CONTROL_MODE', type: 'int', value: "1"},
        {name:'SET_POINT_TEMPERATURE', type: 'double', value:self.on}
      ]
    });

  },

  onClickModeOFF: function(event) {
    conInfo("clickModeOFF - SET_POINT_TEMPERATURE: " + this.off);
    //setDpState(event.data.that.setPointID, event.data.that.off);
    //JControlBtn.off(event.data.that.btnON);

    var self = event.data.that;
    JControlBtn.pressed(self.btnOFF);
    homematic("Interface.putParamset",{'interface': self.iface, 'address' : self.chAddress, 'paramsetKey' : 'VALUES', 'set':
      [
        {name:'SET_POINT_MODE', type: 'int', value: "1"},
        {name:'CONTROL_MODE', type: 'int', value: "1"},
        {name:'SET_POINT_TEMPERATURE', type: 'double', value: self.off}
      ]
    });
  },

  onClickModeParty: function(event) {
    conInfo("clickModeParty");
    var self = this;
    var url = "/pages/msg/setPartyMode.htm";

    var partyStartTime = event.data.that.opts.partyStartValue,
    partyEndTime = event.data.that.opts.partyEndValue,
    partySetPointTemp = event.data.that.opts.partySetPointTempValue;

    var oPartyValues = event.data.that.formatPartyTime(partyStartTime, partyEndTime);
    oPartyValues.stPartyTemp = parseInt(partySetPointTemp);

    var req = jQuery.ajax({
      url : url +"?sid=" + SessionId,
      cache: false,
      dataType: "html"
    });

    req.done(function(htmlContent) {
      partyModeDialog = new PartyModeDialog(translateKey("partyModeDialogTitle"), htmlContent, oPartyValues,function(result) {

        var self = event.data.that;
        var iface = self.iface,
          chAddress = self.chAddress;

        if (result == 1) {
          conInfo("SET PARTYMODE");
          var oPartyMode = self.formatPartyMode(this.getPartyModeObject());

          var partyTimeStart =
            oPartyMode.startYear + "_" + oPartyMode.startMonth + "_" + oPartyMode.startDay + " " + oPartyMode.startHour + ":" + oPartyMode.startMin,
          partyTimeEnd =
            oPartyMode.stopYear + "_" + oPartyMode.stopMonth + "_" + oPartyMode.stopDay + " " + oPartyMode.stopHour + ":" + oPartyMode.stopMin;

          conInfo("iseThermostatHMIP - partyTimeStart: " + partyTimeStart + " - partyTimeEnd: " + partyTimeEnd + " - temp: " + oPartyMode.temp);

          homematic("Interface.putParamset",{'interface': iface, 'address' : chAddress, 'paramsetKey' : 'VALUES', 'set':
            [
              {name:'SET_POINT_MODE', type: 'int', value: "2"},
              {name:'SET_POINT_TEMPERATURE', type: 'double', value: oPartyMode.temp},
              {name:'PARTY_TIME_START', type: 'string', value: partyTimeStart},
              {name:'PARTY_TIME_END', type: 'string', value: partyTimeEnd}
            ]
          });
          //self.setModeButton(self.btnModeParty);
        }
      }, "html");
    });

    req.fail(function() {
      alert("Error while loading " + url);
    });
    //this.setModeButton(this.btnModeParty);
  },

  formatPartyTime: function(partyStart, partyEnd) {

    var result = {};

    if (this.isPartyTimeAvailable(partyStart) && this.isPartyTimeAvailable(partyEnd)) {
      var arPartyStartDate = partyStart.split(" ")[0].split("_"),
        arPartyStartTime = partyStart.split(" ")[1].split(":"),
        startYear = arPartyStartDate[0],
        startMonth = arPartyStartDate[1],
        startDay = arPartyStartDate[2],
        startTime = parseInt(arPartyStartTime[0]) * 60 + parseInt(arPartyStartTime[1]);

      var arPartyStopDate = partyEnd.split(" ")[0].split("_"),
        arPartyStopTime = partyEnd.split(" ")[1].split(":"),
        stopYear = arPartyStopDate[0],
        stopMonth = arPartyStopDate[1],
        stopDay = arPartyStopDate[2],
        stopTime = parseInt(arPartyStopTime[0]) * 60 + parseInt(arPartyStopTime[1]);

      result.stPartyStartDay = startDay;
      result.stPartyStartMonth = startMonth;
      result.stPartyStartYear = startYear;
      result.stPartyStartTime = startTime;

      result.stPartyStopDay = stopDay;
      result.stPartyStopMonth = stopMonth;
      result.stPartyStopYear = stopYear;
      result.stPartyStopTime = stopTime;
    } else {
      result.showEmptyFields = true;
    }

    return result;
  },

  isPartyTimeAvailable: function(timeString) {
    return ((timeString != undefined) && (timeString != "")) ? true : false;
  },

  formatTimeVal: function(val) {
    return (parseInt(val) < 10) ? "0" + val : val;
  },

  formatPartyMode: function(oPartyMode) {
    oPartyMode.startYear = "20" + oPartyMode.startYear;
    oPartyMode.stopYear = "20" + oPartyMode.stopYear;
    oPartyMode.startMonth = this.formatTimeVal(oPartyMode.startMonth);
    oPartyMode.stopMonth = this.formatTimeVal( oPartyMode.stopMonth);
    oPartyMode.startDay = this.formatTimeVal(oPartyMode.startDay);
    oPartyMode.stopDay = this.formatTimeVal(oPartyMode.stopDay);
    oPartyMode.startHour = this.formatTimeVal(oPartyMode.startHour);
    oPartyMode.stopHour = this.formatTimeVal(oPartyMode.stopHour);
    oPartyMode.startMin = this.formatTimeVal(oPartyMode.startMin);
    oPartyMode.stopMin = this.formatTimeVal(oPartyMode.stopMin);
    return oPartyMode;
  },

  hideUnitDegree: function()
  {
    if (this.unitDegree != null) {
      this.unitDegree.hide();
    }
  },

  refresh: function(setPointID)
  {
    if(this.state < this.min) { this.state = this.min; }
    if (this.state > this.max) { this.state = this.max; }
    this.slider.f_setValue((this.state -this.min) * this.factor, true);
    this.percentElem.val(parseFloat(this.state).toFixed(1));
    conInfo("refresh: setting DP "+this.setPointID+" State -------> " + this.state);
    setDpState(setPointID, this.state);
  },

  // SPHM-1231 - When a link between a CLIMATECONTROL_FLOOR_TRANSMITTER (eg. WTH chn. 7) and a CLIMATECONTROL_FLOOR_TRANSCEIVER (e. g. FALMOT-C12 chn. 1 - 12) exists,
  // the buttons Heating and Cooling must be only readable.
  checkIfHeatingCoolingAllowed: function () {
    var self = this;
    if (this.heatingCoolingAllowed) {
      var dev, chn, arLinkPeers = [];

      dev = DeviceList.getDeviceByAddress(this.chAddress.split(":")[0]);
      if (typeof dev != "undefined") {
        jQuery.each(dev.channels, function (index, chn) {
          if (chn.channelType == "CLIMATECONTROL_FLOOR_TRANSMITTER") {
            arLinkPeers = homematic("Interface.getLinkPeers", {'interface': self.iface, 'address': chn.address});
            jQuery.each(arLinkPeers, function (index, chnAddress) {
              chn = DeviceList.getChannelByAddress(chnAddress);
              if (chn.channelType == "CLIMATECONTROL_FLOOR_TRANSCEIVER") {
                self.heatingCoolingAllowed = false;
              }
            });
          }
        });

        if (!this.heatingCoolingAllowed) {
          this.btnHeating.prop("onclick", null);
          this.btnCooling.prop("onclick", null);
          this.btnHeating.unbind("click").click(function () {
            MessageBox.show(translateKey("dialogHint"), translateKey("hintHeatingCoolngNotAllowed"), function () {
              loadChannels(self.devId);
            }, 500, 125);
          }).css("cursor", "default");
          this.btnCooling.unbind("click").click(function () {
            MessageBox.show(translateKey("dialogHint"), translateKey("hintHeatingCoolngNotAllowed"), function () {
              loadChannels(self.devId);
            }, 500, 125);
          }).css("cursor", "default");
        }
      } else {
        window.setTimeout(function() {self.checkIfHeatingCoolingAllowed();}, 2000);
      }
    }
  }

};