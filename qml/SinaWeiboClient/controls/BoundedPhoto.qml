import QtQuick 1.1
Image {
 id:photo
 property int maxDisplayHeight:560
 signal clicked()
 width:parent?parent.width:448
 height:status===Image.Ready&&implicitWidth>0?Math.min(maxDisplayHeight,width*implicitHeight/implicitWidth):0
 sourceSize.width:Math.min(854,Math.max(1,Math.ceil(width)))
 asynchronous:true
 visible:source!==""
 fillMode:Image.PreserveAspectFit
 smooth:true
 MouseArea {anchors.fill:parent;onClicked:photo.clicked()}
}
