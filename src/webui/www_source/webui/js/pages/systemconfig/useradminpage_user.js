UserAdminPageUser = new function()
{
  var MAINMENU_ID = "MAINMENU_OPTIONS";
  
  this.enter = function(options)
  {
    MainMenu.select(MAINMENU_ID);    
    updateContent('/pages/tabs/user/userAdministrationUser.htm');
  };
  
  this.leave = function()
  {
  };

}();
