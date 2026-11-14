/**
 * scrollpane.js
 **/
 
UI.ScrollPane = Class.create(UI.Container, {
  
  /**
   * Konstruktor. Erstellt eine neue ScrollPane
   **/
  initialize: function()
  {
    this.m_isEnabled = true;
    this.m_components = [];
  
    this.m_element = document.createElement("div");
    this.m_element.className = UI.ScrollPane.CLASS_NAME;
    
    this.m_content = document.createElement("div");
    this.m_content.className = UI.ScrollPane.CONTENT_CLASS_NAME;
    this.m_element.appendChild(this.m_content);
  
    this.setWidth(UI.ScrollPane.DEFAULT_WIDTH)
      .setHeight(UI.ScrollPane.DEFAULT_HEIGHT);
  },
  
  /**
   * Setzt die Breite der Komponente in Pixeln
   **/
  setWidth: function(width)
  {
    Element.setStyle(this.m_element, {
      width: parseInt(width) + "px"
    });
    Element.setStyle(this.m_content, {
      width: parseInt(width) + "px"
    });
    return this;
  },
  
  /**
   * Setzt die Höhe der Komponente in Pixeln
   **/
  setHeight: function(height)
  {
    Element.setStyle(this.m_element, {
      height: parseInt(height) + "px"
    });
    Element.setStyle(this.m_content, {
      height: parseInt(height) + "px"
    });
    return this;
  }
  
});

UI.ScrollPane.CLASS_NAME = "UIScrollPane";
UI.ScrollPane.CONTENT_CLASS_NAME = "UIScrollPaneContent";
UI.ScrollPane.DEFAULT_WIDTH  = 100;
UI.ScrollPane.DEFAULT_HEIGHT = 100;

