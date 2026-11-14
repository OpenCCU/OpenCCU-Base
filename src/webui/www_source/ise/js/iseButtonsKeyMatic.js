/**
 * ise/iseButtonsKeyMatic.js
 **/

/**
 * @fileOverview ?
 * @author ise
 **/

/* * * * * * * * * * * * * * * * * * * * * * * *
 * iseButtonsKeyMatic                          *
 * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * @class Bedien-Control für die KeyMatic
 **/ 
iseButtonsKeyMatic = Class.create();

iseButtonsKeyMatic.prototype = {
  /*
   * id = ID of KeyMatic
   */
  initialize: function(id, opts, iViewOnly) {
    this.id = id;
    this.opts = opts;
    this.divOpen = $(opts.idState + "Open");
    this.divClosed = $(opts.idState + "Close");
    this.divOpenDoor = $(opts.idOpen + "OpenDoor");
    this.divStatusIndefinite = $(opts.idUncertain + "na");
        
    if (iViewOnly === 0)
    {
      this.clickClose = this.onClickClose.bindAsEventListener(this);
      Event.observe(this.divClosed, 'mousedown', this.clickClose);
      
      this.clickOpen = this.onClickOpen.bindAsEventListener(this);
      Event.observe(this.divOpen, 'mousedown', this.clickOpen);
      
      this.clickDoorOpen = this.onClickDoorOpen.bindAsEventListener(this);
      Event.observe(this.divOpenDoor, 'mousedown', this.clickDoorOpen);
    }
    
    if (opts.stState == 1)
    { 
      ControlBtn.on(this.divOpen);
    }
    else
    {
      ControlBtn.on(this.divClosed);
    }
    
    if (opts.stUncertain == 1) {
      ControlBtn.on(this.divStatusIndefinite);
    }
    if (opts.stOpen == 1) {
      ControlBtn.on(this.divOpenDoor);
    }
  },
  
  onClickClose: function() {
    ControlBtn.pushed(this.divClosed);
    setDpState (this.opts.idState, 0);
    var t = this;
    new PeriodicalExecuter(function(pe) {
      ControlBtn.on(t.divClosed);
      ControlBtn.off(t.divOpen);
      pe.stop();
    }, 1);
  },
  
  onClickOpen: function() {
    ControlBtn.pushed(this.divOpen);
    setDpState (this.opts.idState, 1);
    var t = this;
    new PeriodicalExecuter(function(pe) {
      ControlBtn.on(t.divOpen);
      ControlBtn.off(t.divClosed);
      pe.stop();
    }, 1);
  },
  
  onClickDoorOpen: function() {
    ControlBtn.pushed(this.divOpenDoor);
    setDpState(this.opts.idOpen, 1);
    var t = this;
    new PeriodicalExecuter(function(pe) {
			if (t.opts.stOpen == 1) { ControlBtn.on(t.divOpenDoor); }
			else                    { ControlBtn.off(t.divOpenDoor); }
      pe.stop();
    }, 1);
  }
};
