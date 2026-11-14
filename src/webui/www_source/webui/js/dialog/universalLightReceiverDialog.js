
// TODO Check if this class is still necessary!
UniveralLightReceiverDialog = Class.create(YesNoDialog,{

  run: function () {
    this.durationValElm = jQuery("#durationVal");
    this.durationUnitElm = jQuery("#durationUnit");
    this.rampTimeValElm = jQuery("#rampTimeVal");
    this.rampTimeUnitElm = jQuery("#rampTimeUnit");
    this.rampTimeOffValElm = jQuery("#rampTimeOffVal");
    this.rampTimeOffUnitElm = jQuery("#rampTimeOffUnit");
  },

  initColorPicker: function(chnId, oInitColor, dlg) {
    //this.initEventHandler(dlg);
    var hsvColorString = "hsv(" + oInitColor.hue + "," + oInitColor.saturation + "%," + oInitColor.level + "%)",
      colorPickerElm = jQuery("#colorPicker");
      colorPickerElm.val(hsvColorString);

    jQuery(colorPickerElm).spectrum({
      preferredFormat: "hsv",
      showInput: false,
      color: hsvColorString,
      //showPalette: true,
      //disabled: pickerState,
      //palette: palette,
      cancelText: translateKey("btnCancel"),
      chooseText: translateKey("btnOk"),
      show: function() {dlg.btnYesHide();},
      hide: function(color) {
        selectedRGBColor = tinycolor(color).toRgbString();
        selectedColor =  tinycolor(color).toHsv();
        dlg.btnYesShow();
      }
    });
  }
});