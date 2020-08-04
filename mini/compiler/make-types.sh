:
grep : parser.y | awk -F: '{printf "%%type <node> %s\n", $1}'
