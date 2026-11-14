isePowerMeterESI = Class.create();

isePowerMeterESI.prototype = {

  initialize: function(id, opts) {
    //console.log(opts);
    var self = this;

    this.iface = "HmIP-RF";
    this.id = id;
    this.opts = opts;

    this.powerMeter = "POWERMETER"; // e. g. Hm-ES-PMSw1-PL-DN-R1
    this.POWERMETER_IEC = "POWERMETER_IEC"; // e. g. Hm-ES-TX-WM

    this.energyMeterTransmitter = "ENERGIE_METER_TRANSMITTER"; // e. g. HmIP-PSM
    this.sensorTypeID = {};
    this.sensorTypeID.gas = "Gas";
    this.sensorTypeID.eletricity = "Electricity";
    this.sensorTypeID.iec = "IEC";
    this.sensorTypeID.unknown = "Unknown";
    this.kiloPrefix = "";

    this.idBtnSelfCalibration = this.opts.idBtnSelfCalibration;

    this.chn = parseInt(this.opts.chn);

    this.summedUpEnergy = 0.0;

    this.arSensorTypes = [
      "SENSOR_UNKNOWN",
      "SENSOR_ES_GAS",
      "SENSOR_ES_LED",
      "SENSOR_ES_IEC",
      "SENSOR_ES_IEC_SML",
      "SENSOR_ES_IEC_SML_WH",
      "SENSOR_ES_IEC_D0_A",
      "SENSOR_ES_IEC_D0_B",
      "SENSOR_ES_IEC_D0_C",
      "SENSOR_ES_IEC_D0_D"
    ];

    this.measurementType = this.getSensorType(); // Connected Sensor

    if (this.chn == 1) {
      this.sensor =  homematic("Interface.getMetadata", {"objectId": this.id, "dataId": "sensor"});
      console.log("this.measurementType: " + this.measurementType ,"this.sensor: " + this.sensor);

      this.showPowerGasFlowPanel();

      if (this.measurementType != this.sensor) {
        console.log("set MetaData sensor: " + this.measurementType);
        homematic("Interface.setMetadata", {"objectId": this.id, "dataId": "sensor", "value": this.measurementType});
      }

    }

    this.EnergyPrice = "unknown";

    this.bindEvents();

    jQuery("[name = '" + this.id + "EnergyCostDeviceUnit']").html(this.opts.unitEnergyCounter).show();
    this.setEnergyCounterPanel();

    this.energyConfig = homematic("system.getEnergyPrice", {}, function (result) {
      self.energyConfig = result;
      // When the energy price config is available and it´s a known sensor (electricity or gas)
      // then show the energy cost
      if (result && (self.measurementType != self.sensorTypeID.unknown)) {
        self.showEnergyCost();
      }
    });
    jQuery("[name = '" + this.id + "sensor" + this.getSensorExtension() + "']").show();
    conInfo("Interface: " + this.opts.iface);
    conInfo("MeasurementType: " + this.measurementType);

  },


  showPowerGasFlowPanel: function() {
    var panelElm = jQuery("[name='curConsumptionPanel_"+this.id+"']"),
      noSensorElm = jQuery("#noSensorAvailable_" + this.id),
      sensorGas = jQuery("#gasFlow_" + this.id),
      sensorElec = jQuery("#power_" + this.id);

    // console.log("showPowerGasFlowPanel", "this.measurementType: " + this.measurementType);

    if ((typeof this.measurementType == "undefined") || (this.measurementType == null) ) {
      panelElm.hide();
      noSensorElm.show();
      return;
    }

    switch (this.measurementType) {
      case this.arSensorTypes[0]:  // SENSOR_UNKNOWN
        panelElm.hide();
        noSensorElm.show();
        break;
      case this.arSensorTypes[1]: // SENSOR_ES_GAS
        noSensorElm.hide();
        panelElm.show();
        sensorElec.hide();
        sensorGas.show();
        break;
      default:
        noSensorElm.hide(); // SENSOR_ES_LED and all SENSOR_ES_IEC sensors (e. g. SENSOR_ES_IEC_SML)
        panelElm.show();
        sensorGas.hide();
        sensorElec.show();
    }
  },

  getSensorType: function() {
    var paramSet = homematic("Interface.getParamset", {"interface": this.opts.iface, "address" : this.opts.chAddress, "paramsetKey" :"MASTER"});

    // With the HmIP-ESI only channel 1 has the parameter CHANNEL_OPERATION_MODE
    return (typeof paramSet.CHANNEL_OPERATION_MODE == "undefined") ? -1 : this.arSensorTypes[parseInt(paramSet.CHANNEL_OPERATION_MODE)];
  },

  buttonPressed: function(btn) {
    var elem = jQuery(btn);
    elem.addClass("ControlBtnOn").removeClass("ControlBtnOff");
    setTimeout(function() {
      elem.addClass('ControlBtnOff').removeClass('ControlBtnOn');
    }, 500);

  },

  bindEvents: function() {
    var self = this;

    jQuery("#"+ this.id + "startSelfCalibration").bind("click", function() {
      // self.searchSensor(self.idBtnSelfCalibration); for setDpState
      self.searchSensor(self.iface, self.opts.chAddress);
    });


    jQuery("#"+ this.id + "resetEnergyCounter").bind("click", function() {
      conInfo("Set EnergyCounter");
      var dlgContent = "<table align='center'><tr><td>"+translateKey('lblEnergyConsumptionInWatt')+"<td><td><input type='text' id='meterReading'></td></tr></table>",
        valMeterReading;

      setEnergyCounterDlg = new YesNoDialog(translateKey("lblSetEnergyCounter"), dlgContent, function(result) {
        if (result == YesNoDialog.RESULT_YES) {
          valMeterReading = parseFloat(jQuery("#meterReading").val());

          // This removes the dialog from the screen
          Layer.remove(this.m_layer);

          if (! isNaN(valMeterReading)) {
            // This sets the new value of the power meter
            homematic("SysVar.setFloat", {"name": self.EnergyCounterID, "value": valMeterReading}, function () {
              self.setEnergyCounterPanel();
              self.showEnergyCost();
            });
          }
        } else {
          // NO pressed
          // This removes the dialog from the screen
          Layer.remove(this.m_layer);
        }
      }, "html");

      setEnergyCounterDlg.btnTextNo(translateKey("dialogBack"));
      setEnergyCounterDlg.btnTextYes(translateKey("btnOk"));

      // Overwrites the original close method
      setEnergyCounterDlg.close = function(result) {
        if (this.m_callback) { this.m_callback(result); }
      };

    });
  },

  /**
   * Returns the jQuery element of a given parameter
   * @param {string} elem The name of the desired element
   * @return {object} The desired element
   */
  getJElemByID: function(elem) {
    return jQuery("#" + this.id + elem);
  },

  getJElemsByName: function(elem) {
    return jQuery("[name='" + this.id + elem + "']");
  },

  getSensorExtension: function() {
    switch (this.measurementType) {
      case  this.sensorTypeID.gas:
        return this.sensorTypeID.gas;
      case this.sensorTypeID.iec:
        return this.sensorTypeID.iec;
      default: return "";
    }
  },

  changeToKilo: function() {
    return (this.summedUpEnergy/1000);
  },

  setEnergyCounterPanel: function() {
    var j_panel = this.getJElemByID("EnergyCounter");
    // For a Hm-ES-TX-WM (POWERMETER_IEC) with a firmware >= 2.0.0 show 4 decimal places
    var decimalPlace = ((this.opts.chType.indexOf(this.POWERMETER_IEC) != -1) && (parseInt(this.opts.devFirmwareMajor) >= 2))  ? 4 : 2;
    this.summedUpEnergy = parseFloat(homematic("SysVar.getValue", {"id" : this.EnergyCounterID})).toFixed(decimalPlace);
    conInfo("setEnergyCounterPanel - this.summedUpEnergy: " + this.summedUpEnergy);

    this.kiloPrefix = "";
    if (this.summedUpEnergy >= 1000.0 && (this.measurementType != this.sensorTypeID.gas) && (this.measurementType != this.sensorTypeID.iec)) {
      this.kiloPrefix = "k";
      this.summedUpEnergy = this.changeToKilo();
    }

    j_panel.html(((this.kiloPrefix == "k") ? this.summedUpEnergy.toFixed(2) : this.summedUpEnergy) + " " + this.kiloPrefix + this.opts.unitEnergyCounter);
  },

  getEnergyConsumption: function(sType) {
    // Electricity sensor
    if ((this.measurementType == this.sensorTypeID.eletricity )) {
      if (sType == "CCU")  return this.summedUpEnergy;
      if (sType == "Device") return this.opts.valEnergyCounter;
    }

    // Gas sensor
    if (this.measurementType == this.sensorTypeID.gas) {
      if (sType == "CCU")  return (this.summedUpEnergy * this.energyConfig.gasHeatingValue * this.energyConfig.gasConditionNumber) ;
      if (sType == "Device") return (this.opts.valEnergyCounterGas * this.energyConfig.gasHeatingValue * this.energyConfig.gasConditionNumber);
    }

    // IEC sensor
    if ((this.measurementType == this.sensorTypeID.iec)) {
      if (sType == "CCU")  return this.summedUpEnergy;
      if (sType == "Device") return this.opts["valEnergyCounter" + this.getSensorExtension()];
    }

  },

  // Some channels aren't allowed to show the energy cost
  // E. g. the channel POWERMETER_IEC2 measures the energy which a client feeds into the power supply system (solar, wind or so)
  isChannelValid2ShowEnergyCost: function() {
    return (this.opts.chType == "POWERMETER_IEC2") ? false : true;
  },

  showEnergyCost: function() {
    if (this.energyConfig && (this.measurementType != this.sensorTypeID.unknown) && ( this.isChannelValid2ShowEnergyCost()) ) {
      var unitFactorCCU = 1000,
        unitFactorDevice = 1000,
        kWh = " kWh";

      if (this.kiloPrefix == "k") {
        unitFactorCCU = 1;
      }

      if ((this.measurementType == this.sensorTypeID.iec)) {
        unitFactorCCU = 1;
        unitFactorDevice = 1;
      }

      if (this.measurementType == this.sensorTypeID.gas) {
        this.EnergyPrice = this.energyConfig.gasPrice;
        unitFactorCCU = 1;
        unitFactorDevice = 1;
      }
      if (this.measurementType == this.sensorTypeID.eletricity || this.measurementType == this.sensorTypeID.iec) this.EnergyPrice = this.energyConfig.curPrice;

      if (this.EnergyPrice != "unknown" && this.energyConfig.currency != null) {
        var j_energyCostCCU = this.getJElemByID("EnergyCostCCU"),
          j_energyCostDevice = this.getJElemByID("EnergyCostDevice"+this.getSensorExtension()),
          energyConsumptionCCU = this.getEnergyConsumption("CCU"),
          energyConsumptionDevice = this.getEnergyConsumption("Device"),
          energyCostCCU = ((energyConsumptionCCU * this.EnergyPrice) / unitFactorCCU).toFixed(2) + " " + this.energyConfig.currency,
          energyCostDevice = ((energyConsumptionDevice * this.EnergyPrice) / unitFactorDevice).toFixed(2) + " " + this.energyConfig.currency;
        if (this.measurementType == this.sensorTypeID.gas) {
          j_energyCostCCU.text(energyConsumptionCCU.toFixed(2) + kWh + " = " + energyCostCCU).show();
          j_energyCostDevice.text(energyConsumptionDevice.toFixed(2) + kWh + " = " + energyCostDevice).show();
        } else {
          j_energyCostCCU.text(energyCostCCU).show();
          j_energyCostDevice.text(energyCostDevice).show();
        }
      } else {
        conInfo("Unknown energy price");
      }
    }
  },

  searchSensor: function(iface, chnAddress) {
    conInfo("searchSensor");
    ShowWaitAnim();
    homematic("Interface.putParamset",{'interface': iface, 'address' : chnAddress, 'paramsetKey' : 'VALUES', 'set':
        [
          {name:'SELF_CALIBRATION', type: 'int', value: 1}
        ]
    },function(result){
      if (result) {
        window.setTimeout(function() {
          HideWaitAnim();
          reloadPage();
        },1000);
      } else {
        HideWaitAnim();
        alert("Please press the system button and try again.");
      }
    });
    HideWaitAnimAutomatically(5);
  }
};