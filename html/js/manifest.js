
/**************************************
****   TrenoLEGO - Web manifest    ****
**** (c) 2026, Alessandro Pedretti ****
**************************************/


/**** Manifest data ****/

var WebAppManifest = {
  "id"              : "trenolego",
  "name"            : "TrenoLEGO",
  "short_name"      : "TrenoLEGO",
  "start_url"       : window.location.origin + "/",
  "display"         : "standalone",
  "background_color": "#d9d9d9",
  "theme_color"     : "navy",
  "icons": [
    {
      "src": window.location.origin + "/img/icon512.png",
      "sizes": "512x512",
      "type": "image/png",
      "purpose": "any"
    }
  ],

  "screenshots": [
    {
      "src": window.location.origin + "/img/icon512.png",
      "sizes": "512x512",
      "type": "image/png",
      "purpose": "any",
    },
    {
      "src": window.location.origin + "/img/icon512.png",
      "sizes": "512x512",
      "type": "image/png",
      "purpose": "any",
      "form_factor": "wide"
    }
  ]
};

const ManifestString = JSON.stringify(WebAppManifest);
const ManifestBlob   = new window.Blob([ManifestString], {type: 'application/json'});
const ManifestURL    = URL.createObjectURL(ManifestBlob);
const ManifestLink   = document.createElement("link");

ManifestLink.rel  = "manifest";
ManifestLink.href = ManifestURL;
document.head.appendChild(ManifestLink);
