VariableListPage = new function()
{
  var MAINMENU_ID = "MAINMENU_OPTIONS";
  
  this.enter = function(options)
  {
    MainMenu.select(MAINMENU_ID);    
    loadSysconfigSysVars(options);
  };
  
  this.leave = function()
  {
  };

}();
