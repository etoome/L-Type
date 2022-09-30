#!/usr/bin/env bash

export MAXIMIZE="true"
export MAXIMIZE_NAME="L-Type"

startup() {

    sleep 3

    while true
    do
        if ! pgrep -x "l-type_server" > /dev/null
        then
            /usr/bin/filter_ready
            /usr/bin/desktop_ready
            cd /opt/l-type && ./l-type_server &
        fi

        if ! pgrep -x "l-type_client-gui" > /dev/null
        then
            /usr/bin/filter_ready
            /usr/bin/desktop_ready
            bash $STARTUPDIR/maximize_window.sh &
            cd /opt/l-type && ./l-type_client-gui
        fi

        sleep 1
    done
}


startup

