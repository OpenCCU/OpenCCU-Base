/**
 * frame.js
 **/

/**
 * Rahmen für Dialog-Fenster
 **/ 
UI.Frame = Class.create(UI.Container, {

  initialize: function()
  {
    var _this_ = this;
  
    this.m_isEnabled  = true; 
    this.m_components = [];
  
    this.m_contentWidth  = UI.Frame.DEFAULT_CONTENT_WIDTH;
    this.m_contentHeight = UI.Frame.DEFAULT_CONTENT_HEIGHT;

    this.m_element = document.createElement("div");
    this.m_element.className = UI.Frame.CLASS_NAME;
  
    this.m_title = document.createElement("div");
    this.m_title.className = UI.Frame.TITLE_CLASS;
    this.m_title.appendChild(document.createTextNode(UI.Frame.DEFAULT_TITLE));
    Event.observe(this.m_title, "mousedown", function(event) {
        new Drag(_this_.m_element, event); 
    });
    this.m_element.appendChild(this.m_title);
  
    this.m_content = document.createElement("div");
    this.m_content.className = UI.Frame.CONTENT_CLASS;
    this.m_element.appendChild(this.m_content);
  
    this.m_resize();
  },

  /**
   * Aktualisiert die Abmessungen des Frames
   **/
  m_resize: function()
  {
    this.m_width  = this.m_contentWidth  + (2 * UI.Frame.BORDER_SIZE);
    this.m_height = this.m_contentHeight + (3 * UI.Frame.BORDER_SIZE) + UI.Frame.TITLE_HEIGHT;
    
    Element.setStyle(this.m_element, {
      width : this.m_width  + "px",
      height: this.m_height + "px"
    });
    Element.setStyle(this.m_title, {
      top   : UI.Frame.BORDER_SIZE  + "px",
      left  : UI.Frame.BORDER_SIZE  + "px",
      width : this.m_contentWidth   + "px",
      height: UI.Frame.TITLE_HEIGHT + "px",
      lineHeight: UI.Frame.TITLE_HEIGHT + "px"
    });
    Element.setStyle(this.m_content, {
      top   : ((2 * UI.Frame.BORDER_SIZE) + UI.Frame.TITLE_HEIGHT) + "px",
      left  : UI.Frame.BORDER_SIZE + "px",
      width : this.m_contentWidth  + "px",
      height: this.m_contentHeight + "px"
    });
    
    return this;
  },
  
  /**
   * Setzt die Höhe und Breite des Content-Bereichs.
   **/
  setContentSize: function(contentWidth, contentHeight)
  {
    this.m_contentWidth  = parseInt(contentWidth);
    this.m_contentHeight = parseInt(contentHeight);
    
    this.m_resize();
    return this;
  },
  
  /**
   * Liefert die Breite des Content-Bereichs in Pixeln
   **/
  getContentWidth: function()
  {
    return this.m_contentWidth;
  },
  
  /**
   * Liefert die Höhe des Content-Bereichs in Pixeln
   **/
  getContentHeight: function()
  {
    return this.m_contentHeight;
  },
  
  /**
   * Liefert die Gesamtbreite des Frames in Pixlen
   **/
  getWidth: function()
  {
    return this.m_width;
  },
  
  /**
   * Liefert die Gesamthöhe des Frames in Pixeln
   **/
  getHeight: function()
  {
    return this.m_height;
  },
  
  /**
   * Setzt den Titel
   **/
  setTitle: function(title)
  {
    this.m_title.innerHTML = "";
    this.m_title.appendChild(document.createTextNode(title));
    return this;
  },
  
  _enable: function()
  {
    this.m_isEnabled = true;
    Element.removeClassName(this.m_element, UI.Frame.CLASS_NAME_DISABLED);
    Element.removeClassName(this.m_title, UI.Frame.TITLE_CLASS_DISABLED);
    Element.removeClassName(this.m_content, UI.Frame.CONTENT_CLASS_DISABLED);
    return this;
  },
  
  _disable: function()
  {
    this.m_isEnabled = false;
    Element.addClassName(this.m_element, UI.Frame.CLASS_NAME_DISABLED);
    Element.addClassName(this.m_title, UI.Frame.TITLE_CLASS_DISABLED);
    Element.addClassName(this.m_content, UI.Frame.CONTENT_CLASS_DISABLED);
    return this;
  }
  
}); 

/**
 * Konstanten
 **/
UI.Frame.DEFAULT_CONTENT_WIDTH  = 320; 
UI.Frame.DEFAULT_CONTENT_HEIGHT = 240;
UI.Frame.DEFAULT_TITLE = "UI.Frame";
UI.Frame.TITLE_HEIGHT = 20;
UI.Frame.BORDER_SIZE = 2;
UI.Frame.CLASS_NAME    = "UIFrame";
UI.Frame.TITLE_CLASS   = "UIFrameTitle";
UI.Frame.CONTENT_CLASS = "UIFrameContent";
UI.Frame.CLASS_NAME_DISABLED = "UIFrameDisabled";
UI.Frame.TITLE_CLASS_DISABLED = "UIFrameTitleDisabled";
UI.Frame.CONTENT_CLASS_DISABLED = "UIFrameContentDisabled";
