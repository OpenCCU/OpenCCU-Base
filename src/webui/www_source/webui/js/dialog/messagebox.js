/**
 * messagebox.js
 * Autor: Falk Werner, eQ-3 Entwicklung GmbH
 **/
 

MessageBox = Class.create({

  initialize: function()
  {
    var screenWidth  = WebUI.getWidth();
    var screenHeight = WebUI.getHeight();
    var frameWidth   = MessageBox.DEFAULT_CONTENT_WIDTH + 20;
    var frameHeight  = MessageBox.DEFAULT_CONTENT_HEIGHT + 50;
    var frameX       = parseInt((screenWidth  - frameWidth)  / 2);
    var frameY       = parseInt((screenHeight - frameHeight) / 2);

    this.msgBox;
    this.screenWidth = screenWidth;
    this.screenHeight = screenHeight;
    this.m_action = null;
    this.frameWidth = frameWidth;
    this.frameHeight = frameHeight;
    
    this.m_layer = document.createElement("div");

    this.m_layer.className = "DialogLayer";
    
    this.m_content = new UI.Text()
      .setText("")
      .setPosition(10, 10)
      .setWidth(this.frameWidth - 20)
      .setHeight(this.frameHeight - 50);
      
    this.m_okButton = new UI.Button()
      .setPosition(parseInt((this.frameWidth - 150) / 2), this.frameHeight - 30)
      .setText("OK")
      .setAction(this.ok, this);
      
    this.m_frame = new UI.Frame()
      .setTitle("Info")
      .setContentSize(this.frameWidth, this.frameHeight)
      .setPosition(frameX, frameY)
      .add(this.m_content)
      .add(this.m_okButton);
      
    this.m_layer.appendChild(this.m_frame.getElement());
    Layer.add(this.m_layer);
  
  },

  setId: function(id)
  {
    this.m_layer.id = id;
    return this;
  },

  centerBarGraph: function(id)
  {
     jQuery("#" + id).css("padding-left" , parseInt((this.frameWidth - 150) / 2));
  },

  setTitle: function(title)
  {
    this.m_frame.setTitle(title);
    return this;
  },
  
  setContentSize: function(width, height)
  {
    this.m_content.setWidth(width);
    this.m_frame.setContentSize(parseInt(width) + 20, parseInt(height) + 50);
    this.m_content.setWidth(width);
    this.m_content.setHeight(height);
    this.m_okButton.setPosition(parseInt(((width - 140) / 2)), parseInt(height) + 10);
    this.m_frame.setPosition(parseInt((this.screenWidth  - width)  / 2),parseInt((this.screenHeight - this.m_frame.getHeight()) / 2) );
    return this;
  },

  hideOkButton: function()
  {
    jQuery(".UIButton").hide();
  },

  setText: function(text)
  {
    this.m_content.setText(text);
    return this;
  },

  centerText: function()
  {
    jQuery(".UIText").addClass("alignCenter");
  },

  setHtml: function(html)
  {
    this.m_content.setHtml(html);
    return this;
  },
  
  setAction: function(action, context)
  {
    if (typeof(context) != "undefined") { this.m_action = action.bind(context); }
    else { this.m_action = action; }
    
    return this;
  },
  
  ok: function()
  {
    Layer.remove(this.m_layer);
    if (typeof this.m_action == "function") { this.m_action(); }
  },

  setOkButtonText: function(lbl) {
    this.m_okButton.setText(lbl);
  }

});

MessageBox.DEFAULT_CONTENT_HEIGHT = 80;
MessageBox.DEFAULT_CONTENT_WIDTH  = 320; 

MessageBox.show = function(title, content, callback, width, height, id, barGraphId, btnText)
{
  this.msgBox = new MessageBox();
  this.msgBox.setTitle(title);
  this.msgBox.setHtml(content);
    
  translatePage(".DialogLayer");
  
  if (callback && callback != "")
  { 
    this.msgBox.setAction(callback);
  }

  if ((width && width != "") && (height && height != ""))
  {
    this.msgBox.setContentSize(width, height);
  }

  if (typeof btnText != "undefined") {
    this.msgBox.setOkButtonText(btnText);
  }

  if (id) {
    this.msgBox.setId(id);
  }

  if (barGraphId) {
    this.msgBox.hideOkButton();
    this.msgBox.centerBarGraph(barGraphId);
  }

};

MessageBox.setText = function(txt) {
  this.msgBox.setText(txt);
};

MessageBox.centerText = function() {
  this.msgBox.centerText();
};

MessageBox.setHtml = function(html) {
  this.msgBox.setHtml(html);
};

MessageBox.close = function() {
  this.msgBox.ok();
};

MessageBox.setContentSize =function(width, height) {
  this.msgBox.setContentSize(width, height);
};
