ControlVariablesPage = new function()
{
  var MAINMENU_ID = "MAINMENU_STATUS";
  
  this.enter = function(options)
  {
    MainMenu.select(MAINMENU_ID);
    loadStatusviewSysVars(options);
  };
  
  this.leave = function()
  {
  };

}();
