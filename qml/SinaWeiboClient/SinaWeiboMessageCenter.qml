import QtQuick 1.1
import "AppTheme.js" as AppTheme
import com.nokia.meego 1.0
import "controls"
import "SinaWeiboErrorCode.js" as ErrorCodeSearchUserPage

Rectangle
{
    id:messageCenterPage
    //anchors.fill:parent
    width: 480
    height: 800
    color: "transparent"
    property int pageWdith: parent.width
    signal itemlistscrolled(int type);
    signal itemSelected(int index);
    property bool allowRefresh: false
    property bool needRefresh: !messageCenterModel.isReadingDB

//    AccTitleBar{
//        id: messageCenterTitle
//        width: parent.width
//        height: 65
//        title:"私信"
//        anchors.top: parent.top
//        z: messageList.z + 1
//    }

    Component {
        id: messageitem

        Item{
            width: parent.width
            height: 100
            MouseArea
            {
                id:contentArea
                anchors.fill: parent
                onClicked:
                {
                    console.log("Item select" + index);
                    messageCenterPage.itemSelected(index);
                }
            }

            //Image
            Item{
                id: senderphotoarea
                width: 80; height: 80
                //anchors.top: parent.top;  anchors.left: parent.left
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter

                Image {
                    width: 64; height: 64
                    //anchors.top: parent.top; anchors.topMargin: (parent.height - height)/2
                    //anchors.left: parent.left; anchors.leftMargin: (parent.width - width)/2
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    smooth: true
                    source: imageAddress;
                    Image {
                        id: mask
                        anchors.fill: parent
                        smooth: true
                        source:settingModel.darkMode?"controls/icons/dark/avatar-corners.svg":"controls/icons/avatar-corners.svg"
                    }

                    Image{
                        id:vipBadge
                        anchors.right: parent.right; anchors.top: parent.top
                        source: "images/icon_VIP.png"
                        visible: isVip;
                        smooth: true
                    }
                }
            }

            //Name
            Item{
                id: nametimearea
                width: parent.width - senderphotoarea.width; //height: 30
                anchors.left: senderphotoarea.right
                anchors.top: senderphotoarea.top
                //anchors.topMargin: 5

                //UserName
                Text {color:AppTheme.color("black",settingModel.darkMode);
                    id: sendername
                    //anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left; anchors.leftMargin: 10
                    anchors.top: parent.top; anchors.topMargin: 10
                    text: senderId
                    width: parent.width/2;
                    elide: Text.ElideRight
                    smooth:true;
                    font.pixelSize: 22;
                }
                Text {
                    id:sendtime
                    anchors.right: parent.right; anchors.rightMargin:20+arrow.paintedWidth
                    anchors.bottom: sendername.bottom
                    text: changeDateFormat(dateInfo)
//                    color: "lightblue"
                    color:AppTheme.color("black",settingModel.darkMode)
                    smooth:true;
                    font.pixelSize: 20;
                }
                Text{
                    id:content;
                    anchors.left: parent.left; anchors.leftMargin: 10
                    anchors.top: sendername.bottom; anchors.topMargin: 10
                    width: parent.width/2;
                    text: shortText
                    color:AppTheme.color("black",settingModel.darkMode)
                    smooth:true;
                    //horizontalAlignment:Text.AlignLeft
                    elide: Text.ElideRight
                    font.pixelSize: 28;
                }
            }

            Image {
                id: line
                anchors.top: parent.top
                anchors.topMargin: 99
                width: parent.width
                //height: 8
                smooth: true
                height:1;source:"";Rectangle {anchors.fill:parent;color:AppTheme.color("#e7ebee",settingModel.darkMode)}
                fillMode:Image.Stretch
            }

            Image {
                id:arrow
                source: ("image://theme/icon-m-common-drilldown-arrow"+(settingModel.darkMode?"-inverse":""))
                anchors.right: parent.right;
                anchors.rightMargin: 10;
                anchors.verticalCenter: parent.verticalCenter
            }

            Rectangle
            {
                id:marginfoucus
                anchors.fill:parent
                color: AppTheme.color("lightsteelblue",settingModel.darkMode);
                opacity: 0.3
                z:parent.z + 1;
                visible: contentArea.pressed;
            }
        }
    }

    property alias actualList:messageList
    property alias refreshInProgress:messageList.refreshInProgress
    property alias interactionLocked:messageList.interactionLocked
    SinaWeiboItemList {id:messageList;anchors.fill:parent;model:messageCenterModel;delegate:messageitem
        onItemlistscrolled:messageCenterPage.itemlistscrolled(type)
    }

    ContextMenu {
        id: contextMenu
        icon: "image://theme/icon-l-contacts"
        titleText: "请选择"
        MenuLayout {
            MenuItem {text: "查看私信"; onClicked: { listPage.changePage(33) } }
            MenuItem {text: "回复私信"; onClicked: { listPage.changePage(25) } }
            MenuItem {text: "查看个人资料"; onClicked: { listPage.changePage(7) }}
            MenuItem {text: "删除这条私信"; onClicked: { query.open() }}
        }
    }

    QueryDialog {
        id: query

        message: "是否删除此私信？"
        acceptButtonText: "是"
        rejectButtonText: "否"
        onAccepted: {
            messageCenterModel.deleteTheMessage(funcContainer.indexForMessageCenter);
        }
    }
    /*Dialog {
        id: query
        title: Item {
            id: titleField
            height: query.style.titleBarHeight
            width: parent.width
            Image {
                id: supplement
                source: "images/icon_popup.png"
                height: parent.height - 10
                width: 75
                fillMode: Image.PreserveAspectFit
                anchors.leftMargin: 5
                anchors.rightMargin: 5
            }
            Label {
                id: titleLabel
                anchors.left: supplement.right
                anchors.verticalCenter: titleField.verticalCenter
                font.capitalization: Font.MixedCase
                color: "white"
                text: "询问"
            }
            Image {
                id: closeButton
                anchors.verticalCenter: titleField.verticalCenter
                anchors.right: titleField.right
                source: "image://theme/icon-m-framework-close"
                MouseArea {
                    id: closeButtonArea
                    anchors.fill: parent
                    onClicked:  { query.reject(); }
                }
            }
        }
        content:Item {
            id: name
            height: childrenRect.height
            Text {
                id: text
                font.pixelSize: 22
                color: "white"
                text: "确定删除此私信？"
            }
        }
        buttons: ButtonRow {
            style: ButtonStyle { }
            anchors.horizontalCenter: parent.horizontalCenter
            Button {
                id: b1;
                text: "确定";
                onClicked: {
                    query.accept();
                    messageCenterModel.deleteTheMessage(funcContainer.indexForMessageCenter);
                }
            }
            Button {id: b2; text: "取消"; onClicked: query.reject()}
        }
    }*/
    onItemSelected: {
        funcContainer.userIDInMyProfilePage = messageCenterModel.getSenderIdbyIndex(index);
        funcContainer.senderScreenName = messageCenterModel.getSenderNamebyIndex(index);
        funcContainer.indexForMessageCenter = index;
        contextMenu.open();
    }
    function changeDateFormat(oldString)
    {
			return oldString; //k
        var bef = funcContainer.tranferStringMonthToInt(oldString.substr(0,3));

        return bef+"-"+oldString.substr(4,8);
    }

}
