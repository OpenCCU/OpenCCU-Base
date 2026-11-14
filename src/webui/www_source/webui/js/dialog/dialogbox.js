/**
 * dialogbox.js
 * Autor: Falk Werner, eQ-3 Entwicklung GmbH
 **/
 
/**
 * Allgmeine Dialogbox für die Kommunikation mit dem Benutzer.
 **/
DialogBox = function(callback, width, height)
{
  /*####################*/
  /*# Private Elemente #*/
  /*####################*/
 
  var DEFAULT_CONTENT_WIDTH  = 320; // Konstante. Standardbreite des Arbeitsbereichs
  var DEFAULT_CONTENT_HEIGHT = 80;  // Konstante. Standardhöhe des Arbeitsbereichs
  var TITLE_HEIGHT           = 20;  // Konstante. Höhe der Titelleiste  
  var FOOTER_HEIGHT          = 40;  // Konstante. Höhe der Fußleiste
  var BORDER_WIDTH           = 2;   // Konstante. Breite des Dialog-Rahmens
  var BUTTON_HEIGHT          = 32;  // Konstante. Höhe eines Buttons
  var BUTTON_WIDTH           = 100; // Konstante. Breite eines Buttons
  var BUTTON_BORDER          = 1;   // Konstante. Rahmenbreite eines Buttons
 
  var m_wrapper;              // DOM-Element. Dialog-Hintergrund
  var m_dialog;               // DOM-Element. Dialogfenster
  var m_title;                // DOM-Element. Dialog-Titel
  var m_content;              // DOM-Element. Inhalt
  var m_footer;               // DOM-Element. Fußleiste
  var m_callback = callback;  // Rückruffunktion
    
  /**
   * Interne Klasse.
   **/
  var DialogBoxButton = function(caption, dialogResult, style)
  {    
    /*####################*/
    /*# Private Elemente #*/
    /*####################*/
    
    var m_button;                       // DOM-Element. Button
    var m_caption;                      // DOM-Element. Beschriftung
    var m_onClickListener;              // onClick-Ereignis
    var m_dialogResult = dialogResult;  // Rückgabewert
    
    /**
     * onClick-Ereignis-Handler.
     **/
    var onClick = function()
    {
      if (m_callback) { m_callback(dialogResult); }
    };
    
    /*########################*/
    /*# Öffentliche Elemente #*/
    /*########################*/
    
    /**
     * Setzt die Beschriftung des Buttons.
     **/
    this.setCaption = function(caption)
    {
      if ("undefined" != typeof(caption))
      {
        m_caption.innerHTML = "";
        m_caption.appendChild(document.createTextNode(caption));
      }  
    };
    
    /**
     * Ermittelt das DOM-Element.
     **/
    this.getElement = function()
    {
      return m_button;
    };
    
    /*###################*/
    /*# Initialisierung #*/
    /*###################*/
    
    // Button-Style
    if ("undefined" == typeof(style["width"])) { style["width"] = BUTTON_WIDTH + "px"; }
    if ("undefined" == typeof(style["float"])) { style["float"] = "left"; }
    style["height"] = BUTTON_HEIGHT + "px";
    style["margin"] = parseInt((FOOTER_HEIGHT - BUTTON_HEIGHT) / 2) + "px";
    
    // Button-Beschriftung
    m_caption = document.createElement("div");
    Element.addClassName(m_caption, "DialogButtonCaption");
    Element.setStyle(m_caption, 
    {
      "margin"    : BUTTON_BORDER + "px",
      "lineHeight": (BUTTON_HEIGHT - (2 * BUTTON_BORDER)) + "px"
    });
    m_caption.appendChild(document.createTextNode(caption));
      
    // Button-Element
    var margin = parseInt((FOOTER_HEIGHT - BUTTON_HEIGHT - (2 * BUTTON_BORDER)) / 2);
    m_button = document.createElement("div");
    Element.addClassName(m_button, "DialogButton");
    Element.setStyle(m_button, style);
    m_button.appendChild(m_caption);    
  
    // Event-Handler
    m_onClickListener = onClick.bindAsEventListener(this);
    Event.observe(m_caption, "click", m_onClickListener);
  };
  
  /*########################*/
  /*# Öffentliche Elemente #*/
  /*########################*/
  
  /**
   * Zeigt die Dialogbox an.
   **/
  this.show = function()
  {
    Layer.add(m_wrapper);
    m_wrapper.show();
  };
  
  /**
   * Schließt die Dialogbox und entfernt sie aus dem DOM.
   **/
  this.close = function()
  {
    Layer.remove(m_wrapper);
  };
    
  /**
   * Setzt den Titel der Dialogbox.
   **/
  this.setTitle = function(title)
  {
    m_title.innerHTML = "";
    m_title.appendChild(document.createTextNode(title));
  };
  
  /**
   * Setzt den Inhalt der Dialogbox mit einem HTML-Text.
   **/
  this.setContentHTML = function(contentHTML)
  {
    m_content.innerHTML = contentHTML;
  };
  
  /**
   * Setzt den Inhalt der Dialogbox mit einem DOM-Element.
   **/
  this.setContent = function(content)
  {
    m_content.innerHTML = "";
    m_content.appendChild(content);
  };
  
  /**
   * Fügt dem Dialog einen Button hinzu.
   **/
  this.addButton = function(caption, dialogResult, style)
  {
    var button = new DialogBoxButton(caption, dialogResult, style);
    m_footer.appendChild(button.getElement());    
    
    return button;
  };
    
  /*###################*/
  /*# Initialisierung #*/
  /*###################*/

  var contentHeight = DEFAULT_CONTENT_HEIGHT;
  var contentWidth  = DEFAULT_CONTENT_WIDTH;
  if ("undefined" != typeof(height)) { contentHeight = height; }
  if ("undefined" != typeof(width )) { contentWidth  = width;  }
  
  var dialogWidth  = contentWidth + (2 * BORDER_WIDTH);
  var dialogHeight = contentHeight + TITLE_HEIGHT + FOOTER_HEIGHT + (4 * BORDER_WIDTH); 
  
  // Titelleiste
  m_title = document.createElement("div");
  Element.addClassName(m_title, "DialogBoxTitle");
  Element.setStyle(m_title, 
  {
    "top"   : BORDER_WIDTH + "px",
    "left"  : BORDER_WIDTH + "px",
    "width" : contentWidth + "px",
    "height": TITLE_HEIGHT + "px"
  });
  
  // Arbeitsbereich
  m_content = document.createElement("div");
  Element.addClassName(m_content, "DialogBoxContent");
  Element.setStyle(m_content, 
  {
    "top"   : TITLE_HEIGHT + (2 * BORDER_WIDTH) + "px",
    "left"  : BORDER_WIDTH + "px",    
    "width" : contentWidth + "px",
    "height": contentHeight + "px"
  });
  
  // Fußleiste
  m_footer = document.createElement("div");
  Element.addClassName(m_footer, "DialogBoxFooter");
  Element.setStyle(m_footer, 
  {
    "top"   : TITLE_HEIGHT + contentHeight + (3 * BORDER_WIDTH) + "px",
    "left"  : BORDER_WIDTH + "px",
    "width" : contentWidth + "px",
    "height": FOOTER_HEIGHT + "px"
  });
  
  // Dialog
  m_dialog = document.createElement("div");
  Element.addClassName(m_dialog, "DialogBox");
  Element.setStyle(m_dialog,   
  {
    "top"   : parseInt((WebUI.getHeight() - dialogHeight) / 2) + "px",
    "left"  : parseInt((WebUI.getWidth()  - dialogWidth)  / 2) + "px",
    "width" : dialogWidth + "px",
    "height": dialogHeight + "px"
  });
  m_dialog.appendChild(m_title);
  m_dialog.appendChild(m_content);
  m_dialog.appendChild(m_footer);
  new Draggable(m_dialog);
  
  // Dialog-Rahmen
  m_wrapper = document.createElement("div");
  Element.addClassName(m_wrapper, "DialogBoxWrapper");
  m_wrapper.appendChild(m_dialog);  
};
 
/**
 * DialogResult
 * Vordefinierte Rückgabewerte der DialogBox.
 **/ 
DialogResult = 
{
  "ABORT" : -1,    
  "CANCEL": -2,   
  "IGNORE": -3,
  "NO"    : -4,
  "NONE"  : -5,
  "OK"    : -6,
  "RETRY" : -7,
  "YES"   : -8
};

