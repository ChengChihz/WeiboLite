const fs=require('fs'),path=require('path'),crypto=require('crypto'),zlib=require('zlib');
const root=path.resolve(__dirname,'..'),strict=process.argv.includes('--verify-release');
const manifest=JSON.parse(fs.readFileSync(path.join(root,'packaging/manifest.json'),'utf8'));
function read(relative){const target=path.resolve(root,relative);if(!target.startsWith(root+path.sep))throw Error('Path outside repository');return fs.readFileSync(target);}
function digest(data,algorithm='sha256'){return crypto.createHash(algorithm).update(data).digest('hex');}
function tar(entries){const blocks=[];for(const entry of entries){const name=entry.name.startsWith('./')?entry.name:'./'+entry.name;
 const encoded=Buffer.from(name);if(encoded.length>100)throw Error('Tar path too long: '+name);
 const data=entry.directory?Buffer.alloc(0):entry.data,h=Buffer.alloc(512);encoded.copy(h);
 const oct=(offset,length,value)=>h.write(value.toString(8).padStart(length-1,'0')+'\0',offset,length,'ascii');
 oct(100,8,entry.mode||0o644);oct(108,8,0);oct(116,8,0);oct(124,12,data.length);oct(136,12,1793836800);
 h.fill(32,148,156);h[156]=entry.directory?53:48;h.write('ustar\0',257);h.write('00',263);
 const sum=h.reduce((a,b)=>a+b,0);h.write(sum.toString(8).padStart(6,'0')+'\0 ',148,8,'ascii');
 blocks.push(h,data);if(data.length%512)blocks.push(Buffer.alloc(512-data.length%512));}
 blocks.push(Buffer.alloc(1024));return Buffer.concat(blocks);}
function ar(entries){const blocks=[Buffer.from('!<arch>\n')];for(const {name,data} of entries){const field=(v,n)=>String(v).padEnd(n,' ');const header=field(name+'/',16)+field(1793836800,12)+field(0,6)+field(0,6)+field('100644',8)+field(data.length,10)+'`\n';blocks.push(Buffer.from(header,'ascii'),data);if(data.length%2)blocks.push(Buffer.from('\n'));}return Buffer.concat(blocks);}
const payload=manifest.map(e=>{if(e.directory)return {name:e.path,mode:e.mode,directory:true};const data=read(e.source);if(strict&&digest(data)!==e.sha256)throw Error('Not identical to published 1.0.0: '+e.path);return {name:e.path,mode:e.mode,data};});
let control=read('packaging/control').toString('utf8');const size=payload.reduce((sum,e)=>sum+(e.directory?1:Math.ceil(e.data.length/1024)),0);control=control.replace(/^Installed-Size:.*$/m,'Installed-Size: '+size);
const md5sums=payload.filter(e=>!e.directory).map(e=>digest(e.data,'md5')+'  '+e.name).join('\n')+'\n';
const deb=ar([{name:'debian-binary',data:Buffer.from('2.0\n')},{name:'control.tar.gz',data:zlib.gzipSync(tar([{name:'control',mode:0o644,data:Buffer.from(control)},{name:'md5sums',mode:0o644,data:Buffer.from(md5sums)}]))},{name:'data.tar.gz',data:zlib.gzipSync(tar(payload))}]);
const output=path.join(root,'dist','WeiboLite_1.0.0_armel.deb');fs.mkdirSync(path.dirname(output),{recursive:true});fs.writeFileSync(output,deb);
console.log(JSON.stringify({output,files:payload.filter(e=>!e.directory).length,verifiedAgainstRelease:strict,sha256:digest(deb)}));
