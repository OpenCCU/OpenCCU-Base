iseAccelerationTransceiver = Class.create();
iseAccelerationTransceiver.prototype = {
  initialize: function (chnId, valMotion, chnAddress, tiltAngle, tiltAngleUnit) {
    var self = this;
    this.chnId = chnId;
    this.valMotion = valMotion;
    this.chnAddress = chnAddress;
    this.tiltAngle = tiltAngle;
    this.tiltAngleUnit = tiltAngleUnit;

    var tmp = homematic("Interface.getMasterValue", {"interface": "HmIP-RF", "address": this.chnAddress, "valueKey": "CHANNEL_OPERATION_MODE"},function(result) {
      var outputElm = jQuery("#accelerationState" + chnId);
      var arMessage = ["",translateKey("lblVibration"),translateKey("lblPosition"), translateKey("lblTilt")],
      arMotion = [translateKey("lblNo"), translateKey("lblYes")],
      arPosition = [translateKey("lblHorizontal"), translateKey("lblNonHorizontal")],
      res = "--";

      switch (parseInt(result)) {
        case 1:
          res = (self.valMotion == "false") ? arMotion[0] : arMotion[1];
          break;
        case 2:
          res = (self.valMotion == "false") ? arPosition[0] : arPosition[1];
          break;
        case 3:
          res = self.tiltAngle + self.tiltAngleUnit;
          break;
      }
      outputElm.html(arMessage[result] + ":<br/>"+ res );
    });
  }
};

iseAccelerationTransceiverTaco = Class.create();
iseAccelerationTransceiverTaco.prototype = {
  initialize: function (chnId, valMotion, chnAddress, tiltAngle, tiltAngleUnit, position) {
    var self = this;
    this.chnId = chnId;
    this.valMotion = valMotion;
    this.chnAddress = chnAddress;
    this.position = (position != -1) ? position : 3; // 0 waagerecht, 1 geneigt, 2 senkrecht. 3 = unknown
    this.tiltAngle = tiltAngle;
    this.tiltAngleUnit = tiltAngleUnit;

    var tmp = homematic("Interface.getMasterValue", {"interface": "HmIP-RF", "address": this.chnAddress, "valueKey": "CHANNEL_OPERATION_MODE"},function(result) {
      var firstElm = jQuery("#firstElm" + chnId),
        secondElm = jQuery("#secondElm" + chnId),
        angleElm = jQuery("#angleElm" + chnId),
        arMessage = ["",translateKey("lblVibration"),translateKey("lblPosition"), translateKey("lblPositionA")],
        arMotion = [translateKey("lblNo"), translateKey("lblYes")],
        arPosition = ["waagerecht","geneigt" ,"senkrecht","--"],
        arPositionA = ["waagerecht","geneigt" ,"geneigt","--"],
        lblAngle = translateKey("lblTilt") + ":<br/>",
        motion = (self.valMotion == "false") ? arMotion[0] : arMotion[1];


      switch (parseInt(result)) {
        case 1:
          firstElm.html(arMessage[result] + "<br/>" +  motion );
          break;
        case 2:
          firstElm.html(arMessage[result]  + ":<br/>" + arPositionA[self.position]);
          break;
        case 3:
          firstElm.html(arMessage[result]  + ":<br/>" + arPosition[self.position]);
          break;
        default: firstElm.hide();
      }
      angleElm.html(lblAngle + self.tiltAngle + self.tiltAngleUnit);

    });
  }
};
