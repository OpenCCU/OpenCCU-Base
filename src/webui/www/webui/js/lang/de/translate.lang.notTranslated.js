// ä=%E4 ö=%F6 ü=%FC Ä=%C4 Ö=%D6 Ü=%DC ß=%DF

jQuery.extend(true, langJSON, {
  "de": {
    "lblCampfire" : "Lagerfeuer",
    "lblRainbow" : "Regenbogen",
    "lblSunrise" : "Sonnenaufgang",
    "lblSunset" : "Sonnenuntergang",
    "lblWaterfall" : "Wasserfall",
    "lblForest" : "Wald",
    "lblEffect7" : "Effekt 7",
    "lblEffect8" : "Effekt 8",
    "lblRedFlashing" : "rotes Blinken",
    "lblGreenWobble" : "gr%FCnes Wabern",

    "lblDisplayModeETRV" : "Displaymodus",
    "optionReducedMode" : "Reduzierter Modus",
    "optionFunctionalMode" : "Funktioneller Modus",

    "lblDisplayColor" : "Displayfarbe",
    "optionNormalColors" : "Normal",
    "optionInvertedColors" : "Invertiert",

    "optionAllowed" : "Berechtigt",
    "optionNotAllowed" : "Nicht berechtigt",

    "lblPermission" : "Berechtigung",

    "stringTablePermissionStateFalse" : "Kanal nicht berechtigt",
    "stringTablePermissionStateTrue" : "Kanal berechtigt",

    "lblAngle" : "Winkel",

    "lblInfoStatusControlDeviceFeedIn" : "Einspeisung Z%E4hler Ger%E4t",
    "lblInfoStatusControlFeedInCounter" : "Einspeisung Z%E4hler CCU",

    "hintHeatingCoolngNotAllowed" :
      "Die Anzeigefelder Heizen / K%FChlen zeigen den aktuell eingestellten Modus an.<br/><br/>" +
      "Ist <b><u>keine</u></b> Verkn%FCpfung zwischen einem HmIP Wandthermostaten und einem HmIP Fu%DFbodenheizungscontroller vorhanden, " +
      "l%E4%DFt sich der Modus hiermit aktiv wechseln.<br/><br/>" +
      "<b>In Heizungsgruppen dienen die beiden Felder nur zur Anzeige des eingestellten Modus. " +
      "Hier ist ein Wechsel nicht m%F6glich.</b>",

    "stringTableAbsoluteAngleStatus" : "Status",

    "stringTableAltitude" : "H%F6he %FCber NN",
    "stringTableAirPressure" : "Luftdruck",
    "lblMeter" : "Meter",

    "optionOneBtn" : "1 Taster",
    "optionTwoBtnLeftRRight" : "2 Taster links/rechts",
    "optionTwoBtnUpDDown" : "2 Taster oben/unten",
    "optionFourBtn" : "4 Taster",

    "lblHeaderFeedbackLevelMode" : "Feedback Helligkeit (Helligkeit der bet%E4tigten Taste)",
    "lblFeedbackLevelMode" : "Feedback Mode",
    "lblFeedbackLevelValue" : "Level Feedback",

    "lblHeaderActivateLevelMode" : "Aktive Helligkeit",

    "lblActivateLevelMode" : "Level Mode",
    "lblActivateLevelValue" : "Level aktive Helligkeit",
    "optionAbsolut" : "Absolut",
    "optionRelativ" : "Relativ",

    "stringTableActivateLevelOnTime" : "Einheit Einschaltdauer",

    "stringTableErrorTempSensorFalse" : "Temperatursensor Ok",
    "stringTableErrorTempSensorTrue" : "Temperatursensor nicht Ok",

    "lblDisplayLayoutMode" : "Anzeigelayout",

    "hintPrgExists_WGT" : "Es besteht mindestens ein Programm. Daher ist die Umschaltung des Modus (Kanal 4) gesperrt.",
    "hintLinkExists_WGT" : "Es besteht mindestens eine Verkn%FCpfung. Daher ist die Umschaltung des Modus (Kanal 4) gesperrt.",

    "optionActual" : "Aktuelle Temperatur",
    "optionSetpoint" : "Solltemperatur",
    "optionActualHumidity" : "Aktuelle Temperatur / Feuchtigkeit",
    "optionCO2" : "CO2",
    "optionActualHumidityCO2" : "Aktuelle Temperatur / Feuchtigkeit / CO2",
    "optionHumidity" : "Feuchtigkeit",

    "modeBWTH" : "Wandthermostat",
    "modeSWITCH" : "Schaltaktor",

    "stringTableWaterFlow" : "Momentaner Durchfluss",
    "stringTableWaterVolume" : "Gesamt-Wassermenge",
    "stringTableWaterVolumeSinceOpen" : "Wassermenge seit %D6ffnung",

    "stringTableWaterFlowStatus" : "Status momentaner Durchfluss",
    "stringTableWaterFlowOverflowFalse" : "Z%E4hler-%DCberlauf: Nein",
    "stringTableWaterFlowOverflowTrue" : "Z%E4hler-%DCberlauf: Ja",
    "stringTableWaterVolumeSinceOpenOverflowFalse" : "Z%E4hler-%DCberlauf seit %D6ffnung: Nein",
    "stringTableWaterVolumeSinceOpenOverflowTrue" : "Z%E4hler-%DCberlauf seit %D6ffnung: Ja",

    "msgResetWaterCounter" : "Hiermit setzten Sie den Z%E4hler der Gesamt-Wassermenge zur%FCck auf 0.00 L",

    "lblValve" : "Ventil",
    "btnWaterOff" : "Geschlossen",
    "btnWaterOn" : "Ge%F6ffnet",

    "stringTableErrorErrorFrostProtection" : "Frostschutz-Fehlfunktion",
    "stringTableErrorErrorFrostProtectionFalse" : "Frostschutz-Fehlfunktion: Nein",
    "stringTableErrorErrorFrostProtectionTrue" : "Frostschutz-Fehlfunktion: Ja",

    "stringTableErrorValveFailure" : "Ventil-Fehlfunktion",
    "stringTableErrorValveFailureFalse" : "Ventil-Fehlfunktion: Nein",
    "stringTableErrorValveFailureTrue" : "Ventil-Fehlfunktion: Ja",

    "stringTableErrorWaterFailure" : "Problem Wasserdurchfluss",
    "stringTableErrorWaterFailureFalse" : "Problem Wasserdurchfluss: Nein",
    "stringTableErrorWaterFailureTrue" : "Problem Wasserdurchfluss: Ja",

    "stringTableValveStateErrorPosition" : "Ventil in Fehlerposition",
    "stringTableValveStateErrorPositionFalse" : "Ventil in Fehlerposition: Nein",
    "stringTableValveStateErrorPositionTrue" : "Ventil in Fehlerposition: Ja",
    "stringTableValveStateTooTight" : "Ventil schwerg%E4ngig / Ventil klemmt",
    "stringTableValveStateTooTightFalse" : "Ventil schwerg%E4ngig / Ventil klemmt: Nein",
    "stringTableValveStateTooTightTrue" : "Ventil schwerg%E4ngig / Ventil klemmt: Ja",
    "stringTableValveStateAdjTooBig" : "Ventilstellbereich zu gro%DF",
    "stringTableValveStateAdjTooBigFalse" : "Ventilstellbereich zu gro%DF: Nein",
    "stringTableValveStateAdjTooBigTrue" : "Ventilstellbereich zu gro%DF: Ja",
    "stringTableValveStateAdjTooSmall" : "Ventilstellbereich zu klein",
    "stringTableValveStateAdjTooSmallFalse" : "Ventilstellbereich zu klein: Nein",
    "stringTableValveStateAdjTooSmallTrue" : "Ventilstellbereich zu klein: Ja",

    "stringTableSoilMoistureTransmitterFilterSize" : "Filtergr%F6%DFe",
    "stringTableMeasurementInterval" : "Messintervall",
    "stringTableMeasurementIntervalUnit" : "Einheit Messintervall",
    "stringTableMeasurementIntervalValue" : "Wert des Messintervalls",
    "stringTableRefMin0" : "Referenzwert 0 % Bodenfeuchte",
    "stringTableRefMax100" : "Referenzwert 100 % Bodenfeuchte",

    "stringTableSoilMoisture" : "Bodenfeuchte",
    "stringTableSoilMoistureRaw" : "Rohwert Bodenfeuchte",
    "stringTableSoilTemperature" : "Bodentemperatur",

    "hintAutomaticDeviceUpdate" : "Das Update wird automatisch im Hintergrund durchgef%FChrt.",

    "comment" : "Release C/2025 not yet translated",
    "stringTableWeatherReceiverDataId" : "Bild ID",
    "optionPictureID" : "ID&nbsp;",

    "optionAutoDetection" : "Automatische Erkennung",
    "optionTrailingEdge" : "Phasenabschnitt",
    "optionLeadingEdge" : "Phasenanschnitt",

    "lblDoorLockDirection" : "Drehrichtung zum Verschlie%DFen",
    "lblDoorEndStopOffsetLocked" : "Winkel von neutral unverschlossen nach geschlossen",
    "lblDoorEndStopOffsetOpen" : "Winkel von neutral unverschlossen nach offen",
    "lblDoorLockTurns" : "Umdrehungen von unverschlossen nach verschlossen",
    "lblDoorLockNeutralPos" : "Neutrale Schl%FCsselposition",
    "lblDoorOpeningDirection" : "%D6ffnungsrichtung",

    "lblAutomaticLock" : "Automatisches Verriegeln",

    "optionIgnoreDoorOpen" : "T%FCrzustand ignorieren",
    "optionSkipHoldTimeOpening" : "Fallen-Haltezeit %FCberspringen",
    "optionSkipRelockDelayClosing" : "Auto-Relock-Delay %FCberspringen",
    "optionSkipHoldTimeOpeningRelockDelayClosing" : "Fallen-Haltezeit und Auto-Relock-Delay %FCberspringen ",

    "optionInwards" : "Innen",
    "optionOutwards" : "Aussen",

    "btnAutoRelockOff" : "Auto-Relock Aus",
    "btnAutoRelockOn" : "Auto-Relock Ein",

    "dialogSetDLPTargetLevelTitle" : "Lock Target Level",

    "errorJammed" : "errorJammed",
    "errorLoadTooLow" : "errorLoadTooLow",
    "errorNoEndStopLock" : "errorNoEndStopLock",
    "errorNoEndStopUnlock" : "errorNoEndStopUnlock",

    "helpLockTargetLevel" : "Hier steht eine Beschreibung des Parameters....",

    "dlp_LOCK_STATE" : "Zustand",
    "dlp_LOCK_STATE_REASON" : "Ausgel%F6st durch:",
    "dlp_LOCK_TEACH_IN_STATE" : "LOCK_TEACH_IN_STATE",
    "dlp_LAST_LOCK_DRIVE_LOAD" : "LAST_LOCK_DRIVE_LOAD",

    /*
    "dlpLockState_0" : "UNKNOWN",
    "dlpLockState_1" : "LOCKED",
    "dlpLockState_2" : "UNLOCKED",
    "dlpLockState_3" : "INVALID",
    */

    "dlpLockState_0" : "ubekannt",
    "dlpLockState_1" : "verriegelt",
    "dlpLockState_2" : "entriegelt",
    //"dlpLockState_3" : "INVALID",
    "dlpLockState_3" : "ung%FCltig",

    /*
    "dlpLockStateReason_0" : "UNKNOWN",
    "dlpLockStateReason_1" : "MOTOR",
    "dlpLockStateReason_2" : "MOTOR_INTERNAL_KEY",
    "dlpLockStateReason_3" : "MOTOR_INTERNAL_PROFILE",
    "dlpLockStateReason_4" : "MOTOR_AUTO_RELOCK_TIMER",
    "dlpLockStateReason_5" : "MOTOR_AUTO_RELOCK_STATE",
    "dlpLockStateReason_6" : "MOTOR_AFTER_OPEN_ACKED",
    "dlpLockStateReason_7" : "MOTOR_AFTER_OPEN",
    "dlpLockStateReason_8" : "MOTOR_AFTER_OPEN_INTERNAL_KEY",
    "dlpLockStateReason_9" : "MANUAL_INSIDE",
    "dlpLockStateReason_10" : "MANUAL",
    */

    "dlpLockStateReason_0" : "unbekannt",
    "dlpLockStateReason_1" : "Motorisch",
    "dlpLockStateReason_2" : "Motorisch, durch interne Taste",
    "dlpLockStateReason_3" : "Motorisch, durch Wochenprogramm",
    "dlpLockStateReason_4" : "Motorisch, durch Auto-Relock nach Timeout",
    "dlpLockStateReason_5" : "Motorisch, durch Auto-Relock nach T%FCrzustands%E4nderung",
    "dlpLockStateReason_6" : "Motorisch, nach Open, ACKed (kein neues Open-Ereignis)",
    "dlpLockStateReason_7" : "Motorisch nach Open",
    "dlpLockStateReason_8" : "Motorisch nach Open durch interne Taste",
    "dlpLockStateReason_9" : "Manuell, von au%DFen",
    "dlpLockStateReason_10" : "Manuell, von innen",
    "dlpLockStateReason_11" : "Manuell",

    "dlpLockTeachInState_0" : "T%FCrschloss-Anlern-Prozedur nicht (mehr) aktiv",
    "dlpLockTeachInState_1" : "Neutralstellung Verriegelt vorgemerkt",
    "dlpLockTeachInState_2" : "Neutralstellung Entriegelt vorgemerkt",
    "dlpLockTeachInState_3" : "beide Neutralstellungen vorgemerkt, bereit f%FCr Endanschlagsfahrt",
    "dlpLockTeachInState_4" : "Anlernfahrt l%E4uft",
    "dlpLockTeachInState_5" : "T%FCrschloss-Anlern-Prozedur erfolgreich abgeschlossen (keine Aussage %FCber Parameter-%DCbertragung)",
    "dlpLockTeachInState_6" : "Neutralstellungen Fehler: ganzzahliges Vielfaches von 360° zwischen Ver- und Entriegelt",
    "dlpLockTeachInState_7" : "Neutralstellungen Fehler: weniger als 1 Umdrehung zwischen Ver- und Entriegelt",
    "dlpLockTeachInState_8" : "Endanschlagsfahrt Fehler: Endanschlag lock vor Neutralstellung Verriegelt",
    "dlpLockTeachInState_9" : "Endanschlagsfahrt Fehler: Endanschlag open vor Neutralstellung Entriegelt",
    "dlpLockTeachInState_10" : "Anlernfahrt Fehler: Abbruch durch anderes Kommando / Ereignis",
    "dlpLockTeachInState_11" : "Anlernfahrt Fehler: Batterien zu schwach",
    "dlpLockTeachInState_12" : "T%FCrschloss-Anlern-Prozedur Fehler: sonstige",

    "stringTableErrorDoorLockedWhileOpen" : "Verschlossen bei offener T%FCr",
    "stringTableErrorDoorOpenedWhileLocked" : "T%FCr im verschlossenem Zustand ge%F6ffnet",
    "stringTableSabotageAcceleration" : "Sabotage Beschleunigungssensor",
    "stringTableSabotageBattery" : "Sabotage Batterie",
    "stringTableSabotageMagneticField" : "Sabotage Magnetsensor",
    "stringTableSabotageVertical" : "Sabotage Tilt-Sensor",

    "stringTableDoorStateTransceiverCalibrate" : "Kalibrieren",

    "stringTableDoorLockStateTransceiverLockTargetLevelResetTeachIn" : "T%FCrschloss-Anlern-Prozedur zur%FCcksetzen",
    "stringTableDoorLockStateTransceiverLockTargetLevelSetNeutralLocked" : "Vormerken als Neutralstellung Verriegelt",
    "stringTableDoorLockStateTransceiverLockTargetLevelSetNeutralUnlocked" : "Vormerken als Neutralstellung Entriegelt",
    "stringTableDoorLockStateTransceiverLockTargetLevelTestrunSaveLockUnlock" : "Endanschl%E4ge pr%FCfen (Fahrt: lock, open) und als Konfiguration speichern",
    "stringTableDoorLockStateTransceiverLockTargetLevelTeachInDriveOpenDoorNeutralUnlocked" : "Anlernfahrt (inkl. Schlosslast) starten bei ge%F6ffneter T%FCr\n" +
      "und Schloss in Neutralstellung Entriegelt und als Konfiguration speichern",
    "stringTableDoorLockStateTransceiverLockTargetLevelTeachInDriveOpenDoorNeutralVertical" : "Anlernfahrt (inkl. Schlosslast) starten bei ge%F6ffneter T%FCr (entriegelt)\n" +
      "bei Schloss mit Neutralstellung vertical und als Konfiguration speichern",
    "stringTableDoorLockStateTransceiverLockTargetLevelTeachInDriveOpenDoorNeutralHorizontal" : "Anlernfahrt (inkl. Schlosslast)starten bei ge%F6ffneter T%FCr (entriegelt)\n" +
      "bei Schloss mit Neutralstellung horizontal und als Konfiguration speichern",
    "stringTableDoorLockStateTransceiverLockTargetLevelLoadCalibrationClosedDoor" : "Anlernen der Schlosslast bei geschlossener T%FCr (Fahrt: lock, unlock)",
    "stringTableDoorLockStateTransceiverLockTargetLevelLoadCalibrationOpenDoor" : "Anlernen der Schlosslast bei ge%F6ffneter T%FCr (Fahrt: lock, unlock)",

    "stringTableAutoRelockState" : "Auto-Relock aktiv: ",

    "noMoreKeys" : ""
  }
});
