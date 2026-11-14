/**
 * ise/iseButtonsWindow.js
 **/

/**
 * @fileOverview ?
 * @author ise
 **/

/* * * * * * * * * * * * * * * * * * * * * * * *
 * iseButtonsWindow                            *
 * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * @class
 **/ 
iseButtonsWindow = Class.create();

iseButtonsWindow.prototype = {
  /*
   * id = datapoint-ID of switch
   * initState = Creation State (0 or 1)
   */
  initialize: function(id, initState) {
    this.id = id;
    this.state = initState;
    this.divOpenH = $(this.id + "OpenH");
    this.divOpenV = $(this.id + "OpenV");
    this.divClosed = $(this.id + "Closed");
    
    switch (initState) {
      case 0:
      case false:
        ControlBtn.on(this.divClosed);
        break;
      case 1:
        ControlBtn.on(this.divOpenV);
        break;
      case 2:
      case true:
        ControlBtn.on(this.divOpenH);
        break;
      default:
        break;
    }    
  }
};


/* * * * * * * * * * * * * * * * * * * * * * * *
 * iseButtonsWinMatic                          *
 * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * @class
 **/ 
iseButtonsWinMatic= Class.create();

iseButtonsWinMatic.prototype = {
  /*
   * id = datapoint-ID of switch
   * initState = Creation State (0 or 1)
   */
  initialize: function(id, opts, iViewOnly)
  {
    this.Window = "WinMatic";
    conInfo(this.Window);
    this.id = id;
    this.opts = opts;
    this.Circle = $(this.id + "Circle");
    this.Perc = $(this.id + "Perc");
    this.divPercUp = $(this.id + "PercUp");
    this.divPercDown = $(this.id + "PercDown");
    this.divStop = $(this.id + "Stop");
    this.divOpen = $(this.id + "Open");
    this.divClose = $(this.id + "Close");
    this.divLocked = $(this.id + "Locked");
    this.divUnknown = $(this.id + "Unknown");
    this.state = this.opts.stLevel * 100;
    Released = false;
    // Draw WinMatic Control
    var s = "<div id='spec"+this.id+"'><div id='"+this.id+"Ctrl' style='position:relative;top:0px;left:0px;line-height:0;background-color: White; width:100px;height:100px;'>" +
            "<img src='/ise/img/window/circle.png' /></div></div>";
    this.Circle.innerHTML = s;
    this.graphics = new jsGraphics(this.id+"Ctrl");
    this.graphics.setColor(WebUI.getColor("active")); // grün
    
    // Add event handlers
    if (iViewOnly === 0) {
      this.bindEvents();
      this.initSpecialDevice();
    }
    this.refresh(false);
  },

  onClickCtrl: function(ev) {
    var pos = Position.page(this.Circle);
    var offsetX = ev.clientX - pos[0];
    var offsetY = ev.clientY - pos[1];
    if (offsetX < 60)
      this.state = 0;
    else {
      if (this.isInZone50(offsetX, offsetY) ) 
        this.state = 50;
      else
        this.state = 100;
    }
    this.refresh();    
  },
  
  isInZone50: function(x, y)
  {
    var px = x;
    var py = y;
    var x1 = 57;
    var y1 = 0;
    var x2 = 57;
    var y2 = 100;
    var x3 = 120;
    var y3 = 0;

    var fAB = (py-y1)*(x2-x1) - (px-x1)*(y2-y1);
    var fCA = (py-y3)*(x1-x3) - (px-x3)*(y1-y3);
    var fBC = (py-y2)*(x3-x2) - (px-x2)*(y3-y2);

    var bRet = false;
    if ( (fAB*fBC > 0) && (fBC*fCA > 0) )
    {
      bRet = true;
    }
    return bRet;
  },
  
  onClickPercUp: function()
  {
    this.state += 10; 
    if( this.state > 100 ) this.state = 100;
    this.Perc.value = this.state;
    this.refresh();
  },
  
  onClickPercDown: function()
  {
    this.state -= 10; 
    if( this.state < 0 ) this.state = 0;
    this.refresh();
  },
  
  onChangePerc: function()
  {
    if( !isNaN( this.Perc.value ) )
    {
      var iTmp = parseInt(this.Perc.value);
      if (iTmp < 0)
        iTmp = 0;
      this.state = iTmp;
      this.refresh();
    }
  },
  
  onClickOpen: function() {
    this.state = 100;
    ControlBtn.pushed(this.divOpen);
    var t = this;
    new PeriodicalExecuter(function(pe) {
      ControlBtn.off(t.divOpen);
      t.refresh();
      pe.stop();
    }, 1);
  },
  
  onClickClose: function() {
    this.state = 0;
    this.Perc.value = this.state;
    ControlBtn.pushed(this.divClose);
    var t = this;
    new PeriodicalExecuter(function(pe) {
      ControlBtn.off(t.divClose);
      t.refresh();
      pe.stop();
    }, 1);
  },
  
  onClickStop: function()
  {
    var t = this;
    conInfo( this.Window + " OnClickStop [ID:"+this.opts.idStop+"]" );
    setDpState(this.opts.idStop, 1);
    ControlBtn.pushed(this.divStop);
    new PeriodicalExecuter(function(pe)
    {
      ControlBtn.off(t.divStop);
      pe.stop();
    }, 1);
  },
  
  onClickLock: function() {
    ControlBtn.pushed(this.divLocked);
    this.state = -5;
    var t = this;
    new PeriodicalExecuter(function(pe) {
      ControlBtn.off(t.divLocked);
      t.refresh();
      pe.stop();
    }, 1);
  },

  bindEvents: function() {
    this.clickCtrl = this.onClickCtrl.bindAsEventListener(this);
    Event.observe(this.Circle, 'mousedown', this.clickCtrl);

    this.clickPercUp = this.onClickPercUp.bindAsEventListener(this);
    Event.observe($(this.id + "PercUp"), 'click', this.clickPercUp);
    this.clickPercDown = this.onClickPercDown.bindAsEventListener(this);
    Event.observe($(this.id + "PercDown"), 'click', this.clickPercDown);
    this.changePerc = this.onChangePerc.bindAsEventListener(this);
    Event.observe(this.Perc, 'change', this.changePerc);

    this.clickOpen = this.onClickOpen.bindAsEventListener(this);
    Event.observe($(this.id + "Open"), 'mousedown', this.clickOpen);

    this.clickClose = this.onClickClose.bindAsEventListener(this);
    Event.observe($(this.id + "Close"), 'mousedown', this.clickClose);

    this.clickLock = this.onClickLock.bindAsEventListener(this);
    Event.observe($(this.id + "Locked"), 'mousedown', this.clickLock);

    this.clickStop = this.onClickStop.bindAsEventListener(this);
    Event.observe($(this.id + "Stop"), 'mousedown', this.clickStop);
  },


  initSpecialDevice: function() {},

  refresh: function(bRefresh)
  {
    this.Perc.value = this.state;
    if (this.state == -5 || this.state == -0.5 || isNaN(this.state))
    {
      this.Perc.value = "0";
    }


    this.graphics.clear();
    var startAngle = 90 - (this.state * 0.45);
    if( (startAngle > 0) && (startAngle < 90))
    {
      // verhindern dass ein voller Kreis gezeichnet wird
      this.graphics.fillArc(-65, 5, 188, 179 , startAngle, 90);
    }
    this.graphics.paint();

    if( typeof( bRefresh ) == "undefined" )
    {
      setDpState(this.opts.idLevel, (this.state / 100));
    }
    if (this.state == -0.5)
    {
      // Zustand verriegelt ??
      ControlBtn.off(this.divOpen);
      ControlBtn.on(this.divClose);
      ControlBtn.on(this.divLocked);
    }
    else
    {
      ControlBtn.off(this.divLocked);
      if (this.state > 0)
      {
        ControlBtn.on(this.divOpen);
        ControlBtn.off(this.divClose);
      }
      else
      {
        ControlBtn.off(this.divOpen);
        ControlBtn.on(this.divClose);
      }
    }

    if( this.opts.stUncertain )
    {
      ControlBtn.on(this.divUnknown);
    }
    else
    {
      ControlBtn.off(this.divUnknown);
    }
  }
};

iseButtonsWin_SC = Class.create(iseButtonsWinMatic, {

  initSpecialDevice: function() {
    this.Window = "WIN_SC";
    conInfo(this.Window);
    this.LEDMode1 = $(this.id + "LEDMode1");
    this.LEDMode2 = $(this.id + "LEDMode2");
    this.LEDMode3 = $(this.id + "LEDMode3");
    this.SummerMode = $(this.id + "SummerMode");
    this.WinterMode = $(this.id + "WinterMode");
    this.HandleLock = $(this.id + "HandleLock");
    this.HandleUnlock = $(this.id + "HandleUnlock");
    this.Open = $(this.id + "Open");
    this.Close = $(this.id + "Close");
    this.Release = $(this.id + "Release");
    this.elmHH = jQuery("#winterModeHH");
    this.elmMM = jQuery("#winterModeMM");
    this.elmSS = jQuery("#winterModeSS");
    this.initControls();
    this.bindAdditionalEvents();
  },


  initControls: function() {
    var controls = [
      this.LEDMode1,
      this.LEDMode2,
      this.LEDMode3,
      this.SummerMode,
      this.WinterMode,
      this.HandleLock,
      this.HandleUnlock
      //this.Open,
      //this.Close,
      //this.Release
    ];

    // All off
    jQuery(controls).each(function(){
      ControlBtn.off(this);
    });

    switch (this.opts.stLEDMode.toString()) {
      case "0" :
        ControlBtn.on(this.LEDMode1);
        break;
      case "1" :
        ControlBtn.on(this.LEDMode2);
        break;
      case "2" :
        ControlBtn.on(this.LEDMode3);
        break;
    }

    if (this.opts.stWinterMode != 111600) {
      ControlBtn.on(this.WinterMode);
      this.setValueWinterMode(true);
    } else {
      ControlBtn.on(this.SummerMode);
      this.setValueWinterMode(false);
    }

    if (this.opts.stHandleLock) {
      ControlBtn.on(this.HandleLock);
    } else {
      ControlBtn.on(this.HandleUnlock);
    }

    // Window is released
    /*
    if (this.opts.stRelease) {
      ControlBtn.on(this.Release);
      ControlBtn.off(this.Open);
      ControlBtn.off(this.Close);
    } else {
      // Window is either closed or open
      ControlBtn.off(this.Release);

      if (this.opts.stLevel <= 0) {
        // Window is closed
        ControlBtn.on(this.Close);
        ControlBtn.off(this.Open);
      } else {
        // Window is open
        ControlBtn.on(this.Open);
        ControlBtn.off(this.Close);
      }
    }
    */
  },

  /**
   * Sets the input fields for the time of the winter mode control
   * @param bMode true = winter mode - false = summer mode
   */
  setValueWinterMode: function(bMode) {
    if (bMode) {
      var oTime = this.getHHMMSSWinterMode();
      this.elmHH.val(oTime.hh);
      this.elmMM.val(oTime.mm);
      this.elmSS.val(oTime.ss);
    } else {
      this.elmHH.val("18");
      this.elmMM.val("12");
      this.elmSS.val("15");
    }
  },

  getHHMMSSWinterMode: function() {
    var s = this.opts.stWinterMode;
    var h = Math.floor(s/3600);
    s -= h*3600;
    var m = Math.floor(s/60);
    s -= m*60;
    return {"hh" : h, "mm":m, "ss": s};
  },

  getSecondsWinterMode: function() {
    return (parseInt(this.elmHH.val() * 3600)) + (parseInt(this.elmMM.val() * 60)) + parseInt(this.elmSS.val());
  },

  checkValue: function(val) {

    switch (val) {
      case "hh" :
          var val = parseInt(this.elmHH.val());
          if (val > 18) {this.elmHH.val("18");break;}
          if (val < 0 ) {this.elmHH.val("0");break;}
          if (isNaN(val)) {this.elmHH.val("0");}
        break;
      case "mm" :
        var val = parseInt(this.elmMM.val());
        if (val > 59) {this.elmMM.val("59");break;}
        if (val < 0 ) {this.elmMM.val("0");break;}
        if (isNaN(val)) {this.elmMM.val("0");}
        break;
      case "ss" :
        var val = parseInt(this.elmSS.val());
        if (val > 59) {this.elmSS.val("59");break;}
        if (val < 0 ) {this.elmSS.val("0");break;}
        if (isNaN(val)) {this.elmSS.val("0");}
        break;
    }
  },

  bindAdditionalEvents : function() {
    var self = this;

    this.clickWinRelease = this.onClickWinRelease.bindAsEventListener(this);
    Event.observe($(this.id + "Release"), 'mousedown', this.clickWinRelease);

    this.clickHandleUnlock = this.onClickHandleUnlock.bindAsEventListener(this);
    Event.observe($(this.id + "HandleUnlock"), 'mousedown', this.clickHandleUnlock);

    this.clickHandleLock = this.onClickHandleLock.bindAsEventListener(this);
    Event.observe($(this.id + "HandleLock"), 'mousedown', this.clickHandleLock);

    this.clickSummerMode = this.onClickSummerMode.bindAsEventListener(this);
    Event.observe(this.SummerMode, 'mousedown', this.clickSummerMode);

    this.clickWinterMode = this.onClickWinterMode.bindAsEventListener(this);
    Event.observe(this.WinterMode, 'mousedown', this.clickWinterMode);

    this.clickLEDMode1 = this.onClickLEDMode1.bindAsEventListener(this);
    Event.observe(this.LEDMode1, 'mousedown', this.clickLEDMode1);

    this.clickLEDMode2 = this.onClickLEDMode2.bindAsEventListener(this);
    Event.observe(this.LEDMode2, 'mousedown', this.clickLEDMode2);

    this.clickLEDMode3 = this.onClickLEDMode3.bindAsEventListener(this);
    Event.observe(this.LEDMode3, 'mousedown', this.clickLEDMode3);

    this.elmHH.bind("blur", function() {self.checkValue("hh");});
    this.elmMM.bind("blur", function() {self.checkValue("mm");});
    this.elmSS.bind("blur", function() {self.checkValue("ss");});

  },

  onClickOpen: function() {
    if (Released == undefined || Released == false) {
      this.state = 100;
      setDpState(this.opts.idLevel, (this.state / 100));
    }
  },

  onClickClose: function() {
    if (Released == undefined || Released == false) {
      this.state = 0;
      this.Perc.value = this.state;
      setDpState(this.opts.idLevel, (this.state / 100));
    }
  },

  _onClickWinRelease : function() {
      //setDpState(this.opts.idRelease, true);
      setDpState(this.opts.idRelease, 1);
      if (this.state > 0) {
        ControlBtn.off(this.Open);
        ControlBtn.off(this.Close);
        ControlBtn.on(this.Release);
      }
  },

  onClickWinRelease : function() {
     setDpState(this.opts.idRelease, 1);
  },

  onClickHandleUnlock : function() {
    //alert("HANDLE UNLOCK");
    setDpState(this.opts.idHandleLock, false);
    //ControlBtn.pushed(this.HandleUnlock);
    //ControlBtn.on(this.HandleUnlock);
    //ControlBtn.off(this.HandleLock);
  },

  onClickHandleLock : function() {
    //alert("HANDLE LOCK");
    setDpState(this.opts.idHandleLock, true);
    //ControlBtn.pushed(this.HandleLock);
    //ControlBtn.off(this.HandleUnlock);
    //ControlBtn.on(this.HandleLock);
  },

  onClickSummerMode : function() {
    //alert("Summer Mode");
    setDpState(this.opts.idWinterMode, 111600);
    ControlBtn.pushed(this.SummerMode);
    var t = this;
    new PeriodicalExecuter(function(pe) {
      //ControlBtn.on(t.SummerMode);
      //ControlBtn.off(t.WinterMode);
      pe.stop();
    }, 1);
  },

  onClickWinterMode : function() {
    //alert("Winter Mode");
    fixTimeForWinterMode = 600;

    this.checkValue("hh");
    this.checkValue("mm");
    this.checkValue("ss");

    //setDpState(this.opts.idWinterMode, this.getSecondsWinterMode()); // for development only

    setDpState(this.opts.idWinterMode, fixTimeForWinterMode);
    ControlBtn.pushed(this.WinterMode);
    var t = this;
    new PeriodicalExecuter(function(pe) {
      //ControlBtn.off(t.SummerMode);
      //ControlBtn.on(t.WinterMode);
      pe.stop();
    }, 1);
  },

  onClickLEDMode1 : function() {
    // LED OFF
    conInfo("WIN_SC.HANDLE_LED_MODE 0");
    setDpState(this.opts.idLEDMode, 0);
    ControlBtn.pushed(this.LEDMode1);
    var t = this;
    new PeriodicalExecuter(function(pe) {
      //ControlBtn.on(t.LEDMode1);
      //ControlBtn.off(t.LEDMode2);
      //ControlBtn.off(t.LEDMode3);
      pe.stop();
    }, 1);
  },

  onClickLEDMode2 : function() {
    // LED LOW
    conInfo("WIN_SC.HANDLE_LED_MODE 1");
    setDpState(this.opts.idLEDMode, 1);
    ControlBtn.pushed(this.LEDMode2);
    var t = this;
    new PeriodicalExecuter(function(pe) {
     //ControlBtn.off(t.LEDMode1);
     //ControlBtn.on(t.LEDMode2);
     //ControlBtn.off(t.LEDMode3);
      pe.stop();
    }, 1);
  },

  onClickLEDMode3 : function() {
    // LED ON
    conInfo("WIN_SC.HANDLE_LED_MODE 2");
    setDpState(this.opts.idLEDMode, 2);
    ControlBtn.pushed(this.LEDMode3);
    var t = this;
    new PeriodicalExecuter(function(pe) {
      //ControlBtn.off(t.LEDMode1);
      //ControlBtn.off(t.LEDMode2);
      //ControlBtn.on(t.LEDMode3);
      pe.stop();
    }, 1);
  },

  onClickPercUp: function()
  {
    if (this.state <= 0) {
      this.state = 20;
    } else {
      this.state += 10;
    }
    if( this.state > 100 ) this.state = 100;
    this.Perc.value = this.state;
    this.refresh();
  },

  onClickPercDown: function()
  {
    if (this.state >= 30) {
      this.state -= 10;
    } else {
      this.state-= 20;
    }
    if( this.state < 0 ) this.state = 0;
    this.refresh();
  }

});

iseButtonsWin_SC_SENSOR = Class.create(iseButtonsWinMatic, {

  initSpecialDevice: function() {
    this.Window = "WIN_SC_SENSOR";
    conInfo(this.Window);
    this.imgWinClosed = "/ise/img/window/closed.png";
    this.imgWinOpen = "/ise/img/window/open_v.png";
    this.imgWinReleased = "/ise/img/window/open_h.png";

    //this.Open = $(this.id + "Open");
    //this.Close = $(this.id + "Close");
    this.stateIndicatorImgElem = jQuery("#" + this.id + "stateIndicatorImg");
    this.stateIndicatorDescrElem = jQuery("#" + this.id + "stateIndicatorDescr");
    this.stateDescrElem = jQuery("#" + this.id + "stateDescr");
    this.windowTypeDescr = jQuery("#" + this.id + "windowType");


    this.level = (this.state < 0 ) ? 0 : this.state;

    jQuery("#" + this.id + "lblPerc").html("Öffnungs-<br/>winkel<br/>" + this.level);
    this.initControls();
  },

  initControls: function() {
    var tmpLevel;

    var release = $$(".j_winSC_Release")[0],
      open = jQuery(".j_winSC_Open")[0],
      close = jQuery(".j_winSC_Close")[0],
      perc = jQuery(".j_winSC_Perc")[0],

      codeReleasedDue = 9,
      codeReleased = 10;

    // Window released
    if ((this.opts.stTipTronicState == codeReleased) || (this.opts.stTipTronicState == codeReleasedDue)) {
      Released = true;
      ControlBtn.on(release);
      ControlBtn.off(open);
      ControlBtn.off(close);

      this.stateIndicatorImgElem.attr('src', this.imgWinReleased);
      this.stateIndicatorDescrElem.html(translateKey("actionStatusControlReleased"));
    } else {
      Released = false;
      new PeriodicalExecuter(function(pe) {
        ControlBtn.off(release); // channel 1
        pe.stop();
      }, 1);
      tmpLevel = this.level;
      // Window open
      if (parseInt(tmpLevel) > 0) {
        ControlBtn.on(open); // channel 1
        ControlBtn.off(close); // channel 1
        ControlBtn.off(release); // channel 1

        this.stateIndicatorImgElem.attr('src', this.imgWinOpen);
        this.stateIndicatorDescrElem.html(translateKey("actionStatusControlOpenB") + "<br/>" + this.level + "%");
      } else {
        // window closed
        ControlBtn.on(close); // channel 1
        ControlBtn.off(open); // channel 1
        ControlBtn.off(release); // channel 1
        this.stateIndicatorImgElem.attr('src', this.imgWinClosed);
        this.stateIndicatorDescrElem.html(translateKey("actionStatusControlClosed"));
      }
    }
    jQuery(perc).val(this.level);
    this.stateDescrElem.html(translateKey("stringTableActorWindowTT_"+this.opts.stTipTronicState));
    this.windowTypeDescr.html(translateKey("stringTableSensorWindowType_"+this.opts.stWindowType));

  },

  // No action when clicking the buttons
  onClickCtrl: function() {},
  onClickOpen: function() {},
  onClickClose : function() {}
});