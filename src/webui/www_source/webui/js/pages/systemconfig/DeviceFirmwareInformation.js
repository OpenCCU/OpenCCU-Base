function compare(a,b) {
  if (a.name < b.name)
    return -1;
  if (a.name > b.name)
    return 1;
  return 0;
}

DeviceFirmwareInformation = new function()
{
  var MAINMENU_ID = "MAINMENU_OPTIONS";

  /**
   * @param options
   *        iface
   *        address
   *        redirect_url
   *        with_group
   **/
  this.enter = function(options)
  {
    ResetPostString();

    MainMenu.select(MAINMENU_ID);
    var poststr = "";

    var arDevList = [];

    jQuery.each(DeviceList.devices, function(index,dev) {
      if (dev.interfaceName != "VirtualDevices") {
        arDevList.push({"address": dev.address, "interfaceName": dev.interfaceName, "name": dev.name});
      }
    });

    // Sort the table by name
    arDevList.sort(compare);

    poststr += "&deviceList=";
    jQuery.each(arDevList, function(index,dev) {
      poststr += dev.address + ' ' + dev.interfaceName + ' ';
    });

    updateContent(UI_PATH + 'ic_deviceFirmwareOverview.cgi', poststr);
  };

  this.leave = function()
  {
  };

}();