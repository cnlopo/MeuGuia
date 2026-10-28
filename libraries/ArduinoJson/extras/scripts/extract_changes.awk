#!/usr/bin/awk -f

/\* / {
    STARTED=1
    EMPTY_LINE=0
}

/^[[:space:]]*$/ {
    EMPTY_LINE=1
}

/^v[[:digit:]]/ {
    if (STARTED) exit
}

!/^[[:space:]]*$/ {
    if (STARTED) {
        if (EMPTY_LINE) {
            print ""
            EMPTY_LINE=0
        }
        print
    }
}
