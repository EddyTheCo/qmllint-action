import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: window
    visible: true

    Rectangle {
        width: 100
        height: 100
        color: "red"
        border.color: "black"
        border.width: 5
        radus: 10
        anchors.centerIn: window1
    }
}
