/**
 * StartPage.js
 **/

var preURL = (WEBUI_VERSION.split(".")[0] < 3) ? "" : "ccu3-";

StartPage = Singleton.create(Page, {
  MAINMENU_ID: "MAINMENU_STARTPAGE",
  downloadURLServer: "https://"+preURL+"update.homematic.com/firmware/download?cmd=download",
  downloadURL : "",
  prevDownloadURL : "",
  devList: [],
  devIndex: 0,
  newFwCounter: 0,
  messageBoxHTML: "",
  fetchDeviceList: false,
  knownTypes: [],
  numberOfKnownTypes: 0,

  /**
   * Konstrutor
   **/
  initialize: function ()
  {
    this.deviceList = [];

    switch (getUPL())
    {
      case UPL_ADMIN:
      case UPL_USER:
        this.resize = this.resizeUser;
        break;
      case UPL_GUEST:
        this.rezise = this.resizeGuest;
        break;
      default:
        break;
    }
  },

  /**
   * Callback. Wird beim betreten der Seite aufgerufen.
   **/
  enter: function(options)
  {
    iseInitUpdateArrays();
    setPath("");
    setFooter("");
    WebUI.resize();
  
    switch (getUPL())
    {
      case UPL_ADMIN:
      case UPL_USER:
        MainMenu.select(this.MAINMENU_ID);
        loadStartPage(options);
        break;
      case UPL_GUEST:
        loadStartPageGuest(options);
        break;
      default:
        break;
    }

    this.serial = homematic("CCU.getSerial");

    this.downloadURL = this.downloadURLServer;

    if (this.prevDownloadURL != this.downloadURL) {
      this.devList = [];
      this.devIndex = 0;
      this.newFwCounter = 0;
      this.fetchDeviceList = false;
      this.knownTypes = [];
      this.numberOfKnownTypes = 0;
    }
    this.prevDownloadURL = this.downloadURL;
    conInfo("Device Fw. downloadURL: " + this.downloadURL);
  },
  
  /**
   * Berechnet die Größe der normalen Startseite (Admin und Benutzer)
   **/
  resizeUser: function()
  {
    var contentHeight     = $("content").getHeight();
    var contentWidth      = $("content").getWidth();
    var width = parseInt(contentWidth / 2);
    
    if ($("contentLeft"))
    {
      //Element.setStyle("contentLeft", {"height": contentHeight + "px", "width": width + "px"});
      Element.setStyle("contentLeft", {"height": contentHeight + "px", "width": "55%"});
    }
    if ($("contentLeft"))
    {
      //Element.setStyle("contentRight", {"height": contentHeight + "px", "width": (width - 10) + "px"});
      Element.setStyle("contentRight", {"height": contentHeight + "px", "width": "44%"});
    }
    
    if ($("favSelector"))
    {   
      var FavSelectorHeight = $("favSelector").getHeight();
      var FAV_MARGIN_HEIGHT = 8;          
      var favViewHeight = (contentHeight - FavSelectorHeight - FAV_MARGIN_HEIGHT) * 0.95;
      $("favView").setStyle({"height": favViewHeight});
    }
  },

  getPageMeasurement: function() {
    var pageElements = ["body","#header", "#menubar", "#favSelector", "#favView", "#footer"],
      elementDim = [];
    jQuery.each(pageElements, function(index, val) {
      var elm = jQuery(val);
      elementDim[val.replace(/^#/,"")] = {
        "height"  : elm.height(),
        "width"   : elm.width()
      };
    });
    return elementDim;
  },

   // WebUI-Version
  showCurrentFirmware: function() {
    //jQuery("#currentFirmware").text(WEBUI_VERSION);

    homematic("Interface.getDeviceDescription", {"interface": "BidCos-RF", "address": "BidCoS-RF"}, function(result) {
      if (result != null) {
        WEBUI_VERSION = result.firmware;
      } else {
        WEBUI_VERSION = "0.0.0";
      }
      jQuery("#currentFirmware").text(WEBUI_VERSION);
    });
  },

  /*evalVersionAGreaterThanB: function(a, b) {
    var aSplit = a.split(".", 3);
    var bSplit = b.split(".", 3);
    if( Array.isArray(aSplit) && aSplit.length == 3) {
        if(Array.isArray(bSplit) && bSplit.length == 3) {
            //major
            var aI = parseInt(aSplit[0]);
            var bI = parseInt(bSplit[0]);
            if( aI > bI ) { return true; }
            if( aI < bI ) { return false; }
            //major equal, cmp minor
            aI = parseInt(aSplit[1]);
            bI = parseInt(bSplit[1]);
            if( aI > bI ) { return true; }
            if( aI < bI ) { return false; }
            //minor equal, check patch
            aI = parseInt(aSplit[2]);
            bI = parseInt(bSplit[2]);
            if( aI > bI) { return true; }
            if( aI <= bI ) { return false; }
        } else { return true; }
    } else { return false; }
    return false;
  },*/

  cmpVersion: function (aa, bb) {
    // 0   Version equal 
    // >0  Version aa newer 
    // <0  Version bb newer  
    const aArr = aa.split(".");
    const bArr = bb.split(".");
    const max = Math.max(aArr.length, bArr.length);
    for (let index = 0; index < max; index++) {
        const a = Number(aArr[index] || 0);
        const b = Number(bArr[index] || 0);
        if (a != b)
            return (a - b);
    }
    return 0;
  },

  showAllDeviceFirmware: function() {
    var self = this;
    if (!this.fetchDeviceList) {
      this.fetchDeviceList = true; // prevents this to be called multiple times (when clicking the start page)
      this.messageBoxHTML = "";
      this.DeviceUpdateList = {};
      this.newFwCounter = 0;
      this.devIndex = 0;
      if (this.devList.length == 0) {
        homematic("Interface.listDevices", {"interface": "BidCos-RF"}, function (deviceList) {
          conInfo("Fetch RF device list");
          if (deviceList) {
            for (var i = 0; i < deviceList.length; i++) {
              var device = deviceList[i];
              if (device.children && device.type != "HM-RCV-50") {
                self.devList.push({"type": device.type, "firmware": device.firmware, "availableFirmware": device.availableFirmware, "updatable": (device.updatable == "1") ? true : false, "address": device.address});
              }
            }
          }
          homematic("Interface.listDevices", {"interface": "BidCos-Wired"}, function (deviceList) {
            conInfo("Fetch Wired device list");
            if (deviceList) {
              for (var i = 0; i < deviceList.length; i++) {
                var device = deviceList[i];
                if (device.children && device.type != "HMW-RCV-50") {
                  self.devList.push({"type": device.type, "firmware": device.firmware, "availableFirmware": device.availableFirmware, "updatable": (device.updatable == "1") ? true : false, "address": device.address});
                }
              }
            }
            homematic("Interface.listDevices", {"interface": "HmIP-RF"}, function (deviceList) {
              conInfo("Fetch HmIP device list");
              if (deviceList) {
                for (var i = 0; i < deviceList.length; i++) {
                  var device = deviceList[i];
                  if (device.children.length > 0 && (device.type != "HmIP-RCV-50")) {
                    self.devList.push({"type": device.type, "firmware": device.firmware, "availableFirmware": device.availableFirmware, "updatable": (device.updatable == "1") ? true : false, "address": device.address});
                    conInfo("hmipdev: "+device.type+" fw:"+device.firmware+" availFw: "+device.availableFirmware);
                  }
                }
              }
              conInfo("Check firmware version of all devices - number of devices: " + self.devList.length);
              if (self.devList.length > 0) {
                function sortorder_devList(a , b) {
                  if (a.type == b.type) {
                    return (a.address < b.address) ? -1 : 1;
                  } else {
                    return (a.type < b.type) ? -1 : 1;
                  }
                }

                self.devList.sort(sortorder_devList);
                self.fetchAndSetDeviceVersion();
              }
            });
          });
        });
      } else {
        self.fetchAndSetDeviceVersion();
      }
    }
  },

  fetchAndSetDeviceVersion: function() {
    var self = this;
    if (this.numberOfKnownTypes == 0) {
      conInfo("Fetch device firmware list from server");
      homematic.com.getListOfAvailableFirmware(function (result) {
        conInfo("List fetched");
        jQuery.each(result, function (index, value) {
          var type = value.type.toLowerCase();
          type = type.replace(/_(?!.*_)/, ' '); //SPHM-1039
          if(value.type=="HmIP-HAP-JS1") {//SPHM-1034 HAP JS1 has different pattern than other devices.
            type = "HmIP-HAP JS1".toLowerCase();
          }
          self.knownTypes[type] = value.version;
          self.numberOfKnownTypes = index + 1;
          if(value.type == "HmIP-HAP") { //SPHM-1022
              self.knownTypes["HmIP-HAP-B1".toLowerCase()] = value.version;
              self.numberOfKnownTypes += 1;
          }
        });
        self.setDeviceVersion(self.devList[self.devIndex].type, self.knownTypes[self.devList[self.devIndex].type.toLowerCase()]);
      });
    } else {
      conInfo("Don't fetch the device firmware list from the server. Use the known list.");
      self.setDeviceVersion(self.devList[self.devIndex].type, self.knownTypes[self.devList[self.devIndex].type.toLowerCase()]);
    }
  },

  setDeviceVersion: function (deviceType, fwVersion) {
    var self = this;
    var deviceTypeForUrl = deviceType.replace(/\ (?!.*\ )/, '_'); //SPHM-1039;
    if(deviceType == "HmIP-HAP JS1") {//SPHM-1034 HAP JS1 has different pattern than other devices.
      deviceTypeForUrl = "HmIP-HAP-JS1";
    }
    if(deviceType == "HmIP-HAP-B1") { //SPHM-1022 Use HAP Fw for HAP-B1
      deviceTypeForUrl = "HmIP-HAP";
    }
    if (fwVersion) {
      var devAddress = self.devList[self.devIndex].address,
        curFw = self.devList[self.devIndex].firmware,
        arCurFw = curFw.split("."),
        availableFW = self.devList[self.devIndex].availableFirmware,// auf der CCU gespeicherte Geräte-Fw.
        devIsUpdatable = self.devList[self.devIndex].updatable,
        arResultMajorMinorPatch,
        resultMajorMinor,
        newFW = fwVersion;

      // The current fw for BidCos devices has no patch version
      if (arCurFw.length == 2) {
        arResultMajorMinorPatch = fwVersion.split(".");
        resultMajorMinor = arResultMajorMinorPatch[0] + "." + arResultMajorMinorPatch[1];
        newFW = resultMajorMinor;
      }

      // generate RefUpdateList with detailed Information per DeviceType for grouped list
      if (devIsUpdatable && fwVersion && (fwVersion != "n/a") && (newFW != curFw)) {
        if (!this.DeviceUpdateList[deviceType]) {
          this.DeviceUpdateList[deviceType] = {};
        }
        var RefUpdateList = this.DeviceUpdateList[deviceType][curFw];
        if (!RefUpdateList) {
          this.DeviceUpdateList[deviceType][curFw] = { DevCnt: 0, FWAvailable: 0, FWUpload: 0, Address: [], downloadURL: this.downloadURL, deviceTypeForUrl: deviceTypeForUrl, fwVersion: fwVersion, curVersion: curFw, availableFirmware: availableFW };
          RefUpdateList = this.DeviceUpdateList[deviceType][curFw];
        }
        RefUpdateList.DevCnt++;
        RefUpdateList.Address.push(self.devList[self.devIndex].address);
        // FW not yet available on the CCU
        if (availableFW != newFW) {
          RefUpdateList.FWUpload++;
        } else {
          RefUpdateList.FWAvailable++;
        }
        self.newFwCounter++;
      }
    }
    self.devIndex++;
    if (self.devIndex < self.devList.length) {
      self.fetchAndSetDeviceVersion();
    } else {
      for (const Typ of Object.keys(this.DeviceUpdateList).sort()) {
        for (const curFw of Object.keys(this.DeviceUpdateList[Typ]).sort(this.cmpVersion)) {
          let Obj = this.DeviceUpdateList[Typ][curFw];
          let curVersion = Obj.curVersion;
          let color = "black";
          if (this.cmpVersion(Obj.curVersion, Obj.fwVersion) > 0) {
            curVersion = "<b>" + Obj.curVersion + "</b>";
            color = "orange";
          }
          this.messageBoxHTML += "<tr>" +
                                   "<td style='text-align:left;vertical-align:top;' height='15px'>" +
                                     Typ +
                                   "</td>" +
                                   "<td style='text-align:center;vertical-align:top;' class='UILink' " +
                                     "onClick='toggleSerialNumberDisplay()'>" +
                                     Obj.DevCnt +
                                   "</td>" +
                                   "<td style='text-align:left;vertical-align:top;display:none;'>" +
                                     Obj.Address.join('<br/>') +
                                   "</td>" +
                                   "<td style='text-align:left;vertical-align:top;color:" + color + "'>" +
                                     curVersion +
                                   "</td>" +
                                   "<td style='text-align:left;vertical-align:top;' class='UILink' " +
                                     ((Obj.FWUpload > 0) ?
                                       "onClick=\"window.location.href='" +
                                         Obj.downloadURL +
                                         "&serial=0&product=" + Obj.deviceTypeForUrl + "'\">" :
                                       "onClick=\"alert(translateKey('hintDevFwAlreadyUploaded'));\">") +
                                         Obj.fwVersion +
                                   "</td>" +
                                   "</tr>";
        }
      }
      self.fetchDeviceList = false;
      self.deleteScriptElements();
      if (self.newFwCounter > 0) {
        self.showHintForAvailableDeviceFirmware();
      }
      conInfo("All devices checked. " + self.newFwCounter + " actualized firmware versions found!");
    }
  },

  showHintForAvailableDeviceFirmware: function() {
    var self = this,
    rowShowDevFirmware = jQuery("#devFwAvailable"),
    btnShowDevFirmware = jQuery("#devFwAvailable .firmwareAvailable");
    btnShowDevFirmware.click(function () {
      self.showAvailableDeviceFirmware();
    });
    jQuery("#devFwAvailableCounter").text("(" + self.newFwCounter + ")");
    rowShowDevFirmware.show();
  },

  deleteScriptElements: function() {
    jQuery("#homematic_com_script").remove();
    jQuery("#homematic_com_script_fw").remove();
  },

  showAvailableDeviceFirmware: function() {
    var groupCounter = 0;
    for (const Typ of Object.keys(this.DeviceUpdateList)) {
      groupCounter += Object.keys(this.DeviceUpdateList[Typ]).length;
    }
    var quoteOpen = (getLang() == "de") ? "&bdquo;" : "&ldquo;",
    quoteClose = (getLang() == "de") ? "&ldquo;" : "&rdquo;";
    var contentHeight = jQuery("#content").innerHeight(),
    maxMessageBoxHeight = (groupCounter + 5) * 20,
    msgBoxHeight = (maxMessageBoxHeight < contentHeight) ? maxMessageBoxHeight : contentHeight;

    MessageBox.show(translateKey("dialogShowDeviceFirmwareTitle"),
    "<table id='fwUpdates' style='width:100%;'>"+
      "<colgroup>" +
      "<col style='width:40%;'>" +
      "<col style='width:19%;'>" +
      "<col style='display:none;'>" +
      "<col style='width:19%;'>" +
      "<col style='width:19%;'>" +
      "</colgroup>" +
      "<th style='text-align:left;vertical-align:top;'>"+translateKey('dialogShowDeviceFirmwareTHDevice')+"</th>"+
      "<th style='text-align:center;vertical-align:top;'>"+translateKey('lblQuantity')+"</th>"+
      "<th style='text-align:left;vertical-align:top;display:none;'>"+translateKey('dialogShowDeviceFirmwareTHSerials')+"</th>"+
      "<th style='text-align:left;vertical-align:top;'>"+translateKey('dialogShowDeviceFirmwareTHCurFw')+"</th>"+
      "<th style='text-align:left;vertical-align:top;'>"+translateKey('dialogShowDeviceFirmwareTHNewFw')+"</th>"+
      this.messageBoxHTML +
      "<tr><td colspan='5' align='left'><br/><b>"+translateKey('dialogShowDeviceFirmwareDescription')+":</b><br/>" +
      translateKey('dialogShowDeviceFirmwareDownloadInfo1') + "<br/>" +
      translateKey('dialogShowDeviceFirmwareDownloadInfo2') + "&nbsp;" +
        quoteOpen + "<div class='UILink' style='display: inline;' onclick='MessageBox.close();WebUI.enter(DeviceFirmware);'>"+translateKey('submenuDeviceFirmware')+"</div>" +
        " => " + translateKey('footerBtnNew') + quoteClose + "&nbsp;" +
        translateKey('dialogShowDeviceFirmwareDownloadInfo3') +
      "</td></tr>" +
      //"<tr><td colspan='3' align='center'><a href='http://www.eq-3.de/downloads.html' target='_blank'>"+translateKey('dialogShowDeviceFirmwareLinkDownload')+"</a></td></tr>" +
    "</table>",
    "",
    450,
    msgBoxHeight
    );
  },
  
  showAddonUpdate: function() {
    homematic('CCU.getAddonVersions', {}, function(addonversions) {
      var addonUpdateList = "";
      if (addonversions && addonversions !== "false") {
        var localaddonversions = addonversions.local;
        var onlineaddonversions = addonversions.online;
        for(var l_obj in localaddonversions) {
          if (typeof localaddonversions[l_obj] === "object") {
        
             for(var o_obj in onlineaddonversions) {
               if (typeof onlineaddonversions[o_obj] === "object") {
             
                 if (onlineaddonversions[o_obj].name == localaddonversions[l_obj].name) {
                    //console.log(onlineaddonversions[o_obj].name +" local: "+localaddonversions[l_obj].localversion+", online: "+onlineaddonversions[o_obj].webversion);
                    if (localaddonversions[l_obj].localversion != onlineaddonversions[o_obj].webversion && onlineaddonversions[o_obj].webversion != '' && onlineaddonversions[o_obj].webversion != 'n/a') {
                      addonUpdateList += "<br/>&#183; "+String(onlineaddonversions[o_obj].name).escapeHTML()+" ("+String(onlineaddonversions[o_obj].webversion).escapeHTML()+")";
                    }
                 }
             
               }
             }
          }
        }
      }
    
      if (addonUpdateList !== "") {
        var self = this,
        rowShowAddonUpdates = jQuery("#addonUpdatesAvailable"),
        btnShowAddonUpdates = jQuery("#addonUpdatesAvailable .firmwareAvailable");
        btnShowAddonUpdates.unbind("click").click(function () {
          showSoftwareCP();
        });
        jQuery("#addonUpdatesAvailableList").html(addonUpdateList);
        rowShowAddonUpdates.show();
      }
    });
  },
  

  showUpdate: function() {
    var fn = function() {
      var result = homematic.com.isUpdateAvailable();
      if ((result) && ($("updateCol"))) {
        var updateText = " (" + homematic.com.getLatestVersion() + " " + translateKey('isAvailable') + ")";
        if (getUPL() == UPL_ADMIN) {
        jQuery("#updateCol").text(updateText)
          .addClass("firmwareAvailable")
          .unbind("click").bind("click", function() {
            showNewFirmwareDownload();
          });
        } else {
          jQuery("#updateCol").text(updateText);
        }
        jQuery("#updateCol").show();
      }
      return result;
    };

    if (!fn()) {
      window.setTimeout(fn, 5000);
    }
  },
  
  /**
   * Berechnet die Größe der Startseite für den Gast
   **/
  resizeGuest: function() { }   
  
  
});
