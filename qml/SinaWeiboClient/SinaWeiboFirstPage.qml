import QtQuick 1.1
import "AppTheme.js" as AppTheme
import com.nokia.meego 1.0
import "controls"
import "SinaWeiboErrorCode.js" as ErrorCodeFstPage

Rectangle
{
    id:homePage
    anchors.fill:parent
    color: AppTheme.color("white",settingModel.darkMode)
    property int pageWdith: parent.width
    property bool refreshing:false
    property bool revealNewPosts:false
    Timer {id:revealTimer;interval:1;onTriggered:sinalist.positionViewAtBeginning()}

    property bool userRefresh:false
    property bool scrollAnimating:false
    property real refreshOffset:0
    NumberAnimation {id:refreshOpen;target:homePage;property:"refreshOffset";duration:180;easing.type:Easing.OutCubic}
    NumberAnimation {id:refreshClose;target:homePage;property:"refreshOffset";duration:180;easing.type:Easing.OutCubic;onRunningChanged:{if(!running&&homePage.refreshOffset<1)homePage.userRefresh=false;}}
    Timer {id:refreshDispatch;interval:200;onTriggered:firstPageModel.getSinaContentFromModel(0)}
    Timer {id:refreshFinish;interval:350;onTriggered:{refreshClose.from=homePage.refreshOffset;refreshClose.to=0;refreshClose.start();}}
    NumberAnimation {id:scrollTop;target:sinalist;property:"contentY";duration:300;easing.type:Easing.OutCubic;onRunningChanged:{if(!running&&homePage.scrollAnimating){homePage.scrollAnimating=false;homePage.refresh();}}}
    function refresh(distance){
        if(userRefresh||refreshing)return;
        revealNewPosts=sinalist.atYBeginning;
        var held=typeof distance==="number"&&distance>=72?Math.min(distance,Math.max(72,height-homepagetitle.height-80)):0;
        userRefresh=true;refreshOffset=held;sinalist.positionViewAtBeginning();
        refreshOpen.from=held;refreshOpen.to=72;refreshOpen.start();refreshDispatch.restart();
    }
    function returnToTopAndRefresh(){
        if(userRefresh||scrollAnimating)return;
        var previous=sinalist.contentY;var top;if(typeof sinalist.originY==="number")top=sinalist.originY;else{sinalist.positionViewAtBeginning();top=sinalist.contentY;sinalist.contentY=previous;}
        if(Math.abs(previous-top)<2){refresh();return;}
        scrollAnimating=true;scrollTop.from=previous;scrollTop.to=top;scrollTop.start();
    }
    function switchFeed(recommended){if(firstPageModel.recommended===recommended)return;sinalist.positionViewAtBeginning();firstPageModel.setRecommended(recommended);}
    function activepage(){
        funcContainer.hometabindex = 0;

        if(funcContainer.needRefresh) {
            funcContainer.needRefresh = false;
            refresh();
        }
    }

    Rectangle {
        id: homepagetitle
        z: 1
        width: parent.width; height: 80; color: AppTheme.color("#f8fafb",settingModel.darkMode)
        Image { id: profileImg; x:16; anchors.verticalCenter:parent.verticalCenter; width:48; height:48; source:loginModel.profileImageUrl; fillMode:Image.PreserveAspectCrop; smooth:true; Image {anchors.fill:parent;source:(settingModel.darkMode?"controls/icons/dark/avatar-corners-header.svg":"controls/icons/avatar-corners-header.svg");smooth:true} }
        Text {x:78;anchors.verticalCenter:parent.verticalCenter;width:parent.width-286;text:loginModel.nickName;font.pixelSize:24;font.bold:true;color:AppTheme.color("#202830",settingModel.darkMode);elide:Text.ElideRight}
        ButtonRow {enabled:!homePage.userRefresh&&!homePage.scrollAnimating&&!homePage.refreshing;anchors.right:parent.right;anchors.rightMargin:16;anchors.verticalCenter:parent.verticalCenter;width:184
         Button {text:"关注";font.pixelSize:22;checkable:true;checked:!firstPageModel.recommended;onClicked:homePage.switchFeed(false)}
         Button {text:"推荐";font.pixelSize:22;checkable:true;checked:firstPageModel.recommended;onClicked:homePage.switchFeed(true)}
        }
        Rectangle { anchors.bottom:parent.bottom; width:parent.width; height:1; color:AppTheme.color("#e7ebee",settingModel.darkMode) }
    }

    Rectangle {anchors.top:homepagetitle.bottom;width:parent.width;height:homePage.refreshOffset;visible:height>0;color:AppTheme.color("white",settingModel.darkMode);clip:true
        Row {anchors.centerIn:parent;spacing:16
            BusyIndicator {width:28;height:28;running:homePage.userRefresh}
            Text {anchors.verticalCenter:parent.verticalCenter;text:"正在刷新…";font.pixelSize:20;color:AppTheme.color("#75808a",settingModel.darkMode)}
        }
    }

    SinaWeiboItemList{
        id: sinalist
        objectName:"homeFeedList"
        anchors.top: homepagetitle.bottom
        anchors.topMargin:homePage.refreshOffset
        refreshInProgress:homePage.userRefresh
        interactionLocked:homePage.userRefresh||homePage.scrollAnimating
        width: parent.width
        height: parent.height-homepagetitle.height-homePage.refreshOffset
        model: firstPageModel
        readMode: funcContainer.readMode
        onItemlistscrolled:{
            console.log("==onItemlistscrolled==  type = ", type);
            if(type===0)homePage.refresh(sinalist.releasedPullDistance);else firstPageModel.getSinaContentFromModel(type);
        }
        onItemSelected:{
            console.log("==onItemSelected==  index = ",index);
            funcContainer.statusIDInDetailPage = firstPageModel.getSinaStudusId(index);
            listPage.changePage(6);
        }
        onShowStatusImage:{
            funcContainer.showDetailImage(firstPageModel.getImageAddress(0,index),firstPageModel.getImageAddress(1,index));
            //funcContainer.showStateImage(0,index);
        }
        onShowForwardImage:{
            funcContainer.showDetailImage(firstPageModel.getImageAddress(2,index),firstPageModel.getImageAddress(3,index));
            //funcContainer.showForwardImage(0,index);
        }
    }

    Component.onCompleted:
    {
        firstPageModel.getFriendsTimeLineResult.connect(homePage.sinaWeiBoFirstFinished);
        if(funcContainer.logined) {
            funcContainer.needRefresh=false;
            homePage.refresh();
        }
    }

    Component.onDestruction:{
        if(null === firstPageModel){
            console.log("=======firstPageModel has been deleted========");
        }
        else{
            firstPageModel.getFriendsTimeLineResult.disconnect(homePage.sinaWeiBoFirstFinished);
        }
    }

    function sinaWeiBoFirstFinished(errCode){
        console.log("SinaWeiboFirstPage.qml errCode:", errCode)
        //Processing
        if(-1 == errCode){
            refreshing=true;
            funcContainer.makeWaitingDialogVisible(!firstPageModel.hasData());
        }
        else{
            refreshing=false;
            if(homePage.userRefresh)refreshFinish.restart();
            funcContainer.completeLoginTransition();
            funcContainer.completeStartup();
            if(errCode===200 && revealNewPosts){revealNewPosts=false;revealTimer.restart();}
            funcContainer.makeWaitingDialogVisible(false);

            if((errCode != 40018) && (200 != errCode)) {
                funcContainer.showPopup(ErrorCodeFstPage.getErrorInfo(errCode));
            }
        }
    }
}

