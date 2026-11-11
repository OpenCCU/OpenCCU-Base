ControlRoomsPage = new function()
{
  var MAINMENU_ID = "MAINMENU_CONTROL";
  
  this.enter = function(options)
  {
    MainMenu.select(MAINMENU_ID);
    loadHandlingRooms(options);
  };
  
  this.leave = function()
  {
  };

}();
