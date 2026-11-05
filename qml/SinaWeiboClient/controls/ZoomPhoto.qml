import QtQuick 1.1
Item {
 id:viewer
 property url source
 property real zoom:1
 property real maximumZoom:4
 property bool pinching:false
 property real gestureZoom:1
 property real gestureX:0.5
 property real gestureY:0.5
 property bool loading:photo.status===Image.Loading
 property bool failed:photo.status===Image.Error
 property real naturalWidth:0
 property real naturalHeight:0
 property real fitScale:naturalWidth>0&&naturalHeight>0?Math.min(width/naturalWidth,height/naturalHeight):1
 property real baseWidth:naturalWidth*fitScale
 property real baseHeight:naturalHeight*fitScale
 property alias panX:canvas.contentX
 property alias panY:canvas.contentY
 property real imageWidth:photo.width
 property real imageHeight:photo.height
 signal tapped()
 function clampPan(){canvas.contentX=Math.max(0,Math.min(canvas.contentX,canvas.contentWidth-canvas.width));canvas.contentY=Math.max(0,Math.min(canvas.contentY,canvas.contentHeight-canvas.height));}
 function resetZoom(){zoom=1;if(canvas){canvas.contentX=0;canvas.contentY=0;}}
 function beginZoom(x,y){gestureZoom=zoom;gestureX=photo.width>0?(canvas.contentX+x-photo.x)/photo.width:0.5;gestureY=photo.height>0?(canvas.contentY+y-photo.y)/photo.height:0.5;}
 function updateZoom(scale,x,y){zoom=Math.max(1,Math.min(maximumZoom,gestureZoom*scale));canvas.contentX=photo.x+gestureX*photo.width-x;canvas.contentY=photo.y+gestureY*photo.height-y;clampPan();}
 function zoomTo(value){if(Math.abs(value-zoom)<0.001)return;beginZoom(width/2,height/2);updateZoom(value/gestureZoom,width/2,height/2);}
 onWidthChanged:resetZoom()
 onHeightChanged:resetZoom()
 Flickable {id:canvas;anchors.fill:parent;clip:true;interactive:!viewer.pinching;contentWidth:Math.max(width,photo.width);contentHeight:Math.max(height,photo.height)
  Image {id:photo;x:(canvas.contentWidth-width)/2;y:(canvas.contentHeight-height)/2;width:viewer.baseWidth*viewer.zoom;height:viewer.baseHeight*viewer.zoom;source:viewer.source;sourceSize.width:2048;asynchronous:true;smooth:true;fillMode:Image.PreserveAspectFit;onStatusChanged:{if(status===Image.Ready)readGeometry.restart();}}
  PinchArea {parent:canvas;anchors.fill:canvas
   onPinchStarted:{viewer.pinching=true;viewer.beginZoom(pinch.center.x,pinch.center.y);}
   onPinchUpdated:viewer.updateZoom(pinch.scale,pinch.center.x,pinch.center.y)
   onPinchFinished:{viewer.pinching=false;viewer.clampPan();}
   MouseArea {anchors.fill:parent;onClicked:tapTimer.restart();onDoubleClicked:{tapTimer.stop();viewer.resetZoom();}}
  }
 }
 Timer {id:readGeometry;interval:1;onTriggered:{viewer.naturalWidth=photo.implicitWidth;viewer.naturalHeight=photo.implicitHeight;viewer.resetZoom();}}
 Timer {id:tapTimer;interval:230;onTriggered:viewer.tapped()}
}
