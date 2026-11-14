/*******************************************************************************
 * popup.js
 * Popup-Fenster.
 *
 * Autor      : Falk Werner
 * Erstellt am: 05.08.2008
 ******************************************************************************/
 
Popup = function(width, height, overflow)
{
  /*##########################################################################*/
  /*# Private Attribute                                                      #*/
  /*##########################################################################*/
  
  var m_id         = "popup_" + Popup.Level;
  var m_wrapperId  = m_id + "_wrapper";
  var m_titlebarId = m_id + "_titlebar";
  var m_contentId  = m_id + "_content";
  var m_footerId   = m_id + "_footer";
  var m_working    = false;
   
  /*##########################################################################*/
  /*# Öffentliche Methoden                                                   #*/
  /*##########################################################################*/
  
  /*****************************************************************************
   * Read-Only. Id des Popups.
   ****************************************************************************/
  this.Id = function() { return m_id; };
  
  this.Working = function(working)
  {
    if ($(m_id)) 
    {
      if ("undefined" != working) { m_working = working; }
      if (true === m_working)     { $(m_id).style.cursor = "wait"; }
      else                        { $(m_id).style.cursor = "default"; }
    }
    return m_working;
  };
  
  /*****************************************************************************
   * Zeigt das Popup an.
   ****************************************************************************/
  this.show = function()
  {
    if ($(m_wrapperId)) { $(m_wrapperId).show(); }
  };
  
  /*****************************************************************************
   * Schließt das Popup.
   ****************************************************************************/
  this.close = function()
  {
    if ($(m_wrapperId))
    {
      // entferne Wrapper 
      // --> $(m_wrapperId).remove() funktioniert beim IE7 nicht
      var container = document.getElementById(Popup.ContainerId);
      var wrapper   = document.getElementById(m_wrapperId);
      container.removeChild(wrapper);
      
      Popup.Z_Index -= Popup.Z_INDEX_INCR;
      Popup.Level--;
      if (0 >= Popup.Level) { $(Popup.ContainerId).hide(); }
    }
  };
  
  /*****************************************************************************
   * Write-Only. Setzt den Text der Titelzeile.
   ****************************************************************************/
  this.Titlebar = function(value)
  {
    if ($(m_titlebarId)) { $(m_titlebarId).innerHTML = value; }
  };
  
  /*****************************************************************************
   * Write-Onyl. Setzt den Inhalt des Popups. Mit dem ersten Setzen wird der
   * Mauscurosr wieder normal und das Ladebild verschwindet.
   ****************************************************************************/
  this.Content = function(value)
  {
  //  if ($(m_id))        { $(m_id).style.cursor    = "default"; }
    this.Working(false);
    if ($(m_contentId)) { $(m_contentId).innerHTML = value; }
  };
  
  /*****************************************************************************
   * Fügt einen Button in der Fußzeile des Popups hinzu.
   ****************************************************************************/
  this.addButton = function(id, name, script, style)
  {
    if ($(m_footerId))
    {
      var          buttonHTML  = "<div class='popup_button'";
                   buttonHTML += " onclick='" + script + "'";
      if (id)    { buttonHTML += " id='"      + id     + "'"; }
      if (style) { buttonHTML += " style='"   + style  + "'"; }
                   buttonHTML += ">"          + name   + "</div>";
      $(m_footerId).innerHTML += buttonHTML;
    } 
  };
  
  /*##########################################################################*/
  /*# Initialisierung                                                        #*/
  /*##########################################################################*/
  
  var popupWidth  = Popup.computeWidth(width);
  var popupHeight = Popup.computeHeight(height);
  var data =
  {
    id           : m_id,
    popupWidth   : popupWidth,
    marginLeft   : parseInt(-popupWidth/2),
    marginTop    : parseInt(-popupHeight/2),
    contentWidth : Popup.computeContentWidth(popupWidth),
    contentHeight: Popup.computeContentHeight(popupHeight),
    overflow     : overflow,
    z_index      : Popup.z_index
  };
  Popup.Z_Index += Popup.Z_INDEX_INCR;
  Popup.Level++;
  
  $(Popup.ContainerId).innerHTML += Popup.Template.process(data);
  $(Popup.ContainerId).show(); 
  
};

/*############################################################################*/
/*# Konstanten                                                               #*/
/*############################################################################*/

//Popup.TEMPLATE_FILE     = "webui/jst/popup.jst";
Popup.TEMPLATE_FILE     = "/webui/jst/popup.jst";
Popup.ContainerId       = "popup_container";
Popup.Z_INDEC_INCR      =  10;
Popup.MIN_MARGIN_SIZE   =  25;
Popup.BORDER_SIZE       =   1;
Popup.INNER_BORDER_SIZE =   1;
Popup.TITLEBAR_HEIGHT   =  20;
Popup.FOOTER_HEIGHT     =  50;
Popup.MIN_HEIGHT        = 240;
Popup.MIN_WIDTH         = 320;

/*############################################################################*/
/*# Statische Attribute                                                      #*/
/*############################################################################*/

Popup.Z_Index  = 9;
Popup.Level    = 0;
Popup.Template = "";

/*############################################################################*/
/*# Statische Methoden                                                       #*/
/*############################################################################*/

/*******************************************************************************
 * Berechnet die Breite des Popups aus der gewünschten Breite des Arbeits-
 * bereichs des Popups.
 ******************************************************************************/
Popup.computeWidth = function(width)
{
  var maxWidth  = WebUI.getWidth();
      maxWidth -= (2 * Popup.MIN_MARGIN_SIZE);
      maxWidth -= (2 * Popup.BORDER_SIZE);
  var minWidth  = Popup.MIN_WIDTH;
  
  if (maxWidth < minWidth)                    { maxWidth = minWidth; }
  if (("max" == width) || (maxWidth < width)) { width    = maxWidth; }
  
  return width;
};

/*******************************************************************************
 * Berechnet die Höhe des Popups aus der gewünschten Höhe des Arbeits-
 * bereichs des Popups.  
 ******************************************************************************/
Popup.computeHeight = function(height)
{
  var maxHeight  = WebUI.getHeight();
      maxHeight -= (2 * Popup.MIN_MARGIN_SIZE);
      maxHeight -= (2 * Popup.BORDER_SIZE);
  var minHeight  = Popup.MIN_HEIGHT;

  if (maxHeight < minHeight)                     { maxHeight = minHeight; }
  if (("max" == height) || (maxHeight < height)) { height    = maxHeight; }
  
  return height;
};

/*******************************************************************************
 * Berechnet die Breite des Arbeitsbereichs aus der tatsächlichen Breite des
 * Popups.
 ******************************************************************************/
Popup.computeContentWidth = function(popupWidth)
{
  var contentWidth  = popupWidth;
      contentWidth -= (2 * Popup.INNER_BORDER_SIZE);
      
  return contentWidth;
};

/*******************************************************************************
 * Berechnet die Höhe des Arbeitsbereichs aus der tatsächlichen Höhe des Popups.
 ******************************************************************************/
Popup.computeContentHeight = function(popupHeight)
{
  var contentHeight  = popupHeight;
      contentHeight -= Popup.TITLEBAR_HEIGHT;
      contentHeight -= Popup.FOOTER_HEIGHT;
      contentHeight -= (6 * Popup.INNER_BORDER_SIZE);
      
  return contentHeight;
};

/*############################################################################*/
/*# Einsprungpunkt                                                           #*/
/*############################################################################*/

Popup.Template = TrimPath.parseTemplate(HttpLoader.getText(Popup.TEMPLATE_FILE));


