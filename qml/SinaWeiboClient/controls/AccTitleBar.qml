import QtQuick 1.1
import "../AppTheme.js" as AppTheme
import com.nokia.meego 1.0

Item {
    id: acctitlebar
    property string bgsource: "../images/titlebg_blue.png"
    property string title: ""
    property string textcolor: AppTheme.color("#202830",settingModel.darkMode)
    property int textOffset: 16
    property int titlesize: 30
    property bool isTexthorizontalCenter: false

    Rectangle { anchors.fill: parent; color: AppTheme.color("#f8fafb",settingModel.darkMode)
        Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: AppTheme.color("#e7ebee",settingModel.darkMode) }
    }

    Label {
        id:titlelabel
        anchors.verticalCenter: parent.verticalCenter
        text: title;
        color: textcolor
        font.pixelSize: titlesize
    }

    state: isTexthorizontalCenter ? "horizontalcenter" : "custom"
    states: [
        State {
             name: "horizontalcenter"
             AnchorChanges {
                 target: titlelabel
                 anchors.horizontalCenter: acctitlebar.horizontalCenter
             }
         },
        State {
             name: "custom"
             AnchorChanges {
                 target: titlelabel
                 anchors.left: acctitlebar.left
             }
             PropertyChanges {
                 target: titlelabel
                 anchors.leftMargin: acctitlebar.textOffset
             }
         }
    ]
}
