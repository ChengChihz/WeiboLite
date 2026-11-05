import QtQuick 1.1
import "../AppTheme.js" as AppTheme
import "TimeFormat.js" as TimeFormat
Item {
 id:sina;width:parent?parent.width:480;height:layout.height+41
 property int showPic:0
 property int imageMode:typeof settingModel!=="undefined"&&typeof settingModel.imageMode==="number"?settingModel.imageMode:typeof settingModel!=="undefined"&&settingModel.autoImages===false?1:0
 property bool loadPhotos:imageMode===0
 property int bodySize:typeof settingModel!=="undefined"&&typeof settingModel.bodyTextSize==="number"?settingModel.bodyTextSize:typeof settingModel!=="undefined"&&settingModel.largeText===true?28:24
 signal itemClicked(int index)
 signal statusImageClicked(int index)
 signal forwardImageClicked(int index)
 function styled(value){return String(value||'').replace(/<a\b([^>]*)>/gi,function(tag,attrs){return /\bstyle\s*=/.test(attrs)?tag:'<a style="color:'+AppTheme.color("#337ea9",settingModel.darkMode)+';text-decoration:none"'+attrs+'>';});}
 Rectangle {anchors.fill:parent;color:AppTheme.color("white",settingModel.darkMode)}
 MouseArea {anchors.fill:parent;onClicked:sina.itemClicked(index)}
 Column {id:layout;x:16;y:24;width:parent.width-32;spacing:8
  Item {id:header;width:parent.width;height:52
   Rectangle {id:avatar;objectName:"feedAvatar";width:48;height:48;radius:12;color:AppTheme.color("#e4edf2",settingModel.darkMode)
    Text {anchors.centerIn:parent;text:String(userNickName||'微').charAt(0);font.pixelSize:24;color:AppTheme.color("#46758d",settingModel.darkMode)}
    Image {anchors.fill:parent;source:image;fillMode:Image.PreserveAspectCrop;smooth:true;visible:source!=""}
    Image {anchors.fill:parent;source:(settingModel.darkMode?"icons/dark/avatar-corners.svg":"icons/avatar-corners.svg");smooth:true}
    Rectangle {visible:isVip;width:15;height:15;radius:7;anchors.right:parent.right;anchors.bottom:parent.bottom;color:"#e6a037";Text {anchors.centerIn:parent;text:"V";font.pixelSize:11;color:"white"}}
   }
   MouseArea {anchors.fill:avatar;onClicked:{if(String(userNickName)!==""){funcContainer.userNameInProfilePage=userNickName;funcContainer.userIDInMyProfilePage=userId;listPage.changePage(7);}}}
   Text {objectName:"feedAuthor";x:62;y:0;width:parent.width-62;height:29;text:userNickName;font.pixelSize:22;color:AppTheme.color("#202830",settingModel.darkMode);elide:Text.ElideRight}
   Text {objectName:"feedTime";x:62;y:31;width:parent.width-62;height:20;text:TimeFormat.compact(untilTime)+(String(fromPlat||'').replace(/<[^>]+>/g,'')!==''?'  来自 '+String(fromPlat||'').replace(/<[^>]+>/g,''):'')+(showPic!==0 && hasPic?" · 图片":"")+(hasMap?" · 位置":"");font.pixelSize:16;color:AppTheme.color("#8b939b",settingModel.darkMode);elide:Text.ElideRight}
  }
  Text {width:parent.width;text:sina.styled(weibocontent);textFormat:Text.RichText;font.pixelSize:sina.bodySize;color:AppTheme.color("#252b32",settingModel.darkMode);wrapMode:Text.WrapAtWordBoundaryOrAnywhere;lineHeight:1.12}
  Image {id:statusPhoto;objectName:"feedStatusPhoto";visible:source!="" && sina.loadPhotos;width:parent.width;height:sourceSize.width>0?Math.min(320,width*sourceSize.height/sourceSize.width):0;source:sina.loadPhotos?statusThumbnailPic:"";fillMode:Image.PreserveAspectFit;smooth:true;MouseArea {anchors.fill:parent;onClicked:sina.statusImageClicked(index)}}
  Rectangle {width:parent.width;height:visible?52:0;visible:sina.imageMode===1&&String(statusThumbnailPic||"")!=="";color:AppTheme.color("#f3f5f7",settingModel.darkMode);radius:8;Text {anchors.centerIn:parent;text:"点击查看图片";font.pixelSize:20;color:AppTheme.color("#5c7b8e",settingModel.darkMode)} MouseArea {anchors.fill:parent;onClicked:sina.statusImageClicked(index)}}
  Rectangle {visible:forwardcontent!=="";width:parent.width;height:forwardColumn.height+24;radius:8;color:AppTheme.color("#f3f5f7",settingModel.darkMode)
   Column {id:forwardColumn;x:12;y:12;width:parent.width-24;spacing:10
    Text {width:parent.width;text:sina.styled(forwardcontent);textFormat:Text.RichText;font.pixelSize:sina.bodySize-3;color:AppTheme.color("#5a6671",settingModel.darkMode);wrapMode:Text.WrapAtWordBoundaryOrAnywhere}
    Image {id:forwardPhoto;objectName:"feedForwardPhoto";visible:source!=="" && sina.loadPhotos;width:parent.width;height:sourceSize.width>0?Math.min(240,width*sourceSize.height/sourceSize.width):0;source:sina.loadPhotos?forwardThumbnailPic:"";fillMode:Image.PreserveAspectFit;smooth:true;MouseArea {anchors.fill:parent;onClicked:sina.forwardImageClicked(index)}}
    Rectangle {width:parent.width;height:visible?52:0;visible:sina.imageMode===1&&String(forwardThumbnailPic||"")!=="";color:AppTheme.color("#e7edf1",settingModel.darkMode);radius:8;Text {anchors.centerIn:parent;text:"点击查看转发图片";font.pixelSize:20;color:AppTheme.color("#5c7b8e",settingModel.darkMode)} MouseArea {anchors.fill:parent;onClicked:sina.forwardImageClicked(index)}}
   }
  }
  StatusActions {width:parent.width;visible:statusId!=="";height:visible?60:0;statusId:typeof actionStatusId!=="undefined"?String(actionStatusId||""):"";comments:commentCount;reposts:forwardCount;likes:typeof likeCount!=="undefined"?likeCount:0;liked:typeof model!=="undefined"&&typeof model.liked!=="undefined"?model.liked:false}

 }
 Rectangle {anchors.bottom:parent.bottom;width:parent.width;height:1;color:AppTheme.color("#e7ebee",settingModel.darkMode)}
}
