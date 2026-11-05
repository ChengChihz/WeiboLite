import QtQuick 1.1
import "AppTheme.js" as AppTheme
import com.nokia.meego 1.0
Page {
 orientationLock:PageOrientation.LockPortrait;

 id:aboutPage
 property bool updatesExpanded:false
 function toggleUpdates(){updatesExpanded=!updatesExpanded;if(!updatesExpanded)settleScroll.restart();}
 Timer {id:settleScroll;interval:1;onTriggered:aboutScroll.contentY=Math.max(0,Math.min(aboutScroll.contentY,aboutScroll.contentHeight-aboutScroll.height))}
 Rectangle {anchors.fill:parent;color:AppTheme.color("#f7f8fa",settingModel.darkMode)}
 Flickable {
  id:aboutScroll;objectName:"aboutScroll";anchors.fill:parent;contentWidth:width;contentHeight:details.height+64;clip:true
  Column {id:details;x:28;y:32;width:parent.width-56;spacing:22
   Image {width:80;height:80;source:"images/app_icon.png";smooth:true}
   Text {text:"微博Lite";font.pixelSize:32;color:AppTheme.color("#202a33",settingModel.darkMode)}
   Text {text:"为 Nokia N9 · MeeGo Harmattan 改进的非官方微博客户端。";font.pixelSize:22;color:AppTheme.color("#40505f",settingModel.darkMode);width:parent.width;wrapMode:Text.Wrap}
   Rectangle {width:parent.width;height:1;color:AppTheme.color("#dce2e7",settingModel.darkMode)}
   Text {text:"Credits";font.pixelSize:24;color:AppTheme.color("#202a33",settingModel.darkMode)}
   Column {width:parent.width;spacing:14
    Text {text:"MeeGo 微博客户端、微博品牌与相关内容©新浪微博及相应权利人";font.pixelSize:20;color:AppTheme.color("#52606c",settingModel.darkMode);width:parent.width;wrapMode:Text.Wrap}
    Text {text:"开源基础与改版：Karin · Weibo++ / Weibomm";font.pixelSize:20;color:AppTheme.color("#52606c",settingModel.darkMode);width:parent.width;wrapMode:Text.Wrap}
    Text {text:"2026年可用性更新与新界面UI设计：Chih";font.pixelSize:20;color:AppTheme.color("#52606c",settingModel.darkMode);width:parent.width;wrapMode:Text.Wrap}
   }
   Text {text:"基于 github.com/glKarin/weibomm 修改，与新浪微博无官方隶属关系";font.pixelSize:18;color:AppTheme.color("#75808a",settingModel.darkMode);width:parent.width;wrapMode:Text.Wrap}
   Rectangle {width:parent.width;height:1;color:AppTheme.color("#e3e7eb",settingModel.darkMode)}
   Column {width:parent.width;spacing:10
    Text {text:"版本：1.0.0\n最后更新：2026-11-05";font.pixelSize:17;color:AppTheme.color("#87929c",settingModel.darkMode);width:parent.width;wrapMode:Text.Wrap}
    Item {id:updatesToggle;objectName:"updatesToggle";width:parent.width;height:52
     Text {anchors.left:parent.left;anchors.verticalCenter:parent.verticalCenter;text:"主要更新";font.pixelSize:18;color:AppTheme.color("#87929c",settingModel.darkMode)}
     Row {anchors.right:parent.right;anchors.verticalCenter:parent.verticalCenter;spacing:10
      Text {text:aboutPage.updatesExpanded?"收起":"展开";font.pixelSize:18;color:AppTheme.color("#337ea9",settingModel.darkMode)}
      Item {width:14;height:22;Rectangle {anchors.centerIn:parent;width:12;height:2;color:AppTheme.color("#337ea9",settingModel.darkMode)} Rectangle {anchors.centerIn:parent;width:2;height:12;visible:!aboutPage.updatesExpanded;color:AppTheme.color("#337ea9",settingModel.darkMode)}}
     }
     MouseArea {anchors.fill:parent;onClicked:aboutPage.toggleUpdates()}
    }
    Text {objectName:"updateNotes";text:"本次 MeeGo 微博 Lite 改版的累计调整：\n\n一、可用性与兼容性\n• 适配现有微博数据、发布、评论、点赞和用户资料接口，修复内容、头像、来源与表情解析\n• 增加应用独立的 HTTPS／TLS 1.2 兼容支持，保留证书校验，使 N9 能够连接现有微博服务\n• 接入官方账号密码、短信登录与安全验证，改善验证框显示、资源加载及登录跳转\n• 新增电脑辅助登录：将本人的电脑端登录会话通过导入 N9，作为官方验证受限时的备用方式\n\n二、视觉及 UI\n• 新设计的微博主页、用户主页、搜索与热搜榜单、消息等界面\n• 更新应用图标、登录与开屏画面\n• 新增深色模式\n• 新增字号调整设置\n• 优化启动展示、下拉刷新\n\n三、功能调整与新增\n• 微博主页提供关注与推荐时间流，并新增快捷转发、评论与点赞功能\n• 更新用户主页，集中展示资料、微博、关注与粉丝信息\n• 微博正文内加入评论预览，支持最新／热门排序，并可进入完整评论页\n• 更新搜索页，加入排名、热搜预览及完整榜单入口\n• 更新 127 个新版微博表情\n• 图片查看支持双指缩放、拖动和缩放滑杆";visible:aboutPage.updatesExpanded;height:visible?paintedHeight:0;font.pixelSize:17;color:AppTheme.color("#87929c",settingModel.darkMode);width:details.width;wrapMode:Text.Wrap;textFormat:Text.PlainText}
    Item {width:parent.width;height:visible?44:0;visible:aboutPage.updatesExpanded
     Text {anchors.centerIn:parent;text:"收起更新说明";font.pixelSize:18;color:AppTheme.color("#337ea9",settingModel.darkMode)}
     MouseArea {anchors.fill:parent;onClicked:aboutPage.toggleUpdates()}
    }
   }
  }
 }
 ScrollDecorator {flickableItem:aboutScroll}
 tools: ToolBarLayout {ToolIcon {iconId:(settingModel.darkMode?"icon-m-toolbar-back-white":"icon-m-toolbar-back");onClicked:listPage.viewBack()}}
}
