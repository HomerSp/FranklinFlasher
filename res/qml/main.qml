import QtQuick 2.5
import QtQuick.Window 2.2
import "components"
import "."

Window {
    Units {
        id: unit
    }

    FontLoader {
        id: openSansRegularFont;
        source: "qrc:/res/fonts/OpenSans-Regular.ttf"
    }

    width: unit.dp(640)
    height: unit.dp(480)
    visible: true

    MainForm {
        anchors.fill: parent
    }
}
