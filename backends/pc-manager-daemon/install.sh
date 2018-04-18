#!/bin/sh
backupPath="/var/lib/pc-manager-daemon/"
backendPath="/usr/lib/python2.7/dist-packages/pc-manager-daemon/"
cd `dirname $0`

cp ./dbus/com.cdos.pc.service /usr/share/dbus-1/system-services/ 
echo "Copy .service file to /usr/share/dbus-1/system-services/"

cp ./dbus/com.cdos.session.service /usr/share/dbus-1/services/ 
echo "Copy .service file to /usr/share/dbus-1/services/"

cp ./dbus/com.cdos.pc.policy /usr/share/polkit-1/actions/
echo "Copy .policy file to /usr/share/polkit-1/actions/"

cp ./dbus/com.cdos.pc.conf /etc/dbus-1/system.d/
echo "Copy .conf file to /etc/dbus-1/system.d/"

if [ ! -d "$backendPath" ]; then
    cp -rf ./src /usr/lib/python2.7/dist-packages/pc-manager-daemon/
    echo "Copy backend folder to /usr/lib/python2.7/dist-packages/pc-manager-daemon/"
else
    rm -rf "$bakendPath"
    cp -rf ./src /usr/lib/python2.7/dist-packages/pc-manager-daemon/
    echo "Copy backend folder to /usr/lib/python2.7/dist-packages/pc-manager-daemon/"
fi

if [ ! -d "$backupPath" ]; then
    cp -rf ./data/beautify /var/lib/pc-manager-daemon/
    cp -rf ./data/processmanager /var/lib/pc-manager-daemon/
    echo "Copy backup folder to /var/lib/pc-manager-daemon/"
fi

rm -f /usr/bin/pc-manager-backend.py
echo "Remove /usr/bin/pc-manager-backend.py"

rm -f /usr/bin/pc-manager-session.py
echo "Remove /usr/bin/pc-manager-session.py"

chmod +x "$backendPath"/src/start_systemdbus.py
ln -s "$backendPath"/src/start_systemdbus.py  /usr/bin/pc-manager-backend.py

chmod +x "$backendPath"/src/start_sessiondbus.py
ln -s "$backendPath"/src/start_sessiondbus.py  /usr/bin/pc-manager-session.py

echo "Build symbol link for service file"
echo "^^ Now, You can run the program in QtCreator!"
