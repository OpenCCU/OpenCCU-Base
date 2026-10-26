/**
 * deviceconfigdialog.js
 **/

/**
 * Dialog für Kanaleinstellungen (Name, Räume, Gewerke, Funktionstest)
 **/ 
DeviceConfigDialog = Singleton.create({
  LAYER_ID: "DeviceConfigDialogLayer",
  NAME_ID: "DeviceConfigDialog_DeviceName",
  SEPARATOR_ID: "DeviceConfigDialog_DeviceChannelsRenameSeparator",
  RENAME_OWN_ID: "DeviceConfigDialog_renameChannelIncludingOwn",
  USABLE_ID: "DeviceConfigDialog_isUsable",
  VISIBLE_ID: "DeviceConfigDialog_isVisible",
  LOGGED_ID: "DeviceConfigDialog_isLogged",
  TEST_RESULT_ID: "DeviceConfigDialogTestResult",
  RESULT_OK: 1,
  RESULT_ABORT: 0,
  POLL_INTERVAL: 3,
  
  /**
   * Konstruktor
   **/
  initialize: function()
  {
    this.template = TrimPath.parseTemplate(DEVICE_CONFIG_DIALOG_JST);
  },
  
  /**
   * Zeigt den Konfigurationsdialog an
   **/
  show: function(device, callback)
  {
    this.m_testId = null;
    this.m_isTestRunning = false;
    
    this.device  = device;
    this.isVisibilityChanged = false;
    this.isUsabilityChanged = false;
    this.isLoggingChanged = false;   
    this.callback = callback;
    this.layer = document.createElement("div");
    this.layer.id = this.LAYER_ID;
    Layer.add(this.layer);
    this.layer.innerHTML = this.template.process({
      device: this.device
    });

    this.device.channels.each(function(channel) {
      if (! channel.isVisible) {
        jQuery("#trAllChnVisible").show();
        return false; //leave each loop
      }
    });

    if ((this.device.typeName.indexOf("Team") != -1) || (isNonCCUDevice(this.device))) {
      this.__hideFunctionTest();
    }
    if (isNonCCUDevice(this.device)) {
      this.__hideLogging();
    }
    translateJSTemplate("#DeviceConfigDialog");
	jQuery("#DeviceConfigDialogTestHintImg").attr('title', translateKey("generalDeviceConfigHintTooltip"));
	jQuery("#DeviceConfigDialogRenameChannelButtonHintImg").attr('title', translateKey("generalDeviceRenameChannelHintTooltip"));
	jQuery("#DeviceConfigDialogRenameIncludingOwnHintImg").attr('title', translateKey("generalDeviceRenameChannelIncludingOwnHint"));
  },

  __hideFunctionTest: function() {
    jQuery("#deviceFunctionTestPanel").hide();
  },

  __hideLogging: function() {
    jQuery("#btnEnableDeviceLogging").hide();
  },

  /**
   * Schließt den Konfigurationsdialog.
   **/
  close: function(result)
  {
    this.m_isTestRunning = false;
    Layer.remove(this.layer);
    if (this.callback) { this.callback(result); }
  },
  
  /**
   * Übernimmt die Änderungen und schließt den Dialog.
   **/
  ok: function()
  {
    if (isTextAllowed($(this.NAME_ID).value))
    {
      this.device.setName($(this.NAME_ID).value);

      var isVisible = $(this.VISIBLE_ID).checked;
      var isUsable  = $(this.USABLE_ID).checked;
      var isLogged  = $(this.LOGGED_ID).checked;
      var devIsVisible = "unknown";
      this.device.channels.each(function(channel) {
        if ((this.isVisibilityChanged) && (isVisible)) {
          channel.setVisibility(isVisible);
          if (devIsVisible == "unknown") {
            homematic("Device.setVisibility", {"id": this.device.id, "isVisible": true});
            devIsVisible = true;
          }
        }
        if (this.isUsabilityChanged) { channel.setUsability(isUsable); }
        if (this.isLoggingChanged) { channel.setLogging(isLogged); }
      }, this);

      this.close(this.RESULT_OK);
    }
  },

  /**
   * Schließt den Dialog ohne die Änderungen zu übernehmen.
   **/
  abort: function()
  {
    this.close(this.RESULT_ABORT);
  },
  
  /**
  * Benennt alle Channel des Device um
  **/

  ready: function()
  {
    this.ok();
  },
  
  renameChannels: function() {
    if (confirm(translateKey("generalDeviceRenameChannelsConfirm"))) {
      var renameOwnChannelNames = $(this.RENAME_OWN_ID).checked;

      this.device.channels.each(function(channel) {
        var chNumber = channel.address.split(':')[1];
        var origChName = this.device.name+":"+chNumber;

        // Channel bereits umbenannt und Option "eigene Namen umbenennen" wurde deaktiviert
        if (channel.name !== origChName && renameOwnChannelNames === false) return true;

        var chName = $(this.NAME_ID).value+ $(this.SEPARATOR_ID).value + chNumber;
        homematic("Channel.setName", {id: channel.id, name: chName});
        channel.setName(chName);
      }, this);
      this.ok();
    }
  },
  
  /**
   * Startet den Funktionstest.
   **/
  startTest: function() {
      var _this_ = this;
      this.m_isTestRunning = true;
      $(this.TEST_RESULT_ID).setStyle({backgroundColor: WebUI.getColor("testActive")});
      homematic("Device.startComTest", {id: this.device.id}, function (testId) {
        _this_.m_testId = testId;
        _this_.pollTest();
      });
  },
  
  /**
   * Callback. Fragt zyklisch das Ergebnis des Funktionstests ab.
   **/
  pollTest: function(timestamp)
  {
    var _this_ = DeviceConfigDialog;
    
    if (typeof(timestamp) == "string")
    {
      var time = timestamp.split(" ")[1];
      $(_this_.TEST_RESULT_ID).setStyle({backgroundColor: WebUI.getColor("testOk")});
      $(_this_.TEST_RESULT_ID).innerHTML = "";
      $(_this_.TEST_RESULT_ID).appendChild(document.createTextNode(time));
      _this_.m_isTestRunning = false;
    }
    
    if (_this_.m_isTestRunning)
    {
      homematic.delay(_this_.POLL_INTERVAL, "Device.pollComTest", {
        id    : _this_.device.id,
        testId: _this_.m_testId
      }, _this_.pollTest);
    }
  }
  
});
