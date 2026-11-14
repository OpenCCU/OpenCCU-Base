NewDeviceListPage = new function()
{
  var MAINMENU_ID = "MAINMENU_OPTIONS";
  
  this.enter = function(options)
  {
    MainMenu.select(MAINMENU_ID);    
    
    // Aktualisiert alle Gewerke
    // ==> neue Geräte werden beim Anlernen automatisch einem Gewerk zugeordnet
    var subsections = SubsectionList.list();
    subsections.each(function(subsection) {
      SubsectionList.beginUpdate(subsection.id);
    });
    
    loadNewDevices(options);
  };
  
  this.leave = function()
  {
  };

}();
