/**
 * roomlist.js
 * Raumliste.
 **/
 
/**
 * Raumliste.
 **/
RoomList = Singleton.create({

  /**
   * Konstruktor
   **/
  initialize: function()
  {
    this.addEvent    = new eQ3.Event();
    this.removeEvent = new eQ3.Event();
    
    this.m_rooms = {};
  },
 
  /**
   * Callback. Aktualisiert einen Raum.
   **/
  update: function(data, callback)
  {
    var room;
    
    if (data !== null)
    {
      var id = data["id"];
      room = this.m_rooms[id];
      
      if (typeof(room) != "undefined")
      {
        room.update(data);
      }
      else
      {
        room = new Room(data);
        this.m_rooms[id] = room;
        this.addEvent.fire(this, {room: room});
      }
    }
    
    if (callback) { callback(); }
    return room;
  },
  
  /**
   * Liefert die Liste aller Räume.
   **/
  list: function() 
  { 
    return Object.values(this.m_rooms);
  },
  
  /**
   * Initialisiert die Raumliste.
   **/
  reload: function(loader)
  {
    var _this_   = this;
    var _loader_ = loader;
    
    homematic("Room.getAll", null, function(rooms) {
      if (rooms)
      {
        for (var i = 0, len = rooms.length; i < len; i++)
        {
          _this_.update(rooms[i]);
        }
      }
//      _loader_.reportLoadingState(1);
      _loader_.ready();
    });
  },
  
  /**
   * Startet die Aktualiserung eines Raums.
   **/
  beginUpdate: function(id, callback)
  {
    var _this_ = this;
    var _callback_ = callback;
    
    homematic("Room.get", {"id": id}, function(data) { _this_.update(data, _callback_); });
  },

  /**
   * Ermittelt einen Raum anhand seiner Id.
   **/
  get: function(id)
  {
    var room = this.m_rooms[id];
    if (typeof(room) != "undefined") { return room; }
    else                             { return null; }
  },
  
  remove: function(room)
  {
    var channelIds = room.channelIds.clone();
    
    delete this.m_rooms[room.id];
    channelIds.each(function(id) { room.removeChannel(id); });
    this.removeEvent.fire(this, {room: room});
  }
  
});
