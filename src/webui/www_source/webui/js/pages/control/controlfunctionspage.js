ControlFunctionsPage = new function()
{
  var MAINMENU_ID = "MAINMENU_CONTROL";
  
  this.enter = function(options)
  {
    MainMenu.select(MAINMENU_ID);
    loadHandlingFunctions(options);
  };
  
  this.leave = function()
  {
  };

}();
