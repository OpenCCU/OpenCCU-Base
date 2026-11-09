ControlProtocolPage = new function()
{
  var MAINMENU_ID = "MAINMENU_STATUS";
  
  this.enter = function(options)
  {
    MainMenu.select(MAINMENU_ID);
    loadStatusviewSysProtocol(options);
  };
  
  this.leave = function()
  {
  };

}();
