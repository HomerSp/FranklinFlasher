import QtQuick 2.5
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0

QtObject {
    function dp( x ) {
        return Math.round( x * Settings.dpiScaleFactor );
    }

    function em( x ) {
        return Math.round( x * TextSingleton.font.pixelSize );
    }
}
