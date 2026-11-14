/**
 * drag.js
 * Drag & Drop
 **/
 
Drag = function(element, event, callback)
{

  var m_x = (document.all) ? window.event.clientX : event.pageX;
  var m_y = (document.all) ? window.event.clientY : event.pageY;
  var m_element  = element; 
  var m_callback = callback;
  var m_top      = m_y - element.offsetTop;      
  var m_left     = m_x - element.offsetLeft;      
  
  /**
   * Callback. Wird beim Bewegen der Maus aufgerufen
   **/
  var onMouseMove = function(event)
  {
    m_x = (document.all) ? window.event.clientX : event.pageX;
    m_y = (document.all) ? window.event.clientY : event.pageY;
    
    if (m_element !== null)
    {
      Element.setStyle(m_element, {
        top : (m_y - m_top ) + "px",
        left: (m_x - m_left) + "px"
      });
    }
  };
  
  /**
   * Callback. Wird beim Loslassen der Maus aufgerufen
   **/
  var onMouseUp = function(event)
  {
    var x = (document.all) ? window.event.clientX : event.pageX;
    var y = (document.all) ? window.event.clientY : event.pageY;
    
    Event.stopObserving(document, "mousemove", onMouseMove);
    Event.stopObserving(document, "mouseip", onMouseUp);
    
    if (m_callback) { m_callback(m_element, x, y); }
  };
  
  Element.absolutize(m_element);
  Element.setStyle(m_element, {marginTop: "0px", marginLeft: "0px"}); // BugFix: relativ positionierte Elemente werden über margin ausgerichtet, absolutize() setzz dies jedoch nicht zurück
  Element.observe(document, "mousemove", onMouseMove);
  Element.observe(document, "mouseup", onMouseUp);

};
