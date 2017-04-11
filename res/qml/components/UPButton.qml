import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.2
import QtQuick.Window 2.2

Button {
    property string scheme

    scheme: "light"
    buttonColor: (scheme === "light")?"#5f92eb":"#fff"
    buttonDisabledColor: (scheme === "light")?"#4f82db":"#eee"
    property color buttonColor
    property color buttonDisabledColor

    Units {
        id: unit
    }

    FontLoader {
        id: openSansRegularFont;
        source: "qrc:/res/fonts/OpenSans-Regular.ttf"
    }

    style: ButtonStyle {
        background:  Rectangle {
            implicitWidth: control.width
            implicitHeight: control.height
            border.width: control.activeFocus ? 2 : 0
            border.color: "#ccc"
            color: (control.enabled)?buttonColor:buttonDisabledColor
            radius: unit.dp(4)

            transitions: Transition {
                NumberAnimation { properties: "border.width"; duration: 100 }
            }

            Rectangle {
                anchors.fill: parent
                radius: unit.dp(4)

                color: (control.scheme === 'light')?'white':'black'
                opacity: (!control.enabled)?0.0:(control.pressed)?0.3:(control.hovered)?0.2:0.0

                Behavior on opacity {
                    NumberAnimation { duration: 100 }
                }
            }
        }
        label: Text {
            color: (control.scheme === "light")?((control.enabled)?"white":"#CCC"):((control.enabled)?"black":"#333")
            text: control.text
            font.family: openSansRegularFont.name
            font.pixelSize: unit.em(1.2)
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }
}

