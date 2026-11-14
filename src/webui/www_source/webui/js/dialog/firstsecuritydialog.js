/**
 * firstsecuritydialog.js
 **/
 

FirstSecurityDialog = Class.create({
 
  initialize: function(title, content, callback, contentType)
  {
    var _this_ = this;

    this.m_contentType = contentType;
    this.m_callback = callback;
    this.m_layer = document.createElement("div");
    this.m_layer.className = "EulaDialogLayer"; 

    var dialog = document.createElement("div");
    dialog.className = "EulaDialog";
    
    var titleElement = document.createElement("div");
    titleElement.className = "EulaDialogTitle";
    titleElement.appendChild(document.createTextNode(title));
    titleElement.onmousedown = function(event) { new Drag(this.parentNode, event); };
    dialog.appendChild(titleElement);
    
    var contentWrapper = document.createElement("div");
    contentWrapper.className = "EulaDialogContentWrapper";
    var contentElement = document.createElement("div");
    contentElement.className = "EulaDialogContent";


    if (this.m_contentType == "html") {
      contentElement.innerHTML = content;
    } else {
      contentElement.appendChild(document.createTextNode(content));
    }

    contentWrapper.appendChild(contentElement);
    
    dialog.appendChild(contentWrapper);

    var pwdElement = document.createElement("div");
    pwdElement.className = "EulaDialogContent";
    pwdElement.id = "pwdLine";
    pwdElement.style = "visibility:hidden; text-align:center";
    pwdElement.appendChild(document.createTextNode(translateKey("lblAttentionNoPasswd")));
    pwdElement.innerHTML = translateKey("lblAttentionNoPasswd");
    contentWrapper.appendChild(pwdElement);

    var pwd = homematic('User.hasUserPWD', {'userID': userId}, function(result) {
     if ((result == false) || (result == null)) {
       jQuery("#pwdLine").css("visibility","visible");
     }
    });

    var footer = document.createElement("div");
    footer.className= "EulaDialogFooter";

    var chkBox = document.createElement("div");
    chkBox.className = "EulaDialog_checkBox";

    var yesCheckBox = document.createElement("input");
    yesCheckBox.type = "checkBox";
    yesCheckBox.id = "eulaReadId";
    chkBox.appendChild(yesCheckBox);

    chkBox.appendChild(document.createTextNode(translateKey('dialogLblAckSecurityHint')));

    yesCheckBox.onchange = function() {
      if (jQuery(this).is(":checked")) {
        jQuery("#yesBtn").show();
      } else {
        jQuery("#yesBtn").hide();
      }
    };

    footer.appendChild(chkBox);

    var yesButton = document.createElement("div");
    yesButton.className = "EulaDialog_yesButton borderRadius5px colorGradient50px";
    yesButton.id = "yesBtn";
    yesButton.appendChild(document.createTextNode(translateKey('btnNext')));
    yesButton.onclick = function() { _this_.yes(); };
    footer.appendChild(yesButton);


    //var noButton = document.createElement("div");
    //noButton.className = "EulaDialog_noButton borderRadius5px colorGradient50px";
    //noButton.appendChild(document.createTextNode(translateKey('dialogEulaBtnCancel')));
    //noButton.onclick = function() { _this_.no(); };
    //footer.appendChild(noButton);
    
    dialog.appendChild(footer);
    
    this.m_layer.appendChild(dialog);
    
    Layer.add(this.m_layer);

    if (jQuery(".EulaDialogContentWrapper").height() >= 400) {
      jQuery(".EulaDialogContentWrapper").css("height", 400);
      jQuery(".EulaDialogContentWrapper").css("overflow", "scroll");
    }

    //AG sorgt dafür, daß die Dialoghöhe sich dynamisch dem Content anpasst.
    jQuery(".EulaDialog").css("height", jQuery(".EulaDialogContentWrapper").height() + 108);
    jQuery(".EulaDialogFooter").css("top", jQuery(".EulaDialogContentWrapper").height() + 26);

    jQuery("#yesBtn").hide();
    this.centerDialog();



  },
    
  close: function(result)
  {
    Layer.remove(this.m_layer);
    if (this.m_callback) { this.m_callback(result); }
  },
  
  yes: function()
  {
    this.close(EulaDialog.RESULT_YES);
  },
  
  no: function()
  {
    this.close(EulaDialog.RESULT_NO);
  },

  centerDialog: function() {
    var dialog = jQuery(".EulaDialog"),
    top = ((jQuery(".EulaDialogLayer").height() / 2) - (jQuery(dialog).height() / 2));
    jQuery(dialog).css({"top":top, "margin-top": ""});
  }

});

EulaDialog.RESULT_NO = 0;
EulaDialog.RESULT_YES = 1;
