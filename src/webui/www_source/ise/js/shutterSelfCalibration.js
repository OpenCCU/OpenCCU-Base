shutterSelfCalibration = Class.create();
shutterSelfCalibration.prototype = {
  initialize: function (chnId, iface, chAddress) {
    self = this;

    this.iface = iface;
    this.chAddress = chAddress;
    this.chMaintenance = chAddress.split(":").first() + ":0";

    this.dlg;

    this.topElems;
    this.bottomElems;

    self.ManualSelfCalibration = {
      CLEAR_SAVED_POSITIONS : 0,
      SAVE_TOP_POSITION     : 1,
      SAVE_BOTTOM_POSITION  : 2,
      REVERSE_MOTION        : 3,
      START_MOTION_WITH_LEVEL_SHORT : 4,
      START_MOTION_WITH_LEVEL_LONG  : 5,
      START_MOTION_WITH_LEVEL_AND_DURATION : 6,
      STOP_MOTION           :7
    };

    this.getDialog();
  },

  getDialog: function() {

    this.topSet = homematic("Interface.getValue", {'interface': this.iface, 'address' : this.chMaintenance, 'valueKey': 'MANUAL_SELF_CALIBRATION_TOP_POS_SET'});
    this.bottomSet = homematic("Interface.getValue", {'interface': this.iface, 'address' : this.chMaintenance, 'valueKey': 'MANUAL_SELF_CALIBRATION_BOTTOM_POS_SET'});


    var title = "Kalibrierung der Endlagen";

    this.dlg = new YesNoDialog(title, this.getHtml(), function(result) {
      if (result == YesNoDialog.RESULT_NO) {
        self.stopMotion();
      }
    }, "html");

    this.dlg.btnTextNo(translateKey("btnCancel"));
    this.dlg.btnTextYes(translateKey("btnReady"));
    this.dlg.btnYesHide();
    this.dlg.setWidth(295);

    this.topElems = jQuery(".j_TopElm");
    this.bottomElems = jQuery(".j_BottomElm");

    this.initEventHandler();
  },

  initEventHandler: function () {
    var saveTopPosElm = jQuery("#btnSaveUp"),
      saveBottomPosElm = jQuery("#btnSaveDown"),

      btnChangeMotorDirElm = jQuery("#btnChangeMotorDir"),
      btnStopMotionElm = jQuery("#btnStopMotion"),
      btnClearEndPosElm = jQuery("#btnClearPos"),

      btnShutterUpElm = jQuery("#btnShutterUp"),
      btnShutterDownElm = jQuery("#btnShutterDown"),
      btnShutterUp100Elm = jQuery("#btnShutterUp100"),
      btnShutterDown100Elm = jQuery("#btnShutterDown100"),
      btnShutterUp400Elm = jQuery("#btnShutterUp400"),
      btnShutterDown400Elm = jQuery("#btnShutterDown400"),
      inputDuration = jQuery("#inputDuration");


    saveTopPosElm.click(function() {self.buttonPressed(this); self.saveTopPos();});
    saveBottomPosElm.click(function() {self.buttonPressed(this); self.saveBottomPos();});

    btnShutterUpElm.click(function (){self.buttonPressed(this); self.upWithDuration();});
    btnShutterDownElm.click(function (){self.buttonPressed(this); self.downWithDuration();});
    btnShutterUp100Elm.click(function (){self.buttonPressed(this); self.up100ms();});
    btnShutterDown100Elm.click(function (){self.buttonPressed(this); self.down100ms();});
    btnShutterUp400Elm.click(function (){self.buttonPressed(this); self.up400ms();});
    btnShutterDown400Elm.click(function (){self.buttonPressed(this); self.down400ms();});

    btnChangeMotorDirElm.click(function (){self.buttonPressed(this); self.changeMotorDir();});
    btnStopMotionElm.click(function (){self.buttonPressed(this); self.stopMotion();});
    btnClearEndPosElm.click(function (){self.buttonPressed(this); self.clearEndPos();});

    inputDuration.on("blur",function() {
      self.checkVal(this);
    });

  },

  getHtml: function() {
    var html = "<table>";
      if (this.topSet == 0) {
        // Change Motor Direction
        html += "<tr>";
        html += "<td colspan='4'>";
        html += "<div id='btnChangeMotorDir' class='CLASS02550c ControlBtnOff'>Change Motor Dir</div>";
        html += "</td>";
        html += "</tr>";
      }

      // Clear all positions
      html += "<tr class='j_finish'>";
        html += "<td colspan='4'>";
          html += "<div id='btnClearPos' class='CLASS02550c ControlBtnOff'>Clear Position</div>";
        html += "</td>";
      html += "</tr>";

      html += "<tr class='j_finish'><td colspan='4'><hr></td></tr>";


    //  html += "<tr><td colspan='4'><div class='CLASS02550c ControlBtnOff' style='cursor:default;'>Kalibrierung der Endlagen</div></td></tr>";

      // Up/Down with duration
      html += "<tr  class='j_finish'>";
        html += "<td>";
          html += "<input id='inputDuration' type='text' class='CLASS02542' style='text-align:center;' size='3' value='0'>";
          html += "<span> Seconds</span>";
        html += "</td>";
        html += "<td>";
          html += "<div id='btnShutterUp' class='CLASS02550 ControlBtnOff j_TopElm'>Up</div>";
        html += "</td>";

        html += "<td>";
          html += "<div id='btnShutterDown' class='CLASS02550 ControlBtnOff j_BottomElm _hidden'>Down</div>";
        html += "</td>";
      html += "</tr>";

      // Up/Down 100ms
      html += "<tr class='j_finish'>";
        html += "<td>";
        html += "</td>";
        html += "<td>";
          html += "<div id='btnShutterUp100' class='CLASS02550 ControlBtnOff j_TopElm'>Up 100</div>";
        html += "</td>";

        html += "<td>";
          html += "<div id='btnShutterDown100' class='CLASS02550 ControlBtnOff j_BottomElm _hidden'>Down 100</div>";
        html += "</td>";
      html += "</tr>";

      // Up/Down 400ms
      html += "<tr>";
        html += "<td>";
          html += "</td>";
        html += "<td>";
          html += "<div id='btnShutterUp400' class='CLASS02550 ControlBtnOff j_TopElm'>Up 400</div>";
        html += "</td>";

        html += "<td>";
          html += "<div id='btnShutterDown400' class='CLASS02550 ControlBtnOff j_BottomElm _hidden'>Down 400</div>";
        html += "</td>";
      html += "</tr>";

      html += "<tr>";
        html += "<td></td>";
        // Save Up Position
        html += "<td colspan='2'>";
          html += "<div id='btnSaveUp' class='CLASS02550 ControlBtnOff j_TopElm' style='width:auto;'>Save Up</div>";
        html += "</td>";
      html += "</tr>";

      html += "<tr>";
      // Save Down Position
        html += "<td></td>";
        html += "<td colspan='2'>";
          html += "<div id='btnSaveDown' class='CLASS02550 ControlBtnOff j_BottomElm hidden' style='width:auto;'>Save Down</div>";
        html += "</td>";
      html += "</tr>";

      html += "<tr class='j_finish'><td colspan='4'><hr></td></tr>";

      html += "<tr class='j_finish'><td colspan='4'><div id='btnStopMotion' class='CLASS02550c ControlBtnOff' style='cursor:default;'>S T O P</div></td></tr>";

      html += "<tr class='j_ready hidden'><td>";
        html += "All cloar<br/><br/>I am ready!";
      html += "</td></tr>";


    html += "</table>";

    return html;
  },

  changeMotorDir: function() {
    homematic("Interface.putParamset",{'interface': this.iface, 'address' : this.chAddress, 'paramsetKey' : 'VALUES', 'set':
        [
          {name:'MANUAL_SELF_CALIBRATION', type: 'int', value: self.ManualSelfCalibration.REVERSE_MOTION}
        ]
    },function(result){conInfo(result);});
  },


  saveTopPos: function() {
    homematic("Interface.putParamset",{'interface': this.iface, 'address' : this.chAddress, 'paramsetKey' : 'VALUES', 'set':
        [
          {name:'MANUAL_SELF_CALIBRATION', type: 'int', value: self.ManualSelfCalibration.SAVE_TOP_POSITION}
        ]
    },function(result){
      //self.topElems.hide();
      //self.bottomElems.show();
      jQuery("#btnChangeMotorDir").hide();
      jQuery("#btnSaveUp").hide();
      jQuery("#btnSaveDown").show();
      self.dlg.resetHeight();
    });
  },

  saveBottomPos: function() {
    homematic("Interface.putParamset",{'interface': this.iface, 'address' : this.chAddress, 'paramsetKey' : 'VALUES', 'set':
        [
          {name:'MANUAL_SELF_CALIBRATION', type: 'int', value: self.ManualSelfCalibration.SAVE_BOTTOM_POSITION}
        ]
    },function(result){
      //self.topElems.show();
      self.dlg.btnYesShow();
      self.dlg.btnNoHide();
      self.topElems.hide();
      self.bottomElems.hide();

      jQuery(".j_finish").hide();
      jQuery(".j_ready").show();


      self.dlg.resetHeight();
    });
  },

  upWithDuration: function() {
    var time = jQuery("#btnDuration").val();

    homematic("Interface.putParamset",{'interface': this.iface, 'address' : this.chAddress, 'paramsetKey' : 'VALUES', 'set':
        [
          {
            name:'MANUAL_SELF_CALIBRATION', type: 'int', value: self.ManualSelfCalibration.START_MOTION_WITH_LEVEL_AND_DURATION,
            name:'LEVEL', type: 'double', value: 1.0,
            name:'MANUAL_SELF_CALIBRATION_DURATION_VALUE', type: 'int', value: time
          }
        ]
    },function(result){});
  },

  downWithDuration: function() {
    var time = jQuery("#btnDuration").val();

    homematic("Interface.putParamset",{'interface': this.iface, 'address' : this.chAddress, 'paramsetKey' : 'VALUES', 'set':
        [
          {
            name:'MANUAL_SELF_CALIBRATION', type: 'int', value: self.ManualSelfCalibration.START_MOTION_WITH_LEVEL_AND_DURATION,
            name:'LEVEL', type: 'double', value: 0.0,
            name:'MANUAL_SELF_CALIBRATION_DURATION_VALUE', type: 'int', value: time
          }
        ]
    },function(result){});
  },

  up100ms: function() {
    homematic("Interface.putParamset",{'interface': this.iface, 'address' : this.chAddress, 'paramsetKey' : 'VALUES', 'set':
        [
          {
            name:'MANUAL_SELF_CALIBRATION', type: 'int', value: self.ManualSelfCalibration.START_MOTION_WITH_LEVEL_SHORT,
            name:'LEVEL', type: 'double', value: 1.0
          }
        ]
    },function(result){});
  },

  down100ms: function() {
    homematic("Interface.putParamset",{'interface': this.iface, 'address' : this.chAddress, 'paramsetKey' : 'VALUES', 'set':
        [
          {
            name:'MANUAL_SELF_CALIBRATION', type: 'int', value: self.ManualSelfCalibration.START_MOTION_WITH_LEVEL_SHORT,
            name:'LEVEL', type: 'double', value: 0.0
          }
        ]
    },function(result){});
  },

  up400ms: function() {
    homematic("Interface.putParamset",{'interface': this.iface, 'address' : this.chAddress, 'paramsetKey' : 'VALUES', 'set':
        [
          {
            name:'MANUAL_SELF_CALIBRATION', type: 'int', value: self.ManualSelfCalibration.START_MOTION_WITH_LEVEL_LONG,
            name:'LEVEL', type: 'double', value: 1.0
          }
        ]
    },function(result){});
  },

  down400ms: function() {
    homematic("Interface.putParamset",{'interface': this.iface, 'address' : this.chAddress, 'paramsetKey' : 'VALUES', 'set':
        [
          {
            name:'MANUAL_SELF_CALIBRATION', type: 'int', value: self.ManualSelfCalibration.START_MOTION_WITH_LEVEL_LONG,
            name:'LEVEL', type: 'double', value: 0.0
          }
        ]
    },function(result){});
  },

  stopMotion: function() {
    console.log("stopMotion");
    homematic("Interface.putParamset",{'interface': this.iface, 'address' : this.chAddress, 'paramsetKey' : 'VALUES', 'set':
        [
          {name:'MANUAL_SELF_CALIBRATION', type: 'int', value: self.ManualSelfCalibration.STOP_MOTION}
        ]
    },function(result){});
  },

  clearEndPos: function() {
    homematic("Interface.putParamset",{'interface': this.iface, 'address' : this.chAddress, 'paramsetKey' : 'VALUES', 'set':
        [
          {name:'MANUAL_SELF_CALIBRATION', type: 'int', value: self.ManualSelfCalibration.CLEAR_SAVED_POSITIONS}
        ]
    },function(result){
      console.log("clearEndPos: ", result);
      console.log(self.dlg);
      self.dlg.close();
      window.setTimeout(function() {
        self.getDialog();
      },750);
    });
  },

  buttonPressed: function(btn) {
    var elem = jQuery(btn);
    elem.addClass("ControlBtnOn").removeClass("ControlBtnOff");
    setTimeout(function() {
      elem.addClass('ControlBtnOff').removeClass('ControlBtnOn');
    }, 500);
  },

  checkVal: function(elm) {
    var timeElm = jQuery(elm),
      val = parseInt(timeElm.val());
    if (isNaN(val) || val < 0) {timeElm.val(0);}
    if (val > 30) {timeElm.val(30);}
  }

};

