/**
 * yesnodialog.js
 **/
 
/**
 * Dialogbox mit den Schaltflächen "Ja" und "Neine"
 * Normalerweise wird als content Text übergeben,
 * wenn contentType 'html' gesetzt ist, kann auch HTML übergeben werden.
 * Die Höhe des Dialoges sollte sich dynamisch der Contentgröße anpassen.
 **/
AcousticSignalController = Class.create({
 
  initialize: function(title, content, value, callback, contentType)
  {
    var _this_ = this;

    this.m_contentType = contentType;
    this.m_callback = callback;
    this.m_layer = document.createElement("div");
    this.m_layer.className = "YesNoDialogLayer"; 

    this.RESULT_NO = 0;
    this.RESULT_YES = 1;
    this.initValue = value;

    var dialog = document.createElement("div");
    dialog.className = "YesNoDialog";
    
    var titleElement = document.createElement("div");
    titleElement.className = "YesNoDialogTitle";
    titleElement.appendChild(document.createTextNode(title));
    titleElement.onmousedown = function(event) { new Drag(this.parentNode, event); };
    dialog.appendChild(titleElement);
    
    var contentWrapper = document.createElement("div");
    contentWrapper.className = "YesNoDialogContentWrapper";
    
    var contentElement = document.createElement("div");
    contentElement.className = "YesNoDialogContent";

    if (this.m_contentType == "html") {
      contentElement.innerHTML = content;
    } else {
      contentElement.appendChild(document.createTextNode(content));
    }

    contentWrapper.appendChild(contentElement);
    
    dialog.appendChild(contentWrapper);

    var footer = document.createElement("div");
    footer.className= "YesNoDialogFooter";
    
    var yesButton = document.createElement("div");
    yesButton.className = "YesNoDialog_yesButton borderRadius5px colorGradient50px";
    yesButton.appendChild(document.createTextNode(translateKey('dialogYes')));
    yesButton.onclick = function() { _this_.yes(); };
    yesButton.id="btnYes";
    footer.appendChild(yesButton);
    
    var noButton = document.createElement("div");
    noButton.className = "YesNoDialog_noButton borderRadius5px colorGradient50px";
    noButton.appendChild(document.createTextNode(translateKey('dialogNo')));
    noButton.onclick = function() { _this_.no(); };
    noButton.id = "btnNo";
    footer.appendChild(noButton);
    
    dialog.appendChild(footer);
    
    this.m_layer.appendChild(dialog);
    
    Layer.add(this.m_layer);
    translatePage(".YesNoDialog");
    this.setHeight();

    this.createHTML();

    this.setDialogElements();
    this.initDialog();

  },

  createHTML: function() {
    jQuery("#tdCombinedParam_Volume").html(this.getHtmlVolume());
  },

  getHtmlVolume: function () {
    var options = "<option value='0'>"+translateKey('lblOff')+"</option>";
    for (var loop= 10; loop <= 100; loop+=10) {
      options+= "<option value='"+loop+"'>"+loop+"%</option>";
    }
    return "<select id='combinedParam_Volume' style='text-align: center;'>" + options + "</select>";
  },

  setDialogElements: function() {

    this.trDurationElms = jQuery("[name='trDuration']");
    this.trRampTimeElms = jQuery("[name='trRampTime']");

    this.volumeElm = jQuery("#combinedParam_Volume");
    this.chkBoxTimeLimitElm = jQuery("#chkBoxTimeLimit");
    this.durationUnitElm = jQuery("#combinedParam_DurationUnit");
    this.durationValueElm = jQuery("#combinedParam_DurationValue");
    this.rampTimeUnitElm = jQuery("#combinedParam_RampTimeUnit");
    this.rampTimeValueElm = jQuery("#combinedParam_RampTimeValue");
    this.repetitionsElm = jQuery("#combinedParam_Repetitions");
    this.choosenFilesElm = jQuery("#combinedParam_ChoosenFiles");
    this.btnChooseSoundFiles = jQuery("#btnChooseSoundFiles");
  },

  initDialog: function() {
    var self = this,
      arElmValues = this.initValue.split(","),
      permanentHR = 31,
      minDuration = 0,
      maxDuration = 16343;

    this.volumeElm.val(arElmValues[0].split("=")[1]);
    this.durationUnitElm.val(arElmValues[1].split("=")[1]);
    this.durationValueElm.val(arElmValues[2].split("=")[1]);

    // Permanent not active. The duration of the sound is limited
    if (this.durationValueElm.val() == permanentHR && this.durationUnitElm.val() == 2) {
      this.chkBoxTimeLimitElm.prop("checked", false);
      this.trDurationElms.css("visibility", "visible");
      this.trDurationElms.css("opacity", "0.2");
    } else {
      this.chkBoxTimeLimitElm.prop("checked", true);
      this.trDurationElms.css("visibility", "visible");
    }

    this.rampTimeUnitElm.val(arElmValues[3].split("=")[1]);
    this.rampTimeValueElm.val(arElmValues[4].split("=")[1]);
    this.repetitionsElm.val(arElmValues[5].split("=")[1]);
    this.choosenFilesElm.val(arElmValues[6].split("=")[1].replace(/;/g, ","));

    this.btnChooseSoundFiles.bind("click", function() {
      chooseSoundFiles(self.choosenFilesElm.val());
    });
    this.chkBoxTimeLimitElm.bind("change", function(){
      if (this.checked) {
        self.durationValueElm.prop('disabled', false);
        self.durationUnitElm.prop('disabled', false);
        self.trDurationElms.fadeTo(1000, 1);
      } else {
        self.durationValueElm.prop('disabled', true);
        self.durationUnitElm.prop('disabled', true);
        self.trDurationElms.fadeTo(1000, 0.2);
        self.durationValueElm.val(permanentHR);
        self.trRampTimeElms.fadeTo(1000, 1);
        self.rampTimeValueElm.prop('disabled', false);
        self.rampTimeUnitElm.prop('disabled', false);
      }
    });

    this.durationValueElm.bind("keyup", function() {
      var min = 0,
        max = (parseInt(self.durationUnitElm.val()) == 2) ? permanentHR: maxDuration;
      this.value = self.checkValidity(this.value,min,max);
      if (this.value == 0) {
        self.trRampTimeElms.fadeTo(1000,0.1);
        self.rampTimeValueElm.prop('disabled', true).val(0);
        self.rampTimeUnitElm.prop('disabled', true);
      } else {
        self.trRampTimeElms.fadeTo(1000,1);
        self.rampTimeValueElm.prop('disabled', false);
        self.rampTimeUnitElm.prop('disabled', false);
      }
    });

    this.durationValueElm.bind("blur", function() {
      var val = parseInt(this.value);

      if (isNaN(val)) {
        this.value = (parseInt(self.durationUnitElm.val()) == 2) ? permanentHR :  maxDuration;
      } else {
        this.value = val;
      }
      self.durationValueElm.keyup();
    });

    this.durationUnitElm.bind("change", function(){
      self.durationValueElm.keyup();
    });

    this.rampTimeValueElm.bind("keyup", function() {
      var min = 0,
        max = (parseInt(self.rampTimeUnitElm.val()) == 2) ? permanentHR: maxDuration;
      this.value = self.checkValidity(this.value,min,max);
    });

    this.rampTimeValueElm.bind("blur", function() {
      var val = parseInt(this.value);

      if (isNaN(val)) {
        this.value = (parseInt(self.rampTimeUnitElm.val()) == 2) ? permanentHR :  maxDuration;
      } else {
        this.value = val;
      }
    });

    this.rampTimeUnitElm.bind("change", function(){
      self.rampTimeValueElm.keyup();
    });

  },

  checkValidity: function(val, min, max) {
    var result = val;
    if (val == "") {result = "";}

    if (parseInt(val) < 0) {result = min;}
    if (parseInt(val) > max) {result = max;}
    return result;
  },


  getConfigString: function() {
    var volume = this.volumeElm.val(),
    durationUnit = (this.chkBoxTimeLimitElm.prop("checked") == false) ? 2 : this.durationUnitElm.val(), // 2  = unit hour
    durationValue = (this.chkBoxTimeLimitElm.prop("checked") == false) ? 31 : this.durationValueElm.val(),
    ramptimeUnit = this.rampTimeUnitElm.val(),
    ramptimeValue = this.rampTimeValueElm.val(),
    repetitions = this.repetitionsElm.val(),
    soundfileList = this.choosenFilesElm.val().replace(/,/g, ";"),

    result = "L="+volume+",DU="+durationUnit+",DV="+durationValue+",RTU="+ramptimeUnit+",RTV="+ramptimeValue+",R="+repetitions+",SL="+ soundfileList;

    Layer.remove(this.m_layer);

    return result;
  },

  close: function(result)
  {
    if (this.m_callback) { this.m_callback(result); }
  },

  yes: function()
  {
    this.close(this.RESULT_YES);
  },
  
  no: function()
  {
    Layer.remove(this.m_layer);
    this.close(this.RESULT_NO);
  },

  btnTextYes: function(btnTxt) {
    jQuery(".YesNoDialog_yesButton").text(btnTxt);
  },

  btnYesHide: function() {
    jQuery("#btnYes").addClass("hidden");
  },

  btnYesShow: function() {
    jQuery("#btnYes").removeClass("hidden");
  },

  btnTextNo: function(btnTxt) {
    jQuery(".YesNoDialog_noButton").text(btnTxt);
  },

  btnNoHide: function() {
    jQuery("#btnNo").addClass("hidden");
  },

  btnNoShow: function() {
    jQuery("#btnNo").removeClass("hidden");
  },

  setHeight: function() {
    var heightContentWrapper = jQuery(".YesNoDialogContentWrapper").height(),
      yesNoElm = jQuery(".YesNoDialog"),
      footerElm = jQuery(".YesNoDialogFooter");

    yesNoElm.css("height", heightContentWrapper + 78);
    footerElm.css("top", heightContentWrapper + 26);
    yesNoElm.css("top", (window.innerHeight / 2) - (yesNoElm.height() / 2));
  },

  resetHeight: function() {
    this.setHeight();
  },

  setWidth: function(dlgWidth) {
    var defaultWith = 600,
      offsetWidth = 4,
      offsetPosYesButton = 109,
      offsetDialogHeight = 78,
      offsetDialogFooterHeight = 26;

    var width = dlgWidth - offsetWidth,
      yesButtonPos = dlgWidth - offsetPosYesButton,
      position = jQuery(".YesNoDialog").position();

    // dlgWidth = (defaultWith < dlgWidth) ? defaultWith : dlgWidth;

    jQuery(".YesNoDialog").width(dlgWidth).css({left: position.left + ((defaultWith - dlgWidth) / 2)});
    jQuery(".YesNoDialogTitle").width(width);
    jQuery(".YesNoDialogContentWrapper").width(width);
    jQuery(".YesNoDialogFooter").width(width);
    jQuery(".YesNoDialog_yesButton").css("left", yesButtonPos);

    //Dialoghöhe an Content anpassen.
    jQuery(".YesNoDialog").css("height", jQuery(".YesNoDialogContentWrapper").height() + offsetDialogHeight);
    jQuery(".YesNoDialogFooter").css("top", jQuery(".YesNoDialogContentWrapper").height() + offsetDialogFooterHeight);
  }
  
});
