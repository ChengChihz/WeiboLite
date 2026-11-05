import QtQuick 1.1
import "../AppTheme.js" as AppTheme
import com.nokia.meego 1.0
Rectangle {
 id:row;width:parent?parent.width:480;height:88;color:tap.pressed?AppTheme.color("#e8eef3",settingModel.darkMode):"transparent"
 property string title:"";property string value:"";property string iconSource:"";property bool showArrow:true
 signal clicked()
 Rectangle {color:AppTheme.color("white",settingModel.darkMode);x:16;anchors.verticalCenter:parent.verticalCenter;width:60;height:60;radius:21;gradient:Gradient {GradientStop {position:0;color:AppTheme.color("#edf4fb",settingModel.darkMode)} GradientStop {position:1;color:AppTheme.color("#dce9f5",settingModel.darkMode)}}
 Image {anchors.centerIn:parent;width:36;height:36;source:row.iconSource;smooth:true}
 }
 Column {x:96;anchors.verticalCenter:parent.verticalCenter;width:parent.width-164;spacing:4
 Text {width:parent.width;text:row.title;font.pixelSize:24;font.bold:true;color:AppTheme.color("#202830",settingModel.darkMode);elide:Text.ElideRight}
 Text {width:parent.width;visible:text!=="";text:row.value;font.pixelSize:19;color:AppTheme.color("#82919c",settingModel.darkMode);elide:Text.ElideRight}
 }
 Image {anchors.right:parent.right;anchors.rightMargin:22;anchors.verticalCenter:parent.verticalCenter;width:24;height:24;source:("image://theme/icon-m-common-drilldown-arrow"+(settingModel.darkMode?"-inverse":""));visible:row.showArrow}
 MouseArea {id:tap;anchors.fill:parent;enabled:row.showArrow;onClicked:row.clicked()}
}
