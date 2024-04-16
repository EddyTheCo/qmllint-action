
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    visible: true
    id:window

    background:Rectangle
    {
        color:Style.backColor1
    }
    GridLayout
    {
        anchors.fill:parent
        columns:4

        PinPad
        {
            id:pinPad
            font.pointSize: 20
            margins:10
            inputMask:"DDDD"
            echoMode:TextInput.PasswordEchoOnEdit

        }

        Button
        {
            text:pinPad.text
            font.pixelSize: 20
            onClicked:popup.open();
        }
        Button
        {
            text:"Button"
            enabled: false
            font.pixelSize: 20
        }
        Button
        {
            text:"Button"
            font.pixelSize: 20
            flat:true
        }
        Button
        {
            text:"Button"
            font.pixelSize: 30
        }
        Button
        {
            text:"Button"
            font.pixelSize: 60
            highlighted: true
        }
        PrevButton
        {
            Layout.maximumWidth: 100
            Layout.maximumHeight: 100
            Layout.fillHeight: true
            Layout.fillWidth: true
            flat:true
        }
        NextButton
        {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.maximumWidth: 100
            Layout.maximumHeight: 100
            highlighted: true
        }
        PrevButton
        {
            Layout.maximumWidth: 100
            Layout.maximumHeight: 100
            Layout.fillHeight: true
            Layout.fillWidth: true
            flat:true
            enabled: false
        }
        NextButton
        {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.maximumWidth: 100
            Layout.maximumHeight: 100
            highlighted: true
            enabled: false
        }
        PrevButton
        {
            shaderIcon:"filledArrowHead"
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.maximumWidth: 50
            Layout.maximumHeight: 50
            highlighted: true
        }

        NextButton
        {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.maximumWidth: 50
            Layout.maximumHeight: 50
            shaderIcon:"filledArrowHead"
            flat:true
        }
        PrevButton
        {
            shaderIcon:"filledArrowHead"
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.maximumWidth: 50
            Layout.maximumHeight: 50
            highlighted: true
            enabled:false
        }

        NextButton
        {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.maximumWidth: 50
            Layout.maximumHeight: 50
            shaderIcon:"filledArrowHead"
            flat:true
            enabled: false
        }


        ThemeSwitch
        {

        }

        CloseButton
        {
            Layout.preferredHeight: 100
            Layout.preferredWidth: 100
            ToolTip.timeout: 5000
            ToolTip.visible: hovered
            ToolTip.text: qsTr("This tool tip is shown ")

        }
        CloseButton
        {
            flat:true
            Layout.preferredHeight: 50
            Layout.preferredWidth: 50
        }
        CloseButton
        {
            enabled:false
            Layout.preferredHeight: 50
            Layout.preferredWidth: 50
        }
        CloseButton
        {
            enabled:false
            flat:true
            Layout.preferredHeight: 100
            Layout.preferredWidth: 100
        }
        Switch
        {
            text:"MySwitch"
        }
        TextField
        {
            placeholderText:"This is the place holder text"
        }
        TextField
        {
            placeholderText:"This is the place holder text"
            font.pixelSize: 30
            ToolTip.timeout: 5000
            ToolTip.visible: hovered
            ToolTip.text: qsTr("This tool tip is shown ")

        }
        TextField
        {
            placeholderText:"This is the place holder text"
            font.pixelSize: 30
            enabled:false
        }
        TextArea
        {
            placeholderText:"This is the place holder text\n for a ext area"
        }
        Item
        {
            Layout.minimumHeight: 100
            Layout.minimumWidth: 200
            ScrollView {
                anchors.fill:parent
                TextArea
                {
                    placeholderText:"This is the place holder text\n for a scrollable text area "
                }
            }
        }


        TextArea
        {
            placeholderText:"This is the place holder text\n for a disabled text area "
            enabled:false
            font.pixelSize: 30

        }
        ComboBox
        {
            editable: true
            model: ListModel {
                id:hrplist

                ListElement { text: "rms" }
                ListElement { text: "iota" }
                ListElement { text: "smr" }
            }
            onAccepted: {
                if (find(editText) === -1)
                    hrplist.append({text: editText})
            }
        }
        ComboBox
        {
            editable: true
            enabled:false
            model: ListModel {
                id:hrplist2

                ListElement { text: "rms" }
                ListElement { text: "iota" }
                ListElement { text: "smr" }
            }
            onAccepted: {
                if (find(editText) === -1)
                    hrplist2.append({text: editText})
            }
        }
        ComboBox
        {
            editable: false

            model: ListModel {

                ListElement { text: "rms" }
                ListElement { text: "iota" }
                ListElement { text: "smr" }
            }

        }
        Frame
        {
            GridLayout
            {
                TabBar {

                    TabButton {
                        text: qsTr("Current")
                    }
                    TabButton {
                        text: qsTr("Restore")

                    }
                }
                SpinBox {
                    id:accountnumber
                    value: 0
                    from:0
                    to: 429496729
                }
                Label
                {
                    text:"this is a label"
                }
                Label
                {
                    text:"this is a label"
                    font.pixelSize: 25
                }
            }
        }
        CheckBox
        {
        }
        DelayButton
        {
            text:"DelayButton"
            delay:2000
        }

    }

    Popup
    {
        id:popup
        anchors.centerIn: Overlay.overlay
        closePolicy: Popup.CloseOnPressOutside
        width:parent.width*0.5
        height:parent.height*0.5
    }

}
