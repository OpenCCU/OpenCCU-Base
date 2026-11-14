/**
 * ise/iseButtonsDisplay.js
 **/

/**
 * @fileOverview ?
 * @author ise
 **/

/* * * * * * * * * * * * * * * * * * * * * * * *
 * iseButtonsDisplay                           *
 * * * * * * * * * * * * * * * * * * * * * * * */

/**
 * @class
 **/ 
iseButtonsDisplay = Class.create();

iseButtonsDisplay.prototype = {
  /*
   * id = ID of Display
   * initState = Creation State (0 or 1)
   */
  initialize: function(id, initState) {
    this.id = id;
    this.state = initState;

/*
    this.divOpen = $(this.id + "Open");
    this.divClosed = $(this.id + "Close");
    this.divOpenDoor = $(this.id + "OpenDoor");
    this.divStatusIndefinite = $(this.id + "StatusIndefinite")
    
    if (initState > 0)
    {
      if (initState == 1) { ControlBtn.on(this.divOpen); }
    }
    else if (initState == -1) { ControlBtn.on(this.divStatusIndefinite); }
    else { ControlBtn.on(this.divClosed); }
*/
  }
};
