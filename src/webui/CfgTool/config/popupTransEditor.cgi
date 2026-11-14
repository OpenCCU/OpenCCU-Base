#!/bin/tclsh
#source ../once.tcl
#sourceOnce cgi.tcl
#sourceOnce session.tcl
#sourceOnce common.tcl

source $env(DOCUMENT_ROOT)/once.tcl
##sourceOnce $env(DOCUMENT_ROOT)/tcl/extern/cgi.tcl
sourceOnce $env(DOCUMENT_ROOT)/cgi.tcl
sourceOnce $env(DOCUMENT_ROOT)/session.tcl
## sourceOnce $env(DOCUMENT_ROOT)/config/ic_common.tcl
sourceOnce $env(DOCUMENT_ROOT)/common.tcl


loadOnce tclrpc.so

array set interfaces ""
array set interface_descriptions ""

proc read_interfaces {} {
    global interfaces interface_descriptions INTERFACES_FILE env
    set retval 1
    if { [ info exist env(BIDCOS_SERVICE) ] } {
        set interfaces(default) "$env(BIDCOS_SERVICE)"
        set interface_descriptions(default) "Default BidCoS Interface"
    } else {
        set fd -1
        catch {set fd [open $INTERFACES_FILE r]}
        if { $fd >=0 } {
            set contents [read $fd]
            while { [regexp -indices {</ipc[^>]*>} $contents range] } {
                set section [string range $contents 0 [lindex $range 1]]
                set contents [string range $contents [expr [lindex $range 1] + 1] end]
                if { 
                        [regexp {<name[^>]*>([^<]+)</name} $section dummy name] &&
                        [regexp {<url[^>]*>([^<]+)</url} $section dummy url] &&
                        [regexp {<info[^>]*>([^<]+)</info} $section dummy description ]
                } {
                    array set interfaces [list $name $url]
                    array set interface_descriptions [list $name $description]
                }
            }
            close $fd
        } else {
            puts "Could not open interface file"
            set retval 0
        }
    }
    return $retval
}

proc put_page {} {
    global interfaces
    
    set iface [lindex [array names interfaces] 0]
    catch {
        import iface
    }
    
    set url $interfaces($iface)

    import channel
    import title
    puts "<div class=\"popupTitle\">$title</div>"
    puts {
        <div style="background-color: White;overflow:auto;width: 100%;height: 100%;">
        <table class="popupTable" style="border:solid 1px Black;" cellpadding="10" cellspacing="0">
            <colgroup>
                <col style="width:10%;"/>
                <col style="width:10%;"/>
                <col style="width:10%;"/>
                <col style="width:10%;"/>
                <col style="width:10%;"/>
                <col style="width:50%;"/>
            </colgroup>
            <tr>
                <th style="border:solid 1px Black;height:40px;">Name</th>
                <th style="border:solid 1px Black;height:40px;">Typen-Bezeichnung</th>
                <th style="border:solid 1px Black;height:40px;">Bild</th>
                <th style="border:solid 1px Black;height:40px;">Bezeichnung</th>
                <th style="border:solid 1px Black;height:40px;">Seriennummer</th>
                <th colspan="2" style="border:solid 1px Black;height:50px;">&Uuml;bertragungsmodus</th>
            </tr>
            <tr class="popupGrayCells" style="padding: 10px;">
    }
    
    array set chan_descr [xmlrpc $url getDeviceDescription [list string $channel]]
    array set dev_descr [xmlrpc $url getDeviceDescription [list string $chan_descr(PARENT)]]
    set name $chan_descr(ADDRESS)
    catch { 
        array set chan_metadata [xmlrpc $url getAllMetadata [list string $chan_descr(ADDRESS)]] 
        set name $chan_metadata(NAME)
    }
    
    puts "<td style='border:solid 1px Black;'>$name</td>"
    puts "<td style='border:solid 1px Black;'>$dev_descr(TYPE)</td>"
    puts "<td style='text-align:left;border:solid 1px Black;background-color:White'>"
    
    puts "<div id='picDivT$channel' style='margin:0; padding:0; position:relative; width:50px; height:50px;'"
    puts " onmouseover='picDivShow(jg_250, \"$dev_descr(TYPE)\", 250, $chan_descr(INDEX), this);' onmouseout='picDivHide(jg_250);' "
    puts "></div>"
  
    puts "</td>"
    puts "<td style='border:solid 1px Black;'>"
    puts "<div id='devdesc'></div>"
    puts "</td>"
    puts "<td style='border:solid 1px Black;'>$chan_descr(ADDRESS)</td>"
    puts "<td style='text-align:left;background-color:White;width:50px;border:solid 1px Black;'>"
    puts "<select id='selMode' onchange='iseSaveMode()'>"
    if { $chan_descr(AES_ACTIVE) } {
        set off_selected ""
        set on_selected "selected"
    } else {
        set on_selected ""
        set off_selected "selected"
    }
    puts "<option $off_selected>Standard</option>"
    puts "<option $on_selected>Gesichert</option>"
    puts "</select>"
    puts "</td>"
    
    puts "<script type='text/javascript'>"
    puts "sPicDivId = 'picDivT$channel';"
    puts "sDevLbl = '$dev_descr(TYPE)';"
    puts "iChnNr = $chan_descr(INDEX);"
    
    puts {$('devdesc').innerHTML = DEV_getDescription( sDevLbl );}
    puts "</script>"
    puts {
                    <td style="width:100%;text-align:left;padding-left:6px;">
                        Hinweis:
                        Bei "Gesichert" authentifiziert sich Sender
                        gegen&uuml;ber dem Empf&auml;nger durch Kenntnis des
                        System-Sicherheitsschl&uuml;ssels.
                        Durch die dadurch bedingte Erh&ouml;hung des
                        Kommunikationsaufkommens verz&ouml;gert sich
                        die Abarbeitung des Befehls geringf&uuml;gig und
                        die Batterielebensdauer verringert sich.
                        Bei "Standard" erfolgt keine Authentifizierung.
                        Durch das dadurch bedingte niedrige
                        Kommunikationsaufkommen ergibt sich eine
                        schnelle Befehlsabarbeitung und maximale
                        Batterielebensdauer.
                    </td>
                </tr>
            </table>
        </div>
    }

    puts {
        <div class="popupControls">
            <div onclick="closeRetSelIdx()" style="width:100px;margin: 5px;">Schliessen</div>
        </div>

        <script type="text/javascript">
    }
    puts "var channel_id = '$channel';"
    puts {
            var jg_0 = new jsGraphics(sPicDivId);  
            InitGD(jg_0, 50);
            Draw(jg_0, sDevLbl, 50, iChnNr);
  
            closeRetSelIdx = function() {
                PopupClose($("selMode").selectedIndex);
            }
    
            iseSaveMode = function() {
                var bAES = ($("selMode").selectedIndex == 1);
                configMetadata.saveProfileParam( channel_id, "MASTER", "AES_ACTIVE", bAES);
            }
        </script>
    }
}

cgi_eval {
    #cgi_debug -on
    cgi_input
    catch {
        import debug
        cgi_debug -on
    }
    read_interfaces
    
    http_head

    if {[session_requestisvalid 0] > 0} then {
        put_page
    }

}

