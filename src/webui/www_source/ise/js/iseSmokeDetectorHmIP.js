/**
 * ise/iseButtonsSwitch.js
 **/

/**
 * @fileOverview ?
 * @author ise
 **/

/**
 * @class
 **/
iseSmokeDetectorHmIP = Class.create();

iseSmokeDetectorHmIP.prototype = {

  initialize: function(id, controlID, dangerState) {

    this.id = id;
    this.state = parseInt(dangerState);
    this.divOn = $(this.id + "On");
    this.divOff = $(this.id + "Off");
    this.idDpState = controlID;
    
    if (this.state == 2) { ControlBtn.on(this.divOn); }
    else  { ControlBtn.on(this.divOff); }
      
    // Add event handlers
    this.clickOff = this.onClickOff.bindAsEventListener(this);
    Event.observe(this.divOff, 'mousedown', this.clickOff);

    this.clickOn = this.onClickOn.bindAsEventListener(this);
    Event.observe(this.divOn, 'mousedown', this.clickOn);


  },
  
  onClickOff: function() {
    ControlBtn.pushed(this.divOff);
    setDpState(this.idDpState, 1, false);
    var t = this;
    new PeriodicalExecuter(function(pe) {
      t.refresh();
      pe.stop();
    }, 1);
  },
 
  onClickOn: function() {
    ControlBtn.pushed(this.divOn);
    setDpState(this.idDpState, 2, false);
    var t = this;
    new PeriodicalExecuter(function(pe) {
      t.refresh();
      pe.stop();
    }, 1);
  },
 
  
  refresh: function() {
    if (this.state == 2) {
      ControlBtn.on(this.divOn);
      ControlBtn.off(this.divOff);
    }
    else {
      ControlBtn.off(this.divOn);
      ControlBtn.on(this.divOff);
    }
  }
};

