/**
 * deletedevicedialog.js
 **/

hasDeviceInternalProgramOrSysvar  = function(device) {
  var result = {"id": 0, "address" : "", "type": "" };
  try {
    jQuery.each(device.channels, function(index, channel) {
      if (
        (channel.channelType == "POWERMETER")
        || (channel.channelType == "POWERMETER_IEC1")
        || (channel.channelType == "ENERGIE_METER_TRANSMITTER")
        || (channel.channelType == "WEATHER_TRANSMIT" /*HmIP-SWO*/)
        || (channel.channelType == "KEY_TRANSCEIVER") /*HnmIP-MOD-RC8*/
        || (channel.channelType == "FLOW_METER_TRANSMITTER") /*HmIP-WSM / ELV-SH-WSM*/
      ) {
        result.id = channel.id;
        result.address = channel.address;
        result.type = channel.channelType;
        return; // leave each loop
      }
    });
  } catch(e) {}

  return result;
};

deleteProgSysvarPOWERMETER = function(chId, chAddress, devLabel) {
  try {
    homematic("SysVar.deleteSysVarByName", {"name": "svEnergyCounter_" + chId + "_" + chAddress}, function () {
      homematic("SysVar.deleteSysVarByName", {"name": "svEnergyCounterOldVal_" + chId}, function () {
        homematic("SysVar.deleteSysVarByName", {"name": "svEnergyCounter_" + chId + "_" + chAddress + "_RESET"}, function () {
          homematic("SysVar.deleteSysVarByName", {"name": "svEnergyCounter_" + chId + "_" + chAddress + "_DEVICE_RESET"}, function () {
            homematic("SysVar.deleteSysVarByName", {"name": "svEnergyCounter_" + chId + "_" + chAddress + "_TMP_OLDVAL"}, function () {
              homematic("Program.deleteProgramByName", {"name": "prgEnergyCounter_" + chId + "_" + chAddress}, function () {
                homematic("Program.deleteProgramByName", {"name": "prgSetEnergyValuesAtMidnight" + chId}, function () {
                  if ((typeof devLabel == "undefined") || (devLabel != "hmip-psmco" && (devLabel != "elv-sh-psmci"))) {
                    conInfo(chAddress + " ProgSysvarPOWERMETER deleted - next: save ObjectModel");
                    window.setTimeout(function () {
                      saveObjectModel();
                    }, 5000);
                  } else {
                    deleteProgSysvarPOWERMETER_FeedIn(chId, chAddress);
                  }
                });
              });
            });
          });
        });
      });
    });
  } catch(e) {console.log(e);}
};

deleteProgSysvarPOWERMETER_FeedIn = function(chId, chAddress) {
  try {
    homematic("SysVar.deleteSysVarByName", {"name": "svEnergyCounterFeedIn_" + chId + "_" + chAddress}, function () {
      homematic("SysVar.deleteSysVarByName", {"name": "svEnergyCounterOldValFeedIn_" + chId}, function () {
        //homematic("SysVar.deleteSysVarByName", {"name": "svEnergyCounter_" + chId + "_" + chAddress + "_RESET"}, function () {
          //homematic("SysVar.deleteSysVarByName", {"name": "svEnergyCounter_" + chId + "_" + chAddress + "_DEVICE_RESET"}, function () {
            //homematic("SysVar.deleteSysVarByName", {"name": "svEnergyCounter_" + chId + "_" + chAddress + "_TMP_OLDVAL"}, function () {
            homematic("Program.deleteProgramByName", {"name": "prgEnergyCounterFeedIn_" + chId + "_" + chAddress}, function () {
              homematic("Program.deleteProgramByName", {"name": "prgSetEnergyValuesAtMidnightFeedIn" + chId}, function () {
                conInfo(chAddress + " ProgSysvarPOWERMETER + FeedIn deleted - next: save ObjectModel");
                window.setTimeout(function(){saveObjectModel();},5000);
              });
            });
            //});
          //});
        //});
      });
    });
  } catch(e) {console.log(e);}
};

deleteProgSysvarPOWERMETER_ESI = function(device) {
  try {
    jQuery.each(device.channels, function (index, chn) {
      if (chn.index >= 1) {
        homematic("Program.deleteProgramByName", {"name": "prgEnergyCounter_" + chn.id + "_" + chn.address});
        homematic("Program.deleteProgramByName", {"name": "prgGasCounter_" + chn.id + "_" + chn.address});
        homematic("Program.deleteProgramByName", {"name": "prgSetEnergyValuesAtMidnight" + chn.id});
        homematic("SysVar.deleteSysVarByName", {"name": "svEnergyCounter_" + chn.id + "_" + chn.address});
        homematic("SysVar.deleteSysVarByName", {"name": "svEnergyCounterOldVal_" + chn.id});
      }
    });
  } catch(e) {console.log(e);}

  conInfo(device.typeName + ": All relevant programs and sysvars deleted - next: save ObjectModel");
  window.setTimeout(function(){saveObjectModel();},5000);
};


deleteProgSysvarPOWERMETER_IGL = function(chId, chAddress) {
  try {
    homematic("SysVar.deleteSysVarByName", {"name": "svEnergyCounterGas_" + chId + "_" + chAddress}, function () {
      homematic("SysVar.deleteSysVarByName", {"name": "svEnergyCounterGasOldVal_" + chId}, function () {
        homematic("SysVar.deleteSysVarByName", {"name": "svEnergyCounterGas_" + chId + "_" + chAddress + "_RESET"}, function () {
          homematic("SysVar.deleteSysVarByName", {"name": "svEnergyCounterGas_" + chId + "_" + chAddress + "_DEVICE_RESET"}, function () {
            homematic("SysVar.deleteSysVarByName", {"name": "svEnergyCounterGas_" + chId + "_" + chAddress + "_TMP_OLDVAL"}, function () {
              homematic("Program.deleteProgramByName", {"name": "prgEnergyCounterGAS_" + chId + "_" + chAddress}, function () {
                conInfo(chAddress + " ProgSysvarPOWERMETER_IGL deleted - next: ProgSysvarPOWERMETER");
                deleteProgSysvarPOWERMETER(chId, chAddress);
              });
            });
          });
        });
      });
    });
  } catch(e) {console.log(e);}
};

deleteProgSysvarPOWERMETER_IEC = function(chId, chAddress) {
  try {
    homematic("SysVar.deleteSysVarByName", {"name": "svEnergyCounterIEC_" + chId + "_" + chAddress}, function () {
      homematic("SysVar.deleteSysVarByName", {"name": "svEnergyCounterIECOldVal_" + chId}, function () {
        homematic("Program.deleteProgramByName", {"name": "prgEnergyCounterIEC_" + chId + "_" + chAddress}, function () {
          conInfo(chAddress + " ProgSysvarPOWERMETER_IEC deleted - next: ProgSysvarPOWERMETER_IGL");
          deleteProgSysvarPOWERMETER_IGL(chId, chAddress);
        });
      });
    });
  } catch(e) {console.log(e);}
};

deleteProgSysvarFLOW_METER_TRANSMITTER = function(chId, chAddress) {
  try {
    homematic("SysVar.deleteSysVarByName", {"name": "svCounter_" + chId + "_" + chAddress}, function () {
      homematic("SysVar.deleteSysVarByName", {"name": "svCounterOldVal_" + chId}, function () {
        //homematic("SysVar.deleteSysVarByName", {"name": "svCounter_" + chId + "_" + chAddress + "_RESET"}, function () {
        //homematic("SysVar.deleteSysVarByName", {"name": "svCounter_" + chId + "_" + chAddress + "_DEVICE_RESET"}, function () {
        //homematic("SysVar.deleteSysVarByName", {"name": "svCounter_" + chId + "_" + chAddress + "_TMP_OLDVAL"}, function () {
        homematic("Program.deleteProgramByName", {"name": "prgFlowCounter_" + chId + "_" + chAddress}, function () {
          homematic("Program.deleteProgramByName", {"name": "prgSetCounterAtMidnight" + chId + "_" + chAddress}, function () {
            conInfo(chAddress + " ProgSysvarFLOW_METER_TRANSMITTER - next: save ObjectModel");
            window.setTimeout(function(){saveObjectModel();},5000);
          });
        });
        //});
        //});
        //});
      });
    });
  } catch(e) {console.log(e);}
};

deleteProgSysvarHmIPWeatherTransmitRainSunshine = function(chId, chAddress) {
  try {
    homematic("SysVar.deleteSysVarByName", {"name": "svHmIPRainCounter_" + chId + "_" + chAddress}, function () {
      homematic("SysVar.deleteSysVarByName", {"name": "svHmIPRainCounterOldVal_" + chId}, function () {
        homematic("SysVar.deleteSysVarByName", {"name": "svHmIPRainCounterYesterday_" + chId}, function () {
          homematic("SysVar.deleteSysVarByName", {"name": "svHmIPRainCounterToday_" + chId}, function () {
            homematic("Program.deleteProgramByName", {"name": "prgRainCounter_" + chId + "_" + chAddress}, function () {
              homematic("Program.deleteProgramByName", {"name": "prgDailySunshineRainCounter_" + chId}, function () {
                conInfo(chAddress + "prgDailySunshineRainCounter and prgHmIPRainCounter deleted - next: delete prgHmIPSunshineCounter");
                deleteProgSysvarHmIPWeatherTransmitSunshine(chId,chAddress);
              });
            });
          });
        });
      });
    });
  } catch(e) {console.log(e);}
};

deleteProgSysvarHmIPWeatherTransmitSunshine = function(chId, chAddress) {
  try {
    homematic("SysVar.deleteSysVarByName", {"name": "svHmIPSunshineCounter_" + chId + "_" + chAddress}, function () {
      homematic("SysVar.deleteSysVarByName", {"name": "svHmIPSunshineCounterOldVal_" + chId}, function () {
        homematic("SysVar.deleteSysVarByName", {"name": "svHmIPSunshineCounterYesterday_" + chId}, function () {
          homematic("SysVar.deleteSysVarByName", {"name": "svHmIPSunshineCounterToday_" + chId}, function () {
            homematic("Program.deleteProgramByName", {"name": "prgSunshineCounter_" + chId + "_" + chAddress}, function () {
              conInfo(chAddress + " prgHmIPSunshineCounter deleted - next: save ObjectModel");
              window.setTimeout(function () {
                saveObjectModel();
              }, 5000);
            });
          });
        });
      });
    });
  } catch(e) {console.log(e);}
};


/**
 * Ablauf:
 *   1) Prüfe, ob direkte Verknüpfungen oder Programme bestehen
 *   2) Anwender muss bestätigen, ob er das Gerät wirklich löschen möchte
 *      --> dabei wird angezeigt, on direkte Verknüpfungen oder Programme bestehen
 *      --> der Anwender wählt eine Löschoption:
 *          - nur ablernen
 *          - in Werkzustand zurücksetzen
 *   3) Falls das Gerät gelöscht werden soll, wird nun der Löschvorgang durchgeführt
 *   4) Falls der Löschvorgang fehlgeschlagen ist, kann der Anwender wählen:
 *      - erneute löschen
 *      - später automatisch löschen
 *      - löschen, auch wenn nicht erreichbar
 *      - abbrechen (nicht löschen)
 */

/**
 * "Bitte warten...", "Prüfe Programme und direkte Verknüpfungen..."
 **/
CheckLinksAndProgramsWindow = Class.create({

  initialize: function(device, callback)
  {
    var onResultHandler = this.m_onResult.bind(this);
    var screenWidth  = WebUI.getWidth();
    var screenHeight = WebUI.getHeight();
    var frameWidth   = CheckLinksAndProgramsWindow.CONTENT_WIDTH;
    var frameHeight  = CheckLinksAndProgramsWindow.CONTENT_HEIGHT;
    var frameX = parseInt((screenWidth  - frameWidth ) / 2);
    var frameY = parseInt((screenHeight - frameHeight) / 2);

    CheckLinksAndProgramsWindow.TITLE = translateKey("CheckLinksAndProgramsWindowTitle");
    CheckLinksAndProgramsWindow.CONTENT = "<img src='/ise/img/ajaxload_white.gif' alt='' style='float:left;margin-right:10px' />" + translateKey("CheckLinksAndProgramsWindowContent");

    this.m_device = device;
    this.m_callback = callback;
    
    this.m_layer = document.createElement("div");
    this.m_layer.className = "DialogLayer";
    
    this.m_frame = new UI.Frame()
      .setTitle(CheckLinksAndProgramsWindow.TITLE)
      .setContentSize(frameWidth, frameHeight)
      .setPosition(frameX, frameY)
      .add(new UI.Text()
        .setPosition(10, 10)
        .setWidth(frameWidth - 10)
        .setHtml(CheckLinksAndProgramsWindow.CONTENT)
      );
      
    this.m_layer.appendChild(this.m_frame.getElement());
    Layer.add(this.m_layer);
    
    this.m_device.hasLinksOrPrograms(onResultHandler);
  },
  
  m_onResult: function(result)
  {
    this.m_frame.dispose();
    Layer.remove(this.m_layer);
    
    if (this.m_callback) { this.m_callback(result); }
  }
  
});

CheckLinksAndProgramsWindow.CONTENT_WIDTH = 320;
CheckLinksAndProgramsWindow.CONTENT_HEIGHT = 60;
//CheckLinksAndProgramsWindow.TITLE = translateKey("CheckLinksAndProgramsWindowTitle");
//CheckLinksAndProgramsWindow.CONTENT = "<img src='/ise/img/ajaxload_white.gif' style='float:left;margin-right:10px' />" + translateKey("CheckLinksAndProgramsWindowContent");

/**
 * "Möchten Sie das Gerät wirklich löschen?"
 **/
ConfirmDeleteDeviceWindow = Class.create({

  initialize: function(device, hasLinksOrPrograms, callback)
  {
    var onDeleteHandler = this.m_onDelete.bind(this);
    var onAbortHandler  = this.m_onAbort.bind(this);
    var onChangeHandler = this.m_onChange.bind(this);
    var screenWidth    = WebUI.getWidth();
    var screenHeight   = WebUI.getHeight();
    var frameWidth     = ConfirmDeleteDeviceWindow.CONTENT_WIDTH;
    var frameHeight    = ConfirmDeleteDeviceWindow.CONTENT_HEIGHT;
    var frameX = parseInt((screenWidth  - frameWidth ) / 2);
    var frameY = parseInt((screenHeight - frameHeight) / 2);

    this.m_device = device;

    ConfirmDeleteDeviceWindow.TITLE = translateKey("ConfirmDeleteDeviceWindowTitle");
    ConfirmDeleteDeviceWindow.QUESTION = translateKey("ConfirmDeleteDeviceWindowQuestion");
    ConfirmDeleteDeviceWindow.DELETE_OPTIONS = translateKey("ConfirmDeleteDeviceWindowDeleteOptions");
    ConfirmDeleteDeviceWindow.REMOVE = translateKey("ConfirmDeleteDeviceWindowRemove");
    ConfirmDeleteDeviceWindow.REMOVE_DESCRIPTION = (! isNonCCUDevice(this.m_device)) ? translateKey("ConfirmDeleteDeviceWindowRemoveDescription") : translateKey("ConfirmDeleteDeviceWindowRemoveDescriptionNoneCCUDevice");
    ConfirmDeleteDeviceWindow.RESET = translateKey("ConfirmDeleteDeviceWindowReset");
    ConfirmDeleteDeviceWindow.RESET_DESCRIPTION = translateKey("ConfirmDeleteDeviceWindowResetDescription");
    ConfirmDeleteDeviceWindow.DELETE_BUTTON = translateKey("ConfirmDeleteDeviceWindowDeleteButton");
    ConfirmDeleteDeviceWindow.ABORT_BUTTON = translateKey("ConfirmDeleteDeviceWindowAbortButton");
    ConfirmDeleteDeviceWindow.WARNING = translateKey("ConfirmDeleteDeviceWindowWarning");

    this.hmIPIdentifier = "HmIP-RF";
    this.isNonCCUDevice = isNonCCUDevice(this.m_device);

    this.m_showRemoveOption = (this.m_device.interfaceName != this.hmIPIdentifier) ? true : false;
    this.m_hasLinksOrPrograms = hasLinksOrPrograms;
    this.m_callback = callback;
    
    this.m_layer = document.createElement("div");
    this.m_layer.className = "DialogLayer";

    this.m_lblDeleteOptions = new UI.Label()
            .setPosition(10, 70)
            .setText(ConfirmDeleteDeviceWindow.DELETE_OPTIONS);

    this.m_listbox = new UI.ListBox()
      .setPosition(30,90)
      .setWidth(frameWidth - 60)
      //.add({name: ConfirmDeleteDeviceWindow.REMOVE, description: ConfirmDeleteDeviceWindow.REMOVE_DESCRIPTION, flags: 0})
      //.add({name: ConfirmDeleteDeviceWindow.RESET, description: ConfirmDeleteDeviceWindow.RESET_DESCRIPTION, flags: 1})
      .setOnChangeCallback(onChangeHandler)
      .selectIndex(0);

    if (this.m_showRemoveOption) {
      this.m_listbox.add({name: ConfirmDeleteDeviceWindow.REMOVE, description: ConfirmDeleteDeviceWindow.REMOVE_DESCRIPTION, flags: 0});
    }

    // The deviceType doesn´t exist when in device inbox.
    // That means as long as a device is in the device inbox a factory reset isn´t possible when deleting the device.
    if(this.m_device.deviceType != null) {
      // The new group device has no factory reset
      if (this.m_device.deviceType.id != "HM-CC-VG-1") {
        this.m_listbox.add({name: ConfirmDeleteDeviceWindow.RESET, description: ConfirmDeleteDeviceWindow.RESET_DESCRIPTION, flags: 1});
      }
    } else {
      // HmIP devices will always be deleted with a factory reset
      if (this.m_device.interfaceName == this.hmIPIdentifier) {
        this.m_listbox.add({name: ConfirmDeleteDeviceWindow.RESET, description: ConfirmDeleteDeviceWindow.RESET_DESCRIPTION, flags: 1});
      }
    }

    this.m_description = new UI.Text()
      .setPosition(30, 120)
      .setWidth(frameWidth - 60)
      .setText(ConfirmDeleteDeviceWindow.REMOVE_DESCRIPTION);

    if (! this.m_showRemoveOption) {
      this.m_description.setText(ConfirmDeleteDeviceWindow.RESET_DESCRIPTION);
    }

    this.m_frame = new UI.Frame()
      .setTitle(ConfirmDeleteDeviceWindow.TITLE)
      .setContentSize(frameWidth, frameHeight)
      .setPosition(frameX, frameY)
      .add(new UI.Text()
        .setPosition(10, 10)
        .setWidth(60)
        .setHeight(60)
        .setHtml(this.m_device.thumbnailHTML)
      )
      .add(new UI.Text()
        .setPosition(75, 10)
        .setWidth(frameWidth - 85)
        .setHtml(ConfirmDeleteDeviceWindow.QUESTION.process({device: this.m_device}))
      )

      .add(this.m_lblDeleteOptions)

      .add(this.m_listbox)

      .add(this.m_description)
      .add(new UI.Button()
        .setPosition(frameWidth - 160, 200)
        .setText(ConfirmDeleteDeviceWindow.DELETE_BUTTON)
        .setAction(onDeleteHandler)
      )

      .add(new UI.Button()
        .setPosition(10,200)
        .setText(ConfirmDeleteDeviceWindow.ABORT_BUTTON)
        .setAction(onAbortHandler)
      );

    if (this.isNonCCUDevice) {
      this.m_frame.remove(this.m_lblDeleteOptions);
      this.m_frame.remove(this.m_listbox);
    }

    if (this.m_hasLinksOrPrograms !== false)
    {
      this.m_frame.add(new UI.Text()
        .setPosition(10,160)
        .setWidth(frameWidth - 10)
        .setHtml(ConfirmDeleteDeviceWindow.WARNING)
      );
    }
    
    this.m_layer.appendChild(this.m_frame.getElement());
    Layer.add(this.m_layer);
  },
  
  m_close: function(result)
  {
    this.m_frame.dispose();
    Layer.remove(this.m_layer);
    if (this.m_callback) { this.m_callback(result); }
  },
  
  m_onDelete: function()
  {
    this.m_close(this.m_listbox.getSelectedItem().flags); 
  },
  
  m_onAbort: function()
  {
    this.m_close(null);
  },
  
  m_onChange: function(item)
  {
    this.m_description.setText(item.description);
  }
});


ConfirmDeleteDeviceWindow.CONTENT_WIDTH = 400;
ConfirmDeleteDeviceWindow.CONTENT_HEIGHT = 230;
/*
ConfirmDeleteDeviceWindow.TITLE = translateKey("ConfirmDeleteDeviceWindowTitle");
ConfirmDeleteDeviceWindow.QUESTION = translateKey("ConfirmDeleteDeviceWindowQuestion");
ConfirmDeleteDeviceWindow.DELETE_OPTIONS = translateKey("ConfirmDeleteDeviceWindowDeleteOptions");
ConfirmDeleteDeviceWindow.REMOVE = translateKey("ConfirmDeleteDeviceWindowRemove");
ConfirmDeleteDeviceWindow.REMOVE_DESCRIPTION = translateKey("ConfirmDeleteDeviceWindowRemoveDescription");
ConfirmDeleteDeviceWindow.RESET = translateKey("ConfirmDeleteDeviceWindowReset");
ConfirmDeleteDeviceWindow.RESET_DESCRIPTION = translateKey("ConfirmDeleteDeviceWindowResetDescription");
ConfirmDeleteDeviceWindow.DELETE_BUTTON = translateKey("ConfirmDeleteDeviceWindowDeleteButton");
ConfirmDeleteDeviceWindow.ABORT_BUTTON = translateKey("ConfirmDeleteDeviceWindowAbortButton");
ConfirmDeleteDeviceWindow.WARNING = translateKey("ConfirmDeleteDeviceWindowWarning");
*/

/**
 * "Bitte warten", "Gerät wird gelöscht..."
 **/
DeleteDeviceWindow = Class.create({
  
  initialize: function(device, flags, callback)
  {
    var onResultHandler = this.m_onResult.bind(this);
    var screenWidth  = WebUI.getWidth();
    var screenHeight = WebUI.getHeight();
    var frameWidth   = DeleteDeviceWindow.CONTENT_WIDTH;
    var frameHeight  = DeleteDeviceWindow.CONTENT_HEIGHT;
    var frameX = parseInt((screenWidth  - frameWidth ) / 2);
    var frameY = parseInt((screenHeight - frameHeight) / 2);

    DeleteDeviceWindow.TITLE = translateKey("DeleteDeviceWindowTitle");
    DeleteDeviceWindow.CONTENT = "<img src='/ise/img/ajaxload_white.gif' alt='' style='float:left;margin-right:10px;'>"+translateKey("DeleteDeviceWindowContent");

    this.m_device   = device;
    this.m_flags    = flags;
    this.m_callback = callback;
    
    this.m_layer = document.createElement("div");
    this.m_layer.className = "DialogLayer";

    this.m_frame = new UI.Frame()
      .setTitle(DeleteDeviceWindow.TITLE)
      .setContentSize(frameWidth, frameHeight)
      .setPosition(frameX, frameY)
      .add(new UI.Text()
        .setPosition(10, 10)
        .setWidth(frameWidth - 10)
        .setHtml(DeleteDeviceWindow.CONTENT)
      );
      
    this.m_layer.appendChild(this.m_frame.getElement());
    Layer.add(this.m_layer);

    device.remove(this.m_flags, onResultHandler);

    if (flags == 5) {
      // Prüfen, ob ein Kanal des Gerätes ein HmIP-Wettersensor ist, als Energy-Counter oder ob das Gerät ein HmIP-MOD-RC8 ist dient.
      // Wenn ja, müssen beim Löschen die dazugehörigen Systemvariablen
      // sowie das enstprechende Systemprogramm zum aktualisieren der Systemvariablen entfernt werden.
      var oChnIdAndAddress = hasDeviceInternalProgramOrSysvar(this.m_device),
        chId = oChnIdAndAddress.id,
        chAddress= oChnIdAndAddress.address;
      if (chId != 0) {
        if (oChnIdAndAddress.type == "POWERMETER" || oChnIdAndAddress.type == "ENERGIE_METER_TRANSMITTER") {
          if (this.m_device.typeName.toLowerCase() != "hmip-esi") {
            deleteProgSysvarPOWERMETER(chId, chAddress, this.m_device.typeName.toLowerCase());
          } else {
            deleteProgSysvarPOWERMETER_ESI(this.m_device);
          }
        } else if (oChnIdAndAddress.type == "POWERMETER_IGL")  {
          deleteProgSysvarPOWERMETER_IGL(chId, chAddress);
        } else if (oChnIdAndAddress.type == "POWERMETER_IEC1") {
          deleteProgSysvarPOWERMETER_IEC(chId, chAddress);
        } else if ((this.m_device.typeName.toLowerCase() == "hmip-swo-b") && (oChnIdAndAddress.type == "WEATHER_TRANSMIT")) {
          deleteProgSysvarHmIPWeatherTransmitSunshine(chId, chAddress);
        } else if (((this.m_device.typeName.toLowerCase() == "hmip-swo-pl") || (this.m_device.typeName.toLowerCase() == "hmip-swo-pr"))  && (oChnIdAndAddress.type == "WEATHER_TRANSMIT"))  {
          deleteProgSysvarHmIPWeatherTransmitRainSunshine(chId, chAddress);
        } else if (oChnIdAndAddress.type == "FLOW_METER_TRANSMITTER") {
          deleteProgSysvarFLOW_METER_TRANSMITTER(chId, chAddress);
        } /*currently not in use else if (this.m_device.typeName.toLowerCase() == "hmip-mod-rc8") {
          this.m_deleteSysvarHmIPModRC8();
        }*/
      } else if ((typeof this.m_device.typeName != "undefined") && (this.m_device.typeName.toLowerCase() == "hmipw-drbl4")) {
        this.m_deleteMetaDataHmIPWBlind();
      }
    }

  },
  
  m_onResult: function(result, error)
  {
    var errorCode = DeleteDeviceDialog.ERROR_NO_ERROR;
    if (error !== null)
    {
      errorCode = error.code;
    }
    var self = this;

    if (isNonCCUDevice(this.m_device)) {
      window.setTimeout(function () {
        self.m_frame.dispose();
        Layer.remove(self.m_layer);

        ConfigData.reload( function() {
          ConfigData.handleReloadDone();
          WebUI.enter(DeviceListPage);
        } );

      }, 7000);
    } else {
      this.m_frame.dispose();
      Layer.remove(this.m_layer);
      if (this.m_callback) { this.m_callback(errorCode); }
    }
  }

});

DeleteDeviceWindow.CONTENT_WIDTH = 320;
DeleteDeviceWindow.CONTENT_HEIGHT = 60;
/*
DeleteDeviceWindow.TITLE = translateKey("DeleteDeviceWindowTitle");
DeleteDeviceWindow.CONTENT = "<img src='/ise/img/ajaxload_white.gif' style='float:left;margin-right:10px;'>"+translateKey("DeleteDeviceWindowContent");
*/

/**
 * "Fehler", "Gerät konnte nicht gelöscht werden"
 **/
ErrorOnDeleteWindow = Class.create({

  initialize: function(device, flags, errorCode, callback)
  {
    var onDeleteHandler = this.m_onDelete.bind(this);
    var onAbortHandler  = this.m_onAbort.bind(this);
    var onChangeHandler = this.m_onChange.bind(this);
    var screenWidth  = WebUI.getWidth();
    var screenHeight = WebUI.getHeight();
    var frameWidth   = ErrorOnDeleteWindow.CONTENT_WIDTH;
    var frameHeight  = ErrorOnDeleteWindow.CONTENT_HEIGHT;
    var frameX = parseInt((screenWidth  - frameWidth ) / 2);
    var frameY = parseInt((screenHeight - frameHeight) / 2); 

    ErrorOnDeleteWindow.TITLE = translateKey("ErrorOnDeleteWindowTitle");
    ErrorOnDeleteWindow.DELETE_OPTIONS = translateKey("ErrorOnDeleteWindowDeleteOptions");
    ErrorOnDeleteWindow.RETRY = translateKey("ErrorOnDeleteWindowRetry");
    ErrorOnDeleteWindow.RETRY_DESCRIPTION = translateKey("ErrorOnDeleteWindowRetryDescription");
    ErrorOnDeleteWindow.DEFER = translateKey("ErrorOnDeleteWindowDefer");
    ErrorOnDeleteWindow.DEFER_DESCRIPTION = translateKey("ErrorOnDeleteWindowDeferDescription");
    ErrorOnDeleteWindow.FORCE = translateKey("ErrorOnDeleteWindowForce");
    ErrorOnDeleteWindow.FORCE_DESCRIPTION = translateKey("ErrorOnDeleteWindowForceDescription");
    ErrorOnDeleteWindow.DEVICE_NOT_REACHABLE = translateKey("ErrorOnDeleteWindowDeviceNotReachable");
    ErrorOnDeleteWindow.UNKNOWN_DEVICE = translateKey("ErrorOnDeleteWindowUnknownDevice");
    ErrorOnDeleteWindow.UKNOWN_ERROR = translateKey("ErrorOnDeleteWindowUnknownError");
    ErrorOnDeleteWindow.ABORT_BUTTON = translateKey("ErrorOnDeleteWindowAbortButton");
    ErrorOnDeleteWindow.DELETE_BUTTON = translateKey("ErrorOnDeleteWindowDeleteButton");

    this.m_device = device;
    this.m_flags  = (flags & DeleteDeviceDialog.FLAG_RESET);
    this.m_errorCode = errorCode;
    this.m_callback = callback;

    this.hmIPIdentifier = "HmIP-RF";

    
    var errorMessage;
    switch(errorCode)
    {
      case DeleteDeviceDialog.ERROR_GENERAL:
        errorMessage = ErrorOnDeleteWindow.DEVICE_NOT_REACHABLE.process({device: this.m_device});
        break;
      case DeleteDeviceDialog.ERROR_UNKNOWN_DEVICE:
        errorMessage = ErrorOnDeleteWindow.UNKNOWN_DEVICE.process({device: this.m_device});
        break;
      default:
        errorMessage = ErrorOnDeleteWindow.UKNOWN_ERROR.process({device: this.m_device});
        break;
    }  
    
    this.m_layer = document.createElement("div");
    this.m_layer.className = "DialogLayer";

    this.m_listbox = new UI.ListBox()
      .setPosition(30, 160)
      .setWidth(frameWidth - 60)
      .add({name: ErrorOnDeleteWindow.RETRY, description: ErrorOnDeleteWindow.RETRY_DESCRIPTION, flags: 0})
      .add({name: ErrorOnDeleteWindow.DEFER, description: ErrorOnDeleteWindow.DEFER_DESCRIPTION, flags: 4})
      .add({name: ErrorOnDeleteWindow.FORCE, description: ErrorOnDeleteWindow.FORCE_DESCRIPTION, flags: 2})
      .setOnChangeCallback(onChangeHandler)
      .selectIndex(0);

    this.m_description = new UI.Text()
      .setPosition(30,190)
      .setWidth(frameWidth - 60)
      .setText(ErrorOnDeleteWindow.RETRY_DESCRIPTION);
    
    this.m_frame = new UI.Frame()
      .setTitle(ErrorOnDeleteWindow.TITLE)
      .setContentSize(frameWidth, frameHeight)
      .setPosition(frameX, frameY)
      .add(new UI.Text()
        .setPosition(10, 10)
        .setWidth(60)
        .setHeight(60)
        .setHtml(this.m_device.thumbnailHTML)
      )
      .add(new UI.Text()
        .setPosition(75, 10)
        .setWidth(frameWidth - 85)
        .setHtml(errorMessage)
      )
      .add(new UI.Label()
        .setPosition(10, 140)
        .setText(ErrorOnDeleteWindow.DELETE_OPTIONS)
      )
      .add(this.m_listbox)
      .add(this.m_description)
      .add(new UI.Button()
        .setPosition(10, 230)
        .setText(ErrorOnDeleteWindow.ABORT_BUTTON)
        .setAction(onAbortHandler)
      )
      .add(new UI.Button()
        .setPosition(frameWidth - 170, 230)
        .setWidth(150)
        .setText(ErrorOnDeleteWindow.DELETE_BUTTON)
        .setAction(onDeleteHandler)
      );
    
    this.m_layer.appendChild(this.m_frame.getElement());
    Layer.add(this.m_layer);
  },
  
  m_close: function(result)
  {
    if (this.m_frame != undefined) {
      this.m_frame.dispose();
    }

    Layer.remove(this.m_layer);

    if (result == null && this.m_flags == 1 && this.m_errorCode == 507) {
      // Prüfen, ob ein Kanal des Gerätes ein HmIP-Wettersensor ist, als Energy-Counter oder ob das Gerät ein HmIP-MOD-RC8 ist dient.
      // Wenn ja, müssen beim Löschen die dazugehörigen Systemvariablen
      // sowie das enstprechende Systemprogramm zum aktualisieren der Systemvariablen entfernt werden.
      var oChnIdAndAddress = hasDeviceInternalProgramOrSysvar(this.m_device),
        chId = oChnIdAndAddress.id,
        chAddress= oChnIdAndAddress.address;
      if (chId != 0) {
        if (oChnIdAndAddress.type == "POWERMETER" || oChnIdAndAddress.type == "ENERGIE_METER_TRANSMITTER") {
          if (this.m_device.typeName.toLowerCase() != "hmip-esi") {
            deleteProgSysvarPOWERMETER(chId, chAddress, this.m_device.typeName.toLowerCase());
          } else {
            deleteProgSysvarPOWERMETER_ESI(this.m_device);
          }
        } else if (oChnIdAndAddress.type == "POWERMETER_IGL")  {
          deleteProgSysvarPOWERMETER_IGL(chId, chAddress);
        } else if (oChnIdAndAddress.type == "POWERMETER_IEC1") {
          deleteProgSysvarPOWERMETER_IEC(chId, chAddress);
        } else if ((this.m_device.typeName.toLowerCase() == "hmip-swo-b") && (oChnIdAndAddress.type == "WEATHER_TRANSMIT")) {
          deleteProgSysvarHmIPWeatherTransmitSunshine(chId, chAddress);
        } else if (((this.m_device.typeName.toLowerCase() == "hmip-swo-pl") || (this.m_device.typeName.toLowerCase() == "hmip-swo-pr"))  && (oChnIdAndAddress.type == "WEATHER_TRANSMIT"))  {
          deleteProgSysvarHmIPWeatherTransmitRainSunshine(chId, chAddress);
        } else if (oChnIdAndAddress.type == "FLOW_METER_TRANSMITTER") {
          deleteProgSysvarFLOW_METER_TRANSMITTER(chId, chAddress);
        } /*currently not in use else if (this.m_device.typeName.toLowerCase() == "hmip-mod-rc8") {
          this.m_deleteSysvarHmIPModRC8();
        }*/
      } else if ((typeof this.m_device.typeName != "undefined") && (this.m_device.typeName.toLowerCase() == "hmipw-drbl4")) {
        this.m_deleteMetaDataHmIPWBlind();
      }
    }

    if (this.m_callback) { this.m_callback(result); }
  },
  
  m_onDelete: function()
  {
    this.m_close(this.m_flags + this.m_listbox.getSelectedItem().flags);
  },
  
  m_onAbort: function()
  {
    this.m_close(null);
  },
  
  m_onChange: function(item)
  {
    this.m_description.setText(item.description);
  }
  
});

ErrorOnDeleteWindow.CONTENT_WIDTH = 400;
ErrorOnDeleteWindow.CONTENT_HEIGHT = 260;

/**
 * Dialogbox zum Löschen eines Geräts
 **/
DeleteDeviceDialog = Class.create({

  /**
   * Erstellt einen neuen DeleteDeviceDialog.
   **/
  initialize: function(device, callback)
  {
    this.m_device   = device;
    this.m_callback = callback;
    this.m_onLinksAndProgramsCheckedHandler = this.m_onLinksAndProgramsChecked.bind(this);
    this.m_onDeleteDeviceConfirmedHandler   = this.m_onDeleteDeviceConfirmed.bind(this);
    this.m_onDeviceDeletedHandler           = this.m_onDeviceDeleted.bind(this);

    this.hmIPIdentifier = "HmIP-RF";


    new CheckLinksAndProgramsWindow(device, this.m_onLinksAndProgramsCheckedHandler);
    
  },

  m_onLinksAndProgramsChecked: function(result)
  {
    this.m_hasLinksOrPrograms = result;
    new ConfirmDeleteDeviceWindow(this.m_device, this.m_hasLinksOrPrograms, this.m_onDeleteDeviceConfirmedHandler);
  },
  
  m_onDeleteDeviceConfirmed: function(flags)
  {
    if (flags !== null)
    {
      this.m_flags = flags;
      new DeleteDeviceWindow(this.m_device, this.m_flags, this.m_onDeviceDeletedHandler);
    }
    else
    {
      // Abbruch
      if (this.m_callback) { this.m_callback(false); }
    }
  },

  m_deleteSysvarHmIPModRC8: function() {
    var numberOfKeyTransceiver = 8;

    // delete the appropriate sysvar
    jQuery.each(this.m_device.channels, function(index, chn) {
      if (chn.channelType == "KEY_TRANSCEIVER") {
        homematic("SysVar.deleteSysVarByName", {"name": "svHmIPModRC8ChnMode_" + chn.id + "_" + chn.address}, function () {
          conInfo("SysVar svHmIPModRC8ChnMode_" + chn.id + "_" + chn.address + " deleted");
          if (index === numberOfKeyTransceiver) {
            window.setTimeout(function(){saveObjectModel();},5000);
          }
        });
      }
    });
  },

  m_deleteMetaDataHmIPWBlind: function() {
    jQuery.each(this.m_device.channels, function(index, chn) {
      if (chn.channelType == "BLIND_VIRTUAL_RECEIVER") {
        homematic("Interface.removeMetadata", {"objectId": chn.address, "dataId" : "channelMode"}, function (result) {
          // console.log("m_deleteMetaDataHmIPWBlind", result);
        });

      }
    });
  },

  m_onDeviceDeleted: function(errorCode)
  {
    //if ((errorCode === DeleteDeviceDialog.ERROR_NO_ERROR) || (errorCode === DeleteDeviceDialog.HmIP_CONFIG_PENDING) || (errorCode === DeleteDeviceDialog.ERROR_UNKNOWN_DEVICE))
    if ((errorCode === DeleteDeviceDialog.ERROR_NO_ERROR) || (errorCode === DeleteDeviceDialog.ERROR_UNKNOWN_DEVICE))
    {
      // Prüfen, ob ein Kanal des Gerätes ein HmIP-Wettersensor ist, als Energy-Counter oder ob das Gerät ein HmIP-MOD-RC8 ist dient.
      // Wenn ja, müssen beim Löschen die dazugehörigen Systemvariablen
      // sowie das enstprechende Systemprogramm zum aktualisieren der Systemvariablen entfernt werden.
      var oChnIdAndAddress = hasDeviceInternalProgramOrSysvar(this.m_device),
        chId = oChnIdAndAddress.id,
        chAddress= oChnIdAndAddress.address;
      if (chId != 0) {
        if (oChnIdAndAddress.type == "POWERMETER" || oChnIdAndAddress.type == "ENERGIE_METER_TRANSMITTER") {
          if (this.m_device.typeName.toLowerCase() != "hmip-esi") {
            deleteProgSysvarPOWERMETER(chId, chAddress, this.m_device.typeName.toLowerCase());
          } else {
            deleteProgSysvarPOWERMETER_ESI(this.m_device);
          }
        } else if (oChnIdAndAddress.type == "POWERMETER_IGL")  {
          deleteProgSysvarPOWERMETER_IGL(chId, chAddress);
        } else if (oChnIdAndAddress.type == "POWERMETER_IEC1") {
          deleteProgSysvarPOWERMETER_IEC(chId, chAddress);
        } else if ((this.m_device.typeName.toLowerCase() == "hmip-swo-b") && (oChnIdAndAddress.type == "WEATHER_TRANSMIT")) {
          deleteProgSysvarHmIPWeatherTransmitSunshine(chId, chAddress);
        } else if (((this.m_device.typeName.toLowerCase() == "hmip-swo-pl") || (this.m_device.typeName.toLowerCase() == "hmip-swo-pr"))  && (oChnIdAndAddress.type == "WEATHER_TRANSMIT"))  {
          deleteProgSysvarHmIPWeatherTransmitRainSunshine(chId, chAddress);
        }  else if (oChnIdAndAddress.type == "FLOW_METER_TRANSMITTER") {
          deleteProgSysvarFLOW_METER_TRANSMITTER(chId, chAddress);
        } /*currently not in use else if (this.m_device.typeName.toLowerCase() == "hmip-mod-rc8") {
          this.m_deleteSysvarHmIPModRC8();
        }*/
      } else if ((typeof this.m_device.typeName != "undefined") && (this.m_device.typeName.toLowerCase() == "hmipw-drbl4")) {
        this.m_deleteMetaDataHmIPWBlind();
      }
      if (this.m_callback) { this.m_callback(true); }
    }
    else
    {
      this.m_errorCode = errorCode;
      if (this.m_flags <= 1) {
        //if (! (this.m_errorCode == DeleteDeviceDialog.ERROR_UNKNOWN_DEVICE && this.hmIPIdentifier == this.m_device.interfaceName)) {
        if (! (this.m_errorCode == DeleteDeviceDialog.ERROR_UNKNOWN_DEVICE) ) {
          new ErrorOnDeleteWindow(this.m_device, this.m_flags, this.m_errorCode, this.m_onDeleteDeviceConfirmedHandler);
        }
      }
    }
  }
});

DeleteDeviceDialog.FLAG_RESET = 0x01;
DeleteDeviceDialog.FLAG_FORCE = 0x02;
DeleteDeviceDialog.FLAG_DEFER = 0x04;
DeleteDeviceDialog.ERROR_NO_ERROR = 0;
DeleteDeviceDialog.ERROR_GENERAL = 501;
DeleteDeviceDialog.ERROR_UNKNOWN_DEVICE = 502;
DeleteDeviceDialog.HmIP_CONFIG_PENDING = 507;

