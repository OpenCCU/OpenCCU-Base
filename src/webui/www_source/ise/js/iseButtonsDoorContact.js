/**
 * ise/iseButtonsDoorContact.js
 **/

/**
 * @fileOverview ?
 * @author ise
 **/

/**
 * @class
 **/ 
iseButtonsDoorContact = Class.create();

iseButtonsDoorContact.prototype = {
  /*
   * id = datapoint-ID of switch
   * initState = Creation State (0 or 1)
   */
  initialize: function(id, initState)
  {
    this.id = id;
    this.state = this.convertInitState(initState);
    this.divOpen = $(this.id + "Open");
    this.divClosed = $(this.id + "Closed");
    
    if( this.state > 0 )
    {
      ControlBtn.on(this.divOpen);
    }
    else 
    {
      ControlBtn.on(this.divClosed);
    }
  },

  convertInitState: function(initState) {

    switch (initState) {
      case "0":
      case "false":
      case "CLOSED":
        return 0;
      case "1":
      case "200":
      case "true":
      case "OPEN":
        return 1;
    }
    return -1;
  }

};