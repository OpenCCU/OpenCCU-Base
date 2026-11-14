/**
 * ise/shutterControl.js
 **/

/**
 * @fileOverview ?
 * @author ise
 **/

/**
 * @class
 **/
shutterControl = Class.create();

shutterControl.prototype = {

  m_color: WebUI.getColor("shutterOpeningDegree"),
  
  /*
   * id = id of div containing shutter-image
   * initState 
   */
  initialize: function(id, initState) {
    this.MAX_HEIGHT = 100;
    this.state = initState;
    

    $("shutter" + id).innerHTML = "<div id='shutterBg" + id + "' style='width:139px;height:"+this.MAX_HEIGHT+"px;background-image:url(/ise/img/shutterCtrl.png);'>" +
        "<div style='text-align:left;' id='spec"+id+"'><div id='shutterCtrl" +id +"' style='border-bottom:solid 1px Black;width:98px;height:"+ parseInt(this.MAX_HEIGHT - 1) + "px;background-color:" + this.m_color + ";position:relative;left:1px;top:1px;'></div>" +
        "</div></div>";
    this.divShutter = $("shutterCtrl" + id);
    this.divShutterBg = $("shutterBg" + id);
  },
  
  setValue: function(val) {
    this.state = val;
    this.divShutter.style.height = parseInt(this.MAX_HEIGHT - (val * (this.MAX_HEIGHT / 100))) + "px";
  }
};