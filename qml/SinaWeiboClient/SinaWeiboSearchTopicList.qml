import QtQuick 1.1
import "AppTheme.js" as AppTheme
import com.nokia.meego 1.0
import com.nokia.extras 1.0

Rectangle {
    id: searchTopicListPage
    property int listCount: 0
    property variant myModel
    color: AppTheme.color("#ffffff",settingModel.darkMode)
    signal itemClicked(int index);

    ListView {
        id: listView
        anchors.fill: parent

        model: myModel

        delegate:  Rectangle {
            id: listItem
            height: 65
            width: parent.width
            color: AppTheme.color("#ffffff",settingModel.darkMode)
            BorderImage {
                id: background
                anchors.fill: parent
                // Fill page porders
//                anchors.leftMargin: -listPage.anchors.leftMargin
//                anchors.rightMargin: -listPage.anchors.rightMargin
                visible: mouseArea.pressed
                source: "image://theme/meegotouch-list-background-pressed-center"
            }

            Label {
                id: mainText
                width: parent.width-20
                elide: Text.ElideRight
                anchors.leftMargin:15
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                text: (index == listCount - 1) ? name :"#"+ name + "#"
                color: (0 === index) ? "#0324B1" : AppTheme.color("#000000",settingModel.darkMode)
                font.pixelSize: 25
            }

            Image {
                source: ("image://theme/icon-m-common-drilldown-arrow"+(settingModel.darkMode?"-inverse":""))
                anchors.right: parent.right;
                anchors.verticalCenter: parent.verticalCenter
                visible: ((index == listCount-1) ? true : false)&&(funcContainer.isshowmoreindicator)
            }

            Image {
                anchors.left:  parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                source: "images/bg_comments_landscape.png"
                visible: true
            }

            MouseArea
            {
                id:mouseArea
                anchors.fill: parent
                onReleased:
                {
                    console.log("Item select : " + index);
                    searchTopicListPage.itemClicked(index);
                }
            }
        }
    }

}
