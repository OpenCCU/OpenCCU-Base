LinkEditProfilePage = new function()
{
  var m_menuId = "MAINMENU_LINKSANDPROGRAMS";
  
  this.enter = function(options)
  {
    var iface    = options.iface;
    var sender   = options.sender;
    var receiver = options.receiver;
    
    MainMenu.select(m_menuId);
    OpenSetProfiles(iface, sender, receiver);
  
  };
  
  this.leave = function()
  {
  };
  
};