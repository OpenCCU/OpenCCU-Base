DeviceFirmware = new function()
{
  var MAINMENU_ID = "MAINMENU_OPTIONS";
  
  this.enter = function(options)
  {
    MainMenu.select(MAINMENU_ID);

    conInfo("Show page device firmware");

    doClearTempView = true;
    if( typeof filter == "string" )
    {
      updateContent("jpages/devicefirmware", "", 'system.SetSessionVar("sessionPrgIsFiltered", true);system.SetSessionVar("sessionPrgFilter","'+filter+'");' );
    }
    else
    {
      updateContent("jpages/devicefirmware");
    }
  };
  
  this.leave = function()
  {
  };

}();
