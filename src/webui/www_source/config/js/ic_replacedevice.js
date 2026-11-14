ReplaceDevice = new function()
{
 var  performReplaceDevice = function(iface, oDevices) {
    this.oDevices = oDevices;
    ProgressBar = new ProgressBarMsgBox("swapDeviceInProgress", 1);
    ProgressBar.show();
    ProgressBar.StartKnightRiderLight();

    var result = homematic("Interface.changeDevice" , {
      "interface" : iface,
      "addressOldDevice" : oDevices.oldDevAddr,
      "addressNewDevice" : oDevices.newDevAddr
    }, function() {
      window.setTimeout(
        function() {
          var self = this;
          ProgressBar.hide();
          ProgressBar.StopKnightRiderLight();
          ConfigData.destroy();
          ConfigData.isPresent = true;
          ConfigData.check(function() {
            WebUI.enter(DeviceListPage);
            MessageBox.show(translateKey("replaceDeviceHintTitle"), translateKey("replaceDeviceHintContent"));

            // Both devices - old and new - are using the same regaID and the same parameters.
            // So we can fetch the the new or the old device to check if one of the channels is of the type POWERMETER or POWERMETER_IGL
            var regaID = homematic("Device.getReGaIDByAddress", {"address": oDevices.newDevAddr}),
            checkDevice = homematic("Device.get", {"id": regaID});

            // Check if the channel type is POWERMETER or POWERMETER_IGL
            // If so than delete the old system variable and create a new one with the new device address.
            // Set the value of the new created system var to the value of the old one.
            if (checkDevice) {
              jQuery.each(checkDevice.channels, function (index, channel) {
                var channelType = GetChannelType(channel);
                if (channelType == "POWERMETER" || channelType == "POWERMETER_IGL" ) {

                  var sysVarPrefix = (channelType == "POWERMETER") ? "svEnergyCounter_" : "svEnergyCounterGas_";

                  var oldSysVarID = sysVarPrefix + channel.id + "_" + self.oDevices.oldDevAddr + ":" + (parseInt(index) + 1),
                  newSysVarID = sysVarPrefix + channel.id + "_" + channel.address;

                  // Fetch the value of the old system variable
                  var ok = homematic("SysVar.getValueByName", {"name": oldSysVarID}, function(oldVal) {
                    conInfo("Value of the old ccu value: " + oldVal);

                    // Create a system variable for the new device and set the value to the one of the old system var
                    homematic("SysVar.createFloat", {"name": newSysVarID, "minValue": 0, "maxValue": (1.7 * Math.pow(10, 308)) - 1, "internal": 1}, function () {
                      homematic("SysVar.setFloat", {"name": newSysVarID, "value": oldVal}, function () {
                        conInfo("Set the value of the new sys var to " + oldVal);
                      });
                    });
                    // Delete the old system variable
                    homematic("SysVar.deleteSysVarByName", {"name": oldSysVarID}, function () {
                      conInfo(oldSysVarID + " deleted");
                      homematic("system.saveObjectModel", {}, function () {
                        conInfo("ObjectModel saved");
                      });
                    });
                  });
                }
              });
            }
          });
        }, 2000);
    });
  };

 var getDlgContent = function(oDevices) {
    var contentHeader = translateKey("replaceDeviceDlgConfirmHeader");
    var contentDevices = oDevices.newDevType + " : " + oDevices.newDevAddr + " " + translateKey("replaceDeviceDlgConfirmContentRow1a") + " " + oDevices.oldDevType + " " + translateKey("replaceDeviceDlgConfirmContentRow1b") + " " + oDevices.oldDevAddr + translateKey("replaceDeviceDlgConfirmContentRow1c");
    contentDevices += (oDevices.oldDevType == oDevices.newDevType) ? translateKey("replaceDeviceDlgConfirmContentRow2a") : translateKey("replaceDeviceDlgConfirmContentRow2b");
    contentDevices += translateKey("replaceDeviceDlgConfirmContentRow3a") + " " + oDevices.oldDevAddr + " " + translateKey("replaceDeviceDlgConfirmContentRow3b");
    var contentFooter = translateKey("replaceDeviceDlgConfirmFooter");

    return contentHeader + contentDevices + contentFooter;
  };

  /* Public */
  this.perform = function(iface, oDevices ) {
    /*
      oDevices has to contain the following keys:
        oldDevAddr
        oldDevType
        newDevAddr
        newDevType
        newDevID
     */
    var title = translateKey("replaceDeviceDlgConfirmTitle");

    new YesNoDialog(title, getDlgContent(oDevices), function(action){
      /*
        action can be
        0 - user selected NO
        1 - user selected YES
       */
      if (action == 1) {
        performReplaceDevice(iface, oDevices);
      }
    }, "html");
  };
}();

