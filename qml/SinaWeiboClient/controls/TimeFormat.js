.pragma library
function pad(n){return n<10?'0'+n:String(n);}
function parseWeiboDate(text){
 var m=String(text||'').match(/^[A-Za-z]{3}\s+([A-Za-z]{3})\s+(\d{1,2})\s+(\d{2}):(\d{2}):(\d{2})\s+([+-])(\d{2})(\d{2})\s+(\d{4})$/);
 if(!m)return null;var month=['Jan','Feb','Mar','Apr','May','Jun','Jul','Aug','Sep','Oct','Nov','Dec'].indexOf(m[1]),day=Number(m[2]),h=Number(m[3]),min=Number(m[4]),sec=Number(m[5]),year=Number(m[9]);
 if(month<0||h>23||min>59||sec>59||Number(m[7])>23||Number(m[8])>59)return null;
 var utc=Date.UTC(year,month,day,h,min,sec),check=new Date(utc);if(check.getUTCFullYear()!==year||check.getUTCMonth()!==month||check.getUTCDate()!==day)return null;
 var offset=(Number(m[7])*60+Number(m[8]))*(m[6]==='+'?1:-1);return new Date(utc-offset*60000);
}
function compact(text,nowMillis){
 var raw=String(text||''),date=parseWeiboDate(raw);if(!date)return raw.length<=18?raw:'时间未知';
 var now=new Date(typeof nowMillis==='number'?nowMillis:new Date().getTime()),clock=pad(date.getHours())+':'+pad(date.getMinutes());
 var days=(Date.UTC(now.getFullYear(),now.getMonth(),now.getDate())-Date.UTC(date.getFullYear(),date.getMonth(),date.getDate()))/86400000;
 if(days===0)return '今天 '+clock;if(days===1)return '昨天 '+clock;
 if(date.getFullYear()===now.getFullYear())return pad(date.getMonth()+1)+'-'+pad(date.getDate())+' '+clock;
 return date.getFullYear()+'-'+pad(date.getMonth()+1)+'-'+pad(date.getDate());
}
