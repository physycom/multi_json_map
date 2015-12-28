/* Copyright 2015 - Marco Di Cristina, Alessandro Fabbri, Andrea Ferracin, Mattia Guidetti, Stefano Sinigardi,  */

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

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <string>

#include "jsoncons/json.hpp"

#include "html_headers.h"

using namespace std;

#define MAJOR_VERSION           1
#define MINOR_VERSION           0

std::vector<std::string> colors_button;
std::vector<std::string> colors_text;

string HSLtoRGB(double hue, double sat, double light) {
	double red, green, blue;
	red = green = blue = light;                     // grey is default
	double v;
	v = light <= 0.5 ? light*(1. + sat) : light + sat - light*sat;
	if(v > 0) {
		double m, sv;
		int sextant;
		double fract, vsf, mid1, mid2;
		m = light + light - v; sv = (v - m) / v;
		hue *= 6.0; sextant = int(hue);
		fract = hue - sextant; vsf = v*sv*fract; mid1 = m + vsf; mid2 = v - vsf;
		switch(sextant) {
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
		<< std::setfill('0') << std::setw(2) << std::hex << int(red  )
		<< std::setfill('0') << std::setw(2) << std::hex << int(green) 
		<< std::setfill('0') << std::setw(2) << std::hex << int(blue ) ;
	return stream.str();
};


void usage(char* progname){
  // Usage
    std::cout << "Usage: " << progname << " path/to/config" << std::endl;
    exit(1);
}

int main(int argc, char** argv){

  std::cout << "Display_Multiple_Trips (for Google Maps only) v" << MAJOR_VERSION << "." << MINOR_VERSION << std::endl;

// Parsing command line
  std::string config_name;
  if( argc == 2 ){
    config_name = argv[1];
  }
  else { 
    std::cout << "ERROR: No arguments. Read usage and relaunch properly." << std::endl; 
    usage(argv[0]); 
  }

// Safe config file parsing
  std::vector<std::string> input_names, input_tags;
  std::string output_name, output_title,input_folder("");
  std::string key, equal, value;
  ifstream filein(config_name);
  if( !filein ) {
    cout << "Configuration file " << config_name << " not found. Quitting..." << endl;
    exit(3);
  }
  while ( filein >> key >> equal >> value ){
    if ( key.substr(0,4) == "JSON" ){
      while( input_names.size() > input_tags.size() ) input_tags.push_back("DEFAULT");
      input_names.push_back(value);
    }
    else if ( key.substr(0,3) == "TAG" ){
      input_tags.push_back(value);
    }
    else if ( key == "HTML" ){
      while( input_names.size() > input_tags.size() ) input_tags.push_back("DEFAULT");
      output_name = value;
    }
		else if(key == "TITLE") {
			output_title = value;
		}
		else if(key == "FOLDER") {
			input_folder = value;
		}
    else{ 
      cout << "Key " << key << " unknown. Edit " << config_name << endl;
      exit(3);
    }
  }
  filein.close();

// Checks
	double index = 0;
  if( input_names.size() != input_tags.size() ){
    std::cout << "ALARM ALARM i " << input_names.size() << "  t " << input_tags.size() << std::endl;
    exit(1);
  }
  for (auto i : input_names) {
    filein.open(input_folder+i);
    if (!filein) {
      std::cout << "ERROR unable to open " << input_folder + i << ". Quitting..." << std::endl;
      exit(1);
    }
    filein.close();
  }

// Constructing DEFAULT tags and colors 
  for(int i=0; i<input_names.size(); i++){
    if( input_tags[i] == "DEFAULT" ) input_tags[i] = input_names[i].substr(0,input_names[i].size()-5);
		colors_button.push_back(HSLtoRGB(index / double(input_names.size()), 1, 0.4));
		colors_text.push_back("000000");
		index++;
  }

// Summary
  std::cout << "Sizes i " << input_names.size() << "  t " << input_tags.size() << std::endl;
  std::cout << "Inputs" << std::endl;
  for(size_t i=0; i<input_names.size(); i++)
    std::cout << i << " : " << input_names[i] << "     " << input_tags[i] << std::endl; 
  std::cout << "Output" << std::endl;
  std::cout << output_name << "   " << output_title << std::endl;

// Import JSON into vector
  std::vector<jsoncons::json> trips_coordinate;
  for( auto i : input_names ){
    jsoncons::json gps_records = jsoncons::json::parse_file(input_folder+i);
    trips_coordinate.push_back( gps_records );
  }

  std::cout << "Trips size : " << trips_coordinate.size() << std::endl;
  for( auto t : trips_coordinate ) std::cout << "Points : " << t.size() << std::endl;

// Write html
  std::ofstream output(input_folder+output_name);
  output << pre_header;
  output << "\t\t\t" << output_title;
  output << post_header;
  for( auto t : input_tags )
    output << "\t\t\tvar Trajectory_" << t << ";\n";
    output << "\n\t\t\tvar map;\n";
    output << "\n\t\t\tfunction initialize(){\n";
  for( size_t i=0; i<input_tags.size(); i++ ){
    output << "\t\t\t\tvar Locations_" << input_tags[i] << " = [\n\n";
    if( trips_coordinate[i].is_object() ) {    // object style
      int j = 0;
      for(auto rec = trips_coordinate[i].begin_members(); rec != trips_coordinate[i].end_members(); ++rec, ++j) {
        try{
          output 
            << "[" 
            << std::fixed << std::setprecision(6) 
            << ( rec->value().has_member("lat") ? rec->value()["lat"].as<double>() : 90.0 ) 
            << "," 
            << ( rec->value().has_member("lon") ? rec->value()["lon"].as<double>() : 0.0 )
            <<",'" 
            << (rec->value().has_member("date") ? rec->value()["date"].as<std::string>() : "NULL" )
            << "']" 
            << ((j != trips_coordinate[i].size() - 1) ? ',' : ' ')
            << "\n";
        }
        catch (const std::exception& e){
          std::cerr << e.what() << std::endl;
        }
      }
    }
    else if ( trips_coordinate[i].is_array() ){    // array style
      for (size_t j = 0; j < trips_coordinate[i].size(); ++j){
        try{
          output 
            << "[" 
            << std::fixed << std::setprecision(6) 
            << ( trips_coordinate[i][j].has_member("lat") ? trips_coordinate[i][j]["lat"].as<double>() : 90.0 )
            << "," 
            << ( trips_coordinate[i][j].has_member("lon") ? trips_coordinate[i][j]["lon"].as<double>() :  0.0 )
            <<",'"
            << ( trips_coordinate[i][j].has_member("date") ? trips_coordinate[i][j]["date"].as<std::string>() : "NULL" )
            << "']" 
            << ( (j != trips_coordinate[i].size()-1) ? ',' : ' ' ) 
            << "\n";
        }
        catch (const std::exception& e){
          std::cerr << e.what() << std::endl;
        }
      }
    }
    output << "\n\t\t\t\t]\n\n";
  }
  output << "\n";
  output << "\t\t\t\tmap = new google.maps.Map(document.getElementById(\'map\'), {\n";
  output << "\t\t\t\t\tzoom: 17,\n";
  output << "\t\t\t\t\tcenter: new google.maps.LatLng(Locations_" << input_tags[0] << "[1][0], Locations_" << input_tags[0] << "[1][1]),\n";
  output << "\t\t\t\t\tmapTypeId: google.maps.MapTypeId.ROADMAP\n";  
  output << "\t\t\t\t});\n";
  for( size_t i=0; i<input_tags.size(); i++ ){
    std::string t = input_tags[i];
    output << "\n\t\t\t\tvar PolyPath_" << t << " = [];\n";
    output << "\t\t\t\tfor(i=0; i<Locations_" << t << ".length; i++) {\n";
    output << "\t\t\t\t\tPolyPath_" << t << ".push(new google.maps.LatLng( Locations_" << t << "[i][0], "
           << "Locations_" << t << "[i][1] ) )\n";
    output << "\t\t\t\t}\n";
    output << "\n\t\t\t\tTrajectory_" << t << " = new google.maps.Polyline({\n";
    output << "\t\t\t\t\tpath: PolyPath_" << t << ",\n";
    output << "\t\t\t\t\tgeodesic: true,\n";
    output << "\t\t\t\t\tstrokeColor: \'#" << colors_button[i] << "\',\n";
    output << "\t\t\t\t\tstrokeOpacity: 1.,\n";
    output << "\t\t\t\t\tstrokeWeight: 2\n";
    output << "\t\t\t\t});\n";
  }
  output << "\n";
  for( auto t : input_tags )
    output << "\t\t\t\tTrajectory_" << t << ".setMap(map);\n";
  output << "\n\t\t\t\truler_map = new RulerMap(map);\n";
  output << "\t\t\t}\n\n";
  for( auto t : input_tags ){
    output << "\t\t\tfunction toggle_" << t << "(){\n";
    output << "\t\t\t\tTrajectory_" << t << ".setMap(Trajectory_" << t << ".getMap() ? null : map);\n";
    output << "\t\t\t};\n";
  }
  output << "\n\t\t\tgoogle.maps.event.addDomListener(window, \'load\', initialize);\n";
  output << "\n\t\t</script>" << std::endl;
  output << "\t\t<div id=\"panel\">" << std::endl;
  for( size_t i=0; i<input_tags.size(); i++ ){
    output << "\t\t\t<button onclick=\"toggle_" << input_tags[i] << "()\" style=\"background-color:#" 
           << colors_button[i] << "; color:#" << colors_text[i] << "\">" << input_tags[i] << "</button>\n";
  }
  output << "\t\t</div>" << std::endl;
  output << "\t</body>" << std::endl;
  output << "</html>" << std::endl;
  output.close();

  return 0;
}