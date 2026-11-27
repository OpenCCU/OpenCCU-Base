jQuery.extend(true,langJSON, {
  "de" : {
    "HelpTitle" : "Information",
    "FD_CYCLIC_TIMEOUT" :
      "Die Intervalle, in denen die CCU ein Telegramm erwartet, sind in der jeweils dem Ger%E4t zugeh%F6rigen XML-Datei (/firmware/rftypes) zu finden.<br/><br/>Beispiel:<br/><i>&lt;device version='1' rx_modes='...' cyclic_timeout='600'&gt;</i><br/>= mind. alle 600 Sekunden erwartet die CCU ein Telegramm<br/><br/>Weitere Informationen gibt es auf der <a target='_blank' rel='noopener noreferrer' href='https://github.com/jp112sdl/HB-UNI-Sen-DUMMY-BEACON-V2#cyclic_timeout-werte-der-homematic-ger%E4te'>Projektseite im Github</a>.",
    "FD_DEVICE_TYPE" :
      "<b>Aktor:</b><br/>Der DUMMY-BEACON quittiert empfangene Schaltbefehle an das zu simulierende Ger%E4t mit einem <i>ACK</i><br/><br/><b>Sensor:</b><br/>Der DUMMY-BEACON sendet zyklisch ein Telegramm an die CCU."  
  },
  "en" :{
    "HelpTitle" : "Information",
    "FD_CYCLIC_TIMEOUT" :
      "The intervals in which the CCU expects a message are given in the XML file of the respective device (/firmware/rftypes).<br/><br/>Example:<br/><i>&lt;device version='1' rx_modes='...' cyclic_timeout='600'&gt;</i><br/>= the CCU expects a message at least every 600 seconds<br/><br/>More information can be found on the <a target='_blank' rel='noopener noreferrer' href='https://github.com/jp112sdl/HB-UNI-Sen-DUMMY-BEACON-V2#cyclic_timeout-werte-der-homematic-ger%E4te'>project page on GitHub</a>.",
    "FD_DEVICE_TYPE" :
      "<b>Actuator:</b><br/>The DUMMY-BEACON acknowledges switching commands received for the simulated device with an <i>ACK</i>.<br/><br/><b>Sensor:</b><br/>The DUMMY-BEACON cyclically sends a message to the CCU."  
  }
});