/**
 * ise/js/PowerMeter.js
 * POWERMETER. POWERMETER_IGL
 **/

/**
 * @author gral
 **/

/**
 * @class Control for the 'POWERMETER'
 **/ 
isePowerMeter = Class.create();

isePowerMeter.prototype = {

  initialize: function(id, opts) {
    conInfo("PowerMeter");
    var self = this;
    this.opts = opts;
    this.powerMeter = "POWERMETER"; // e. g. Hm-ES-PMSw1-PL-DN-R1
    this.POWERMETER_IEC = "POWERMETER_IEC"; // e. g. Hm-ES-TX-WM
    this.energyMeterTransmitter = "ENERGIE_METER_TRANSMITTER"; // e. g. HmIP-PSM
    this.arMeasurementTypes = [];
    this.sensorTypeID = {};
    this.sensorTypeID.gas = "Gas";
    this.sensorTypeID.eletricity = "Electricity";
    this.sensorTypeID.iec = "IEC";
    this.sensorTypeID.unknown = "Unknown";
    this.kiloPrefix = "";
    this.hasFeedIn = ((this.opts.chLabel == "HmIP-PSMCO") || (this.opts.chLabel == "ELV-SH-PSMCI" )) ? true : false;

    this.id = id;
    this.summedUpEnergy = 0.0;
    this.summedUpFeedIn = 0.0;
    this.measurementType = this.getSensorType();
    conInfo("Identified sensor: " + this.measurementType);

    this.panel2Show = null;

    // Channel 2 is only visible when a IEC-Sensor is attached
    if (this.opts.chType == "POWERMETER_IEC2") {
      this.panel2Show = (this.measurementType == this.sensorTypeID.iec) ? this.getJElemsByName("channelActiv") : this.getJElemByID("channelNotActiv");

      this.panel2Show.show();
    } else {
      this.panel2Show = this.getJElemsByName("channelActiv");
      this.panel2Show.show();
    }

    this.EnergyCounterID = "svEnergyCounter" + this.getSensorExtension() + "_" + this.id + "_" + this.opts.chAddress;
    this.EnergyCounterRESETID = "svEnergyCounter_" + this.id + "_" + this.opts.chAddress + "_RESET";
    this.EnergyCounterOldValID = "svEnergyCounter" + this.getSensorExtension() + "OldVal_" + this.id;
    this.EnergyPrice = "unknown";

    this.EnergyCounterFeedInID = "svEnergyCounterFeedIn_" + this.id + "_" + this.opts.chAddress;

    this.bindEvents();

    this.setGenericInfoPanels(this.getVisibleInfoPanels());

    // When gas sensor change the unit from Wh to m^3 and change the EnergyCounterID
    if (this.measurementType == this.sensorTypeID.gas) {
      this.opts.unitEnergyCounter = "m<sup>3</sup>";
      this.EnergyCounterID = "svEnergyCounterGas_" + this.id + "_" + this.opts.chAddress;
      this.EnergyCounterRESETID = "svEnergyCounterGas_" + this.id + "_" + this.opts.chAddress + "_RESET";
      this.EnergyCounterOldValID = "svEnergyCounterGasOldVal_" + this.id;
    } else if (this.measurementType == this.sensorTypeID.unknown) {
      // Unknown sensor
      this.opts.unitEnergyCounter = "";
    } else if (this.measurementType == this.sensorTypeID.iec) {
      this.opts.unitEnergyCounter = this.opts.unitEnergyCounterIEC;
    }
    jQuery("[name = '" + this.id + "EnergyCostDeviceUnit']").html(this.opts.unitEnergyCounter).show();
    this.setEnergyCounterPanel();

    if (this.hasFeedIn) {
      this.setEnergyCounterFeedInPanel();
    }

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

  getVisibleInfoPanels: function() {
    switch (this.opts.chType) {
      //Wireless sender for power meter sensor
      case "POWERMETER_IGL":
      case "POWERMETER_IEC1":
      case "POWERMETER_IEC2":
        return ["Power"];
      default:
        // POWERMETER - power meter switch actuator
        return ["Voltage", "Current", "Power", "Frequency"];
    }
  },

  getSensorType: function() {
    // Determine the type of measurement (gas or electicity)
    var paramSet = homematic("Interface.getParamset", {"interface": this.opts.iface, "address" : this.opts.chAddress, "paramsetKey" :"MASTER"});

    /* No meter type available
       this.energyMeterTransmitter = e. g. a PSM or a Hm-ES-TX-WM Fw. >= 2.0.0
    */
    if (typeof paramSet.METER_TYPE == "undefined") {
      return ((this.opts.chType == this.energyMeterTransmitter) || (this.opts.chType == this.powerMeter)) ? this.sensorTypeID.eletricity : this.sensorTypeID.iec;
    }

    // No IEC Sensor
    // 0 = Gas-Sensor, 1 = Electricity (IR-Sensor), 2 = Electricity (LED-Sensor), 3 = Unknown

    // IEC-Sensor
    // 0 = Gas-Sensor, 1 = Electricity (IR-Sensor), 2 = Electricity (LED-Sensor), 3 = IEC-Sensor, 4 = Unknown

    switch (this.opts.chType) {
      case "POWERMETER":
      case "POWERMETER_IGL":
        this.arMeasurementTypes = ["Gas", "Electricity", "Electricity", "Unknown"];
        break;
      case "POWERMETER_IEC1":
      case "POWERMETER_IEC2":
        this.arMeasurementTypes = ["Gas", "Electricity", "Electricity", "IEC", "Unknown"];
        break;
    }
    return this.arMeasurementTypes[parseInt(paramSet.METER_TYPE)];
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


    jQuery("#"+ this.id + "resetEnergyCounter").bind("click", function(){
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

    // Feed In
    jQuery("#"+ this.id + "resetFeedInCounter").bind("click", function(){
      conInfo("Set Feed In Counter");
      var dlgContent = "<table align='center'><tr><td>"+translateKey('lblEnergyConsumptionInWatt')+"<td><td><input type='text' id='meterReading'></td></tr></table>",
        valMeterReading;

      setEnergyCounterDlg = new YesNoDialog(translateKey("lblSetEnergyCounter"), dlgContent, function(result) {
        if (result == YesNoDialog.RESULT_YES) {
          valMeterReading = parseFloat(jQuery("#meterReading").val());

          // This removes the dialog from the screen
          Layer.remove(this.m_layer);

          if (! isNaN(valMeterReading)) {
            // This sets the new value of the power meter
            homematic("SysVar.setFloat", {"name": self.EnergyCounterFeedInID, "value": valMeterReading}, function () {
              self.setEnergyCounterFeedInPanel();
              //self.showEnergyCost();
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
   * Sets the different parameter elements of the powermeter
   */
  setGenericInfoPanels: function(arElements) {
    var self = this;
    jQuery.each(arElements, function(index, panel) {
      self.setGenericPanel(panel);
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
  /**
   * Sets the text (value + unit) of the desired element
   * @param {string} panel The name of the desired element
   */
  setGenericPanel: function(panel) {
    var sensorExtension = this.getSensorExtension();
    var j_panel = this.getJElemByID(panel),
      value = this.opts["val" + panel + sensorExtension],
      unit = this.opts["unit" + panel + sensorExtension];
    if (value && unit) {
      j_panel.text(value.toFixed(2) + " " + unit);
    }
  },

  changeToKilo: function(x) {
    if (x == "summedUpFeedIn") {
      return (this.summedUpFeedIn/1000).toFixed(3);
    } else {
      return (this.summedUpEnergy/1000).toFixed(3);
    }

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
      this.summedUpEnergy = this.changeToKilo('summedUpEnergy');
    }

    j_panel.html(this.summedUpEnergy + " " + this.kiloPrefix + this.opts.unitEnergyCounter);
  },

  setEnergyCounterFeedInPanel: function() {
    var j_panel = this.getJElemByID("energyCounterFeedIn");
    var decimalPlace = 2;

    this.summedUpFeedIn = parseFloat(homematic("SysVar.getValue", {"id" : this.EnergyCounterFeedInID})).toFixed(decimalPlace);
    conInfo("setEnergyCounterFeedInPanel - this.summedUpFeedIn: " + this.summedUpFeedIn);

    this.kiloPrefix = "";
    if (this.summedUpFeedIn >= 1000.0) {
      this.kiloPrefix = "k";
      this.summedUpFeedIn = this.changeToKilo('summedUpFeedIn');
    }

    j_panel.html(this.summedUpFeedIn + " " + this.kiloPrefix + this.opts.unitEnergyCounter);
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
          j_energyCostCCU.text(energyConsumptionCCU.toFixed(1) + kWh + " = " + energyCostCCU).show();
          j_energyCostDevice.text(energyConsumptionDevice.toFixed(1) + kWh + " = " + energyCostDevice).show();
        } else {
          j_energyCostCCU.text(energyCostCCU).show();
          j_energyCostDevice.text(energyCostDevice).show();
        }
      } else {
        conInfo("Unknown energy price");
      }
    }
  }
};


