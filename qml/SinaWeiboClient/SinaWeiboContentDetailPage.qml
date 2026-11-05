import QtQuick 1.1
import "AppTheme.js" as AppTheme
import com.nokia.meego 1.0
import SinaWeiboTypeLib 1.0
import "controls"
import "controls/TimeFormat.js" as TimeFormat
Page {
 orientationLock:PageOrientation.LockPortrait;

 id:detailPage
 property string statusId:String(funcContainer.statusIDInDetailPage||"")
 property bool busy:false
 property string errorText:""
 property variant attachments:[]
 property variant previewComments:[]
 property bool commentsBusy:false
 property string commentsError:""
 function openComments(){funcContainer.commentStatusID=detailPage.statusId;funcContainer.commentOrderOverride=previewModel.latest?1:0;listPage.changePage(10);}
 Timer {id:previewTimer;interval:320;onTriggered:previewModel.getContentCommentFromModel(detailPage.statusId,0)}
 CommentModel {id:previewModel;previewLimit:15;latest:settingModel.latestComments;onGetContentCommentResult:{detailPage.commentsBusy=errCode===-1;if(errCode===200){detailPage.previewComments=previewModel.previewItems(15);detailPage.commentsError="";}else if(errCode!==-1)detailPage.commentsError="评论暂时无法加载";}}
 function refresh(){if(statusId!==""){detailModel.connectModel();detailModel.getSinaWeiboDetail(statusId);}}
 function openUser(){if(detailModel.userID!==""){funcContainer.userIDInMyProfilePage=detailModel.userID;funcContainer.userNameInProfilePage=detailModel.userName;listPage.changePage(7);}}
 function openLink(link){var url=String(link);if(url.indexOf("//")===0)url="https:"+url;if(url.indexOf("/")===0)url="https://m.weibo.cn"+url;if(url.indexOf("https://")===0||url.indexOf("http://")===0)Qt.openUrlExternally(url);}
 function styled(value){return String(value||"").replace(/<a\b([^>]*)>/gi,function(tag,attrs){return /\bstyle\s*=/.test(attrs)?tag:'<a style="color:'+AppTheme.color("#337ea9",settingModel.darkMode)+';text-decoration:none"'+attrs+'>';});}
 Component.onCompleted:loadTimer.restart()
 Timer {id:loadTimer;interval:300;onTriggered:detailPage.refresh()}
 Component.onDestruction:detailModel.disConnectModel()
 Rectangle {anchors.fill:parent;color:AppTheme.color("white",settingModel.darkMode)}
 DetailModel {id:detailModel;onGetRequestResult:{detailPage.busy=errCode===-1;if(errCode!==-1&&errCode!==200)detailPage.errorText="微博暂时无法加载，请重试";else detailPage.errorText="";if(errCode===200){detailPage.attachments=detailModel.GetPageInfo();previewTimer.restart();}}}
 DeleteModel {id:deleteModel;onGetDeleteResult:{if(errCode===200)listPage.viewBack();else if(errCode!==-1)funcContainer.showPopup("删除未成功，请稍后重试");}}
 AccTitleBar {id:heading;width:parent.width;height:65;title:"微博正文"
  Item {anchors.right:parent.right;width:64;height:65;Image {anchors.centerIn:parent;width:34;height:34;source:(settingModel.darkMode?"controls/icons/dark/refresh.svg":"controls/icons/refresh.svg")}MouseArea {anchors.fill:parent;onClicked:detailPage.refresh()}}
 }
 Flickable {anchors.top:heading.bottom;anchors.bottom:parent.bottom;width:parent.width;contentWidth:width;contentHeight:body.height+32;clip:true
  Column {id:body;x:16;y:16;width:parent.width-32;spacing:18
   Item {width:parent.width;height:60
    Image {id:avatar;width:52;height:52;source:detailModel.userProfileImg;asynchronous:true;sourceSize.width:104;sourceSize.height:104;fillMode:Image.PreserveAspectCrop;smooth:true
     Image {anchors.fill:parent;source:(settingModel.darkMode?"controls/icons/dark/avatar-corners.svg":"controls/icons/avatar-corners.svg")}
     Rectangle {visible:detailModel.userVerified;width:16;height:16;radius:8;anchors.right:parent.right;anchors.bottom:parent.bottom;color:"#e6a037";Text {anchors.centerIn:parent;text:"V";font.pixelSize:11;color:"white"}}
    }
    Text {x:68;y:0;width:parent.width-68;text:detailModel.userName;font.pixelSize:24;color:AppTheme.color("#202830",settingModel.darkMode);elide:Text.ElideRight}
    Text {x:68;y:34;width:parent.width-68;text:TimeFormat.compact(detailModel.statusCreateTime);font.pixelSize:16;color:AppTheme.color("#8b939b",settingModel.darkMode);elide:Text.ElideRight}
    MouseArea {anchors.fill:parent;onClicked:detailPage.openUser()}
   }
   Text {width:parent.width;text:detailPage.busy?"正在加载…":detailPage.errorText;font.pixelSize:20;color:AppTheme.color("#82919c",settingModel.darkMode);wrapMode:Text.Wrap;visible:text!==""}
   Text {width:parent.width;text:detailPage.styled(detailModel.statusText);textFormat:Text.RichText;font.pixelSize:settingModel.bodyTextSize;color:AppTheme.color("#252b32",settingModel.darkMode);wrapMode:Text.WrapAtWordBoundaryOrAnywhere;lineHeight:1.12;onLinkActivated:detailPage.openLink(link)}
   BoundedPhoto {width:parent.width;source:detailModel.statusImage;onClicked:funcContainer.showDetailImage(detailModel.statusImage,detailModel.statusImage)}
   Rectangle {width:parent.width;height:forwardBody.height+24;radius:8;color:AppTheme.color("#f3f5f7",settingModel.darkMode);visible:detailModel.retweetedID!==""
    Column {id:forwardBody;x:12;y:12;width:parent.width-24;spacing:12
     Text {width:parent.width;text:"@"+detailModel.retweetedUserName;font.pixelSize:21;color:AppTheme.color("#337ea9",settingModel.darkMode);wrapMode:Text.Wrap}
     Text {width:parent.width;text:detailPage.styled(detailModel.retweetedText);textFormat:Text.RichText;font.pixelSize:settingModel.bodyTextSize-3;color:AppTheme.color("#5a6671",settingModel.darkMode);wrapMode:Text.WrapAtWordBoundaryOrAnywhere;onLinkActivated:detailPage.openLink(link)}
     BoundedPhoto {width:parent.width;maxDisplayHeight:400;source:detailModel.retweetedImage;onClicked:funcContainer.showDetailImage(detailModel.retweetedImage,detailModel.retweetedImage)}
    }
   }
   Text {width:parent.width;text:String(detailModel.statusSource||"").replace(/<[^>]+>/g,"");font.pixelSize:16;color:AppTheme.color("#929aa2",settingModel.darkMode);elide:Text.ElideRight}
   Repeater {model:detailPage.attachments
    Rectangle {property string target:String(modelData.page_url||(modelData.media_info?modelData.media_info.h5_url:"")||"");width:body.width;height:target!==""?64:0;visible:target!=="";radius:8;color:AppTheme.color("#f3f5f7",settingModel.darkMode)
     Text {anchors.fill:parent;anchors.margins:12;text:modelData.page_title||modelData.content1||"查看附加内容";font.pixelSize:20;color:AppTheme.color("#337ea9",settingModel.darkMode);elide:Text.ElideRight;verticalAlignment:Text.AlignVCenter}
     MouseArea {anchors.fill:parent;onClicked:detailPage.openLink(parent.target)}
    }
   }
   Rectangle {width:parent.width;height:1;color:AppTheme.color("#e7ebee",settingModel.darkMode)}
   Row {width:parent.width;height:52;spacing:16
    Text {width:parent.width-220;anchors.verticalCenter:parent.verticalCenter;text:"评论 · "+detailModel.statusCommentCount;font.pixelSize:22;color:AppTheme.color("#202830",settingModel.darkMode)}
    ButtonRow {width:204;enabled:!detailPage.commentsBusy
     Button {text:"最新";font.pixelSize:20;checkable:true;checked:previewModel.latest;onClicked:{previewModel.latest=true;previewTimer.restart();}}
     Button {text:"热门";font.pixelSize:20;checkable:true;checked:!previewModel.latest;onClicked:{previewModel.latest=false;previewTimer.restart();}}
    }
   }
   Item {width:parent.width;height:visible?42:0;visible:detailPage.commentsBusy;BusyIndicator {anchors.centerIn:parent;width:28;height:28;running:parent.visible}}
   Text {width:parent.width;visible:!detailPage.commentsBusy&&(detailPage.commentsError!==""||detailPage.previewComments.length===0);text:detailPage.commentsError||"暂无评论";font.pixelSize:20;color:AppTheme.color("#87929c",settingModel.darkMode)}
   Repeater {model:detailPage.previewComments
    Item {width:body.width;height:commentBody.height+20
     Image {width:40;height:40;source:modelData.image;fillMode:Image.PreserveAspectCrop;asynchronous:true;smooth:true;Image {anchors.fill:parent;source:(settingModel.darkMode?"controls/icons/dark/avatar-corners.svg":"controls/icons/avatar-corners.svg")} MouseArea {anchors.fill:parent;onClicked:{funcContainer.userIDInMyProfilePage=String(modelData.userId);listPage.changePage(7);}}}
     Column {id:commentBody;x:52;width:parent.width-52;spacing:6
      Text {width:parent.width;text:modelData.userNickName;font.pixelSize:21;color:AppTheme.color("#52606c",settingModel.darkMode);elide:Text.ElideRight}
      Text {width:parent.width;text:detailPage.styled(modelData.weibocontent);textFormat:Text.RichText;wrapMode:Text.WrapAtWordBoundaryOrAnywhere;font.pixelSize:settingModel.bodyTextSize-2;color:AppTheme.color("#252b32",settingModel.darkMode);onLinkActivated:detailPage.openLink(link)}
      Text {text:TimeFormat.compact(modelData.untilTime);font.pixelSize:16;color:AppTheme.color("#929aa2",settingModel.darkMode)}
     }
     Rectangle {anchors.bottom:parent.bottom;width:parent.width;height:1;color:AppTheme.color("#edf0f2",settingModel.darkMode)}
    }
   }
   Button {text:"查看全部评论";width:parent.width;enabled:detailModel.statusID!=="";onClicked:detailPage.openComments()}

  }
 }
 QueryDialog {id:deleteQuery;message:"是否删除该条微博？";acceptButtonText:"是";rejectButtonText:"否";onAccepted:{deleteModel.connectModel();deleteModel.deleteStatus(detailPage.statusId);}}
 tools:ToolBarLayout {visible:false
  ToolIcon {iconId:(settingModel.darkMode?"icon-m-toolbar-back-white":"icon-m-toolbar-back");onClicked:listPage.viewBack()}
  ToolIcon {transform:Translate {y:3} iconSource:(settingModel.darkMode?"image://nighticon/icon_forward.png":"images/icon_forward.png");onClicked:{if(detailModel.statusID!==""){funcContainer.repostStatusID=detailModel.statusID;funcContainer.repostStatusText="";listPage.changePage(8);}}}
  ToolIcon {transform:Translate {y:3} iconSource:(settingModel.darkMode?"image://nighticon/icon_comment.png":"images/icon_comment.png");onClicked:{if(detailModel.statusID!==""){funcContainer.commentStatusID=detailModel.statusID;listPage.changePage(9);}}}
  ToolIcon {visible:detailModel.userID!==""&&detailModel.userID===String(loginModel.userID);iconId:(settingModel.darkMode?"icon-m-toolbar-delete-white":"icon-m-toolbar-delete");onClicked:deleteQuery.open()}
 }
}
