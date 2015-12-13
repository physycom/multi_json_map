# multi_json_map
Along the lines of [json_to_html](https://github.com/physycom/json_to_html) here you have a similar tool which parse a number of JSON files containing gnss coordinates (according to **physycom** standard) and produces an html script to compare them in Google Maps.

### Installation
Make with ``make`` makefile, provided in the repo.

### Usage
Type
```
distri.exe path/to/config
```
where the config file should rensemble
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
*TAG* fields are optional whereas the others fields are mandatory.
