/**
 * @class
 **/
iseAlarmSirenHMIP = Class.create();

iseAlarmSirenHMIP.prototype = {

  initialize: function(opts) {
    conInfo(opts);
    this.opts = opts;
    this.chnId = this.opts.chID;
    this.iface = this.opts.chInterface;
    this.chAddress = this.opts.chAddress;

    this.startAlarmElm = jQuery("#"+this.chnId+"startAlarm");

    this.initAllElements();

    // Add event handlers

  },

  initAllElements: function() {
    var self = this;
    /* Not in use currently
    if (self.opts.ZoneActive[1]) {
      var zoneActive,
        zoneAlarmElm,
        dataPointID,
        elemID,
        elemIsChecked,
        arZoneActive,
        arZoneAlarm;

      for (var loop = 1; loop < 8; loop++) {
        arZoneActive = self.opts.ZoneActive[loop].split(",");
        arZoneAlarm = self.opts.ZoneAlarm[loop].split(",");
        dataPointID = arZoneActive[0];
        elemID = arZoneAlarm[0];
        elemIsChecked = (arZoneActive[1] == "true") ? true : false;
        zoneAlarmElm = jQuery("#ZoneAlarm_" + loop + "_" + elemID);

        zoneAlarmElm.prop("checked", elemIsChecked);
        zoneAlarmElm.val(loop); // we use this in the method setAlarmZone >> elm.val()

        zoneAlarmElm.change(function () {
          self.setAlarmZone(this.id);
        });
      }
    }
    */
    this.startAlarmElm.click(function() {
      self.activateAlarm();
    });

  },

  /*
  setAlarmZone: function(elmID) {
    var elm = jQuery("#" + elmID);
    setDpState(this.opts.ZoneActive[parseInt(elm.val())].split(",")[0], elm.prop("checked"), true);
  },
  */

  // Here we have to use putParamset for the whole bunch of parameters
  activateAlarm: function() {
    var self = this;
    var url = "/pages/msg/asir_setAlarm.htm";

    var req = jQuery.ajax({
      url : url +"?sid=" + SessionId,
      cache: false,
      dataType: "html"
    });

    req.done(function(htmlContent) {

      setAlarmDialog = new ASIR_SetAlarmDialog(translateKey("setAlarmDialogTitle"), htmlContent, function(result) {

        var oUnit = {};
        var acousticSignal = this.selectedAcousticSignalElm.val(),
          opticalSignal = this.selectedOpticalSignalElm.val(),
          durationUnit = this.durationUnitElm.val(),
          durationValue = this.durationValueElm.val();

        oUnit.S = 0; oUnit.M = 1; oUnit.H = 2; oUnit.D = 3;

        if (result == 1) {
          conInfo("SET ALARM");
          homematic("Interface.putParamset",{'interface': self.iface, 'address' : self.chAddress, 'paramsetKey' : 'VALUES', 'set':
           [
             {name:'DURATION_UNIT', type: 'int', value: oUnit[durationUnit]},
             {name:'DURATION_VALUE', type: 'int', value: durationValue},
             {name:'ACOUSTIC_ALARM_SELECTION', type: 'int', value: acousticSignal},
             {name:'OPTICAL_ALARM_SELECTION', type: 'int', value: opticalSignal}
           ]
         },function(result){conInfo(result);});
        }
      }, "html");
    });

    req.fail(function() {
      alert("Error while loading " + url);
    });
  }


};

