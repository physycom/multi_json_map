---
documentclass: physycomen
title:  "multi_json_map"
author: "Di Cristina, Fabbri, Guidetti, Sinigardi"
---

<a href="http://www.physycom.unibo.it"> 
<div class="image">
<img src="https://cdn.rawgit.com/physycom/templates/697b327d/logo_unibo.png" width="90" height="90" alt="© Physics of Complex Systems Laboratory - Physics and Astronomy Department - University of Bologna"> 
</div>
</a>
<a href="https://travis-ci.org/physycom/multi_json_map"> 
<div class="image">
<img src="https://travis-ci.org/physycom/multi_json_map.svg?branch=master" width="90" height="20" alt="Build Status"> 
</div>
</a>
<a href="https://ci.appveyor.com/project/cenit/multi-json-map"> 
<div class="image">
<img src="https://ci.appveyor.com/api/projects/status/uleaal97meki5e6x?svg=true" width="90" height="20" alt="Build Status"> 
</div>
</a>

### Purpose
This tool parses a number of .json files containing GNSS coordinates as per our file format and produces an .html webpage to compare them in Google Maps.

### Installation
**make** and a **C++11** compatible compiler are required. Clone the repo and type ``make``, it should be enough in most cases!   
There's also a **VS2015** solution avalaible.   
Contains [jsoncons](https://github.com/danielaparker/jsoncons) as a git submodule.

### Usage
Type
```
multi_json_map.exe path/to/config
```
where the config file should resemble
```
JSON1  = trip1.json
TAG1   = label1
JSON2  = trip2.json
TAG2   = label2
HTML   = trips.html
TITLE  = MultiTrip
FOLDER = working/path
```
*TAG\** and *FOLDER* fields are optional whereas the other fields are mandatory. A dummy config file can be generated by typing
```
multi_json_map.exe -conf_t
```
