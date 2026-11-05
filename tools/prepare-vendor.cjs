const fs=require('fs'),path=require('path'),crypto=require('crypto'),cp=require('child_process');
const root=path.resolve(__dirname,'..'),dir=path.join(root,'third_party');
const archive=path.join(dir,'mbedtls-3.6.7.tar.bz2');
const hash=crypto.createHash('sha256').update(fs.readFileSync(archive)).digest('hex');
if(hash!=='a7e8bcbec0e6f761b4af24f25677626b35f762f68eef79c08677a363212d11f6')throw Error('Mbed TLS archive checksum mismatch');
if(fs.existsSync(path.join(dir,'mbedtls-3.6.7')))throw Error('Vendor directory already exists; inspect it before extracting again');
cp.execFileSync('tar',['-xjf',archive,'-C',dir],{stdio:'inherit'});
console.log('Verified and extracted Mbed TLS 3.6.7');
