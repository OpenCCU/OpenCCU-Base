/**
 * regamonitor.js
 **/
 
/**
 * Überwacht die Logikschicht "ReGa" und gibt eine Meldung, falls diese sich nicht meldet.
 **/
ReGaMonitor = Class.create({
  
  initialize: function()
  {
    this.m_pollHandler     = this.poll.bind(this);
    this.m_responseHandler = this.response.bind(this);
    this.m_isRunning = true;
    this.m_failureCount = 0;
    this.poll();
  },
  
  poll: function()
  {
    if (this.m_isRunning)
    {
      homematic("ReGa.isPresent", null, this.m_responseHandler);
    }
  },
  
  response: function(result)
  {
    if (this.m_isRunning)
    {
      if (result === true)
      {
        this.m_pollHandler.delay(ReGaMonitor.INTERVAL);
        this.m_failureCount = 0;
      }
      else
      {
        this.m_failureCount++;
        if (this.m_failureCount < ReGaMonitor.FAILURE_RETRY)
        {
          this.m_pollHandler.delay(ReGaMonitor.INTERVAL);
        }
        else
        {
          new CrashDialog("ReGa");
        }
      }
    }
  },
  
  stop: function()
  {
    this.m_isRunning = false;
  }
  
});

ReGaMonitor.INTERVAL = 30;
ReGaMonitor.FAILURE_RETRY = 3;
