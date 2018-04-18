TEMPLATE = aux

inst1.files += ../backends/pc-manager-daemon/src/
inst1.path = /usr/lib/python2.7/dist-packages/pc-manager-daemon/
inst2.files += ../backends/pc-manager-daemon/data/beautify/autostart/
inst2.path = /var/lib/pc-manager-daemon/
inst3.files += ../backends/pc-manager-daemon/data/beautify/plymouth/
inst3.path = /var/lib/pc-manager-daemon/
inst4.files += ../backends/pc-manager-daemon/data/beautify/sound-theme/
inst4.path = /var/lib/pc-manager-daemon/
inst5.files += ../backends/pc-manager-daemon/data/processmanager/
inst5.path = /var/lib/pc-manager-daemon/

INSTALLS += inst1 \
    inst2 \
    inst3 \
    inst4 \
    inst5
