# msoconv

[![Build Status](https://travis-ci.org/taoabc/msoconv.svg?branch=production)](https://travis-ci.org/taoabc/msoconv)

Document convert package which use microsoft automation

Only for windows platform

## Release 0.2
1. Now we can convert document with C++ native code, not exe, :laughing:.

## attention
2. Only support PowerPoint to pdf yet.

## next
2. Convert any document type which microsoft office support

## usage

```javascript
const msoconv = require('msoconv')
msoconv('C:/test.pptx', 'C:/test.pdf')
.then(() => {
  console.log('convert ok')
})
.catch(err => {
  // 1 unknown error
  // 2 parameter error
  // 3 powerpoint error
  // 4 presentations error
  // 5 open file error
  // 6 unsupport type
  // 7 save as error
})
```