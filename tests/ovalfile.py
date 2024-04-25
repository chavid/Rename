
# Previously source ../env.bash

targets = [

    { "name" : "my_utility_help" , "command" : "mu --help" },

]

# Filtres

run_filters_out = [
]

diff_filters_in = [
  {"name": "all", "re": "^(.*)$", "apply": "%"}
]

