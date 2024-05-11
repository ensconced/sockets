const crypto = require("node:crypto");

const dataView = new DataView(new ArrayBuffer(28));

function randUInt32() {
  return Math.floor(Math.random() * 2 ** 32);
}

function randUInt16() {
  return Math.floor(Math.random() * 2 ** 16);
}

const localIpv4 = randUInt32();
const localPort = randUInt16();
const remoteIpv4 = randUInt32();
const remotePort = randUInt16();
const secretKey = [
  0x81, 0x6c, 0x66, 0x75, 0x16, 0xf2, 0xe5, 0xc, 0xf1, 0x5f, 0x21, 0xe9, 0xcb,
  0xab, 0x5a, 0xf2,
];

let byteOffset = 0;
dataView.setUint32(0, localIpv4, false);
byteOffset += 4;
dataView.setUint16(byteOffset, localPort, false);
byteOffset += 2;
dataView.setUint32(byteOffset, remoteIpv4, false);
byteOffset += 4;
dataView.setUint16(byteOffset, remotePort, false);
byteOffset += 2;

for (let i = 0; i < 16; i++) {
  dataView.setUint8(byteOffset, secretKey[i]);
  byteOffset++;
}

const md5 = crypto.createHash("md5").update(dataView).digest();

console.log({ localIpv4, localPort, remoteIpv4, remotePort, secretKey, md5 });
