/* Copyright 2015-2017 - Marco Di Cristina, Alessandro Fabbri, Andrea Ferracin, Mattia Guidetti, Stefano Sinigardi,  */

/***************************************************************************
This file is part of multi_json_map.

multi_json_map is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

multi_json_map is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with multi_json_map. If not, see <http://www.gnu.org/licenses/>.
***************************************************************************/

#include <iostream>
#include <fstream>
#include <string>

#include "jsoncons/json.hpp"

#include "html_headers.h"

#define MAJOR_VERSION           1
#define MINOR_VERSION           3

std::vector<std::string> colors_button;
std::vector<std::string> colors_text;

std::string HSLtoRGB(double hue, double sat, double light) {
  double red, green, blue;
  red = green = blue = light;                     // grey is default
  double v;
  v = light <= 0.5 ? light*(1. + sat) : light + sat - light*sat;
  if (v > 0) {
    double m, sv;
    int sextant;
    double fract, vsf, mid1, mid2;
    m = light + light - v; sv = (v - m) / v;
    hue *= 6.0; sextant = int(hue);
    fract = hue - sextant; vsf = v*sv*fract; mid1 = m + vsf; mid2 = v - vsf;
    switch (sextant) {
    case 0: red = v; green = mid1; blue = m; break;
    case 1: red = mid2; green = v; blue = m; break;
    case 2: red = m; green = v; blue = mid1; break;
    case 3: red = m; green = mid2; blue = v; break;
    case 4: red = mid1; green = m; blue = v; break;
    case 5: red = v; green = m; blue = mid2; break;
    }
  }
  std::stringstream stream;

  red *= 255;
  green *= 255;
  blue *= 255;
  stream
    << std::setfill('0') << std::setw(2) << std::hex << int(red)
    << std::setfill('0') << std::setw(2) << std::hex << int(green)
    << std::setfill('0') << std::setw(2) << std::hex << int(blue);
  return stream.str();
};


void usage(char* progname) {
  std::cerr << "Usage: " << progname << " path/to/config" << std::endl;
  std::cerr << "      To display on google maps a number of json encoded set of GNSS points." << std::endl << std::endl;
  std::cerr << "Usage: " << progname << " -conf_t" << std::endl;
  std::cerr << "      To generate a dummy config file." << std::endl << std::endl;
}

int main(int argc, char** argv) {
  std::cout << "Display_Multiple_Trips (for Google Maps only) v" << MAJOR_VERSION << "." << MINOR_VERSION << std::endl;

  // Parsing command line
  std::string config_name;
  if (argc == 2) {
    config_name = argv[1];
    if (config_name == "-conf_t") {
      std::ofstream config("config.cfg");
      config << R"(TAG1   = label for data 1
JSON1  = file for data 1
TAG1   = label for data 1
MODE1  = (m)arkers or (p)olylines, none for both
JSON2  = file for data 2
HTML   = html filename
TITLE  = html tab title
FOLDER = set working folder (optional)
)";
      std::cerr << "Config template created. Quitting..." << std::endl;
      exit(-1);
    }
  }
  else {
    std::cerr << "ERROR: No arguments. Read usage and relaunch properly." << std::endl;
    usage(argv[0]);
    exit(-2);
  }

  // Safe config file parsing
  std::vector<std::string> input_names, input_tags, input_modes;
  std::string output_name, output_title, input_folder("");
  std::string key, equal, value;
  std::ifstream filein(config_name);
  if (!filein) {
    std::cerr << "Configuration file " << config_name << " not found. Quitting..." << std::endl;
    exit(-3);
  }
  while (filein >> key >> equal >> value) {
    if (key[0] == '#' || key[0] == '!') {
      std::getline(filein, key);
      continue;
    }
    else if (key.substr(0, 4) == "JSON") {
      while (input_names.size() > input_tags.size()) input_tags.push_back("DEFAULT");
      while (input_names.size() > input_modes.size()) input_modes.push_back("");
      input_names.push_back(value);
    }
    else if (key.substr(0, 3) == "TAG") {
      std::string s;
      std::getline(filein, s);
      input_tags.push_back(value + s);
    }
    else if (key.substr(0, 4) == "MODE") {
      input_modes.push_back(value);
    }
    else if (key == "HTML") {
      while (input_names.size() > input_tags.size()) input_tags.push_back("DEFAULT");
      while (input_names.size() > input_modes.size()) input_modes.push_back("");
      output_name = value;
    }
    else if (key == "TITLE") {
      std::getline(filein, output_title);
      output_title = value + output_title;
    }
    else if (key == "FOLDER") {
      input_folder = value;
    }
    else {
      std::cerr << "Key " << key << " unknown. Edit " << config_name << std::endl;
      exit(-4);
    }
  }
  filein.close();

  // Checks
  double index = 0;
  if (input_names.size() != input_tags.size()) {
    std::cerr << "WARNING: names(" << input_names.size() << ") and tags(" << input_tags.size() << "don't match!" << std::endl;
    exit(-5);
  }
  if (input_names.size() != input_modes.size()) {
    std::cerr << "WARNING: names(" << input_names.size() << ") and modes(" << input_modes.size() << "don't match!" << std::endl;
    exit(-6);
  }
  for (auto i : input_names) {
    filein.open(input_folder + i);
    if (!filein) {
      std::cerr << "ERROR unable to open " << input_folder + i << ". Quitting..." << std::endl;
      exit(-6);
    }
    filein.close();
  }
  if (output_name == "") output_name = "multi_map.html";

  // Constructing DEFAULT tags and colors 
  for (int i = 0; i < (int)input_names.size(); i++) {
    if (input_tags[i] == "DEFAULT") input_tags[i] = input_names[i].substr(0, input_names[i].size() - 5);
    colors_button.push_back(HSLtoRGB(index / double(input_names.size()), 1, 0.4));
    colors_text.push_back("000000");
    index++;
  }

  // Import JSON into vector
  std::vector<jsoncons::json> trips_coordinate;
  for (auto i : input_names) {
    jsoncons::json gps_records;
    try {
      gps_records = jsoncons::json::parse_file(input_folder + i);
    }
    catch (std::exception &e) {
      std::cerr << "Skipping invalid JSON input : " << input_folder + i << std::endl;
      std::cerr << "EXCEPTION: " << e.what() << std::endl;
      continue;
    }
    trips_coordinate.push_back(gps_records);
  }

  // Summary
  std::cout << "Trip number : " << input_names.size() << std::endl;
  std::cout << "Input files : " << std::endl;
  for (size_t i = 0; i < input_names.size(); i++)
    std::cout << i + 1 << "/" << input_names.size() << ")\t" << input_names[i] << " \"" << input_tags[i] << "\" [records " << trips_coordinate[i].size() << "]" << std::endl;
  std::cout << "Output html : " << output_name << ((output_title == "") ? "" : " \"" + output_title + "\"") << std::endl;

  // Generate html
  std::ofstream output(input_folder + output_name);
  output << pre_header;
  output << "\t\t" << ((output_title == "") ? "Multi-trip" : output_title);
  output << post_header;

  for (auto t : input_tags) {
    output << "\t\tvar Trajectory_" << t << ";\n";
    output << "\t\tvar Markers_" << t << " = [];\n\n";
  }

  output << "\t\tvar map;\n\n";

  output << "\t\tfunction initialize(){\n";
  for (size_t i = 0; i < input_tags.size(); i++) {
    output << "\t\t\t\tvar Locations_" << input_tags[i] << " = [\n\n";
    if (trips_coordinate[i].is_object()) {    // object style
      int j = 0;
      for (auto rec = trips_coordinate[i].begin_members(); rec != trips_coordinate[i].end_members(); ++rec, ++j) {
        try {
          output
            << "["
            << std::fixed << std::setprecision(6)
            << (rec->value().has_member("lat") ? rec->value()["lat"].as<double>() : 90.0)
            << ","
            << (rec->value().has_member("lon") ? rec->value()["lon"].as<double>() : 0.0)
            << ",'<p>"
            << "#" << j << "<br />"
            << (rec->value().has_member("date") ? rec->value()["date"].as<std::string>() : "NULL")
            << "</p>']"
            << ((j != trips_coordinate[i].size() - 1) ? ',' : ' ')
            << "\n";
        }
        catch (const std::exception& e) {
          std::cerr << e.what() << std::endl;
        }
      }
    }
    else if (trips_coordinate[i].is_array()) {    // array style
      for (size_t j = 0; j < trips_coordinate[i].size(); ++j) {
        try {
          output
            << "["
            << std::fixed << std::setprecision(6)
            << (trips_coordinate[i][j].has_member("lat") ? trips_coordinate[i][j]["lat"].as<double>() : 90.0)
            << ","
            << (trips_coordinate[i][j].has_member("lon") ? trips_coordinate[i][j]["lon"].as<double>() : 0.0)
            << ",'<p>"
            << "#" << j << "<br />"
            << (trips_coordinate[i][j].has_member("date") ? trips_coordinate[i][j]["date"].as<std::string>() : "NULL")
            << "']"
            << ((j != trips_coordinate[i].size() - 1) ? ',' : ' ')
            << "\n";
        }
        catch (const std::exception& e) {
          std::cerr << e.what() << std::endl;
        }
      }
    }
    output << "\n\t\t\t\t]\n\n";
  }
  output << "\t\t\t\tmap = new google.maps.Map(document.getElementById(\'map\'), {\n";
  output << "\t\t\t\t\tmapTypeId: google.maps.MapTypeId.ROADMAP\n";
  output << "\t\t\t\t});\n";
  output << "\t\t\t\tvar infowindow = new google.maps.InfoWindow();\n";
  output << "\t\t\t\tvar bounds = new google.maps.LatLngBounds();\n";

  for (size_t i = 0; i < input_tags.size(); ++i) {
    std::string t = input_tags[i];

    output << "\n\t\t\t\t//////// TRIP " << t << std::endl;
    output << "\t\t\t\tvar PolyPath_" << t << " = [];\n";
    output << "\t\t\t\tfor(i=0; i<Locations_" << t << ".length; i++) {\n";
    output << "\t\t\t\t\tvar point = new google.maps.LatLng( Locations_" << t << "[i][0], Locations_" << t << "[i][1] )\n\n";

    output << "\t\t\t\t\tPolyPath_" << t << ".push(point);\n";
    output << "\t\t\t\t\tbounds.extend(point);\n\n";

    if (input_modes[i] == "m" || input_modes[i] == "") {
      output << "\t\t\t\t\tMarker = new google.maps.Marker({\n";
      output << "\t\t\t\t\t\tposition: point,\n";
      output << "\t\t\t\t\t\tmap : map,\n";
      output << "\t\t\t\t\t\ticon : {\n";
      output << "\t\t\t\t\t\t\tpath: google.maps.SymbolPath.CIRCLE,\n";
      output << "\t\t\t\t\t\t\tstrokeColor : '#" << colors_button[i] << "',\n";
      output << "\t\t\t\t\t\t\tscale : 3\n";
      output << "\t\t\t\t\t\t}\n";
      output << "\t\t\t\t\t});\n";
      output << "\t\t\t\t\tMarkers_" << t << ".push(Marker);\n\n";
    }

    output << "\t\t\t\t\tgoogle.maps.event.addListener(Marker, 'click', (function(marker, i) {\n";
    output << "\t\t\t\t\t\treturn function() {\n";
    output << "\t\t\t\t\t\t\tinfowindow.setContent(Locations_" << t << "[i][2]);\n";
    output << "\t\t\t\t\t\t\tinfowindow.open(map, marker);\n";
    output << "\t\t\t\t\t\t}\n";
    output << "\t\t\t\t\t})(Marker, i));\n";
    output << "\t\t\t\t}\n\n";

    output << "\t\t\t\tTrajectory_" << t << " = new google.maps.Polyline({\n";
    output << "\t\t\t\t\tpath: PolyPath_" << t << ",\n";
    output << "\t\t\t\t\tgeodesic: true,\n";
    output << "\t\t\t\t\tstrokeColor: \'#" << colors_button[i] << "\',\n";
    output << "\t\t\t\t\tstrokeOpacity: 1.,\n";
    output << "\t\t\t\t\tstrokeWeight: 2\n";
    output << "\t\t\t\t});\n";
  }
  output << "\n";
  for (size_t i = 0; i < input_tags.size(); ++i) {
    if (input_modes[i] == "p" || input_modes[i] == "") {
      output << "\t\t\t\tTrajectory_" << input_tags[i] << ".setMap(map);\n";
    }
  }
  output << "\n\t\t\t\truler_map = new RulerMap(map);\n";
  output << "\n\t\t\t\tmap.fitBounds(bounds);\n";
  output << "\t\t\t}\n\n";
  for (size_t i = 0; i < input_tags.size(); ++i) {
    output << "\t\t\tfunction toggle_" << input_tags[i] << "(){\n";
    if (input_modes[i] == "p" || input_modes[i] == "") {
      output << "\t\t\t\tTrajectory_" << input_tags[i] << ".setMap(Trajectory_" << input_tags[i] << ".getMap() ? null : map);\n";
    }
    if (input_modes[i] == "m" || input_modes[i] == "") {
      output << "\t\t\t\t\tfor (i = 0; i < Markers_" << input_tags[i] << ".length; i++) {\n";
      output << "\t\t\t\t\t\tvar mark = Markers_" << input_tags[i] << "[i];\n";
      output << "\t\t\t\t\t\tmark.setMap(mark.getMap() ? null : map);\n";
      output << "\t\t\t\t\t}\n";
    }
    output << "\t\t\t\t};\n";
  }
  output << "\n\t\t\tgoogle.maps.event.addDomListener(window, \'load\', initialize);\n";
  output << "\n\t\t</script>" << std::endl;
  output << "\t\t<div id=\"panel\">" << std::endl;
  for (size_t i = 0; i < input_tags.size(); i++) {
    output << "\t\t\t<button onclick=\"toggle_" << input_tags[i] << "()\" style=\"background-color:#"
      << colors_button[i] << "; color:#" << colors_text[i] << "\">" << input_tags[i] << "</button>\n";
  }
  output << "\t\t</div>" << std::endl;
  output << "\t</body>" << std::endl;
  output << "</html>" << std::endl;
  output.close();

  return 0;
}
