/**
 * button.js
 **/
 
/**
 * Button
 **/
UI.Button = Class.create(UI.Component, {

  /**
   * Konstrukor. Erstellt einen neuen Button
   **/
  initialize: function()
  {
    var _this_ = this;
    
    this.m_isEnabled = true;
    this.m_action    = null;
    
    this.m_element = document.createElement("div");
    this.m_element.className = UI.Button.CLASS_NAME;
    Element.setStyle(this.m_element, {
      width : UI.Button.DEFAULT_WIDTH  + "px",
      height: UI.Button.DEFAULT_HEIGHT + "px"
    });
    
    this.m_text = document.createElement("div");
    this.m_text.className = UI.Button.TEXT_CLASS_NAME;
    this.m_text.appendChild(document.createTextNode(UI.Button.DEFAULT_TEXT));
    Element.setStyle(this.m_text, {
      top: UI.Button.BORDER_SIZE + "px",
      left: UI.Button.BORDER_SIZE + "px",
      width: (UI.Button.DEFAULT_WIDTH  - (2 * UI.Button.BORDER_SIZE)) + "px",
      height: (UI.Button.DEFAULT_HEIGHT - (2 * UI.Button.BORDER_SIZE)) + "px",
      lineHeight: (UI.Button.DEFAULT_HEIGHT - (2 * UI.Button.BORDER_SIZE)) + "px"
    });
    Event.observe(this.m_text, "click", function(event) {
      if (_this_.m_action) { _this_.m_action(event); }
    });
    Event.observe(this.m_text, "mouseover", this._onMouseOver);
    Event.observe(this.m_text, "mouseout" , this._onMouseOut);
    Event.observe(this.m_text, "mousedown", this._onMouseDown);
    Event.observe(this.m_text, "mouseup"  , this._onMouseUp);
    this.m_element.appendChild(this.m_text);
    
  },
  
  /**
   * Setzt den Text des Button
   **/
  setText: function(text)
  {
    this.m_text.innerHTML = "";
    this.m_text.appendChild(document.createTextNode(text));
    return this;
  },

  setId: function(id) {
    this.m_text.id = id;
    return this;
  },

  /**
   * Setzt den OnClick-EventHandler
   **/
  setAction: function(action, context)
  {
    if (typeof(context) != "undefined") { this.m_action = action.bind(context); }
    else                                { this.m_action = action; }
    return this;
  },
	
	_onMouseOver: function(event)
	 {
		//this.className = "UIButtonHighlight";
	 },
	 
	 _onMouseOut: function(event)
	 {
		//this.className = "UIButtonText";
	 },
	 
	 _onMouseDown: function(event)
	 {
		//this.className = "UIButtonPressed";
	 },
	 
	 _onMouseUp: function(event)
	 {
		//this.className = "UIButtonHighlight";
	 }

});

UI.Button.TEXT_CLASS_NAME = "UIButtonText StdButton";
UI.Button.CLASS_NAME      = "UIButton";
UI.Button.HIGHLIGHT_CLASS_NAME = "UIButtonHighlight";
UI.Button.DEFAULT_TEXT    = "UI.Button";
UI.Button.BORDER_SIZE     = 0;
UI.Button.DEFAULT_WIDTH   = 150;
UI.Button.DEFAULT_HEIGHT  = 22; 
