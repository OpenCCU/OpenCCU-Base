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
    var frameWidth   = parseInt(screenWidth * 0.8);
    var frameHeight  = parseInt(screenHeight * 0.8);
    var frameX       = parseInt((screenWidth  - frameWidth)  / 2);
    var frameY       = parseInt((screenHeight - frameHeight) / 2);

    HMScriptExecutor.TITLE = translateKey("dialogScriptExecuterTitle");
    HMScriptExecutor.TEXT_RUN = translateKey("dialogScriptExecuterBtnExecute");
    HMScriptExecutor.TEXT_CLOSE = translateKey("dialogScriptExecuterBtnClose");
    
    this.m_layer = document.createElement("div");
    this.m_layer.className = "DialogLayer";

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
      .add(new UI.Button()
        .setPosition(5, frameHeight-30)
        .setText(HMScriptExecutor.TEXT_RUN)
        .setAction(this.run, this)
      )
      .add(new UI.Button()
        .setPosition(frameWidth-155, frameHeight-30)
        .setText(HMScriptExecutor.TEXT_CLOSE)
        .setAction(this.close, this)
      );
      
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
    this.m_input.setSize("100%", parseInt((this.m_frame.getContentHeight()-70) * 0.6));

    this.m_output = CCUCodeMirror6.fromTextArea(document.getElementById('output'), {
      mode: "default",
      readOnly: true,
      lineWrapping: true,
      lineNumbers: false
    });
    this.m_output.setSize("100%", parseInt((this.m_frame.getContentHeight()-70) * 0.4));
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
        if( iseStripAll(t.responseText).length == 0 )
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
    Layer.remove(this.m_layer);
  }
  
});

HMScriptExecutor.CONTENT_WIDTH = 800;
HMScriptExecutor.CONTENT_HEIGHT = 500;

