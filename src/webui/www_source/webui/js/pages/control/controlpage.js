ControlPage = new function()
{
  var MAINMENU_ID = "MAINMENU_CONTROL";
  
  this.enter = function(options)
  {
    MainMenu.select(MAINMENU_ID);
    loadStatus(options);
  };
  
  this.leave = function()
  {
    // open the systemLanguage config dialog if no systemLanguage was selected yet
    if (homematic('CCU.getSystemLanguage') === 'none') {
      CreatePopup(ID_SYSTEM_LANGUAGE_CONFIG);
    }
  };

}();
