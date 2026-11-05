import QtQuick 1.1
import "AppTheme.js" as AppTheme
import com.nokia.meego 1.0
import "controls"
import "SinaWeiboErrorCode.js" as ErrorCodeSearchUserPage

Page {
 orientationLock:PageOrientation.LockPortrait;

    id:sendpage
    anchors.fill:parent
    property int pageWdith: parent.width

    Rectangle{
        anchors.fill: parent
        color: AppTheme.color("#f0f1f2",settingModel.darkMode)
    }

    AccTitleBar{
        id: friendPageTitle
        width: parent.width
        height: 65
        title:"私信"
        anchors.top: parent.top
        z: infortable.z + 1
    }
    //tool bar without delete button
    ToolBarLayout
    {
        id: toolbar
        visible: false
        ToolIcon
        {
            iconId: theme.inverted ? "icon-m-toolbar-back-white" : "icon-m-toolbar-back";
            onClicked: {
                    listPage.viewBack();
            }
        }
    }

    tools: toolbar

    Rectangle{color:AppTheme.color("white",settingModel.darkMode);
        id:infortable
        anchors.top: friendPageTitle.bottom
        width: parent.width
        height: 100
        z:flickable.z+1
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
                source: messageCenterModel.getSenderImagebyIndex(funcContainer.indexForMessageCenter);
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
                    visible: messageCenterModel.getVipbyIndex(funcContainer.indexForMessageCenter);
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
                text: messageCenterModel.getSenderNamebyIndex(funcContainer.indexForMessageCenter);
                width: parent.width/2;
                elide: Text.ElideRight
                smooth:true;
                font.pixelSize: 28;
            }
            Text {
                id:sendtime
                anchors.right: parent.right; anchors.rightMargin:20
                anchors.bottom: sendername.bottom
                text: sendpage.changeDateFormat(messageCenterModel.getSendTimebyIndex(funcContainer.indexForMessageCenter));
                color: AppTheme.color("black",settingModel.darkMode)
                smooth:true;
                font.pixelSize: 20;
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
    }

    Flickable {
        id: flickable
        anchors {left: parent.left; right: parent.right; top:infortable.bottom; topMargin:5}
//        width: parent.width
        height: parent.height - friendPageTitle.height - infortable.height - toolbar.height
        contentWidth: parent.width
        contentHeight: editbox.implicitHeight
        flickableDirection: Flickable.VerticalFlick

        TextArea {platformStyle:AppInputStyle {}

            id:editbox
            anchors {left: parent.left; right: parent.right}
            height:implicitHeight
            text: messageCenterModel.getContentbyIndex(funcContainer.indexForMessageCenter);
            readOnly: true
        }
    }

    function changeDateFormat(oldString)
    {
			return oldString; //k
        var bef = funcContainer.tranferStringMonthToInt(oldString.substr(0,3));

        return bef+"-"+oldString.substr(4,8);
    }

}
