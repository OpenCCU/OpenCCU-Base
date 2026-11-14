ControlProgramsPage = new function()
{
  var MAINMENU_ID = "MAINMENU_CONTROL";
  
  this.enter = function(options)
  {
    MainMenu.select(MAINMENU_ID);
    loadHandlingPrograms();
  };
  
  this.leave = function()
  {
  };

}();
