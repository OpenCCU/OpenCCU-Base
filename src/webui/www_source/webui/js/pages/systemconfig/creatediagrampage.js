CreateDiagramPage = new function()
{
  var MAINMENU_ID = "MAINMENU_OPTIONS";
  
  this.enter = function(options)
  {
    MainMenu.select(MAINMENU_ID);
    /*
    // response should be:
    // ID : string
    // KEYS : array of strings
    // NAME : string
    var response = homematic("Diagram.getDataSourceGroups");
    if (response) {
      jQuery.each(response, function(index, val) {
        console.dir(val);
      });
    }
    */

    conInfo("create diagram page");

    doClearTempView = true;
    if( typeof filter == "string" )
    {
      updateContent("jpages/diagram/settings", "", 'system.SetSessionVar("sessionPrgIsFiltered", true);system.SetSessionVar("sessionPrgFilter","'+filter+'");' );
    }
    else
    {
      updateContent("jpages/diagram/settings");
    }
  };
  
  this.leave = function()
  {
  };

}();
