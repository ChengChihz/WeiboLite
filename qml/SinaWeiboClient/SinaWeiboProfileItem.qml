import QtQuick 1.0
import "AppTheme.js" as AppTheme

Item {
    id: profileitem
    property string title
    property string content
    property int anchtype
    property int textSize
    signal itemClicked(int index);

    anchors.top: parent.top; anchors.topMargin: anchtype > 2 ? parent.height/2 : 0;
    anchors.left: parent.left; anchors.leftMargin: (anchtype%2 === 0)  ? parent.width/2 : 0;
    width: parent.width/2
    height: parent.height/2
    Text {color:AppTheme.color("black",settingModel.darkMode);
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        font.pixelSize: textSize
        text: "<body> <center> <font color = #337ea9>"+ content + " </font color> "+ "<br/>" + title + "</center></body>"
    }
    MouseArea{
        anchors.fill: parent
        onClicked: {
            profileitem.itemClicked(anchtype);
        }
    }
}
