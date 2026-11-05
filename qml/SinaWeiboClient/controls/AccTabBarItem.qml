import QtQuick 1.0
import "../AppTheme.js" as AppTheme

Item {
    id:tabButton
    width: 100
    height: 62
    //property bool currentOne: false
    property bool notRightOne: true
    property string content: ""
    signal clickthis

    Text {
        anchors.centerIn: parent
        text: content
        color:"white"
        font.pixelSize: 25
    }
    Rectangle{color:AppTheme.color("white",settingModel.darkMode);
        id:line
        anchors.right: parent.right
        width: 1
        height: parent.height
        visible: tabButton.notRightOne
    }
    MouseArea{
        anchors.fill: parent
        onClicked: {
            tabButton.clickthis()
        }
    }
}
