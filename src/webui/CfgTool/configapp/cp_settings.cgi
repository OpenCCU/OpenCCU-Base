#!/bin/tclsh
source ../once.tcl
sourceOnce common.tcl
sourceOnce session.tcl
source configfile.tcl
load tclrpc.so

set RFD_URL $env(BIDCOS_SERVICE)
set UI_CONFIGFILE $env(CONFIG_ROOT)/ui_settings.conf
    


proc action_change_key {} {
    global env RFD_URL
    
    http_head
    import key1
    import key2
    
    if { "$key1" != "$key2" } {
        put_message "Sicherheitsschl&uuml;ssel setzen - Fehler" "Die beiden eingegebenen Schl&uuml;ssel stimmen nicht &uuml;berein." "\"Zur&uuml;ck\" \"CreateCPPopup('$env(SCRIPT_NAME)');\""
        return
    }
    if { [string length "$key1"] < 5 } {
        put_message "Sicherheitsschl&uuml;ssel setzen - Fehler" "Der eingegebene Schl&uuml;ssel ist zu kurz. Geben Sie einen Schl&uuml;ssel ein, der mindestens 5 Zeichen lang ist." "\"Zur&uuml;ck\" \"CreateCPPopup('$env(SCRIPT_NAME)');\""
        return
    }
    
    if { [catch {xmlrpc $RFD_URL changeKey $key1}] } {
        put_message "Sicherheitsschl&uuml;ssel setzen - Fehler" {
            Der Schl&uuml;ssel konnte nicht gesetzt werden. Das liegt vermutlich daran, da&szlig; der aktuelle Schl&uuml;ssel noch nicht an alle Komponenten
            &uuml;bertragen wurde. Hinweise darauf finden Sie in den Servicemeldungen.
        } "\"Zur&uuml;ck\" \"CreateCPPopup('$env(SCRIPT_NAME)');\""
        return
    }
    put_message "Sicherheitsschl&uuml;ssel setzen - OK" {
        Der Schl&uuml;ssel wurde erfolgreich ge&auml;ndert. In den Servicemeldungen finden Sie Hinweise auf Komponenten, an die der neue Schl&uuml;ssel noch nicht
        &uuml;bertragen wurde. Diese Komponenten m&uuml;ssen noch in den Anlernmodus gebracht werden. N&auml;here Informationen finden Sie in der Bedienungsanleitung
        der entsprechenden Komponenten.
        } "\"Zur&uuml;ck\" \"CreateCPPopup('$env(SCRIPT_NAME)');\""
}

proc action_change_link_options {} {
    global env UI_CONFIGFILE
    
    import expert_mode
    
    set ui_config(ExpertMode) $expert_mode
    
    write_config $UI_CONFIGFILE ui_config

    action_put_page
}

proc put_message {title msg args} {
    division {class="popupTitle"} {
        puts $title
    }
    division {style="overflow: auto;width: 100%;height: 100%;"} {
        table {class="popupTable"} {style="font-size: 8pt;color: Black;background-color: White;"} {border="1"} {
            table_row {style="padding: 10px 0px 10px 0px;background-color: White; color: Black;"} {
                table_data {
                    puts $msg
                }
            }
        }
    }
    division {class="popupControls"} {
        table {
            table_row {
                if { [llength $args] < 1 } { set args {{"Zur&uuml;ck" "PopupClose();"}}}
                foreach b $args {
                    table_data {style="border:0; width:10%"} {
                        division {style="width: 100px;margin: 5px;"} "onClick=\"[lindex $b 1]\"" {
                            puts [lindex $b 0]
                        }
                    }
                }
            }
        }
    }
}

proc action_put_page {} {
    global env sid RFD_URL UI_CONFIGFILE
    
    array set paramset [xmlrpc $RFD_URL getParamset "BidCoS-RF:0" MASTER]
    set cur_key_index $paramset(AES_KEY)
    
    array set ui_config ""
    catch { read_config $UI_CONFIGFILE ui_config }
    set expert_mode 0
    if { [info exists ui_config(ExpertMode)] } {
        set expert_mode [expr ( $ui_config(ExpertMode) != 0 ) && ( $ui_config(ExpertMode) != "false" ) ]
    }
    
    http_head
    
    division {class="popupTitle"} {
        puts "Einstellungen"
    }
    division {style="background-color: White;overflow: auto;width: 100%;height: 100%;"} {
        table {class="popupTable"} {style="font-size: 8pt;"} {border="1"} {
            table_row {style="padding: 10px 0px 10px 0px;"} {
                table_data {style="background-color:#999999;color: White;vertical-align: middle;font-size: 10pt;"} {
                    puts "System-<br>Sicherheitsschl&uuml;ssel"
                }
                table_data {style="color: Black;background-color: White;"} {
                    table {style="color: Black;background-color: White;font-size: 8pt;"} {
                        table_row {
                            table_data {colspan="3"} {
                                puts "System-Sicherheitsschl&uuml;ssel Eingabe"
                            }
                        }
                        table_row {
                            td {width="20"} {}
                            table_data {align="left"} {
                                puts "Sicherheitsschl&uuml;ssel:"
                            }
                            table_data {align="right"} {
                                cgi_text key1= {size="16"} {id="text_key1"} {type="password"}
                            }
                        }
                        table_row {
                            td {width="20"} {}
                            table_data {align="left"} {
                                puts "Sicherheitsschl&uuml;ssel - Wiederholung:"
                            }
                            table_data {align="right"} {
                                cgi_text key2= {size="16"} {id="text_key2"} {type="password"}
                            }
                        }
                        table_row {
                            table_data {align="right"} {style="color: Black;"} {colspan="3"} {
                                division {class="popupControls"} {style="border: 0"} {
                                    division {style="width:200px;margin: 5px;"} {onClick="OnChangeKey()"} {
                                        puts "Schl&uuml;ssel &uuml;bernehmen"
                                    }
                                }
                            }
                        }
                    }
                }
                table_data {align="left"} {style="color: Black;background-color: White;"} {
                    if { $cur_key_index } {
                        puts "Der System-Sicherheitsschl&uuml;ssel wurde bisher $cur_key_index mal ge&auml;ndert.<br /><br />"
                    } else {
                        puts "Der System-Sicherheitsschl&uuml;ssel wurde noch nicht ge&auml;ndert.<br /><br />"
                    }
                    puts {<div style="color: Red;">}
                    puts "Achtung!"
                    number_list {
                        li {
                            Der System-Sicherheitsschl&uuml;ssel sollte mindestens 8 Zeichen lang sein!
                        }
                        li {
                            Notieren Sie sich Ihren System-Sicherheitsschl&uuml;ssel und bewahren Sie diesen an einem
                            sicheren Ort auf.
                            <br>
                            Aus Sicherheitsgr&uuml;nden besteht keine(!) M&ouml;glichkeit, den System-Sicherheitsschl&uuml;ssel
                            zur&uuml;ckzusetzen oder zu umgehen.
                        }
                    }
                    puts {</div>}
                }
            }
            table_row {style="padding: 10px 0px 10px 0px;"} {
                table_data {style="background-color:#999999;color: White;vertical-align: middle;font-size: 10pt;"} {
                    puts "Verkn&uuml;pfungen"
                }
                table_data {style="color: Black;background-color: White;"} {
                    table {style="color: Black;background-color: White;font-size: 8pt;"} {
                        table_row {
                            table_data {
                                set checked ""
                                if { !$expert_mode } { set checked "checked" }
                                puts "<input type='checkbox' id='chk_not_expert' $checked></input>"
                                puts "Vereinfachte Verkn&uuml;pfungskonfiguration aktivieren"
                            }
                        }
                        table_row {
                            table_data {align="right"} {style="color: Black;"} {
                                division {class="popupControls"} {style="border: 0"} {
                                    division {style="width:200px;margin: 5px;"} {onClick="OnChangeLinkOptions()"} {
                                        puts "&Auml;nderungen &uuml;bernehmen"
                                    }
                                }
                            }
                        }
                    }
                }
                table_data {align="left"} {style="color: Black;background-color: White;"} {
                    
                }
            }
        }
    }
    division {class="popupControls"} {
        table {
            table_row {
                table_data {style="border:0; width:10%"} {
                    division {style="width: 100px;margin: 5px;"} {onClick="PopupClose();"} {
                        puts "Zur&uuml;ck"
                    }
                }
            }
        }
    }
    puts ""
    cgi_javascript {
        puts "var url = \"$env(SCRIPT_NAME)\";"
        puts {
            OnChangeKey = function() {
                dlgPopup.hide();
                dlgPopup.setWidth(400);
                dlgPopup.LoadFromFile(url, "action=change_key&key1="+document.getElementById("text_key1").value+"&key2="+document.getElementById("text_key2").value);
            };
            OnChangeLinkOptions = function() {
                dlgPopup.hide();
                //dlgPopup.setWidth(400);
                var expert_mode = !document.getElementById("chk_not_expert").checked;
                dlgPopup.LoadFromFile(url, "action=change_link_options&expert_mode="+expert_mode);
            };
        }
    }
}

cgi_eval {
    #cgi_debug -on
    cgi_input
    catch {
        import debug
        cgi_debug -on
    }

    set action "put_page"

    catch { import action }

    if {[session_requestisvalid 8] > 0} then action_$action
}

