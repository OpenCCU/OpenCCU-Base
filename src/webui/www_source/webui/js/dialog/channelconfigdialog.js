/**
 * channelconfig.js
 **/

/**
 * Dialog für Kanaleinstellungen (Name, Räume, Gewerke, Funktionstest)
 **/ 
ChannelConfigDialog = Singleton.create({
  PLUS_IMAGE_SRC: "/ise/img/plus.png",
  MINUS_IMAGE_SRC: "/ise/img/minus.png",
  LAYER_ID: "ChannelConfigDialogLayer",
  NAME_ID: "ChannelConfigDialog_ChannelName",
  USABLE_ID: "ChannelConfigDialog_isUsable",
  VISIBLE_ID: "ChannelConfigDialog_isVisible",
  LOGGED_ID: "ChannelConfigDialog_isLogged",
  MODE_ID: "ChannelConfigDialog_Mode",
  ROOMLIST_ID: "ChannelConfigDialogRooms",
  SUBSECTIONLIST_ID: "ChannelConfigDialogFuncs",
  TEST_RESULT_ID: "ChannelConfigDialogTestResult",
  RESULT_ABORT: 0,
  RESULT_OK: 1,  
  POLL_INTERVAL: 3,
  
  /**
   * Konstruktor
   **/
  initialize: function()
  {
    this.template = TrimPath.parseTemplate(CHANNEL_CONFIG_DIALOG_JST);
  },
  
  /**
   * Zeigt den Konfigurationsdialog an
   **/
  show: function(channel, callback)
  {
    this.m_testId = null;
    this.m_isTestRunning = false;
    
    this.isRoomListVisible = false;
    this.isSubsectionListVisible = false;
    this.channel  = channel;
    this.callback = callback;
    this.layer = document.createElement("div");
    this.layer.id = this.LAYER_ID;
    Layer.add(this.layer);
    this.layer.innerHTML = this.template.process({
      channel: this.channel,
      isRoomListVisible: this.isRoomListVisible,
      isSubsectionListVisible: this.isSubsectionListVisible,
      rooms: RoomList.list().ex_sortBy("name"),
      funcs: SubsectionList.list().ex_sortBy("name")
    });

    if (this.channel.typeName.indexOf("Team") != -1) {
       this.__hideFunctionTest();
    }

    if (isNonCCUDevice(this.channel)) {
      this.__hideLogging();
    }
    if (this.channel.deviceType.id == "HmIPW-DRAP") {
      this.__hideRoomFunctionSelection();
    }
    translateJSTemplate("#ChannelConfigDialog");
    translatePage("#ChannelConfigDialogRooms, #ChannelConfigDialogFuncs");
    jQuery("#generalChannelConfigLblSender").val(translateKey("generalChannelConfigLblSender"));
    jQuery("#generalChannelConfigLblReceiver").val(translateKey("generalChannelConfigLblReceiver"));
    jQuery("#generalChannelConfigLblNone").val(translateKey("generalChannelConfigLblNone"));
  },

  __hideFunctionTest: function() {
    jQuery("#channelFunctionTestPanel").hide();
  },

  __hideLogging: function() {
    jQuery("#btnEnableChannelLogging").hide();
  },

  __hideRoomFunctionSelection: function() {
    jQuery("#ChannelConfigDialogSectionRoom, #ChannelConfigDialogSectionFunc").hide();
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
      var channel = this.channel,
        arRooms = [], arSubSection = [];
    
      channel.setName($(this.NAME_ID).value);
      channel.setVisibility($(this.VISIBLE_ID).checked);
      if ($(this.VISIBLE_ID).checked) {
        iseDevices.setVisible(channel.id, "id", true);
      }
      channel.setUsability($(this.USABLE_ID).checked);
      channel.setLogging($(this.LOGGED_ID).checked);
      channel.setMode($(this.MODE_ID).options[$(this.MODE_ID).options.selectedIndex].value);

      arRooms = $A($(this.ROOMLIST_ID));

      // SPHM-1153
      jQuery(arRooms).each(function(index,_room) {
        var room = RoomList.get(_room.value);
        if (jQuery(_room).prop("checked")) {
          room.addChannel(channel.id, true);
        } else {
          room.removeChannel(channel.id, true);
        }
      });

      arSubSection = $A($(this.SUBSECTIONLIST_ID));

      // SPHM-1153
      jQuery(arSubSection).each(function(index,_subSection) {
        var subsection = SubsectionList.get(_subSection.value);
        if (jQuery(_subSection).prop("checked")) {
          subsection.addChannel(channel.id, true);
        } else {
          subsection.removeChannel(channel.id, true);
        }
      });
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
   * Startet den Funktionstest.
   **/
  startTest: function()
  {
    var _this_ = this;
    
    this.m_isTestRunning = true;
    $(this.TEST_RESULT_ID).setStyle({backgroundColor: WebUI.getColor("testActive")});
    homematic("Channel.startComTest", {id: this.channel.id}, function(testId) {
      _this_.m_testId = testId;
      _this_.pollTest();
    });
  },

  showConfigCentralLinkHelp: function() {
    var title = translateKey("dialogHelpCentralLinkTitle"),
    content = translateKey("dialogHelpCentralLinkContent");
    MessageBox.show(title, content,'', 475, 450);
  },

  /**
   * Returns the interface that reports the button events of the channel
   * to the CCU (BidCos-RF for KEY, HmIP-RF for KEY_TRANSCEIVER and
   * MULTI_MODE_INPUT_TRANSMITTER), or null if the channel has none.
   **/
  getCentralLinkInterface: function(channel)
  {
    var iface = channel.device ? channel.device.interfaceName : null;
    if ((channel.channelType == "KEY_TRANSCEIVER" || channel.channelType == "MULTI_MODE_INPUT_TRANSMITTER") && iface == "HmIP-RF") { return iface; }
    if (channel.channelType == "KEY" && iface == "BidCos-RF") { return iface; }
    return null;
  },

  /**
   * Sets the reference counter of the given values and reports the result
   * once all requests have finished.
   **/
  setCentralLink: function(valueIds, refCounter, successKey)
  {
    var iface = this.getCentralLinkInterface(this.channel),
      addr = this.channel.address,
      pending = valueIds.length,
      failed = false;
    if (!iface) { return; }
    var done = function(result, error) {
      if (error || !result) { failed = true; }
      pending--;
      if (pending === 0) {
        MessageBox.show(translateKey('dialogHint'), translateKey(failed ? 'dialogCentralLinkError' : successKey).replace('%s', addr), '', 400, 160);
      }
    };
    jQuery.each(valueIds, function(i, valueId) {
      homematic("Interface.reportValueUsage", {"interface": iface, "address": addr, "valueId": valueId, "refCounter": refCounter}, done);
    });
  },

  createCentralLink: function()
  {
    var _this_ = this;
    new YesNoDialog(translateKey('dialogSafetyCheck'), translateKey('dialogQuestionAddLink').replace('%s', this.channel.address+' <> CCU'), function(result) {
      if (result == YesNoDialog.RESULT_YES) {
        _this_.setCentralLink(["PRESS_SHORT"], "1", "dialogCentralLinkActivated");
      }
    }, "html");
  },
  
  removeCentralLink: function()
  {
    var _this_ = this;
    new YesNoDialog(translateKey('dialogSafetyCheck'), translateKey('dialogQuestionRemoveLink').replace('%s', this.channel.address+' <> CCU'), function(result) {
      if (result == YesNoDialog.RESULT_YES) {
        _this_.setCentralLink(["PRESS_SHORT", "PRESS_LONG"], "0", "dialogCentralLinkDeactivated");
      }
    }, "html");
  },

  /**
   * Callback. Fragt zyklisch das Ergebnis des Funktionstests ab.
   **/
  pollTest: function(timestamp)
  {
    var _this_ = ChannelConfigDialog;
    
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
      homematic.delay(_this_.POLL_INTERVAL, "Channel.pollComTest", {
        id    : _this_.channel.id,
        testId: _this_.m_testId
      }, _this_.pollTest);
    }
  },
  
  /**
   * Blendet die Raumliste ein bzw. aus.
   **/
  toggleRooms: function(image)
  {
    if (this.isRoomListVisible === true)
    {
      $(this.ROOMLIST_ID).hide();
      image.src = this.PLUS_IMAGE_SRC;
      this.isRoomListVisible = false;
    }
    else
    {
      $(this.ROOMLIST_ID).show();
      image.src = this.MINUS_IMAGE_SRC;
      this.isRoomListVisible = true;
    }
  },
  
  /**
   * Blendet die Gewerkeliste ein bzw. aus.
   **/
  toggleFuncs: function(image)
  {
    if (this.isSubsectionListVisible === true)
    {
      $(this.SUBSECTIONLIST_ID).hide();
      image.src = this.PLUS_IMAGE_SRC;
      this.isSubsectionListVisible = false;
    }
    else
    {
      $(this.SUBSECTIONLIST_ID).show();
      image.src = this.MINUS_IMAGE_SRC;
      this.isSubsectionListVisible = true;
    }
  }
});
