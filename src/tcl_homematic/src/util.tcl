namespace eval ::HomeMatic::Util {
  namespace export LoadFile SaveFile
}

proc ::HomeMatic::Util::LoadFile { filename } {
  set content {}

  catch {
    set fd [open $filename r]
    catch { set content [read $fd] }
    close $fd
  }
  
  return $content
}

proc ::HomeMatic::Util::SaveFile { filename content } {
  set result 0
  
  catch {
    set fd [open $filename w]
    catch { puts -nonewline $fd $content }
    close $fd
  }
  
  return $result
}




