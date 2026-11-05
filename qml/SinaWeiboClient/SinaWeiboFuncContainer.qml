import QtQuick 1.1
import "AppTheme.js" as AppTheme
import com.nokia.meego 1.0
import "controls"
import "SinaWeiboQMLCommonMethod.js" as QMLCommonMethod
import com.nokia.extras 1.0
import QtMultimediaKit 1.1

PageStackWindow {
    property string selectedAccountId:""
    property string selectedAccountSecret:""
    function activateAccount(id,secret){selectedAccountId=String(id);selectedAccountSecret=String(secret);accountTimer.restart();}
    Timer {id:accountTimer;interval:1;onTriggered:{
        var account=funcContainer.selectedAccountId;var secret=funcContainer.selectedAccountSecret;
        funcContainer.selectedAccountId="";funcContainer.selectedAccountSecret="";
        funcContainer.hometabindex=0;
        if(account===String(loginModel.userID)){funcContainer.navigateTo(12);return;}
        funcContainer.logined=false;funcContainer.needRefreshMsg=true;newMsgNotification.reset();
        accountModel.login(account,secret);funcContainer.navigateTo(12);
    }}

    Binding {target:theme;property:"inverted";value:settingModel.darkMode}
    id: funcContainer
    function navigateTo(page){var navigation=funcContainer.initialPage;if(!navigation||typeof navigation.changePage!=="function")return false;navigation.changePage(page);return true;}
    property bool startupBusy:true
    function completeStartup(){if(startupBusy)startupDone.restart();}
    Timer {id:startupDone;interval:300;onTriggered:funcContainer.startupBusy=false}
    Timer {interval:35000;running:funcContainer.startupBusy;onTriggered:funcContainer.startupBusy=false}
    Rectangle {anchors.fill:parent;z:300;visible:funcContainer.startupBusy;color:AppTheme.color("white",settingModel.darkMode)
        MouseArea {anchors.fill:parent}
        Image {visible:!settingModel.darkMode;anchors.fill:parent;source:width>height?"images/blog_loading_landscape.png":"images/blog_loading.png";fillMode:Image.PreserveAspectFit;smooth:true}
        Column {visible:settingModel.darkMode;anchors.centerIn:parent;spacing:24;Image {anchors.horizontalCenter:parent.horizontalCenter;width:96;height:96;source:"images/app_icon.png";smooth:true} Text {text:"微博Lite";font.pixelSize:32;color:"#edf2f7";anchors.horizontalCenter:parent.horizontalCenter}}
        Column {anchors.horizontalCenter:parent.horizontalCenter;anchors.bottom:parent.bottom;anchors.bottomMargin:64;spacing:16
            BusyIndicator {anchors.horizontalCenter:parent.horizontalCenter;width:40;height:40;running:funcContainer.startupBusy}
            Text {text:"正在加载微博…";font.pixelSize:20;color:AppTheme.color("#75808a",settingModel.darkMode)}
        }
    }
    property bool loginTransitionBusy:false
    function completeLoginTransition(){if(loginTransitionBusy)loginTransitionDone.restart();}
    Timer {id:loginTransitionDone;interval:350;onTriggered:funcContainer.loginTransitionBusy=false}
    Timer {interval:45000;running:funcContainer.loginTransitionBusy;onTriggered:{funcContainer.loginTransitionBusy=false;funcContainer.showPopup("时间流加载超时，请检查网络后刷新。");}}
    Rectangle {anchors.fill:parent;z:500;visible:funcContainer.loginTransitionBusy;color:AppTheme.color("#f7f8fa",settingModel.darkMode)
        MouseArea {anchors.fill:parent}
        Column {anchors.centerIn:parent;spacing:24
            BusyIndicator {anchors.horizontalCenter:parent.horizontalCenter;width:48;height:48;running:funcContainer.loginTransitionBusy}
            Text {text:"登录成功，正在加载微博…";font.pixelSize:22;color:AppTheme.color("#52606c",settingModel.darkMode)}
        }
    }

    Binding { target: pageStack.currentPage; property: "orientationLock"; value: PageOrientation.LockPortrait }

    property string insertTopic;
    property string searchTopic;
    property string insertContact;
    property string searchContact;

    property string loginName;
    property string loginPw;
    property bool needRefresh: false;
    property bool needRefreshMsg: false
//    property bool needRelogin: false;
    property bool logined: false;
    property int listDragDistance: -120
    property string selectImageSrc: ""
    property string savedImageSrc: ""

    //used for swith view
    property variant replyRetweetedId;
    property variant statusIDInDetailPage;
    property string userIDInMyProfilePage;
    property string userNameInProfilePage;
    property variant repostStatusID;
    property string repostStatusText;
    property variant commentStatusID;
    property int profileId: 0 //0--my profile, 1--anther user profile
    property int microBlogId: 0 //0--NewMicroBlog, 1--Comment, 2--Forword
    property int hometabindex: 0
    property int readMode: 0
    property string searchContent;
    property string topicName;
    property bool showStatusOfTrend: true
    property bool showUserInfo: true
    property string senderScreenName;
    property int indexForMessageCenter:0
    property int contactsType: 0
    //property bool typeForMessage: false //0-message, 1-my friendlist, 2-my fan's friendlist
    property int typeForListContainer: 0 //0--my list, 1-my fan's list, 2-message
    property string myfansId;
    //property string userId;
    property bool listNeedRefresh: true
    property int listContainerTabIndex: 0

    //for picViewer
    property string picSource
    property bool isEditable: false
    property bool isPicDeleted: false

    //for web view
    property string webviewurl

    //for db cache
    property int pageNumforCache: 0

    property bool isshowmoreindicator: true

    property int commentOrderOverride:-1
    property string commentHeader: ""
    //for detail image
//    property string smallPicForDetail
//    property string bigPicForDetail

//    platformStyle: defaultStyle;
//    ApplicationWindowStyle { id: defaultStyle }

    // ListPage is what we see when the app starts, it links to the component specific pages
    initialPage: PageList{}
//    showStatusBar : false
    function showBanner(contenttext){
        banner.text = contenttext;
        banner.show();
    }

    function makeWaitingDialogVisible(type){
        if(type&&(startupBusy||loginTransitionBusy)){waitingDialog.visible=false;return;}
        if(type){
            processing.start();
            waitingDialog.visible = true;
        }
        else{
            processing.stop();
            waitingDialog.visible = false;
        }
    }

    function showWaitingDialog(type){
        waitingDialog.visible = type&&!startupBusy&&!loginTransitionBusy;
    }

    function showPopup(showText){
        popup.popupContent = showText;
        popup.open();
    }

    Timer {
        id: processing
        interval: 45000; running: false; repeat: false;
        onTriggered: {
            processing.running = false;
            makeWaitingDialogVisible(false);
            funcContainer.showPopup("操作超时！");
        }
    }

    Item{
        id:waitingDialog
        visible: false
        anchors.fill: parent;
        z:100
        Rectangle{
            id:shaderLayer
            anchors.bottom: parent.bottom
            width: parent.width
            height: parent.height
            color: AppTheme.color("darkgray",settingModel.darkMode)
            opacity: 0.5
            //radius: funcContainer.style.cornersVisible ? 13 : 0
            MouseArea {
                anchors.fill: parent
            }
        }
        Item {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            height: 100
            width: 100
            BusyIndicator {
                id: busyindicator
                style: BusyIndicatorStyle { size: "large" }
                running:  true
            }
        }

    }

    QueryDialog {
        id: popup
        property string popupContent

        message: popupContent
        acceptButtonText: "确定"
    }

    InfoBanner{
        id: banner
        z:100
        topMargin: 37
    }

    SoundEffect {
        id: soundEffect
        source: "qrc:msgcome"
    }

    function playnotification(){
        soundEffect.volume = qt_funcView.getSystemVolume() / 100;
        console.log("====soundEffect.volume====", soundEffect.volume);
        soundEffect.play()
    }

    function showDetailImage(smalladdr,bigaddr)
    {
        var source=bigaddr||smalladdr;
        if(!source)return;
        funcContainer.picSource=source;funcContainer.isEditable=false;funcContainer.navigateTo(37);
    }
/*
    function showStateImage(cata,index)
    {
        switch(cata)
        {
            case 0:     //for first page
            {
                detailPic.smallSource = firstPageModel.getImageAddress(0,index);
                detailPic.bigSource = firstPageModel.getImageAddress(1,index);
                break;
            }
            case 1:     //for msg page
            {
                detailPic.smallSource = atMeModel.getImageAddress(0,index);
                detailPic.bigSource = atMeModel.getImageAddress(1,index);
                break;
            }
            case 2:     //for my WeiBo list page
            {
                detailPic.smallSource = myWeiboModel.getImageAddress(0,index);
                detailPic.bigSource = myWeiboModel.getImageAddress(1,index);
                break;
            }
            case 3:     //for SuiBianKanKan page
            {
                detailPic.smallSource = suiBianKanModel.getImageAddress(0,index);
                detailPic.bigSource = suiBianKanModel.getImageAddress(1,index);
                break;
            }
            case 4:     //for SearchBlog page
            {
                detailPic.smallSource = searchBlogModel.getImageAddress(0,index);
                detailPic.bigSource = searchBlogModel.getImageAddress(1,index);
                break;
            }
            case 5:     //for TrendsStatus page
            {
                detailPic.smallSource = trendBlogModel.getImageAddress(0,index);
                detailPic.bigSource = trendBlogModel.getImageAddress(1,index);
                break;
            }
            case 6:     //for my Favorites list page
            {
                detailPic.smallSource = myFavoritesModel.getImageAddress(0,index);
                detailPic.bigSource = myFavoritesModel.getImageAddress(1,index);
                break;
            }
            default:
            {
                detailPic.smallSource = "";
                detailPic.bigSource = "";
            }
        }
        detailPic.visible = true;
    }
    function showForwardImage(cata,index)
    {
        switch(cata)
        {
            case 0:     //for first page
            {
                detailPic.smallSource = firstPageModel.getImageAddress(2,index);
                detailPic.bigSource = firstPageModel.getImageAddress(3,index);
                break;
            }
            case 1:     //for msg page
            {
                detailPic.smallSource = atMeModel.getImageAddress(2,index);
                detailPic.bigSource = atMeModel.getImageAddress(3,index);
                break;
            }
            case 2:     //for my WeiBo list page
            {
                detailPic.smallSource = myWeiboModel.getImageAddress(2,index);
                detailPic.bigSource = myWeiboModel.getImageAddress(3,index);
                break;
            }
            case 3:     //for SuiBianKanKan page
            {
                detailPic.smallSource = suiBianKanModel.getImageAddress(2,index);
                detailPic.bigSource = suiBianKanModel.getImageAddress(3,index);
                break;
            }
            case 4:     //for SearchBlog page
            {
                detailPic.smallSource = searchBlogModel.getImageAddress(2,index);
                detailPic.bigSource = searchBlogModel.getImageAddress(3,index);
                break;
            }
            case 5:     //for TrendsStatus page
            {
                detailPic.smallSource = trendBlogModel.getImageAddress(2,index);
                detailPic.bigSource = trendBlogModel.getImageAddress(3,index);
                break;
            }
            case 6:     //for my Favorites list page
            {
                detailPic.smallSource = myFavoritesModel.getImageAddress(2,index);
                detailPic.bigSource = myFavoritesModel.getImageAddress(3,index);
                break;
            }
            default:
            {
                detailPic.smallSource = "";
                detailPic.bigSource = "";
            }
        }
        detailPic.visible = true;
    }
*/
    function tranferStringMonthToInt(month)
    {
        if(month == "Jan")
        {
            return "01";
        }
        if(month == "Feb")
        {
            return "02";
        }
        if(month == "Mar")
        {
            return "03";
        }
        if(month == "Apr")
        {
            return "04";
        }
        if(month == "May")
        {
            return "05";
        }
        if(month == "Jun")
        {
            return "06";
        }
        if(month == "Jul")
        {
            return "07";
        }
        if(month == "Aug")
        {
            return "08";
        }
        if(month == "Sep")
        {
            return "09";
        }
        if(month == "Oct")
        {
            return "10";
        }
        if(month == "Nov")
        {
            return "11";
        }
        if(month == "Dec")
        {
            return "12";
        }
    }
}
