var
  getWGDImagePath = function(category) {
    var path = "";

    if (typeof  category == "undefined") {
      path = "/ise/img/icons_hmipw_wgd/";
    } else if (category == "weather") {
      path = path = "/ise/img/icons_hmipw_wgd/weather/";
    }

    return path;
  },
  getWGDDefaultImage = function(category) {
    return "_0000_fallback.png";
  },

  getWGDImageCollectionWeather = function() {
    var image = {
      0: ["_0000_fallback.png",0],
      1: ["_921_00_attention.png",189],
      2: ["_921_01_cloud sun rain.png",190],
      3: ["_921_02_cloud.png",1912],
      4: ["_921_03_cloud-lightning.png",192],
      5: ["_921_04_cloud-moon.png",193],
      6: ["_921_05_cloud-sun.png",194],
      7: ["_921_07_fine-dust.png",195],
      8: ["_921_08_fog.png",196],
      9: ["_921_09_hail",197],

      10: ["_921_10_Humidity.png",198],
      11: ["_921_11_medium-rain.png",199],
      12: ["_921_12_moon.png",200],
      13: ["_921_13_rain water 1.png",201],
      14: ["_921_14_rain water 2.png",202],
      15: ["_921_15_rain.png",203],
      16: ["_921_16_rain-status-no.png",204],
      17: ["_921_17_rain-status-yes.png",205],
      18: ["_921_20_snow.png",206],
      19: ["_921_21_snowflake.png",207],

      20: ["_921_22_solar-cell.png",208],
      21: ["_921_23_sun duration.png",209],
      22: ["_921_24_brightness.png",210],
      23: ["_921_25_sun.png",211],
      24: ["_921_26_sun-course.png",212],
      25: ["_921_27_Sunrise.png",213],
      26: ["_921_28_sunset.png",214],
      27: ["_921_29_temperature.png",215],
      28: ["_921_30_thunderstorm.png",216],
      29: ["_921_31_unknown-weather.png",217],

      30: ["_921_32_uv-radiation.png",218],
      31: ["_921_33_Wind direction.png",219],
      32: ["_921_35_windvane.png",220],
      33: ["_921_36_windy.png",221]


    };
    return image;
  };

  getWGDImageCollection = function(category) {
  var image;
  if (typeof  category == "undefined" || category == "--") {
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
    this.category = this.extraParam;
    this.selectedIconNo = 0;
    this.selectedIcon = "";
    this.activeIcon = 0;
    this.imagePath = getWGDImagePath(this.category);


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
        jQuery.each(getWGDImageCollection(this.category), function(index, val) {
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
