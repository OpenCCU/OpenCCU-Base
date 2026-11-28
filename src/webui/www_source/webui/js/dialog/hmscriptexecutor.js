/**
 * hmscriptexecutor.js
 **/
 
/**
 * @fileOverview HM Script Executor
 * @author F. Werner (eQ-3)
 **/
 
/**
 * @class HMScriptExecturor
 * Dialog zum Testen von HomeMatic Script.
 **/
HMScriptExecutor = Class.create({

  initialize: function()
  {
    var screenWidth  = WebUI.getWidth();
    var screenHeight = WebUI.getHeight();
    var frameWidth   = parseInt(screenWidth * HMScriptExecutor.WIDTH_RATIO);
    var frameHeight  = parseInt(screenHeight * HMScriptExecutor.HEIGHT_RATIO);
    // the size set by the grip last time, if there is one
    var savedSize    = loadDialogSize(HMScriptExecutor.STORAGE_KEY, HMScriptExecutor.MIN_WIDTH, HMScriptExecutor.MIN_HEIGHT);
    if (savedSize) {
      var savedContentSize = HMScriptExecutor.getContentSize(savedSize.width, savedSize.height);
      frameWidth  = savedContentSize.width;
      frameHeight = savedContentSize.height;
    }
    var frameX       = parseInt((screenWidth  - frameWidth)  / 2);
    var frameY       = parseInt((screenHeight - frameHeight) / 2);

    HMScriptExecutor.TITLE = translateKey("dialogScriptExecuterTitle");
    HMScriptExecutor.TEXT_RUN = translateKey("dialogScriptExecuterBtnExecute");
    HMScriptExecutor.TEXT_CLOSE = translateKey("dialogScriptExecuterBtnClose");
    
    this.m_layer = document.createElement("div");
    this.m_layer.className = "DialogLayer";

    // positioned by m_layout()
    this.m_runButton = new UI.Button()
      .setText(HMScriptExecutor.TEXT_RUN)
      .setAction(this.run, this);
    this.m_closeButton = new UI.Button()
      .setText(HMScriptExecutor.TEXT_CLOSE)
      .setAction(this.close, this);

    this.m_frame = new UI.Frame(true)
      .setTitle(HMScriptExecutor.TITLE)
      .setContentSize(frameWidth, frameHeight)
      .setPosition(frameX, frameY)
      .add(new UI.Text()
        .setStyle('margin-left: 5px')
        .setHtml('<b>' + translateKey("dialogScriptExecuterLblInput") + '</b>')
        .setClass("")
      )
      .add(new UI.Textarea()
        .setID("code")
        .setName("code")
        .setText('WriteLine("'+ translateKey("dialogScriptExecuterHelloWorld") +'");')
      )
      .add(new UI.Text()
        .setStyle('margin-left: 5px')
        .setHtml('<b>' + translateKey("dialogScriptExecuterLblOutput") + '</b>')
        .setClass("")
      )
      .add(new UI.Textarea()
        .setID("output")
        .setName("output")
      )
      .add(this.m_runButton)
      .add(this.m_closeButton);
      
    this.m_layer.appendChild(this.m_frame.getElement());
    Layer.add(this.m_layer);

    this.m_input = CCUCodeMirror6.fromTextArea(document.getElementById('code'), {
      mode: "text/x-rega",
      autofocus: true,
      matchBrackets: true,
      autoCloseBrackets: true,
      lineWrapping: true,
      lineNumbers: true,
      foldGutter: true,
      indentUnit: 2,
      tabSize: 2,
      indentWithTabs: false,
      gutters: ["CodeMirror-linenumbers", "CodeMirror-foldgutter"],
      extraKeys: {"Ctrl-Space": "autocomplete",
                  "Ctrl-Q": function(cm){ cm.foldCode(cm.getCursor()); },
                  "F11": function(cm) { cm.setOption("fullScreen", !cm.getOption("fullScreen")); },
                  "Esc": function(cm) { if (cm.getOption("fullScreen")) cm.setOption("fullScreen", false); },
                  "Alt-F": "findPersistent",
                  "Tab": function(cm) {
                    if (cm.somethingSelected()) {
                      var sel = cm.getSelection("\n");
                      // Indent only if there are multiple lines selected, or if the selection spans a full line
                      if (sel.length > 0 && (sel.indexOf("\n") > -1 || sel.length === cm.getLine(cm.getCursor().line).length)) {
                        cm.execCommand("indentMore");
                        return;
                      }
                    }
                    if (cm.options.indentWithTabs)
                      cm.execCommand("insertTab");
                    else
                      cm.execCommand("insertSoftTab");
                  },
                  "Shift-Tab": function(cm) {
                    if (cm.somethingSelected()) {
                      var sel = cm.getSelection("\n");
                      // Outdent only if there are multiple lines selected, or if the selection spans a full line
                      if (sel.length > 0 && (sel.indexOf("\n") > -1 || sel.length === cm.getLine(cm.getCursor().line).length)) {
                        cm.execCommand("indentLess");
                        return;
                      }
                    }
                    var charSize = cm.options.tabSize;
                    if (cm.options.indentWithTabs)
                      charSize = 1;
                    var c = cm.getCursor();
                    var lineText = cm.getRange({line: c.line, ch: c.ch - charSize}, {line: c.line, ch: c.ch});
                    var m = /^(\s+)/.exec(lineText.reverse());
                    if(m && m.length == 2)
                      cm.replaceRange('', {line: c.line, ch: c.ch - m[1].length}, {line: c.line, ch: c.ch});
                  }}
    });

    this.m_output = CCUCodeMirror6.fromTextArea(document.getElementById('output'), {
      mode: "default",
      readOnly: true,
      lineWrapping: true,
      lineNumbers: false
    });
    this.m_layout(frameWidth, frameHeight);

    var _this_ = this;
    enableDialogResize(this.m_frame.getElement(), {
      minWidth: HMScriptExecutor.MIN_WIDTH,
      minHeight: HMScriptExecutor.MIN_HEIGHT,
      storageKey: HMScriptExecutor.STORAGE_KEY,
      onResize: function(width, height) {
        var contentSize = HMScriptExecutor.getContentSize(width, height);
        _this_.m_layout(contentSize.width, contentSize.height);
      }
    });
  },

  /**
   * Lays out the dialog for a content area of the given size (px): the
   * buttons at its bottom, the close button left of the resize grip, and the
   * input and output editor in the height left by the labels and buttons.
   **/
  m_layout: function(contentWidth, contentHeight)
  {
    var buttonY      = contentHeight - HMScriptExecutor.BUTTON_BOTTOM;
    var editorHeight = contentHeight - HMScriptExecutor.EDITOR_RESERVED_HEIGHT;

    this.m_frame.setContentSize(contentWidth, contentHeight);
    this.m_runButton.setPosition(HMScriptExecutor.BUTTON_MARGIN, buttonY);
    this.m_closeButton.setPosition(contentWidth - UI.Button.DEFAULT_WIDTH - HMScriptExecutor.BUTTON_MARGIN
                                   - HMScriptExecutor.RESIZE_GRIP_SIZE, buttonY);
    this.m_input.setSize("100%", parseInt(editorHeight * HMScriptExecutor.INPUT_HEIGHT_RATIO));
    this.m_output.setSize("100%", parseInt(editorHeight * HMScriptExecutor.OUTPUT_HEIGHT_RATIO));
  },

  run: function()
  {
    this.m_output.setValue("");
    var _this_ = this;
    var code = this.m_input.getValue();
    var val = "";
    var ths = "2459";
    var src = "";
    var url = '/esp/system.htm?sid='+SessionId;
    var pb = "";
    pb += 'string action = "SyntaxCheck";';
    pb += 'string code = ^'+code.replace(/\^/g, '^#\'^\'#^')+'^;';
    pb += 'string val = "'+val+'";';
    pb += 'string ths = "'+ths+'";';
    pb += 'string src = "'+src+'";';
    var opt =
    {
      postBody: ReGa.encode(pb),
      onComplete: function(t)
      {
        // only run the script that passed a completed syntax check
        if( t.status < 200 || t.status >= 300 )
        {
          _this_.m_output.setValue(translateKey("lblError") + ": HTTP " + t.status);
        }
        else if( iseStripAll(t.responseText).length == 0 )
        {
          homematic("ReGa.runScript", {script: code}, function(response, error)
          {
            if (error === null)
            {
              _this_.m_output.setValue(response);
            }
            else
            {
              _this_.m_output.setValue(translateKey("lblError") + ": " + Object.toJSON(error));
            }
          });
        }
        else
        {
          _this_.m_output.setValue(t.responseText);
        }
      }
    };
    new Ajax.Request(url,opt);
  },
  
  close: function()
  {
    disableDialogResize(this.m_frame.getElement());
    Layer.remove(this.m_layer);
  }
  
});

// fixed size of the content area up to the CodeMirror 6 editor (#340), replaced
// by WIDTH_RATIO and HEIGHT_RATIO
// HMScriptExecutor.CONTENT_WIDTH = 800;
// HMScriptExecutor.CONTENT_HEIGHT = 500;

HMScriptExecutor.WIDTH_RATIO  = 0.65; // content area when opened, as part of the
HMScriptExecutor.HEIGHT_RATIO = 0.8;  // browser window
HMScriptExecutor.MIN_WIDTH    = 360;  // smallest size of the dialog (px, with border)
HMScriptExecutor.MIN_HEIGHT   = 300;  // when resized by the grip
HMScriptExecutor.EDITOR_RESERVED_HEIGHT = 70;  // height of the content area not used by the editors
HMScriptExecutor.INPUT_HEIGHT_RATIO     = 0.6; // parts of the remaining height for the
HMScriptExecutor.OUTPUT_HEIGHT_RATIO    = 0.4; // input and the output editor
HMScriptExecutor.BUTTON_MARGIN    = 5;  // gap of the buttons to the left and right edge
HMScriptExecutor.BUTTON_BOTTOM    = 30; // top of the buttons above the bottom of the content area
HMScriptExecutor.RESIZE_GRIP_SIZE = 16; // grip of enableDialogResize(), see
                                        // ".DialogResizable > .ui-resizable-se" in dialog.css
HMScriptExecutor.STORAGE_KEY = "scriptExecutor"; // the size set by the grip, see loadDialogSize()

/**
 * Size of the content area for a size of the frame including its border, as
 * the grip and loadDialogSize() give it.
 **/
HMScriptExecutor.getContentSize = function(frameWidth, frameHeight)
{
  return {
    width:  frameWidth  - (2 * UI.Frame.BORDER_SIZE),
    height: frameHeight - (3 * UI.Frame.BORDER_SIZE) - UI.Frame.TITLE_HEIGHT
  };
};

