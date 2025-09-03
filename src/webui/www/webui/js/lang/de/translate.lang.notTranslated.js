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

    "btnAutoRelockOff" : "Auto Relock Aus",
    "btnAutoRelockOn" : "Auto Relock Ein",

    "dialogSetDLPTargetLevelTitle" : "Lock Target Level",

    "errorJammed" : "errorJammed",
    "errorLoadTooLow" : "errorLoadTooLow",
    "errorNoEndStopLock" : "errorNoEndStopLock",
    "errorNoEndStopUnlock" : "errorNoEndStopUnlock",

    "helpLockTargetLevel" : "Hier steht eine Beschreibung des Parameters....",

    "dlp_LOCK_STATE" : "LOCK_STATE",
    "dlp_LOCK_STATE_REASON" : "LOCK_STATE_REASON",
    "dlp_LOCK_TEACH_IN_STATE" : "LOCK_TEACH_IN_STATE",
    "dlp_LAST_LOCK_DRIVE_LOAD" : "LAST_LOCK_DRIVE_LOAD",

    "dlpLockState_0" : "UNKNOWN",
    "dlpLockState_1" : "LOCKED",
    "dlpLockState_2" : "UNLOCKED",
    "dlpLockState_3" : "INVALID",

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

    "dlpLockTeachInState_0" : "TEACH_IN_INACTIVE",
    "dlpLockTeachInState_1" : "NEUTRAL_POSITION_LOCKED_SET",
    "dlpLockTeachInState_2" : "NEUTRAL_POSITION_UNLOCKED_SET",
    "dlpLockTeachInState_3" : "NEUTRAL_POSITION_BOTH_SET",
    "dlpLockTeachInState_4" : "TEACH_IN_DRIVE_RUNNING",
    "dlpLockTeachInState_5" : "TEACH_IN_SUCCESSFUL",
    "dlpLockTeachInState_6" : "ERROR_NEUTRAL_POSITION_INCOMPLETE_TURNS",
    "dlpLockTeachInState_7" : "ERROR_NEUTRAL_POSITION_LESS_ONE_TURN",
    "dlpLockTeachInState_8" : "ERROR_END_STOP_DRIVE_LOCK_BEFORE_NEUTRAL_POSITION",
    "dlpLockTeachInState_9" : "ERROR_END_STOP_DRIVE_UNLOCK_BEFORE_NEUTRAL_POSITION",
    "dlpLockTeachInState_10" : "ERROR_END_STOP_DRIVE_UNLOCK_BEFORE_NEUTRAL_POSITION",
    "dlpLockTeachInState_11" : "ERROR_TEACH_IN_DRIVE_CANCELED",
    "dlpLockTeachInState_12" : "ERROR_TEACH_IN_DRIVE_BATTERY_LOW",

    "stringTableErrorDoorLockedWhileOpen" : "Verschlossen bei offener T%FCr",
    "stringTableErrorDoorOpenedWhileLocked" : "T%FCr im verschlossenem Zustand ge%F6ffnet",
    "stringTableSabotageAcceleration" : "Sabotage Beschleunigungssensor",
    "stringTableSabotageBattery" : "Sabotage Batterie",
    "stringTableSabotageMagneticField" : "Sabotage Magnetsensor",
    "stringTableSabotageVertical" : "Sabotage Tilt-Sensor",

    "stringTableDoorStateTransceiverCalibrate" : "Kalibrieren",

    "stringTableDoorLockStateTransceiverLockTargetLevelResetTeachIn" : "RESET_TEACH_IN",
    "stringTableDoorLockStateTransceiverLockTargetLevelSetNeutralLocked" : "SET_NEUTRAL_LOCKED",
    "stringTableDoorLockStateTransceiverLockTargetLevelSetNeutralUnlocked" : "SET_NEUTRAL_UNLOCKED",
    "stringTableDoorLockStateTransceiverLockTargetLevelTestrunSaveLockUnlock" : "TESTRUN_SAVE_LOCK_UNLOCK",
    "stringTableDoorLockStateTransceiverLockTargetLevelTeachInDriveOpenDoorNeutralUnlocked" : "TEACH_IN_DRIVE_OPEN_DOOR_NEUTRAL_UNLOCKED",
    "stringTableDoorLockStateTransceiverLockTargetLevelTeachInDriveOpenDoorNeutralVertical" : "TEACH_IN_DRIVE_OPEN_DOOR_NEUTRAL_VERTICAL",
    "stringTableDoorLockStateTransceiverLockTargetLevelTeachInDriveOpenDoorNeutralHorizontal" : "TEACH_IN_DRIVE_OPEN_DOOR_NEUTRAL_HORIZONTAL",
    "stringTableDoorLockStateTransceiverLockTargetLevelLoadCalibrationClosedDoor" : "LOAD_CALIBRATION_CLOSED_DOOR",
    "stringTableDoorLockStateTransceiverLockTargetLevelLoadCalibrationOpenDoor" : "LOAD_CALIBRATION_OPEN_DOOR",

    "stringTableAutoRelockState" : "AUTO_RELOCK_STATE",

    "noMoreKeys" : ""
  }
});
