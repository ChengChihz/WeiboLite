import QtQuick 1.1
import "../AppTheme.js" as AppTheme
Item {
 id:actions;height:60
 property string statusId:""
 property int comments:0
 property int reposts:0
 property int likes:0
 property bool liked:false
 property bool busy:false
 property bool changed:false
 property bool localLiked:false
 property int localLikes:0
 property bool effectiveLiked:changed?localLiked:liked
 onStatusIdChanged:{changed=false;busy=false;}
 property bool previousLiked:false
 property int previousLikes:0
 function rollback(){localLiked=previousLiked;localLikes=previousLikes;changed=true;busy=false;}
 function toggle(){
  if(busy||statusId==="")return;
  previousLiked=effectiveLiked;previousLikes=changed?localLikes:likes;
  localLiked=!previousLiked;localLikes=Math.max(0,previousLikes+(previousLiked?-1:1));changed=true;busy=true;
  if(!microBlog.requestLike(statusId,previousLiked)){rollback();funcContainer.showPopup("点赞未成功，请稍后重试");}
 }
 Connections {target:typeof microBlog!=="undefined"?microBlog:null
  onLikeFinished:{if(String(statusId)!==actions.statusId||!actions.busy)return;if(success)actions.busy=false;else{actions.rollback();funcContainer.showPopup(message||"点赞未成功，请稍后重试");}}
 }

 Row {anchors.fill:parent
  Repeater {model:3
   Item {width:actions.width/3;height:actions.height
    Rectangle {anchors.fill:parent;color:tap.pressed?AppTheme.color("#f0f4f7",settingModel.darkMode):"transparent"}
    Row {anchors.centerIn:parent;spacing:7
     Image {width:26;height:26;smooth:true;sourceSize.width:52;sourceSize.height:52;anchors.verticalCenter:parent.verticalCenter;source:index===0?(settingModel.darkMode?"icons/dark/repost.svg":"icons/repost.svg"):index===1?(settingModel.darkMode?"icons/dark/comment.svg":"icons/comment.svg"):(settingModel.darkMode?"icons/dark/like.svg":"icons/like.svg")}
     Text {font.pixelSize:20;color:index===2&&actions.effectiveLiked?AppTheme.color("#337ea9",settingModel.darkMode):AppTheme.color("#79858f",settingModel.darkMode);text:index===0?"转发"+(actions.reposts>0?" "+actions.reposts:""):index===1?"评论"+(actions.comments>0?" "+actions.comments:""):(actions.effectiveLiked?"已赞":"赞")+((actions.changed?actions.localLikes:actions.likes)>0?" "+(actions.changed?actions.localLikes:actions.likes):"")}
    }
    MouseArea {id:tap;anchors.fill:parent;enabled:actions.statusId!==""&&(index!==2||!actions.busy);onClicked:{if(index===0){funcContainer.repostStatusID=actions.statusId;funcContainer.repostStatusText="";listPage.changePage(8);}else if(index===1){funcContainer.commentStatusID=actions.statusId;listPage.changePage(9);}else actions.toggle();}}
   }
  }
 }
}
