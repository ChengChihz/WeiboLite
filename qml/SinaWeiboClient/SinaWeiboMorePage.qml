import QtQuick 1.1
import "AppTheme.js" as AppTheme
import com.nokia.meego 1.0
import "controls"
Rectangle {
 id:morePage;anchors.fill:parent;color:AppTheme.color("#f7f8fa",settingModel.darkMode)
 function activepage(){funcContainer.hometabindex=3;}
 AccTitleBar {id:heading;width:parent.width;height:65;title:"更多"}
 Flickable {anchors.top:heading.bottom;anchors.bottom:parent.bottom;width:parent.width;contentWidth:width;contentHeight:items.height+24;clip:true
 Column {id:items;y:12;width:parent.width
 OptionRow {title:"设置";iconSource:Qt.resolvedUrl((settingModel.darkMode?"controls/icons/dark/option-gear.svg":"controls/icons/option-gear.svg"));onClicked:listPage.changePage(17)}
 OptionRow {title:"账号管理";iconSource:Qt.resolvedUrl((settingModel.darkMode?"controls/icons/dark/option-account.svg":"controls/icons/option-account.svg"));onClicked:listPage.changePage(19)}
 OptionRow {title:"深色模式";iconSource:Qt.resolvedUrl((settingModel.darkMode?"controls/icons/dark/option-night.svg":"controls/icons/option-night.svg"));showArrow:false;ThemeSwitch {anchors.right:parent.right;anchors.rightMargin:18;anchors.verticalCenter:parent.verticalCenter;checked:settingModel.darkMode;onCheckedChanged:settingModel.darkMode=checked}}
 OptionRow {title:"关于";iconSource:Qt.resolvedUrl((settingModel.darkMode?"controls/icons/dark/option-about.svg":"controls/icons/option-about.svg"));onClicked:listPage.changePage(16)}
 }}
}
