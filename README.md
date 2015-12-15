[![Build Status Master](https://travis-ci.org/physycom/multi_json_map.png?branch=master)](https://travis-ci.org/physycom/multi_json_map "master")


Along the lines of [json_to_html](https://github.com/physycom/json_to_html) here you have a similar tool which parses a number of JSON files containing gnss coordinates (according to **physycom** standard) and produces an html script to compare them in Google Maps.

### Installation
Make with ``make`` makefile, provided in the repo.

### Usage
Type
```
distri.exe path/to/config
```
where the config file should resemble
```
JSON1 = lun.json
TAG1  = LUN
JSON2 = mar.json
TAG2  = MAR
JSON3 = mer.json
TAG3  = MER
JSON4 = GIO.json
HTML  = trips.html
TITLE = MultiTrip
```
*TAG* fields are optional whereas the other fields are mandatory.
