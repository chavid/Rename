
# Previously source ../env.bash

targets = [

    { "name" : "help" , "command" : "rn --help" },
    { "name" : "txt_md" , "command" : "rn -c %.txt %.md" },
    { "name" : "tirets" , "command" : "rn -c %_% %-%" },
    { "name" : "dir" , "command" : "rn -c %Dir% %Directory%" },

]

# Filtres

run_filters_out = [
]

diff_filters_in = [
  {"name": "all", "re": "^(.*)$", "apply": "%"}
]

