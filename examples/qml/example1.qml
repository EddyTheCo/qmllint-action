
import QtQuick
import QtQuick.Controls

ApplicationWindow {
    visible: true
    id:window

Rectangle {
    width: 100
    height: 100
    color: "red"
    border.color: "black"
    border.width: 5
    radius: 10
    anchors.centerIn:wrongWindowId
}

}
