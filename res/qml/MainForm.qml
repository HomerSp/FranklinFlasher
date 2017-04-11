import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1
import "components"

Rectangle {
    anchors.fill: parent

    color: '#009688'

    ColumnLayout {
        anchors.centerIn: parent
        width: unit.dp(360)
        spacing: unit.dp(24)

        TextArea {
            anchors {left: parent.left; right: parent.right}
            width: unit.dp(360)
            implicitHeight: unit.dp(80)
            textColor: "white"
            text: (flashButton.enabled)?flashDevice + " Flash tool\nPlease turn on the device and connect to its wifi now.":flashDevice + " Flash tool"
            font.family: openSansRegularFont.name
            font.pixelSize: unit.em(1.3)
            horizontalAlignment: Text.AlignHCenter

            backgroundVisible: false
            frameVisible: false
            readOnly: true
            textFormat: Text.PlainText
            wrapMode: Text.Wrap
            horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
            verticalScrollBarPolicy: horizontalScrollBarPolicy
        }

        TextArea {
            anchors {left: parent.left; right: parent.right}
            width: unit.dp(360)
            implicitHeight: unit.dp(80)
            objectName: "statusText"
            id: statusText
            textColor: "white"
            text: "Initializing, please wait..."
            font.family: openSansRegularFont.name
            font.pixelSize: unit.em(1.2)
            horizontalAlignment: Text.AlignHCenter

            backgroundVisible: false
            frameVisible: false
            readOnly: true
            textFormat: Text.PlainText
            wrapMode: Text.Wrap
            horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
            verticalScrollBarPolicy: horizontalScrollBarPolicy

            function setStatus(status) {
                text = status
            }
        }

        ProgressBar {
            id: flashingProgress
            objectName: 'flashingProgress'
            width: unit.dp(240)
            anchors {left: parent.left; right: parent.right}

            minimumValue: 0
            maximumValue: 100
            value: 0

            style: ProgressBarStyle {
                background: Rectangle {
                    color: 'transparent'

                    implicitHeight: unit.dp(8)
                }
                progress: Rectangle {
                    color: '#FFF'
                }
            }

            function setProgressMax(max) {
                maximumValue = max
            }
            function setProgress(progress) {
                value = progress
            }
        }

        UPButton {
            id: flashButton
            objectName: "flashButton"
            anchors.horizontalCenter: parent.horizontalCenter
            scheme: "dark"
            text: qsTr("Flash")
            enabled: false
            onClicked: {
                enabled = false
            }

            function enable() {
                enabled = true
            }
        }
    }
}
