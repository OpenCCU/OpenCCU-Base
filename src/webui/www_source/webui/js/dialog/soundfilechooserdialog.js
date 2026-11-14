SoundfileChooserDialog = Class.create({
 
  initialize: function(activeSounds, callback)
  {
    var _this_ = this;

    this.m_title = translateKey("btnChooseSongs");

    this.m_callback = callback;
    this.m_layer = document.createElement("div");
    this.m_layer.className = "YesNoDialogLayer"; 

    this.RESULT_NO = "Cancel";
    this.RESULT_YES = "OK";

    this.activeSounds = activeSounds;

    var dialog = document.createElement("div");
    dialog.className = "YesNoDialogA";


    var titleElement = document.createElement("div");
    titleElement.className = "YesNoDialogTitleA";
    titleElement.appendChild(document.createTextNode(this.m_title));
    titleElement.onmousedown = function(event) { new Drag(this.parentNode, event); };
    dialog.appendChild(titleElement);
    
    var contentWrapper = document.createElement("div");
    contentWrapper.className = "YesNoDialogContentWrapperA";
    
    var contentElement = document.createElement("div");
    contentElement.className = "YesNoDialogContentA";

    contentElement.innerHTML = this.getHTML();

    contentWrapper.appendChild(contentElement);
    
    dialog.appendChild(contentWrapper);

    var footer = document.createElement("div");
    footer.className= "YesNoDialogFooterA";
    
    var yesButton = document.createElement("div");
    yesButton.className = "YesNoDialogA_yesButton borderRadius5px colorGradient50px";
    yesButton.appendChild(document.createTextNode(translateKey('footerBtnOk')));
    yesButton.onclick = function() { _this_.yes(); };
    yesButton.id="btnYesA";
    footer.appendChild(yesButton);
    
    var noButton = document.createElement("div");
    noButton.className = "YesNoDialogA_noButton borderRadius5px colorGradient50px";
    noButton.appendChild(document.createTextNode(translateKey('footerBtnCancel')));
    noButton.onclick = function() { _this_.no(); };
    noButton.id = "btnNoA";
    footer.appendChild(noButton);
    
    dialog.appendChild(footer);
    
    this.m_layer.appendChild(dialog);
    Layer.add(this.m_layer);

    translatePage(".YesNoDialogA");

    this.setHeight();

    this.initDialog();

  },

  getHTML: function() {
    var result = "",
      x= 0,
      count = 0,
      songNr;

    result += "<div>";
      result += "<table align='center'>";

        for (var row = 0; row < 3; row++) {
          result += "<tr>";
          for (x = row; x < (row + 4); x++) {
            result += "<td>";
            result += "<select id='songNr_"+count+"'>";
            result += "<option value='-1'></option>";
            result += "<option value='0'>"+translateKey('internalSystemSound')+"</option>";
            for (songNr = 1; songNr <= 253; songNr++) {
              result += "<option value='" + songNr + "'>"+translateKey('lblSoundFileNr')+ ": " + songNr + "</option>";
            }
            result += "</select>";
            result += "</td>";
            count++;
          }
          result += "</tr>";
        }
      result += "</table>";
    result += "</div>";
    return result;
  },

  initDialog: function() {
    var arSound = this.activeSounds.split(",");
    jQuery.each(arSound, function(index, songNr) {
      jQuery("#songNr_" + index).val(songNr);
    });
  },

  getChoosenSongs: function() {
    var result = "",
      songNr;

    for(var loop = 0; loop < 12; loop++) {
      songNr = jQuery("#songNr_" + loop).val();
      if (songNr != -1) {
        result += songNr+ ";";
      }
    }

    return result.slice(0,-1);
  },

  close: function(result)
  {
    Layer.remove(this.m_layer);
    if (this.m_callback) { this.m_callback(result); }
  },

  yes: function()
  {
    //this.close(this.RESULT_YES);
    this.close(this.getChoosenSongs());
  },
  
  no: function()
  {
    this.close(this.RESULT_NO);
  },

  btnTextYes: function(btnTxt) {
    jQuery(".YesNoDialogA_yesButton").text(btnTxt);
  },

  btnYesHide: function() {
    jQuery("#btnYesA").addClass("hidden");
  },

  btnYesShow: function() {
    jQuery("#btnYesA").removeClass("hidden");
  },

  btnTextNo: function(btnTxt) {
    jQuery(".YesNoDialogA_noButton").text(btnTxt);
  },

  btnNoHide: function() {
    jQuery("#btnNoA").addClass("hidden");
  },

  btnNoShow: function() {
    jQuery("#btnNoA").removeClass("hidden");
  },

  setHeight: function() {
    var heightContentWrapper = jQuery(".YesNoDialogContentWrapperA").height(),
      yesNoElm = jQuery(".YesNoDialogA"),
      footerElm = jQuery(".YesNoDialogFooterA");

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
      position = jQuery(".YesNoDialogA").position();

    // dlgWidth = (defaultWith < dlgWidth) ? defaultWith : dlgWidth;

    jQuery(".YesNoDialogA").width(dlgWidth).css({left: position.left + ((defaultWith - dlgWidth) / 2)});
    jQuery(".YesNoDialogTitleA").width(width);
    jQuery(".YesNoDialogContentWrapperA").width(width);
    jQuery(".YesNoDialogFooterA").width(width);
    jQuery(".YesNoDialogA_yesButton").css("left", yesButtonPos);

    //Dialoghöhe an Content anpassen.
    jQuery(".YesNoDialogA").css("height", jQuery(".YesNoDialogContentWrapperA").height() + offsetDialogHeight);
    jQuery(".YesNoDialogFooterA").css("top", jQuery(".YesNoDialogContentWrapperA").height() + offsetDialogFooterHeight);
  }
  
});
