import QtQuick 1.1
Item {id:myPage;anchors.fill:parent;property bool is1stactive:true
 function activepage(){funcContainer.hometabindex=2;if(is1stactive){is1stactive=false;home.initialized=false;}home.ensureLoaded();}
 ProfileTimeline {id:home;anchors.fill:parent;userId:String(loginModel.userID);initialName:loginModel.nickName}
}
