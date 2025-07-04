var
  getWGDImagePath = function(category) {
    var path = "";

    if (typeof  category == "undefined") {
      path = "/ise/img/icons_hmipw_wgd/";
    } else if (category == "weather") {
      path = path = "/ise/img/icons_hmipw_wgd/weather";
    }

    return path;
  },
  getWGDDefaultImage = function(category) {
    return "_0000_fallback.png";
  },

  getWGDImageCollectionWeather = function() {
    var image = {
      0: ["_0000_fallback.png",0],
      1: ["_921_00_attention.png",1],
      2: ["_921_01_cloud sun rain.png",2],
      3: ["_921_02_cloud.png",3],
      4: ["_921_03_cloud-lightning.png",4]
    };
    return image;
  };

  getWGDImageCollection = function(category) {
  var image;
  if (typeof  category == "undefined") {
    image = {
      0: ["_0000_fallback.png", 0],
      1: ["_000_000_lightbulb0.png", 1],
      2: ["_001_000_tablelamp0.png", 12],
      3: ["_002_000_spotoff.png", 23],
      4: ["_010_000_socket_off.png", 25],
      5: ["_020_000_shutter0.png", 27]
      /*  Not desired according to discussion with PM and Developer.
      5 : ["_050_20_windows_closed.png",43],
      6 : ["_050_64_window_closed.png",52]
       */
    };
  }

  if (category == "weather") {
    image = getWGDImageCollectionWeather();
  }
  return image;
};


WGDSelectIconDialog = Class.create(YesNoDialog,{

  run: function () {
    var self = this;
    this.selectedIconNo = 0;
    this.selectedIcon = "";
    this.activeIcon = 0;
    this.imagePath = getWGDImagePath();

    jQuery(".YesNoDialogContentWrapper").css("background-color", "grey");

    window.setTimeout(function() {
      self._initIconPreview();
    }, 50);
  },

  _initIconPreview: function() {
    var previewElm = jQuery("#anchor_"+ this.chn);

    previewElm.html(this._getHTML());

    this.resetHeight();
  },

  _getHTML: function() {
    var self = this,
      result ="",
      radioBoxSelected = "",
      rows = 8, rowCounter=0;

    setSelectedIconNo = function(iconNo) {
      self.selectedIconNo = iconNo;

    };

    setSelectedIcon = function(icon) {
      self.selectedIcon = icon;
    };

    //console.log(image[1][0]); // prints the name of the first image
    result += "<table style='width:100%'>";
      result += "<tr>";
        jQuery.each(getWGDImageCollection(), function(index, val) {
          if ((radioBoxSelected == "") && (self.activeIcon == val[1])) {
              radioBoxSelected = "checked";
              setSelectedIcon(self.imagePath + val[0]);
              setSelectedIconNo(val[1]);
          } else {
            radioBoxSelected = "";
          }
          if (index / rows == Math.floor(index / rows)) {
            result += "<tr>";
          }
          rowCounter++;
          result += "<td style='border: 1px solid #999;' onclick='jQuery(\"#imgSel_"+index+"\").prop(\"checked\", true);setSelectedIconNo("+val[1]+");setSelectedIcon(\"" + self.imagePath + val[0] + "\");'><table><tr>";
          result += "<td>";
          result += "<img src='" + self.imagePath + val[0] + "' alt='' style='height:24px;'>";
          result += "</td>";
          result += "<td>";
          result += "<input id='imgSel_"+index+"' type='radio' name='image' "+radioBoxSelected+" value='" + val[1] + "' onclick='setSelectedIconNo(this.value);setSelectedIcon(\"" + self.imagePath + val[0] + "\");'>";
          result += "</td>";
          result += "</tr></table></td>";

          if (rowCounter == rows * 2) {
            result += "</tr>";
            rowCounter = 0;
          }
        });
      result += "</tr>";
    result += "</table>";
    return result;
  },

  getSelectedIconNo: function() {
    return this.selectedIconNo;
  },

  getSelectedIcon: function() {
    return this.selectedIcon;
  }

});
