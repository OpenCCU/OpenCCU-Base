/**
 * interfacemonitor.js
 **/
 
/**
 * Überwacht einen Schnittstellenprozess
 **/
InterfaceMonitor = Class.create({
  
  initialize: function(interfaceName)
  {
    this.m_interface = interfaceName;
    this.m_pollHandler = this.poll.bind(this);
    this.m_responseHandler = this.response.bind(this);
    this.m_isRunning = true;
    this.m_failureCount = 0;
    this.storageElm = jQuery("body");
    this.poll();
  },
  
  poll: function()
  {
    if (this.m_isRunning)
    {
      homematic("Interface.isPresent", {"interface": this.m_interface}, this.m_responseHandler);
    }
  },
  
  response: function(result)
  {
    if (this.m_isRunning)
    {
      if (result === true)
      {
        this.m_pollHandler.delay(InterfaceMonitor.INTERVAL);
        this.m_failureCount = 0;
        this.storageElm.data(this.m_interface, true);
      }
      else
      {
        this.m_failureCount++;
        if (this.m_failureCount < InterfaceMonitor.FAILURE_RETRY)
        {
          this.m_pollHandler.delay(InterfaceMonitor.INTERVAL);
          this.storageElm.data(this.m_interface, false);
        }
        else
        {
          new CrashDialog(this.m_interface);
        }
      }
    }
  },
  
  stop: function()
  {
    this.m_isRunning = false;
  }
  
});

InterfaceMonitor.INTERVAL = 30;
InterfaceMonitor.FAILURE_RETRY = 3;
InterfaceMonitor.start = function()
{
  InterfaceMonitor.m_monitors = [];
  
  var result = homematic("Interface.listInterfaces", null);
  if (result)
  {
    result.each(function (iface) {
      InterfaceMonitor.m_monitors.push(new InterfaceMonitor(iface["name"]));
    });
  }
};

InterfaceMonitor.stop = function()
{
  var monitors = InterfaceMonitor.m_monitors.clone();
  monitors.each(function(monitor) { monitor.stop(); });
};
