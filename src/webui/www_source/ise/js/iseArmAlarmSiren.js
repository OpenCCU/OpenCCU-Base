/**
 * @class
 **/
iseArmAlarmSiren = Class.create();

iseArmAlarmSiren.prototype = {

  initialize: function(id, initState, idDpState) {
    conInfo("iseArmAlarmSiren - id: " + id + " - initState: " + initState + " - idDpState: " + idDpState);
    this.id = id;
    this.state = initState;
    this.divOff = $(this.id + "Off");
    this.divExtern = $(this.id + "Extern"); // all sensors
    this.divIntern = $(this.id + "Intern"); // only the outside sensors
    this.divBlocked = $(this.id + "Blocked");
    this.idDpState = idDpState;

    this.initAllElements();

    // Add event handlers
    this.clickOff = this.onClickOff.bindAsEventListener(this);
    Event.observe(this.divOff, 'mousedown', this.clickOff);

    this.clickExtern = this.onClickExtern.bindAsEventListener(this);
    Event.observe(this.divExtern, 'mousedown', this.clickExtern);

    this.clickIntern = this.onClickIntern.bindAsEventListener(this);
    Event.observe(this.divIntern, 'mousedown', this.clickIntern);

  },

  initAllElements: function() {
    ControlBtn.off(this.divOff);
    ControlBtn.off(this.divExtern);
    ControlBtn.off(this.divIntern);
    ControlBtn.off(this.divBlocked);
    switch(this.state) {
      case 0:
        ControlBtn.on(this.divOff);
        break;
      case 1:
        ControlBtn.on(this.divIntern); // outside sensors
        break;
      case 2:
        ControlBtn.on(this.divExtern); // all sensors
        break;
      case 3:
        ControlBtn.on(this.divBlocked);
    }
  },

  onClickOff: function() {
    ControlBtn.pushed(this.divOff);
    setDpState(this.idDpState, 0, false);
    var t = this;
    new PeriodicalExecuter(function(pe) {
      t.refresh();
      pe.stop();
    }, 1);
  },

  // all sensors
  onClickExtern: function() {
    ControlBtn.pushed(this.divExtern);
    setDpState(this.idDpState, 2, false);
    var t = this;
    new PeriodicalExecuter(function(pe) {
      t.refresh();
      pe.stop();
    }, 1);
  },

  // outside sensors
  onClickIntern: function() {
    ControlBtn.pushed(this.divIntern);
    setDpState(this.idDpState, 1, false);
    var t = this;
    new PeriodicalExecuter(function(pe) {
      t.refresh();
      pe.stop();
    }, 1);
  },

  refresh: function() {
    this.initAllElements();
  }
};