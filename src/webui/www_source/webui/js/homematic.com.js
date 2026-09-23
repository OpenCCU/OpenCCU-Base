
/**
 * homematic.com.js
 * Schnittstelle zu HomeMatic.com
 **/
 
homematic.com = 
{
  m_latestVersion: translateKey("lblAvailableFirmwareVersionNotKnown"),
  m_isUpdateAvailable: false,
  m_latestDeviceFw : "",
  m_callback: null,
  m_URLServer: "",

  m_product: "",
  init: function()
  {
    this.m_ccuProduct = getProduct();
    this.preURL = (this.m_ccuProduct < 3) ? "" : "ccu3-";
    this.m_product ="HM-CCU" + this.m_ccuProduct;
    this.m_URLServer = "https://"+this.preURL+"update.homematic.com";
    this.m_fieldTestURLServer = "https://fieldtest-ccu3-update.homematic.com";

    this.serial = homematic("CCU.getSerial");
    this.serial = ((this.serial != "") && (typeof this.serial != "undefined") && (this.serial != null)) ? this.serial : "0";

    // The server should return a string like "homematic.com.setLatestVersion('2.4.212');"
    var script = document.createElement("script");
    script.id = "homematic_com_script";
    script.type = "text/javascript";
    script.src = this.m_URLServer + "/firmware/download?cmd=js_check_version&version="+WEBUI_VERSION+"&product="+this.m_product+"&serial=" + this.serial;
    $("body").appendChild(script);
  },

  // For testing only
  _init: function()
  {
    availableVersion = "unknown";
    peLoop = 0;
    // window.setTimout is only for testing the real server delay. In production this has of course to be removed.
    window.setTimeout(function(){
      // The server should return a string like "availableVersion=2.6.0.1;"
      var script = document.createElement("script");
      script.id = "homematic_com_script";
      script.type = "text/javascript";
      script.src = "version.js"; // rega/pages/version.js - has to be in one of these days the correct server url
      $('body').appendChild(script);
    }, 5000);

    new PeriodicalExecuter(function(pe) {
      conInfo.log("check available Version!");
      conInfo.log("correct version: " +availableVersion.match(/^[0-9]+.[0-9]+.[0-9]+/));
      if (availableVersion != "unknown") {
        homematic.com.setLatestVersion(availableVersion);
        StartPage.showUpdate();
        pe.stop();
      }
      // Try 30 seconds (10 * 3) to get the available version.
      // After that stop the polling.
      if (peLoop > 9) {
        pe.stop();
      }
      peLoop++;
    }, 3);
  },

  isUpdateAvailable: function()
  {
    return homematic.com.m_isUpdateAvailable;
  },
  
  getLatestVersion: function()
  {
    return homematic.com.m_latestVersion;
  }, 
  

  /* Not in use anymore - instead we now use getListOfAvailableFirmware */
  getAvailableDeviceFirmware: function(product, index, callback) {
      // The server should return a string like "homematic.com.setLatestVersion('1.2.2', 'product');"
      var script = document.createElement("script");
      script.id = "homematic_com_script_" + index;
      script.type = "text/javascript";
      script.src =  this.m_URLServer + "/firmware/download?cmd=js_check_version&product=" + product + "&serial=0";
      $("body").appendChild(script);
      homematic.com.callback = callback;
  },

  getListOfAvailableFirmware: function(callback) {
      var fieldTestActive = "/etc/config/fieldTestActive";

      // The server should return a string like "homematic.com.setDeviceFirmwareVersions([{"type":"HM-MOD-Re-8","version":"1.0.0"},{"type":"HM-MOD-Re-8","version":"1.0.0"}])"
      var script = document.createElement("script");
      script.id = "homematic_com_script_fw";
      script.type = "text/javascript";
      // script.src =  this.m_URLServer + "/firmware/api/firmware/search/DEVICE";

      if (homematic('CCU.existsFile', {'file': fieldTestActive})) {
        script.src = this.m_fieldTestURLServer + "/firmware/api/firmware/search/DEVICE?product=HM-CCU"+getProduct()+"&version="+WEBUI_VERSION+"&serial=" + this.serial;
      } else {
        script.src = this.m_URLServer + "/firmware/api/firmware/search/DEVICE?product=HM-CCU" + getProduct() + "&version=" + WEBUI_VERSION;
      }
      $("body").appendChild(script);
      homematic.com.callback = callback;
  },


  showCCULicense: function(callback) {
    var barGraphTimeout = 60000;

    MessageBox.show(
    translateKey('dlgLoadLicense'),
    ' <br/><br/><img id="msgBoxBarGraph" src="/ise/img/anim_bargraph.gif" alt=""><br/>',
    '','320','75','fwUpload', 'msgBoxBarGraph');

    timeoutBargraph = window.setTimeout(function() {
      // Hide the messagebox after 'barGraphTimeout' seconds without a response from this.m_URLServer
      MessageBox.close();
      HideWaitAnim();
      jQuery("#homematic_license_script").remove();
      // Show a error message
      MessageBox.show(translateKey("dialogTitleHomeMaticInfo"), translateKey("dlgErrorLoadLicense"), '', '320', '75');
    },barGraphTimeout);

    var lang = getLang();
    if (lang != "de" && lang != "en") {lang = getDefaultLang();}

    var script = document.createElement("script");
    script.id = "homematic_license_script";
    script.type = "text/javascript";
    script.src = this.m_URLServer + "/firmware/download?cmd=release_note&product="+this.m_product+"&serial=3014&version="+homematic.com.m_latestVersion+"&locale=" + lang;

    $("body").appendChild(script);
    homematic.com.callback = callback;
  },

  // For testing only
  showCCUDummyLicense: function(callback) {
    var lang = getLang();
    if (lang != "de" && lang != "en") {lang = getDefaultLang();}
    var result = "<b>TEST TEXT</b><br /><p>Das ist die Lizenz</p><p>Sprache: "+lang+"</p>";
    homematic.com.callback = callback;
    window.setTimeout(function() {homematic.com.getCCU2LicenceText(result);}, 2000);
  },

  /**
   * wird von homematic.com zurück geliefert
   **/
  setLatestVersion: function(latestVersion, product)
  {
    if (product == this.m_product) {
      homematic.com.m_latestVersion = latestVersion;
      homematic.com.m_isUpdateAvailable = (WEBUI_VERSION != latestVersion);
    } else {
      if (homematic.com.callback != null) {
        homematic.com.callback(latestVersion, product);
      }
    }
  },

  // wird von homematic.com zurück geliefert
  setDeviceFirmwareVersions: function(result) {
    homematic.com.callback(result);
  },

  // wird von homematic.com zurück geliefert
  getCCU2LicenceText: function(result) {
    homematic.com.callback(result);
  }
};
