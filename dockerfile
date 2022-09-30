FROM kasmweb/core-ubuntu-bionic:1.10.0
USER root

ENV HOME /home/kasm-default-profile
ENV STARTUPDIR /dockerstartup
WORKDIR $HOME

######### Start Customizations ###########

######### Builder ###########

RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive \
    apt-get install --no-install-recommends -y \
    build-essential \
    libsqlite3-dev \
    libgcrypt20-dev \
    libsdl2-dev \
    libsdl2-image-dev \
    libsdl2-ttf-dev \
    libsdl2-mixer-dev \
    python3 && \
    apt-get autoclean && \
    rm -rf \
    /var/lib/apt/lists/* \
    /var/tmp/* \
    /tmp/*

COPY . /l-type
WORKDIR /l-type

RUN make bin/server && \
    make bin/client-gui

######### Installer ###########  

RUN mkdir -p /opt/l-type/
RUN mv bin/server /opt/l-type/l-type_server
RUN mv bin/client-gui /opt/l-type/l-type_client-gui
RUN mkdir -p /opt/l-type/keys/
RUN mv keys/* /opt/l-type/keys/
RUN mkdir -p /opt/l-type/static/
RUN mv static/* /opt/l-type/static/

WORKDIR $HOME

RUN rm -rf /l-type
RUN chown -R 1000:0 /opt/l-type/
RUN mkdir -p /tmp/l-type && chown -R 1000:0 /tmp/l-type

COPY ./kasm/custom_startup.sh $STARTUPDIR/custom_startup.sh
RUN chmod +x $STARTUPDIR/custom_startup.sh
RUN chmod 755 $STARTUPDIR/custom_startup.sh
COPY ./kasm/maximize_window.sh $STARTUPDIR/maximize_window.sh
RUN chmod +x $STARTUPDIR/maximize_window.sh
RUN chmod 755 $STARTUPDIR/maximize_window.sh


# Update the desktop environment to be optimized for a single application
RUN cp $HOME/.config/xfce4/xfconf/single-application-xfce-perchannel-xml/* $HOME/.config/xfce4/xfconf/xfce-perchannel-xml/
RUN rm -rf /usr/share/extra/backgrounds/bg_default.png
RUN apt-get remove -y xfce4-panel

######### End Customizations ###########

RUN chown 1000:0 $HOME

ENV HOME /home/kasm-user
WORKDIR $HOME
RUN mkdir -p $HOME && chown -R 1000:0 $HOME

USER 1000
