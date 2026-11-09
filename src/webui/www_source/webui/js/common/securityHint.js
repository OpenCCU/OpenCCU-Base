function showSecurityDialog() {

  var fileSecHint = "/etc/config/userAckSecurityHint",
  result = homematic('CCU.existsFile', {'file': fileSecHint});

  if(! result) {

    var lang = getLang();

    var url = "/pages/msg/securityFirstStart_"+lang+".htm";

    var req = jQuery.ajax({
      url : url +"?sid=" + SessionId,
      cache: false,
      dataType: "html"
    });

    req.done(function(data) {
      //new FirstSecurityDialog(translateKey("thSafetyNote"), translateKey("dialogSecurityFirstStart"), function (action) {
      var dlg = new FirstSecurityDialog(translateKey("thSafetyNote"), data, function (action) {
        /*
         action can be
         0 - user selected NO
         1 - user selected YES
         */
        if (action == 1) {
          jQuery(".Layer0").show();
          homematic("CCU.setSecurityHint");
          if (getProduct() >= 3) {
            WebUI.enter(StartPage);
            new InstallWizard();
          } else {
            WebUI.enter(StartPage);
          }
        }
      }, "html");
    });

    req.fail(function(data) {
      conInfo("Security  not available");
    });
  }
};

