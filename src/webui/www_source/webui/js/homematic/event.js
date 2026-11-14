/**
 * eQ3.HomeMatic.Event.js
 **/
 
/**
 * @fileOverview Ereignisse von der HomeMatic Zentrale
 * @author F. Werner (eQ-3)
 **/
 
if (!eQ3) { eQ3 = {}; }
if (!eQ3.HomeMatic) { eQ3.HomeMatic = {}; }

eQ3.HomeMatic.Event = Singleton.create({
  
  INTERVALL: 5,
  
  initialize: function()
  {
    this.m_listeners = { };
    this.m_poll = this.poll.bind(this);

    this.m_dispatch = this.dispatch.bind(this);
    
    homematic("Event.subscribe", null);

    if ((typeof preventInterfaceCheck == "undefined") || (! preventInterfaceCheck)) {
      this.m_pe = new PeriodicalExecuter(this.m_poll, this.INTERVALL);
    }
  },
  
  poll: function(pe)
  {
    homematic("Event.poll", null, this.m_dispatch);
  },
  
  dispatch: function(events, error)
  {
    if (!error)
    {
      for (var i = 0, len = events.length; i < len; i++)
      {
        this.fire(events[i]);
      }
    }
  },
  
  stop: function()
  {
    this.m_pe.stop();
    homematic("Event.unsubscribe", null);
  },
  
  subscribe: function(eventType, listener)
  {
    var listeners = this.m_listeners[eventType];
    if (!listeners)
    {
      listeners = [];
      this.m_listeners[eventType] = listeners;
    }
    
    listeners.push(listener); 
  },

  unsubscribe: function(eventType, listener)
  {
    var listeners = this.m_listeners[eventType];
    
    if (listeners)
    {
      this.m_listeners[eventType] = listeners.without(listener);
    }
  },
  
  fire: function(event)
  {
    var type = event.type;
    var data = event.data; 
    var listeners = this.m_listeners[type];
    
    if (listeners)
    {
      listeners.each(function(listener) { listener(type, data); });
    }
  }
  
});