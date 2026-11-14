/**
 * ise/DoorOpener.js
 * Türöffner.
 **/

/**
 * @fileOverview ?
 * @author ise
 **/

/**
 * @class Bedien-Control für den Türöffner
 **/ 
DoorOpenerControl = Class.create();
DoorOpenerControl.prototype = {

  /**
   * Konstruktor.
   *   channelId  : Id des Türöffners (Kanal)
   *   dataPointId: Id des Datenpunkts DOOROPENER.STATE
   *   iViewOnly  :     0: Control bedienbar
   *                sonst: Control nicht bedienbar
   **/
  initialize: function(channelId, dataPointId, iViewOnly) 
  {
    this.channelId     = channelId;
    this.dataPointId   = dataPointId;
    this.doorOpenerDiv = $(channelId + "DoorOpener");
    
    if( this.doorOpenerDiv ) { ControlBtn.off(this.doorOpenerDiv); }
    
    if (iViewOnly === 0) 
    {
      this.click = this.onClick.bindAsEventListener(this);
      Event.observe(this.doorOpenerDiv, 'mousedown', this.click);
      Element.setStyle(this.doorOpenerDiv, {"cursor": "pointer"});
    }
  },
  
  /**
   * Ereignis: Click auf this.doorOpenerDiv
   * Öffnet die Tür.
   **/
  onClick: function() 
  {
    setDpState(this.dataPointId, 1);
    ControlBtn.pushed(this.doorOpenerDiv);
    var t = this;
    new PeriodicalExecuter(function(pe) {
      ControlBtn.off(t.doorOpenerDiv);
      pe.stop();
    }, 1);
  }

};
