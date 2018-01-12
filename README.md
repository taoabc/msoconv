# msoconv
Document convert package which use microsoft automation

## attention
1. We use exe to convert first! So if you use msoconv in electron, you may need spawn exe in `app.asar.unpacked` folder.
2. version 0.0.4 only support PowerPoint to pdf

## next
1. write C++ addon code to convert
2. Support every document type for microsofr office

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