#/bin/bash
counter=1
{
for t in *.json; do
	echo "TAG$counter = $counter"
	echo "JSON$counter = $t"
	((counter++))
done
echo "HTML = map.html" 
} > map.cfg

multi_json_map.exe map.cfg
