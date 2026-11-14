/**
 * crashdialog.js
 **/
 
CrashDialog = Class.create({

  initialize: function(name)
  {
    var screenWidth  = WebUI.getWidth();
    var screenHeight = WebUI.getHeight();
    var frameWidth   = CrashDialog.CONTENT_WIDTH;
    var frameHeight  = CrashDialog.CONTENT_HEIGHT;
    var frameX       = parseInt((screenWidth  - frameWidth)  / 2);
    var frameY       = parseInt((screenHeight - frameHeight) / 2);
    
    this.m_layer = document.createElement("div");
    this.m_layer.className = "DialogLayer";
    
    this.m_frame = new UI.Frame()
      .setTitle(name)
      .setContentSize(frameWidth, frameHeight)
      .setPosition(frameX, frameY)
      .add(new UI.Text()
        .setPosition(5, 5)
        .setWidth(frameWidth - 10)
        .setHtml(translateKey("crashDialogContent"))
      )
      .add(new UI.Button()
        .setPosition(125, 230)
        .setText(translateKey("crashDialogBtnClose"))
        .setAction(this.close.bind(this))
      );
  
    this.m_layer.appendChild(this.m_frame.getElement());
    Layer.add(this.m_layer);
  },

  close: function()
  {
    Layer.remove(this.m_layer);
  }
  
});

CrashDialog.CONTENT_WIDTH = 400;
CrashDialog.CONTENT_HEIGHT = 260;
/*
CrashDialog.TEXT  = "<div style='font-weight: bold;'>Eine Komponente der HomeMatic Zentrale reagiert nicht mehr.</div>"
                  + "<p>Hierfür kann es eine Reihe von Ursachen geben:</p>"
                  + "<ul>"
                  + "<li>es besteht keine Netzwerk-Verbindung</li>"
                  + "<li>die Stromversorung der HomeMatic Zentrale wurde unterbrochen</li>"
                  + "<li>mindestens eine Komponente der HomeMatic Zentrale ist abgestürzt</li>"
                  + "</ul>"
                  + "<p>Überprüfen Sie die Netzwerk-Verbindung und die Stromversorgung der HomeMatic Zentrale. Starten Sie ggf. die HomeMatic Zentrale neu.</p>";
*/

