namespace eval ::HomeMatic::Addon {
  global ::HomeMatic::Addon::_CONFIG_FILE_
  
  namespace export GetAll AddConfigPage
  
  set ::HomeMatic::Addon::_CONFIG_FILE_ "/etc/config/hm_addons.cfg"
}

proc ::HomeMatic::Addon::AddConfigPage { id url name description } {
  global ::HomeMatic::Addon::_CONFIG_FILE_
  set filename $::HomeMatic::Addon::_CONFIG_FILE_
  
  array set addon {}
  set addon(ID) $id
  set addon(CONFIG_URL) $url
  set addon(CONFIG_NAME) $name
  set addon(CONFIG_DESCRIPTION) $description
  
  array set addons [::HomeMatic::Util::LoadFile $filename]
  set addons($id) [array get addon]
  ::HomeMatic::Util::SaveFile $filename [array get addons]
}

proc ::HomeMatic::Addon::GetAll { } {
  global ::HomeMatic::Addon::_CONFIG_FILE_
  set filename $::HomeMatic::Addon::_CONFIG_FILE_

  return [::HomeMatic::Util::LoadFile $filename]
}



