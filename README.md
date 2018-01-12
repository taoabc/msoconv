# msoconv
Document convert package which use microsoft automation

Only for ms windows platform

## attention
1. We use exe to convert first! So if you use msoconv in electron, you may need spawn exe in `app.asar.unpacked` folder.
2. Only support PowerPoint to pdf yet.

## next
1. write C++ addon code to convert
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