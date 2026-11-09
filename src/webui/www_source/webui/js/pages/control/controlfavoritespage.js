ControlFavoritesPage = new function()
{

  var MAINMENU_ID = "MAINMENU_FAVORITES";
  
  this.enter = function(options)
  {
    MainMenu.select(MAINMENU_ID);
    loadFavorites(options);
  };
  
  this.leave = function()
  {
  };

}();
